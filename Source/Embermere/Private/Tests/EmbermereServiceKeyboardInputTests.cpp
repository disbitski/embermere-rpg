#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Characters/EmbermerePracticeTargetActor.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "InputKeyEventArgs.h"
#include "Framework/Application/SlateApplication.h"
#include "Misc/AutomationTest.h"
#include "UI/EmbermerePlayerHudWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/SWidget.h"
#include "Widgets/SVirtualWindow.h"

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
		Hud->CloseQuestLedgerPanel();
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

	bool Interact()
	{
		return Controller->InteractWithNearestActor();
	}

	void ActivateAbilitySlot(int32 SlotIndex)
	{
		Controller->ActivateHotbarSlot(SlotIndex);
	}

	void ResetEmptyInteractionFeedback()
	{
		Controller->LastEmptyInteractionFeedbackTimeSeconds = -1.0;
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereLootCapacityFeedbackTest,
	"Embermere.Enemy.LootCapacityFeedback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLootCapacityFeedbackTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	F.World->AddController(F.Controller);
	TestTrue(TEXT("Loot messages have a world player controller"), F.World->GetFirstPlayerController() == F.Controller);
	AEmbermereEnemyCharacter* Enemy = F.World->SpawnActor<AEmbermereEnemyCharacter>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	UEmbermereItemData* Filler = NewObject<UEmbermereItemData>();
	if (!TestNotNull(TEXT("Loot feedback enemy exists"), Enemy) || !Tonic || !Filler)
	{
		return false;
	}
	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 5;
	Filler->DisplayName = FText::FromString(TEXT("Filler"));
	Filler->MaxStack = 1;
	Enemy->LootItem = Tonic;
	Enemy->LootQuantity = 1;
	F.Character->Inventory->MaxSlots = 1;
	TestTrue(TEXT("Only bag slot is filled"), F.Character->Inventory->AddItem(Filler, 1, false));
	const int32 ChatBefore = F.Hud->GetChatMessageCount();
	const int32 CopperBefore = F.Character->Wallet->Copper;
	const int32 ExperienceBefore = F.Character->Stats->CurrentExperience;

	TestFalse(TEXT("Full bag rejects automatic loot"), Enemy->GrantLootTo(F.Character));
	TestEqual(TEXT("Rejected loot adds one readable chat line"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	UTextBlock* Feedback = F.Hud->WidgetTree->FindWidget<UTextBlock>(TEXT("ChatMessageText_0"));
	if (TestNotNull(TEXT("Rejected loot feedback renders in chat"), Feedback))
	{
		TestEqual(TEXT("Full-bag feedback names the missed item"), Feedback->GetText().ToString(),
			FString(TEXT("No room for Marsh Tonic x1.")));
	}
	TestEqual(TEXT("Rejected loot leaves the filler untouched"), F.Character->Inventory->GetItemQuantity(Filler), 1);
	TestEqual(TEXT("Rejected loot grants no tonic"), F.Character->Inventory->GetItemQuantity(Tonic), 0);
	TestEqual(TEXT("Rejected loot leaves copper unchanged"), F.Character->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Rejected loot leaves XP unchanged"), F.Character->Stats->CurrentExperience, ExperienceBefore);
	TestEqual(TEXT("Rejected loot changes no quest record"), F.Character->QuestLog->QuestStates.Num(), 0);

	TestTrue(TEXT("Bag slot can be freed"), F.Character->Inventory->RemoveItem(Filler, 1));
	TestTrue(TEXT("Nearly full tonic stack enters the bag"), F.Character->Inventory->AddItem(Tonic, 4, false));
	TestTrue(TEXT("Loot fits into the existing stack"), Enemy->GrantLootTo(F.Character));
	TestEqual(TEXT("Exact stack cap is reached"), F.Character->Inventory->GetItemQuantity(Tonic), 5);
	TestEqual(TEXT("Successful loot retains received and looted feedback"), F.Hud->GetChatMessageCount(), ChatBefore + 3);
	UTextBlock* Looted = F.Hud->WidgetTree->FindWidget<UTextBlock>(TEXT("ChatMessageText_2"));
	if (TestNotNull(TEXT("Successful loot message renders in chat"), Looted))
	{
		TestEqual(TEXT("Successful loot keeps its existing copy"), Looted->GetText().ToString(),
			FString(TEXT("Looted Marsh Tonic x1")));
	}
	TestFalse(TEXT("Full tonic stack rejects another drop"), Enemy->GrantLootTo(F.Character));
	TestEqual(TEXT("Repeated rejection does not overfill the stack"), F.Character->Inventory->GetItemQuantity(Tonic), 5);
	TestEqual(TEXT("Repeated rejection adds one chat line"), F.Hud->GetChatMessageCount(), ChatBefore + 4);
	Enemy->LootQuantity = 0;
	TestFalse(TEXT("Zero-quantity loot stays disabled"), Enemy->GrantLootTo(F.Character));
	Enemy->LootQuantity = 1;
	Enemy->bLootEnabled = false;
	TestFalse(TEXT("Disabled loot stays disabled"), Enemy->GrantLootTo(F.Character));
	TestEqual(TEXT("No-drop paths do not claim a full bag"), F.Hud->GetChatMessageCount(), ChatBefore + 4);
	Enemy->bLootEnabled = true;
	Tonic->MaxStack = 0;
	TestFalse(TEXT("Malformed loot data is rejected"), Enemy->GrantLootTo(F.Character));
	TestEqual(TEXT("Malformed loot does not claim a capacity failure"), F.Hud->GetChatMessageCount(), ChatBefore + 4);
	F.Controller->SetPawn(nullptr);
	F.World->RemoveController(F.Controller);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereEmptyInteractionFeedbackTest,
	"Embermere.Input.EmptyInteractionFeedback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEmptyInteractionFeedbackTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	const int32 ChatBefore = F.Hud->GetChatMessageCount();
	const int32 CopperBefore = F.Character->Wallet->Copper;
	const int32 ExperienceBefore = F.Character->Stats->CurrentExperience;

	TestFalse(TEXT("No nearby owner is not interacted with"), F.Interact());
	TestEqual(TEXT("Empty interaction explains the range failure"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	UTextBlock* FeedbackText = F.Hud->WidgetTree->FindWidget<UTextBlock>(TEXT("ChatMessageText_0"));
	if (TestNotNull(TEXT("Feedback is rendered in chat"), FeedbackText))
	{
		TestEqual(TEXT("Feedback gives a useful next step"), FeedbackText->GetText().ToString(),
			FString(TEXT("No one close enough to interact with.")));
	}

	TestFalse(TEXT("A repeated empty request still has no owner"), F.Interact());
	TestEqual(TEXT("Repeated key presses do not flood chat"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	F.ResetEmptyInteractionFeedback();
	TestFalse(TEXT("An empty request remains rejected after cooldown"), F.Interact());
	TestEqual(TEXT("Feedback can return after cooldown"), F.Hud->GetChatMessageCount(), ChatBefore + 2);
	TestEqual(TEXT("Empty interaction leaves copper unchanged"), F.Character->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Empty interaction leaves XP unchanged"), F.Character->Stats->CurrentExperience, ExperienceBefore);
	TestEqual(TEXT("Empty interaction does not deliver an item"), F.Character->Inventory->Stacks.Num(), 0);
	F.Controller->SetPawn(nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereHotbarRejectionFeedbackTest,
	"Embermere.Input.HotbarRejectionFeedback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereHotbarRejectionFeedbackTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	FEmbermereAbilityDefinition Strike;
	Strike.AbilityId = TEXT("TestStrike");
	Strike.DisplayName = FText::FromString(TEXT("Strike"));
	Strike.TargetKind = EEmbermereAbilityTargetKind::Enemy;
	Strike.EffectType = EEmbermereAbilityEffectType::Damage;
	Strike.Range = 250.0f;
	Strike.ManaCost = 10.0f;
	F.Character->Hotbar->SetAbilityInSlot(0, Strike);
	auto ChatLine = [&F](int32 RowIndex)
	{
		const UTextBlock* Row = F.Hud->WidgetTree->FindWidget<UTextBlock>(
			*FString::Printf(TEXT("ChatMessageText_%d"), RowIndex));
		return Row ? Row->GetText().ToString() : FString();
	};

	const int32 InitialMessages = F.Hud->GetChatMessageCount();
	const int32 InitialQuestRecords = F.Character->QuestLog->QuestStates.Num();
	const float InitialMana = F.Character->Stats->CurrentMana;
	F.ActivateAbilitySlot(0);
	TestEqual(TEXT("Missing target produces one explanation"), F.Hud->GetChatMessageCount(), InitialMessages + 1);
	TestEqual(TEXT("Missing target copy"), ChatLine(0), FString(TEXT("Select a target for Strike.")));
	TestEqual(TEXT("Missing target spends no mana"), F.Character->Stats->CurrentMana, InitialMana);

	AEmbermerePracticeTargetActor* Target = F.World->SpawnActor<AEmbermerePracticeTargetActor>();
	if (!TestNotNull(TEXT("Practice target exists"), Target))
	{
		return false;
	}
	Target->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
	F.Character->Combat->SetTarget(Target);
	const float TargetHealth = Target->Stats->CurrentHealth;
	F.ActivateAbilitySlot(0);
	TestEqual(TEXT("Out-of-range request produces one explanation"), F.Hud->GetChatMessageCount(), InitialMessages + 2);
	TestEqual(TEXT("Out-of-range copy"), ChatLine(1), FString(TEXT("Strike is out of range.")));
	TestEqual(TEXT("Out-of-range request spends no mana"), F.Character->Stats->CurrentMana, InitialMana);
	TestEqual(TEXT("Out-of-range request deals no damage"), Target->Stats->CurrentHealth, TargetHealth);

	Target->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
	F.Character->Stats->CurrentMana = 0.0f;
	F.ActivateAbilitySlot(0);
	TestEqual(TEXT("Low-mana request produces one explanation"), F.Hud->GetChatMessageCount(), InitialMessages + 3);
	TestEqual(TEXT("Low-mana copy"), ChatLine(2), FString(TEXT("Not enough mana for Strike.")));
	TestEqual(TEXT("Low-mana request spends no mana"), F.Character->Stats->CurrentMana, 0.0f);
	TestEqual(TEXT("Low-mana request deals no damage"), Target->Stats->CurrentHealth, TargetHealth);
	TestEqual(TEXT("No rejection changes quest records"), F.Character->QuestLog->QuestStates.Num(), InitialQuestRecords);
	TestEqual(TEXT("No rejection delivers loot"), F.Character->Inventory->Stacks.Num(), 0);

	F.Character->Stats->CurrentMana = InitialMana;
	F.ActivateAbilitySlot(0);
	TestTrue(TEXT("Valid strike still applies damage"), Target->Stats->CurrentHealth < TargetHealth);
	TestEqual(TEXT("Valid strike spends its exact mana cost"), F.Character->Stats->CurrentMana, InitialMana - 10.0f);
	const float HealthAfterStrike = Target->Stats->CurrentHealth;
	const float ManaAfterStrike = F.Character->Stats->CurrentMana;
	F.ActivateAbilitySlot(0);
	TestEqual(TEXT("Cooldown still blocks a second hit"), Target->Stats->CurrentHealth, HealthAfterStrike);
	TestEqual(TEXT("Cooldown still blocks a second mana charge"), F.Character->Stats->CurrentMana, ManaAfterStrike);
	F.Controller->SetPawn(nullptr);
	return true;
}

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

namespace
{
struct FFocusedServiceWindow
{
	TSharedPtr<SWidget> PreviousFocus = FSlateApplication::Get().GetKeyboardFocusedWidget();
	TSharedRef<SVirtualWindow> Window = SNew(SVirtualWindow).Size(FVector2D(1280, 900));

	explicit FFocusedServiceWindow(FEmbermereServiceKeyboardFixture& Fixture)
	{
		Window->SetContent(Fixture.SlateWidget.ToSharedRef());
		Window->SlatePrepass();
		FSlateApplication::Get().RegisterVirtualWindow(Window);
	}

	~FFocusedServiceWindow()
	{
		FSlateApplication::Get().ClearKeyboardFocus(EFocusCause::Cleared);
		FSlateApplication::Get().UnregisterVirtualWindow(Window);
		if (PreviousFocus.IsValid())
		{
			FSlateApplication::Get().SetKeyboardFocus(PreviousFocus);
		}
	}

	static void Key(const FKey& Key, bool bRepeat = false, bool bRelease = true)
	{
		const FKeyEvent Event(Key, FModifierKeysState(), 0, bRepeat, 0, 0);
		FSlateApplication::Get().ProcessKeyDownEvent(Event);
		if (bRelease)
		{
			FSlateApplication::Get().ProcessKeyUpEvent(Event);
		}
	}
};

bool CheckFocusedService(FAutomationTestBase& Test, bool bVendor)
{
	FEmbermereServiceKeyboardFixture F;
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>(F.Character);
	Vendor->SetStockData(LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock")));
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>(F.Character);
	Trainer->SetOfferingsData(LoadObject<UEmbermereTrainerOfferingsData>(nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings")));
	const auto Open = [&]() { return bVendor ? F.Hud->ShowVendor(Vendor) : F.Hud->ShowTrainer(Trainer); };
	const auto Selected = [&]() { return bVendor ? F.Hud->GetSelectedVendorStockIndex() : F.Hud->GetSelectedTrainerOfferingIndex(); };
	const auto Visible = [&]() { return bVendor ? F.Hud->IsVendorPanelVisible() : F.Hud->IsTrainerPanelVisible(); };
	Test.TestTrue(TEXT("Service opens"), Open());
	FFocusedServiceWindow Window(F);
	UButton* Action = Cast<UButton>(F.Hud->GetWidgetFromName(bVendor ? TEXT("VendorBuyButton") : TEXT("TrainerActionButton")));
	UButton* Close = Cast<UButton>(F.Hud->GetWidgetFromName(bVendor ? TEXT("VendorCloseButton") : TEXT("TrainerCloseButton")));
	if (!Test.TestNotNull(TEXT("Native action exists"), Action) || !Test.TestNotNull(TEXT("Native close exists"), Close))
	{
		return false;
	}
	Test.TestTrue(TEXT("Real Slate focus path reaches action"), FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget()));
	Test.TestTrue(TEXT("Action really has focus"), Action->HasKeyboardFocus());
	FFocusedServiceWindow::Key(EKeys::Down);
	Test.TestEqual(TEXT("Focused Down selects unavailable row"), Selected(), 1);
	FFocusedServiceWindow::Key(EKeys::Up);
	Test.TestEqual(TEXT("Focused Up restores first row"), Selected(), 0);
	FFocusedServiceWindow::Key(EKeys::Up);
	Test.TestEqual(TEXT("Focused Up wraps"), Selected(), 1);
	FFocusedServiceWindow::Key(EKeys::Down);
	Test.TestEqual(TEXT("Focused Down wraps"), Selected(), 0);
	FFocusedServiceWindow::Key(EKeys::Down, true);
	Test.TestEqual(TEXT("Repeated navigation is ignored"), Selected(), 0);
	Test.TestEqual(TEXT("Navigation cannot spend"), F.Character->Wallet->Copper, 40);
	Test.TestEqual(TEXT("Navigation cannot grant XP"), F.Character->Stats->CurrentExperience, 0);
	Test.TestEqual(TEXT("Navigation cannot deliver items"), F.Character->Inventory->Stacks.Num(), 0);
	FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget());
	FFocusedServiceWindow::Key(EKeys::Enter, false, false);
	FFocusedServiceWindow::Key(EKeys::Enter, true, false);
	Test.TestEqual(TEXT("Native press waits for release"), F.Character->Wallet->Copper, 40);
	FSlateApplication::Get().ProcessKeyUpEvent(FKeyEvent(EKeys::Enter, FModifierKeysState(), 0, false, 0, 0));
	Test.TestEqual(TEXT("Native Enter commits once"), F.Character->Wallet->Copper, bVendor ? 32 : 30);
	FFocusedServiceWindow::Key(EKeys::SpaceBar);
	Test.TestEqual(TEXT("Native Space remains available"), F.Character->Wallet->Copper, bVendor ? 24 : 20);
	Test.TestEqual(TEXT("Native action exact XP"), F.Character->Stats->CurrentExperience, bVendor ? 0 : 50);
	if (bVendor)
	{
		Test.TestEqual(TEXT("Native actions deliver two tonics"), F.Character->Inventory->GetItemQuantity(Vendor->StockData->Entries[0].Item), 2);
	}
	FSlateApplication::Get().SetKeyboardFocus(Close->TakeWidget());
	FFocusedServiceWindow::Key(EKeys::Down);
	Test.TestEqual(TEXT("Arrows work from close button too"), Selected(), 1);
	FFocusedServiceWindow::Key(EKeys::Enter);
	Test.TestFalse(TEXT("Focused close keeps native Enter meaning"), Visible());
	Test.TestEqual(TEXT("Close never becomes Buy or Train"), F.Character->Wallet->Copper, bVendor ? 24 : 20);
	Open();
	FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget());
	FFocusedServiceWindow::Key(EKeys::Escape);
	Test.TestFalse(TEXT("Focused Escape closes service"), Visible());
	FFocusedServiceWindow::Key(EKeys::Enter);
	Test.TestEqual(TEXT("Closed focus cannot repeat transaction"), F.Character->Wallet->Copper, bVendor ? 24 : 20);
	return !Test.HasAnyErrors();
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereVendorFocusedKeyboardTest,
	"Embermere.UI.ServiceKeyboard.VendorFocus",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereVendorFocusedKeyboardTest::RunTest(const FString& Parameters)
{
	return CheckFocusedService(*this, true);
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereTrainerFocusedKeyboardTest,
	"Embermere.UI.ServiceKeyboard.TrainerFocus",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereTrainerFocusedKeyboardTest::RunTest(const FString& Parameters)
{
	return CheckFocusedService(*this, false);
}

namespace
{
bool AddKeyboardLedgerRecords(FEmbermereServiceKeyboardFixture& F, FAutomationTestBase& Test)
{
	UEmbermereQuestData* Active = NewObject<UEmbermereQuestData>(F.Character);
	Active->QuestId = TEXT("KeyboardActive");
	Active->ObjectiveId = TEXT("KeyboardActiveObjective");
	Active->Title = FText::FromString(TEXT("Active keyboard quest"));
	Active->RequiredObjectiveCount = 3;
	UEmbermereQuestData* Completed = NewObject<UEmbermereQuestData>(F.Character);
	Completed->QuestId = TEXT("KeyboardCompleted");
	Completed->ObjectiveId = TEXT("KeyboardCompletedObjective");
	Completed->Title = FText::FromString(TEXT("Completed keyboard quest"));
	Completed->RequiredObjectiveCount = 1;
	Completed->RewardExperience = 50;
	Completed->RewardCopper = 10;
	Test.TestTrue(TEXT("Accept active keyboard fixture"), F.Character->QuestLog->AcceptQuest(Active));
	Test.TestTrue(TEXT("Accept completed keyboard fixture"), F.Character->QuestLog->AcceptQuest(Completed));
	Test.TestTrue(TEXT("Active fixture advances once"), F.Character->QuestLog->AddObjectiveProgressForQuest(
		Active->QuestId, Active->ObjectiveId, 1));
	Test.TestTrue(TEXT("Completed fixture reaches requirement"), F.Character->QuestLog->AddObjectiveProgressForQuest(
		Completed->QuestId, Completed->ObjectiveId, 1));
	Test.TestTrue(TEXT("Fixture commits reward before input"), F.Character->QuestLog->TryCompleteQuest(Completed));
	return !Test.HasAnyErrors();
}

void CheckKeyboardLedgerOwners(FEmbermereServiceKeyboardFixture& F, FAutomationTestBase& Test)
{
	Test.TestEqual(TEXT("Keyboard never duplicates rewards or spends copper"), F.Character->Wallet->Copper, 50);
	Test.TestEqual(TEXT("Keyboard never duplicates XP"), F.Character->Stats->CurrentExperience, 50);
	Test.TestEqual(TEXT("Keyboard never delivers items"), F.Character->Inventory->Stacks.Num(), 0);
	Test.TestEqual(TEXT("Exactly two quest records remain"), F.Character->QuestLog->QuestStates.Num(), 2);
	FEmbermereQuestState State;
	Test.TestTrue(TEXT("Active record survives"), F.Character->QuestLog->GetQuestStateById(TEXT("KeyboardActive"), State));
	Test.TestEqual(TEXT("Active progress unchanged"), State.CurrentObjectiveCount, 1);
	Test.TestFalse(TEXT("Active record cannot be completed by keyboard presentation"), State.bCompleted);
	Test.TestTrue(TEXT("Completed history survives"), F.Character->QuestLog->GetQuestStateById(TEXT("KeyboardCompleted"), State));
	Test.TestEqual(TEXT("Completed progress unchanged"), State.CurrentObjectiveCount, 1);
	Test.TestTrue(TEXT("Completed flag survives"), State.bCompleted);
	Test.TestEqual(TEXT("Ledger geometry stays fixed"), F.Hud->GetQuestLedgerPanelDimensions(), FVector2D(620, 550));
	Test.TestEqual(TEXT("Ledger row geometry stays fixed"), F.Hud->GetQuestLedgerRowDimensions(), FVector2D(596, 30));
	Test.TestEqual(TEXT("Ledger detail geometry stays fixed"), F.Hud->GetQuestLedgerDetailDimensions(), FVector2D(596, 120));
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereLedgerControllerKeyboardTest,
	"Embermere.UI.QuestLedgerKeyboard.Controller",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereLedgerControllerKeyboardTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Press(EKeys::J);
	TestTrue(TEXT("J opens empty Ledger"), F.Hud->IsQuestLedgerPanelVisible());
	F.Press(EKeys::Down);
	F.Press(EKeys::Up);
	F.Press(EKeys::Enter);
	TestTrue(TEXT("Empty keyboard input cannot invent focus"), F.Character->QuestLog->FocusedQuestId.IsNone());
	F.Press(EKeys::Escape);
	TestFalse(TEXT("Empty Ledger closes"), F.Hud->IsQuestLedgerPanelVisible());
	if (!AddKeyboardLedgerRecords(F, *this))
	{
		return false;
	}
	F.Press(EKeys::J);
	TestEqual(TEXT("Ledger aligns to completed tracked record"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	F.Press(EKeys::Down);
	TestEqual(TEXT("Controller Down wraps to active record"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	TestEqual(TEXT("Selection cannot change tracker"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	F.Event(EKeys::Down, IE_Repeat);
	TestEqual(TEXT("Controller repeat does not navigate"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	F.Event(EKeys::Down, IE_Released);
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Controller Enter explicitly focuses active record"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardActive")));
	F.Press(EKeys::Up);
	TestEqual(TEXT("Controller Up wraps to completed record"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	F.Queue(EKeys::Enter, IE_Pressed);
	F.Event(EKeys::Escape, IE_Pressed);
	F.Event(EKeys::Enter, IE_Released);
	F.Event(EKeys::Escape, IE_Released);
	TestFalse(TEXT("Close wins over same-frame focus request"), F.Hud->IsQuestLedgerPanelVisible());
	TestEqual(TEXT("Closing cannot focus selected completed record"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardActive")));
	TestFalse(TEXT("Close restores cursor-hidden input"), F.Controller->bShowMouseCursor);
	F.Press(EKeys::J);
	F.Press(EKeys::Down);
	F.Queue(EKeys::Enter, IE_Pressed);
	F.Event(EKeys::I, IE_Pressed);
	F.Event(EKeys::Enter, IE_Released);
	F.Event(EKeys::I, IE_Released);
	TestTrue(TEXT("Inventory takes over"), F.Hud->IsInventoryPanelVisible());
	TestFalse(TEXT("Inventory closes Ledger before dispatch"), F.Hud->IsQuestLedgerPanelVisible());
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Peer panel cannot focus stale selection"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardActive")));
	CheckKeyboardLedgerOwners(F, *this);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereLedgerFocusedKeyboardTest,
	"Embermere.UI.QuestLedgerKeyboard.FocusedButtons",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereLedgerFocusedKeyboardTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	if (!AddKeyboardLedgerRecords(F, *this))
	{
		return false;
	}
	TestTrue(TEXT("Ledger opens"), F.Hud->ToggleQuestLedgerPanel());
	FFocusedServiceWindow Window(F);
	UButton* Action = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("QuestLedgerFocusButton")));
	UButton* Close = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("QuestLedgerCloseButton")));
	if (!TestNotNull(TEXT("Native focus action exists"), Action) || !TestNotNull(TEXT("Native close exists"), Close))
	{
		return false;
	}
	FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget());
	TestTrue(TEXT("Action really has native focus"), Action->HasKeyboardFocus());
	FFocusedServiceWindow::Key(EKeys::Down);
	TestEqual(TEXT("Focused Ledger Down wraps to active record"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	TestTrue(TEXT("Selected detail follows arrows"), F.Hud->GetQuestLedgerSelectedDetailDisplayText().ToString().Contains(TEXT("Active keyboard quest")));
	TestEqual(TEXT("Focused arrows cannot change tracker"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	FFocusedServiceWindow::Key(EKeys::Down, true);
	TestEqual(TEXT("Focused repeated Down is ignored"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	FFocusedServiceWindow::Key(EKeys::Up);
	TestEqual(TEXT("Focused Ledger Up wraps"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	FFocusedServiceWindow::Key(EKeys::Up);
	TestEqual(TEXT("Focused Ledger Up selects active record"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	FFocusedServiceWindow::Key(EKeys::Down);
	TestEqual(TEXT("Focused Ledger Down selects history"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	FSlateApplication::Get().SetKeyboardFocus(Close->TakeWidget());
	TestTrue(TEXT("Close really has native focus"), Close->HasKeyboardFocus());
	FFocusedServiceWindow::Key(EKeys::Down);
	TestEqual(TEXT("Arrows work from Ledger Close"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	FFocusedServiceWindow::Key(EKeys::Enter);
	TestFalse(TEXT("Close Enter closes instead of focusing quest"), F.Hud->IsQuestLedgerPanelVisible());
	TestEqual(TEXT("Close Enter leaves tracker unchanged"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	F.Hud->ToggleQuestLedgerPanel();
	FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget());
	FFocusedServiceWindow::Key(EKeys::Down);
	FFocusedServiceWindow::Key(EKeys::Enter, false, false);
	FFocusedServiceWindow::Key(EKeys::Enter, true, false);
	TestEqual(TEXT("Native focus waits for release"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	FSlateApplication::Get().ProcessKeyUpEvent(FKeyEvent(EKeys::Enter, FModifierKeysState(), 0, false, 0, 0));
	TestEqual(TEXT("Native Enter focuses selected active record"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardActive")));
	FFocusedServiceWindow::Key(EKeys::SpaceBar);
	TestEqual(TEXT("Duplicate native focus is idempotent"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardActive")));
	FFocusedServiceWindow::Key(EKeys::Down);
	FFocusedServiceWindow::Key(EKeys::SpaceBar);
	TestEqual(TEXT("Native Space can focus completed history"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	FFocusedServiceWindow::Key(EKeys::Escape);
	TestFalse(TEXT("Focused Ledger Escape closes"), F.Hud->IsQuestLedgerPanelVisible());
	TestFalse(TEXT("Focused close restores input"), F.Controller->bShowMouseCursor);
	FFocusedServiceWindow::Key(EKeys::Enter);
	TestEqual(TEXT("Closed Ledger cannot focus stale row"), F.Character->QuestLog->FocusedQuestId, FName(TEXT("KeyboardCompleted")));
	CheckKeyboardLedgerOwners(F, *this);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereLedgerSavedQuestKeyboardTest,
	"Embermere.UI.QuestLedgerKeyboard.SavedQuests",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereLedgerSavedQuestKeyboardTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* Mara = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* StillWaters = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters"));
	if (!TestNotNull(TEXT("Saved Mara quest resolves"), Mara) ||
		!TestNotNull(TEXT("Saved Still Waters quest resolves"), StillWaters))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	TestTrue(TEXT("Fixture accepts saved Mara quest"), F.Character->QuestLog->AcceptQuest(Mara));
	TestTrue(TEXT("Fixture accepts saved Still Waters quest"), F.Character->QuestLog->AcceptQuest(StillWaters));
	TestTrue(TEXT("Saved Still Waters reaches its exact objective"),
		F.Character->QuestLog->AddObjectiveProgressForQuest(
			StillWaters->QuestId, StillWaters->ObjectiveId, 1));
	TestTrue(TEXT("Fixture completes Still Waters before keyboard inspection"),
		F.Character->QuestLog->TryCompleteQuest(StillWaters));
	const int32 Copper = F.Character->Wallet->Copper;
	const int32 Experience = F.Character->Stats->CurrentExperience;
	const int32 InventoryStacks = F.Character->Inventory->Stacks.Num();
	TestEqual(TEXT("Only the fixture reward changed copper"), Copper, 50);
	TestEqual(TEXT("Only the fixture reward changed XP"), Experience, 50);
	TestTrue(TEXT("Saved-quest Ledger opens"), F.Hud->ToggleQuestLedgerPanel());
	TestEqual(TEXT("Completed saved quest begins selected"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	TestEqual(TEXT("Both saved records are visible"), F.Hud->GetQuestLedgerVisibleRecordCount(), 2);

	FFocusedServiceWindow Window(F);
	UButton* Action = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("QuestLedgerFocusButton")));
	UButton* Close = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("QuestLedgerCloseButton")));
	if (!TestNotNull(TEXT("Saved-quest focus button exists"), Action) ||
		!TestNotNull(TEXT("Saved-quest close button exists"), Close))
	{
		return false;
	}
	TestTrue(TEXT("Native Slate focuses the action"), FSlateApplication::Get().SetKeyboardFocus(Action->TakeWidget()));
	FFocusedServiceWindow::Key(EKeys::Down);
	TestEqual(TEXT("Down wraps to saved Mara"), F.Hud->GetSelectedQuestLedgerIndex(), 0);
	TestTrue(TEXT("Selected detail uses Mara's saved instruction"),
		F.Hud->GetQuestLedgerSelectedDetailDisplayText().ToString().Contains(
			Mara->ObjectiveInstructions.ToString()));
	TestEqual(TEXT("Selection leaves tracker on completed Still Waters"),
		F.Character->QuestLog->FocusedQuestId, StillWaters->QuestId);
	FFocusedServiceWindow::Key(EKeys::Enter);
	TestEqual(TEXT("Native Enter focuses saved Mara"), F.Character->QuestLog->FocusedQuestId, Mara->QuestId);
	FFocusedServiceWindow::Key(EKeys::Up);
	TestEqual(TEXT("Up wraps back to saved Still Waters"), F.Hud->GetSelectedQuestLedgerIndex(), 1);
	TestTrue(TEXT("Selected detail uses Still Waters' saved instruction"),
		F.Hud->GetQuestLedgerSelectedDetailDisplayText().ToString().Contains(
			StillWaters->ObjectiveInstructions.ToString()));
	TestEqual(TEXT("Selection alone leaves tracker on Mara"), F.Character->QuestLog->FocusedQuestId, Mara->QuestId);
	FFocusedServiceWindow::Key(EKeys::SpaceBar);
	TestEqual(TEXT("Native Space focuses saved Still Waters"),
		F.Character->QuestLog->FocusedQuestId, StillWaters->QuestId);
	FSlateApplication::Get().SetKeyboardFocus(Close->TakeWidget());
	FFocusedServiceWindow::Key(EKeys::Enter);
	TestFalse(TEXT("Native close ends the saved-quest Ledger"), F.Hud->IsQuestLedgerPanelVisible());
	FFocusedServiceWindow::Key(EKeys::Enter);
	TestEqual(TEXT("Closed button cannot refocus a stale row"),
		F.Character->QuestLog->FocusedQuestId, StillWaters->QuestId);
	TestEqual(TEXT("Keyboard inspection cannot spend copper"), F.Character->Wallet->Copper, Copper);
	TestEqual(TEXT("Keyboard inspection cannot grant XP"), F.Character->Stats->CurrentExperience, Experience);
	TestEqual(TEXT("Keyboard inspection cannot deliver items"), F.Character->Inventory->Stacks.Num(), InventoryStacks);
	FEmbermereQuestState MaraState;
	FEmbermereQuestState StillWatersState;
	TestTrue(TEXT("Mara record remains present"), F.Character->QuestLog->GetQuestStateById(Mara->QuestId, MaraState));
	TestEqual(TEXT("Mara objective stays at zero"), MaraState.CurrentObjectiveCount, 0);
	TestFalse(TEXT("Mara completion cannot be triggered by Ledger keys"), MaraState.bCompleted);
	TestTrue(TEXT("Still Waters record remains present"),
		F.Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState));
	TestEqual(TEXT("Still Waters objective remains complete"), StillWatersState.CurrentObjectiveCount, 1);
	TestTrue(TEXT("Still Waters reward state remains complete"), StillWatersState.bCompleted);
	return !HasAnyErrors();
}

#endif
