#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Data/EmbermereItemData.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "InputKeyEventArgs.h"
#include "Misc/AutomationTest.h"
#include "UI/EmbermerePlayerHudWidget.h"
#include "Widgets/SWidget.h"

struct FEmbermereServiceKeyboardFixture
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	AEmbermerePlayerController* Controller = nullptr;
	AEmbermereCharacter* Character = nullptr;
	UEmbermerePlayerHudWidget* Hud = nullptr;
	TSharedPtr<SWidget> SlateWidget;

	FEmbermereServiceKeyboardFixture()
	{
		GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
		Controller = World->SpawnActor<AEmbermerePlayerController>();
		Character = World->SpawnActor<AEmbermereCharacter>();
		Controller->bShowCharacterCreationOnFirstPlay = false;
		Controller->PlayerInput = NewObject<UPlayerInput>(Controller);
		Hud = NewObject<UEmbermerePlayerHudWidget>(Controller);
		Hud->Initialize();
		SlateWidget = Hud->TakeWidget();
		Hud->BindToCharacter(Character);
		Controller->PlayerHudWidget = Hud;
		Character->Wallet->SetCopperForPrototype(40);
	}

	~FEmbermereServiceKeyboardFixture()
	{
		Controller->PlayerInput->FlushPressedKeys();
		Hud->CloseVendor();
		Hud->CloseTrainer();
		Hud->BindToCharacter(nullptr);
		SlateWidget.Reset();
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	}

	void Queue(const FKey& Key, EInputEvent Type)
	{
		// Detached input owner and simulated device both use PLATFORMUSERID_NONE.
		Controller->InputKey(FInputKeyEventArgs::CreateSimulated(Key, Type, 1.0f));
	}

	void Event(const FKey& Key, EInputEvent Type)
	{
		Queue(Key, Type);
		Controller->PlayerTick(1.0f / 60.0f);
	}

	void Press(const FKey& Key)
	{
		Event(Key, IE_Pressed);
		Event(Key, IE_Released);
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereVendorKeyboardInputTest,
	"Embermere.UI.ServiceKeyboard.Vendor",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorKeyboardInputTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	if (!TestNotNull(TEXT("Saved stock exists"), Stock) || !TestEqual(TEXT("Two stock rows"), Stock->Entries.Num(), 2))
	{
		return false;
	}
	FEmbermereServiceKeyboardFixture F;
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>(F.Character);
	Vendor->SetStockData(Stock);
	TestTrue(TEXT("Vendor opens"), F.Hud->ShowVendor(Vendor));
	F.Character->Inventory->MaxSlots = 0;
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Full bag rejects without charge"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Full bag receives nothing"), F.Character->Inventory->Stacks.Num(), 0);
	F.Character->Inventory->MaxSlots = 24;
	UEmbermereVendorStockData* InvalidStock = DuplicateObject<UEmbermereVendorStockData>(Stock, F.Character);
	InvalidStock->Entries[0].UnitPriceCopper = 0;
	Vendor->SetStockData(InvalidStock);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Invalid price rejects without charge"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Invalid price delivers nothing"), F.Character->Inventory->Stacks.Num(), 0);
	Vendor->SetStockData(Stock);
	F.Press(EKeys::Down);
	TestEqual(TEXT("Routed Down selects pack stock"), F.Hud->GetSelectedVendorStockIndex(), 1);
	F.Press(EKeys::Down);
	TestEqual(TEXT("Down wraps stock"), F.Hud->GetSelectedVendorStockIndex(), 0);
	F.Press(EKeys::Up);
	TestEqual(TEXT("Up wraps to pack"), F.Hud->GetSelectedVendorStockIndex(), 1);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Enter buys selected pack for 30"), F.Character->Wallet->Copper, 10);
	TestEqual(TEXT("Pack delivered exactly once"), F.Character->Inventory->GetItemQuantity(Stock->Entries[1].Item), 1);
	TestEqual(TEXT("Finite pack exhausted"), Vendor->GetRemainingQuantity(1), 0);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Exhausted row rejects key request"), F.Character->Wallet->Copper, 10);
	TestEqual(TEXT("Exhausted row cannot duplicate pack"), F.Character->Inventory->GetItemQuantity(Stock->Entries[1].Item), 1);
	F.Press(EKeys::Up);
	F.Event(EKeys::Enter, IE_Pressed);
	TestEqual(TEXT("Tonic primary action costs eight"), F.Character->Wallet->Copper, 2);
	F.Character->Wallet->SetCopperForPrototype(40);
	F.Event(EKeys::Enter, IE_Repeat);
	F.Controller->PlayerTick(1.0f / 60.0f);
	TestEqual(TEXT("Repeat cannot charge even when affordable"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Held input does not duplicate delivery"), F.Character->Inventory->GetItemQuantity(Stock->Entries[0].Item), 1);
	F.Event(EKeys::Enter, IE_Released);
	F.Character->Wallet->SetCopperForPrototype(2);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Insufficient-funds request preserves copper"), F.Character->Wallet->Copper, 2);
	TestEqual(TEXT("Insufficient-funds request preserves bag"), F.Character->Inventory->GetItemQuantity(Stock->Entries[0].Item), 1);
	F.Hud->SelectInventoryItem(0);
	F.Press(EKeys::RightBracket);
	TestEqual(TEXT("Brackets still select bag identities"), F.Hud->GetSelectedInventoryStackIndex(), 1);
	TestEqual(TEXT("Bag selection does not change stock"), F.Hud->GetSelectedVendorStockIndex(), 0);
	F.Character->Wallet->SetCopperForPrototype(40);
	F.Queue(EKeys::Enter, IE_Pressed);
	F.Event(EKeys::Escape, IE_Pressed);
	F.Event(EKeys::Enter, IE_Released);
	F.Event(EKeys::Escape, IE_Released);
	TestEqual(TEXT("Close takes precedence over same-frame Buy"), F.Character->Wallet->Copper, 40);
	TestFalse(TEXT("Escape closes vendor"), F.Hud->IsVendorPanelVisible());
	TestFalse(TEXT("Closing restores cursor-hidden state"), F.Controller->bShowMouseCursor);
	F.Character->Wallet->SetCopperForPrototype(40);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Closed service cannot act"), F.Character->Wallet->Copper, 40);
	F.Hud->ShowVendor(Vendor);
	F.Queue(EKeys::Enter, IE_Pressed);
	F.Event(EKeys::I, IE_Pressed);
	F.Event(EKeys::Enter, IE_Released);
	F.Event(EKeys::I, IE_Released);
	TestTrue(TEXT("I hands off to inventory"), F.Hud->IsInventoryPanelVisible());
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Peer panel cannot act on old vendor"), F.Character->Wallet->Copper, 40);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereTrainerKeyboardInputTest,
	"Embermere.UI.ServiceKeyboard.Trainer",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerKeyboardInputTest::RunTest(const FString& Parameters)
{
	UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	if (!TestNotNull(TEXT("Saved offerings exist"), Offerings))
	{
		return false;
	}
	FEmbermereServiceKeyboardFixture F;
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>(F.Character);
	Trainer->SetOfferingsData(Offerings);
	TestTrue(TEXT("Trainer opens"), F.Hud->ShowTrainer(Trainer));
	F.Press(EKeys::Down);
	TestEqual(TEXT("Routed Down inspects locked advanced lesson"), F.Hud->GetSelectedTrainerOfferingIndex(), 1);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Level gate preserves copper"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Level gate preserves XP"), F.Character->Stats->CurrentExperience, 0);
	TestTrue(TEXT("Exact rejection stays readable"), F.Hud->GetTrainerDisplayText().ToString().Contains(
		TEXT("Advanced Combat Drills requires level 2.")));
	F.Press(EKeys::Down);
	TestEqual(TEXT("Down wraps lessons"), F.Hud->GetSelectedTrainerOfferingIndex(), 0);
	F.Event(EKeys::Enter, IE_Pressed);
	TestEqual(TEXT("Enter commits exact cost"), F.Character->Wallet->Copper, 30);
	TestEqual(TEXT("Enter commits exact XP"), F.Character->Stats->CurrentExperience, 25);
	F.Event(EKeys::Enter, IE_Repeat);
	F.Controller->PlayerTick(1.0f / 60.0f);
	TestEqual(TEXT("Repeat cannot charge again"), F.Character->Wallet->Copper, 30);
	TestEqual(TEXT("Repeat cannot grant more XP"), F.Character->Stats->CurrentExperience, 25);
	F.Event(EKeys::Enter, IE_Released);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("New press repeats according to offering data"), F.Character->Wallet->Copper, 20);
	TestEqual(TEXT("Repeatable lesson grants XP once per new press"), F.Character->Stats->CurrentExperience, 50);
	F.Press(EKeys::Up);
	TestEqual(TEXT("Up wraps lessons"), F.Hud->GetSelectedTrainerOfferingIndex(), 1);
	F.Press(EKeys::LeftBracket);
	TestEqual(TEXT("Trainer brackets retain existing selection"), F.Hud->GetSelectedTrainerOfferingIndex(), 0);
	F.Character->Wallet->SetCopperForPrototype(1);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Insufficient copper rejects without spend"), F.Character->Wallet->Copper, 1);
	TestEqual(TEXT("Insufficient copper rejects without XP"), F.Character->Stats->CurrentExperience, 50);
	F.Press(EKeys::Escape);
	TestFalse(TEXT("Escape closes trainer"), F.Hud->IsTrainerPanelVisible());
	TestFalse(TEXT("Trainer close restores cursor-hidden state"), F.Controller->bShowMouseCursor);
	F.Character->Wallet->SetCopperForPrototype(40);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Closed trainer cannot grant XP"), F.Character->Stats->CurrentExperience, 50);
	F.Hud->ShowTrainer(Trainer);
	UEmbermereTrainerOfferingsData* InvalidOfferings = DuplicateObject<UEmbermereTrainerOfferingsData>(Offerings, F.Character);
	InvalidOfferings->Offerings[0].CopperCost = 0;
	Trainer->SetOfferingsData(InvalidOfferings);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Malformed offering preserves copper"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Malformed offering preserves XP"), F.Character->Stats->CurrentExperience, 50);
	Trainer->SetOfferingsData(Offerings);
	F.Press(EKeys::I);
	TestTrue(TEXT("Inventory handoff works"), F.Hud->IsInventoryPanelVisible());
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Inventory cannot trigger stale lesson"), F.Character->Wallet->Copper, 40);
	return !HasAnyErrors();
}

#endif
