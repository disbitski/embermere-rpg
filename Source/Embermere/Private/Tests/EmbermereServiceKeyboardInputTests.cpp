#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Characters/EmbermerePracticeTargetActor.h"
#include "Characters/EmbermereTrainerServiceActor.h"
#include "Characters/EmbermereVendorServiceActor.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTargetingComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/BoxComponent.h"
#include "Components/Button.h"
#include "Components/SceneComponent.h"
#include "Components/TextBlock.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Data/EmbermereRulesData.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/PlayerInput.h"
#include "GameFramework/WorldSettings.h"
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
		if (World->HasBegunPlay())
		{
			World->EndPlay(EEndPlayReason::Quit);
		}
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

	void CycleTarget()
	{
		Controller->CycleTarget();
	}

	void ResetEmptyInteractionFeedback()
	{
		Controller->LastEmptyInteractionFeedbackTimeSeconds = -1.0;
	}

	void HandleDeath()
	{
		Controller->HandleControlledCharacterDied();
	}

	void Respawn()
	{
		Controller->RespawnControlledCharacter();
	}

	void RefreshInputMode()
	{
		Controller->RefreshInteractiveInputMode();
	}
};

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereQuestRewardBagFeedbackTest,
	"Embermere.Quests.RewardBagFeedback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestRewardBagFeedbackTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* Mara = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	if (!TestNotNull(TEXT("Saved Mara quest exists"), Mara) ||
		!TestFalse(TEXT("Mara has an item reward"), Mara->RewardItem.IsNull()))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	AActor* Service = F.World->SpawnActor<AActor>();
	UEmbermereInteractableComponent* Interaction = NewObject<UEmbermereInteractableComponent>(Service);
	Interaction->bShowWorldMarker = false;
	Interaction->DisplayName = FText::FromString(TEXT("Mara Fenwatch"));
	Interaction->QuestToOffer = Mara;
	Interaction->bUseQuestStateDialogue = true;
	Interaction->RegisterComponent();
	UEmbermereQuestLogComponent* Log = F.Character->QuestLog;
	UEmbermereInventoryComponent* Bag = F.Character->Inventory;
	TestTrue(TEXT("Quest is accepted"), Log->AcceptQuest(Mara));
	TestTrue(TEXT("Three committed objectives make Mara ready"),
		Log->AddObjectiveProgressForQuest(Mara->QuestId, Mara->ObjectiveId, 3));
	UEmbermereItemData* Filler = NewObject<UEmbermereItemData>();
	Filler->ItemId = TEXT("TurnInFiller");
	Filler->MaxStack = 1;
	Bag->MaxSlots = 1;
	TestTrue(TEXT("Only bag slot is occupied"), Bag->AddItem(Filler, 1, false));
	const int32 ChatBefore = F.Hud->GetChatMessageCount();
	const int32 CopperBefore = F.Character->Wallet->Copper;
	const int32 XpBefore = F.Character->Stats->CurrentExperience;
	Interaction->Interact(F.Character);
	FEmbermereQuestState State;
	TestTrue(TEXT("Rejected turn-in remains ready and incomplete"),
		Log->GetQuestStateById(Mara->QuestId, State) &&
		State.CurrentObjectiveCount == Mara->RequiredObjectiveCount && !State.bCompleted);
	TestEqual(TEXT("Rejected turn-in preserves copper"), F.Character->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Rejected turn-in preserves XP"), F.Character->Stats->CurrentExperience, XpBefore);
	TestEqual(TEXT("Rejected turn-in grants no reward item"), Bag->Stacks.Num(), 1);
	TestEqual(TEXT("Ready greeting is followed by one useful bag reason"),
		F.Hud->GetChatMessageCount(), ChatBefore + 2);
	UTextBlock* Failure = F.Hud->WidgetTree->FindWidget<UTextBlock>(
		*FString::Printf(TEXT("ChatMessageText_%d"), ChatBefore + 1));
	if (TestNotNull(TEXT("Quest rejection is visible in chat"), Failure))
	{
		TestEqual(TEXT("Bag rejection copy is exact"), Failure->GetText().ToString(),
			FString(TEXT("Make room in your bags for the quest reward.")));
	}

	TestTrue(TEXT("Filler can be removed"), Bag->RemoveItem(Filler, 1));
	Interaction->Interact(F.Character);
	TestTrue(TEXT("Retry completes Mara exactly once"),
		Log->GetQuestStateById(Mara->QuestId, State) && State.bCompleted);
	TestEqual(TEXT("Retry grants exact copper"), F.Character->Wallet->Copper,
		CopperBefore + Mara->RewardCopper);
	TestEqual(TEXT("Retry grants exact XP"), F.Character->Stats->CurrentExperience,
		XpBefore + Mara->RewardExperience);
	TestEqual(TEXT("Retry grants one item"), Bag->Stacks.Num(), 1);
	F.Controller->SetPawn(nullptr);
	F.World->RemoveController(F.Controller);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereVendorRangeTransactionsTest,
	"Embermere.Vendor.RangeTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorRangeTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	if (!TestNotNull(TEXT("Saved vendor stock exists"), Stock) ||
		!TestTrue(TEXT("Tonic stock row exists"), Stock && Stock->Entries.IsValidIndex(0)))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector::ZeroVector);
	AEmbermereVendorServiceActor* Service = F.World->SpawnActor<AEmbermereVendorServiceActor>();
	if (!TestNotNull(TEXT("Art-free vendor service exists"), Service) ||
		!TestNotNull(TEXT("Vendor transaction owner exists"), Service ? Service->Vendor.Get() : nullptr))
	{
		return false;
	}
	Service->SetActorLocation(FVector(300.0f, 0.0f, 0.0f));
	Service->Vendor->SetStockData(Stock);
	UEmbermereVendorComponent* Vendor = Service->Vendor;
	UEmbermereInventoryComponent* Inventory = F.Character->Inventory;
	UEmbermereWalletComponent* Wallet = F.Character->Wallet;
	UEmbermereItemData* Tonic = Stock->Entries[0].Item;
	TestTrue(TEXT("Pawn is controlled for the range fixture"), F.Character->GetController() == F.Controller);
	TestEqual(TEXT("Nearby purchase remains eligible"),
		Vendor->CanPurchase(0, 1, Inventory, Wallet), EEmbermereVendorPurchaseResult::Success);
	TestEqual(TEXT("Nearby purchase succeeds"),
		Vendor->TryPurchase(0, 1, Inventory, Wallet), EEmbermereVendorPurchaseResult::Success);
	TestEqual(TEXT("Nearby purchase costs exactly 8 copper"), Wallet->Copper, 32);
	TestEqual(TEXT("Nearby purchase delivers one tonic"), Inventory->GetItemQuantity(Tonic), 1);

	F.Character->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
	TestEqual(TEXT("Distant purchase preflight rejects for range"),
		Vendor->CanPurchase(0, 1, Inventory, Wallet), EEmbermereVendorPurchaseResult::OutOfRange);
	const EEmbermereVendorPurchaseResult DistantPurchase = Vendor->TryPurchase(0, 1, Inventory, Wallet);
	TestEqual(TEXT("Distant purchase transaction rejects for range"),
		DistantPurchase, EEmbermereVendorPurchaseResult::OutOfRange);
	TestEqual(TEXT("Distant purchase gives a useful reason"),
		Vendor->GetPurchaseResultText(DistantPurchase, 0, 1).ToString(), FString(TEXT("Move closer to the vendor.")));
	TestEqual(TEXT("Distant purchase preserves copper"), Wallet->Copper, 32);
	TestEqual(TEXT("Distant purchase preserves stock delivery"), Inventory->GetItemQuantity(Tonic), 1);
	const EEmbermereVendorSellResult DistantSale = Vendor->TrySell(Tonic, 1, Inventory, Wallet);
	TestEqual(TEXT("Distant sale rejects for range"), DistantSale, EEmbermereVendorSellResult::OutOfRange);
	TestEqual(TEXT("Distant sale gives the same useful reason"),
		Vendor->GetSellResultText(DistantSale, Tonic, 1).ToString(), FString(TEXT("Move closer to the vendor.")));
	TestEqual(TEXT("Distant sale preserves copper"), Wallet->Copper, 32);
	TestEqual(TEXT("Distant sale preserves tonic"), Inventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Distant sale creates no buyback"), Vendor->GetBuybackEntryCount(), 0);

	F.Character->SetActorLocation(FVector::ZeroVector);
	TestEqual(TEXT("Nearby sale still succeeds"),
		Vendor->TrySell(Tonic, 1, Inventory, Wallet), EEmbermereVendorSellResult::Success);
	TestEqual(TEXT("Nearby sale credits exact value"), Wallet->Copper, 35);
	TestEqual(TEXT("Nearby sale records buyback"), Vendor->GetBuybackEntryCount(), 1);
	F.Character->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
	const EEmbermereVendorBuybackResult DistantBuyback = Vendor->TryBuyback(0, 1, Inventory, Wallet);
	TestEqual(TEXT("Distant buyback rejects for range"), DistantBuyback, EEmbermereVendorBuybackResult::OutOfRange);
	TestEqual(TEXT("Distant buyback gives the same useful reason"),
		Vendor->GetBuybackResultText(DistantBuyback, Tonic, 3, 1).ToString(),
		FString(TEXT("Move closer to the vendor.")));
	TestEqual(TEXT("Distant buyback preserves copper"), Wallet->Copper, 35);
	TestEqual(TEXT("Distant buyback preserves entry"), Vendor->GetBuybackEntryCount(), 1);
	TestEqual(TEXT("Distant buyback delivers no tonic"), Inventory->GetItemQuantity(Tonic), 0);
	F.Character->SetActorLocation(FVector::ZeroVector);
	TestEqual(TEXT("Returning nearby permits buyback"),
		Vendor->TryBuyback(0, 1, Inventory, Wallet), EEmbermereVendorBuybackResult::Success);
	TestEqual(TEXT("Buyback debits exact recorded price"), Wallet->Copper, 32);
	TestEqual(TEXT("Buyback returns one tonic"), Inventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Range transactions cannot grant XP"), F.Character->Stats->CurrentExperience, 0);
	TestEqual(TEXT("Range transactions cannot change quests"), F.Character->QuestLog->QuestStates.Num(), 0);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereTrainerRangeTransactionsTest,
	"Embermere.Trainer.RangeTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerRangeTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	if (!TestNotNull(TEXT("Saved trainer offerings exist"), Offerings))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector::ZeroVector);
	AEmbermereTrainerServiceActor* Service = F.World->SpawnActor<AEmbermereTrainerServiceActor>();
	if (!TestNotNull(TEXT("Art-free trainer service exists"), Service) ||
		!TestNotNull(TEXT("Trainer transaction owner exists"), Service ? Service->Trainer.Get() : nullptr))
	{
		return false;
	}
	Service->SetActorLocation(FVector(300.0f, 0.0f, 0.0f));
	Service->Trainer->SetOfferingsData(Offerings);
	UEmbermereTrainerComponent* Trainer = Service->Trainer;
	UEmbermereStatsComponent* Stats = F.Character->Stats;
	UEmbermereWalletComponent* Wallet = F.Character->Wallet;
	TestTrue(TEXT("Nearby trainer panel opens"), F.Hud->ShowTrainer(Trainer));
	TestEqual(TEXT("Nearby lesson remains eligible"),
		Trainer->CanTrain(0, Stats, Wallet), EEmbermereTrainingResult::Success);
	TestTrue(TEXT("Nearby lesson succeeds through the HUD"), F.Hud->TrainSelectedOffering());
	TestEqual(TEXT("Nearby lesson charges exact copper"), Wallet->Copper, 30);
	TestEqual(TEXT("Nearby lesson grants exact XP"), Stats->CurrentExperience, 25);

	F.Character->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
	TestTrue(TEXT("Trainer panel remains open after moving away"), F.Hud->IsTrainerPanelVisible());
	TestEqual(TEXT("Distant lesson preflight rejects for range"),
		Trainer->CanTrain(0, Stats, Wallet), EEmbermereTrainingResult::OutOfRange);
	TestFalse(TEXT("Distant HUD action rejects"), F.Hud->TrainSelectedOffering());
	TestEqual(TEXT("Distant lesson reports a useful reason"),
		Trainer->GetTrainingResultText(EEmbermereTrainingResult::OutOfRange, 0).ToString(),
		FString(TEXT("Move closer to the trainer.")));
	TestEqual(TEXT("Distant lesson preserves copper"), Wallet->Copper, 30);
	TestEqual(TEXT("Distant lesson preserves XP"), Stats->CurrentExperience, 25);
	TestEqual(TEXT("Distant lesson preserves quest records"), F.Character->QuestLog->QuestStates.Num(), 0);
	TestEqual(TEXT("Distant lesson preserves inventory"), F.Character->Inventory->Stacks.Num(), 0);

	F.Character->SetActorLocation(FVector::ZeroVector);
	AEmbermereCharacter* OtherCharacter = F.World->SpawnActor<AEmbermereCharacter>();
	OtherCharacter->Wallet->SetCopperForPrototype(40);
	TestEqual(TEXT("Another character's wallet cannot pay for this customer"),
		Trainer->TryTrain(0, Stats, OtherCharacter->Wallet), EEmbermereTrainingResult::OutOfRange);
	TestEqual(TEXT("Mismatched wallet remains unchanged"), OtherCharacter->Wallet->Copper, 40);
	TestEqual(TEXT("Mismatched wallet cannot grant XP"), Stats->CurrentExperience, 25);
	F.Controller->UnPossess();
	TestEqual(TEXT("Unpossessed character cannot train"),
		Trainer->TryTrain(0, Stats, Wallet), EEmbermereTrainingResult::OutOfRange);
	F.Controller->Possess(F.Character);
	TestTrue(TEXT("Returning nearby permits the same HUD action"), F.Hud->TrainSelectedOffering());
	TestEqual(TEXT("Second nearby lesson charges exact copper"), Wallet->Copper, 20);
	TestEqual(TEXT("Second nearby lesson grants exact XP"), Stats->CurrentExperience, 50);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereDeathPanelHandoffTest,
	"Embermere.Input.DeathPanelHandoff",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereDeathPanelHandoffTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	if (!TestNotNull(TEXT("Saved vendor stock exists"), Stock) ||
		!TestNotNull(TEXT("Saved trainer offerings exist"), Offerings))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	F.World->AddController(F.Controller);
	F.Character->Stats->InitializeVitals();
	F.RefreshInputMode();
	TestTrue(TEXT("Inventory starts visible"), F.Hud->IsInventoryPanelVisible());
	TestTrue(TEXT("Open inventory owns the cursor"), F.Controller->bShowMouseCursor);

	const int32 Copper = F.Character->Wallet->Copper;
	const int32 Experience = F.Character->Stats->CurrentExperience;
	const int32 QuestRecords = F.Character->QuestLog->QuestStates.Num();
	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	TestFalse(TEXT("Death closes inventory"), F.Hud->IsInventoryPanelVisible());
	TestFalse(TEXT("Death releases the cursor"), F.Controller->bShowMouseCursor);
	TestFalse(TEXT("Death releases UI click routing"), F.Controller->bEnableClickEvents);

	F.Hud->ToggleInventoryPanel();
	F.RefreshInputMode();
	TestTrue(TEXT("A panel opened during the death hold is visible"), F.Hud->IsInventoryPanelVisible());
	F.Respawn();
	TestFalse(TEXT("Respawn closes a death-hold panel"), F.Hud->IsInventoryPanelVisible());
	TestFalse(TEXT("Respawn restores cursor-hidden play"), F.Controller->bShowMouseCursor);

	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>(F.Character);
	Vendor->SetStockData(Stock);
	TestTrue(TEXT("Vendor opens after recovery"), F.Hud->ShowVendor(Vendor));
	F.RefreshInputMode();
	TestTrue(TEXT("Vendor owns UI input"), F.Controller->bShowMouseCursor);
	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	TestFalse(TEXT("Death closes the remote vendor panel"), F.Hud->IsVendorPanelVisible());
	TestFalse(TEXT("Death releases vendor cursor capture"), F.Controller->bShowMouseCursor);
	F.Respawn();
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Stale vendor action cannot spend copper"), F.Character->Wallet->Copper, Copper);

	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>(F.Character);
	Trainer->SetOfferingsData(Offerings);
	TestTrue(TEXT("Trainer opens after recovery"), F.Hud->ShowTrainer(Trainer));
	F.RefreshInputMode();
	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	TestFalse(TEXT("Death closes the remote trainer panel"), F.Hud->IsTrainerPanelVisible());
	F.Respawn();
	F.Press(EKeys::Enter);
	TestEqual(TEXT("Stale trainer action cannot spend copper"), F.Character->Wallet->Copper, Copper);

	TestTrue(TEXT("Ledger opens after recovery"), F.Hud->ToggleQuestLedgerPanel());
	F.RefreshInputMode();
	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	TestFalse(TEXT("Death closes the Ledger without changing quest state"), F.Hud->IsQuestLedgerPanelVisible());
	F.Respawn();
	TestEqual(TEXT("Death and recovery cannot grant XP"), F.Character->Stats->CurrentExperience, Experience);
	TestEqual(TEXT("Death and recovery cannot deliver inventory"), F.Character->Inventory->Stacks.Num(), 0);
	TestEqual(TEXT("Death and recovery cannot change quests"), F.Character->QuestLog->QuestStates.Num(), QuestRecords);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereTargetCycleNoOpFeedbackTest,
	"Embermere.Input.TargetCycleNoOpFeedback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTargetCycleNoOpFeedbackTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector::ZeroVector);
	F.Character->SetActorRotation(FRotator::ZeroRotator);
	AEmbermerePracticeTargetActor* First = F.World->SpawnActor<AEmbermerePracticeTargetActor>();
	if (!TestNotNull(TEXT("Practice target exists"), First) ||
		!TestNotNull(TEXT("Targeting component exists"), F.Character->Targeting.Get()))
	{
		return false;
	}
	First->SetActorLocation(FVector(250.0f, 0.0f, 0.0f));
	const int32 ChatBefore = F.Hud->GetChatMessageCount();
	F.CycleTarget();
	TestTrue(TEXT("First Tab selects the practice target"), F.Character->Combat->CurrentTarget == First);
	TestEqual(TEXT("First selection is announced"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	F.CycleTarget();
	TestTrue(TEXT("Single candidate stays selected"), F.Character->Combat->CurrentTarget == First);
	TestEqual(TEXT("Unchanged selection does not bury chat"), F.Hud->GetChatMessageCount(), ChatBefore + 1);

	AEmbermereEnemyCharacter* Second = F.World->SpawnActor<AEmbermereEnemyCharacter>();
	if (!TestNotNull(TEXT("Second target exists"), Second))
	{
		return false;
	}
	Second->SetActorLocation(FVector(400.0f, 80.0f, 0.0f));
	F.CycleTarget();
	TestTrue(TEXT("Tab still advances to another target"), F.Character->Combat->CurrentTarget == Second);
	TestEqual(TEXT("Changed selection is announced"), F.Hud->GetChatMessageCount(), ChatBefore + 2);
	F.CycleTarget();
	TestTrue(TEXT("Tab wraps to first target"), F.Character->Combat->CurrentTarget == First);
	TestEqual(TEXT("Wrapped selection is announced"), F.Hud->GetChatMessageCount(), ChatBefore + 3);

	F.Character->SetActorRotation(FRotator(0.0f, 180.0f, 0.0f));
	F.CycleTarget();
	TestNull(TEXT("Empty cone clears the target"), F.Character->Combat->CurrentTarget.Get());
	TestEqual(TEXT("Empty cone retains its feedback"), F.Hud->GetChatMessageCount(), ChatBefore + 4);
	F.Controller->SetPawn(nullptr);
	F.World->RemoveController(F.Controller);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereDeadTargetCycleSafetyTest,
	"Embermere.Input.DeadTargetCycleSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereDeadTargetCycleSafetyTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector::ZeroVector);
	F.Character->SetActorRotation(FRotator::ZeroRotator);
	AEmbermerePracticeTargetActor* Target = F.World->SpawnActor<AEmbermerePracticeTargetActor>();
	if (!TestNotNull(TEXT("Practice target exists"), Target))
	{
		return false;
	}
	Target->SetActorLocation(FVector(250.0f, 0.0f, 0.0f));
	F.CycleTarget();
	TestTrue(TEXT("Living Tab selects the target"), F.Character->Combat->CurrentTarget == Target);

	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	TestNull(TEXT("Death clears the selected target"), F.Character->Combat->CurrentTarget.Get());
	const int32 DeathChatCount = F.Hud->GetChatMessageCount();
	F.CycleTarget();
	TestNull(TEXT("Tab cannot reacquire a target during death hold"), F.Character->Combat->CurrentTarget.Get());
	TestEqual(TEXT("Dead Tab adds no target announcement"), F.Hud->GetChatMessageCount(), DeathChatCount);

	F.Respawn();
	F.CycleTarget();
	TestTrue(TEXT("Recovered Tab selects the target again"), F.Character->Combat->CurrentTarget == Target);
	return !HasAnyErrors();
}

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereDeadInteractionSafetyTest,
	"Embermere.Input.DeadInteractionSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereDeadInteractionSafetyTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	if (!TestNotNull(TEXT("Saved vendor stock exists"), Stock))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector::ZeroVector);
	AEmbermereVendorServiceActor* Service = F.World->SpawnActor<AEmbermereVendorServiceActor>();
	if (!TestNotNull(TEXT("Nearby art-free service exists"), Service))
	{
		return false;
	}
	Service->SetActorLocation(FVector(300.0f, 0.0f, 0.0f));
	Service->Vendor->SetStockData(Stock);
	TestTrue(TEXT("Living F opens the nearby service"), F.Interact());
	TestTrue(TEXT("Living service panel is visible"), F.Hud->IsVendorPanelVisible());
	F.Hud->CloseVendor();

	const int32 CopperBefore = F.Character->Wallet->Copper;
	const int32 ExperienceBefore = F.Character->Stats->CurrentExperience;
	F.Character->Stats->ForceDeath();
	F.HandleDeath();
	const int32 DeathChatCount = F.Hud->GetChatMessageCount();
	Service->SetActorLocation(FVector(1000.0f, 0.0f, 0.0f));
	F.ResetEmptyInteractionFeedback();
	TestFalse(TEXT("Dead F with no nearby owner is rejected"), F.Interact());
	TestEqual(TEXT("Dead F does not post empty-range chat"), F.Hud->GetChatMessageCount(), DeathChatCount);
	Service->SetActorLocation(FVector(300.0f, 0.0f, 0.0f));
	TestFalse(TEXT("Dead F cannot interact with a nearby service"), F.Interact());
	TestFalse(TEXT("Dead F cannot reopen the vendor panel"), F.Hud->IsVendorPanelVisible());
	TestEqual(TEXT("Dead F does not post service dialogue"), F.Hud->GetChatMessageCount(), DeathChatCount);
	TestEqual(TEXT("Dead F preserves copper"), F.Character->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Dead F preserves XP"), F.Character->Stats->CurrentExperience, ExperienceBefore);
	TestEqual(TEXT("Dead F preserves quest records"), F.Character->QuestLog->QuestStates.Num(), 0);
	TestEqual(TEXT("Dead F preserves inventory"), F.Character->Inventory->Stacks.Num(), 0);

	F.Respawn();
	TestTrue(TEXT("Recovered character can use nearby F again"), F.Interact());
	TestTrue(TEXT("Recovered service panel opens"), F.Hud->IsVendorPanelVisible());
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereBlockedInteractionTest,
	"Embermere.Input.BlockedInteraction",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereBlockedInteractionTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	if (!TestNotNull(TEXT("Saved vendor stock exists"), Stock))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.World->AddController(F.Controller);
	F.Character->SetActorLocation(FVector(0.0f, 0.0f, 90.0f));
	AEmbermereVendorServiceActor* Service = F.World->SpawnActor<AEmbermereVendorServiceActor>();
	AActor* ClearService = F.World->SpawnActor<AActor>();
	AActor* Wall = F.World->SpawnActor<AActor>();
	if (!TestNotNull(TEXT("In-range service exists"), Service) ||
		!TestNotNull(TEXT("Alternate service exists"), ClearService) ||
		!TestNotNull(TEXT("Solid wall actor exists"), Wall))
	{
		return false;
	}
	Service->SetActorLocation(FVector(260.0f, 0.0f, 90.0f));
	Service->Vendor->SetStockData(Stock);
	USceneComponent* ClearRoot = NewObject<USceneComponent>(ClearService);
	ClearService->SetRootComponent(ClearRoot);
	ClearRoot->RegisterComponent();
	UEmbermereInteractableComponent* ClearInteraction = NewObject<UEmbermereInteractableComponent>(ClearService);
	ClearInteraction->bShowWorldMarker = false;
	ClearInteraction->DisplayName = FText::FromString(TEXT("Side service"));
	ClearInteraction->DialogueText = FText::FromString(TEXT("This route is clear."));
	ClearInteraction->RegisterComponent();
	ClearService->SetActorLocation(FVector(100.0f, 500.0f, 90.0f));
	UBoxComponent* WallBox = NewObject<UBoxComponent>(Wall);
	WallBox->SetBoxExtent(FVector(10.0f, 120.0f, 120.0f));
	WallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallBox->SetCollisionResponseToAllChannels(ECR_Block);
	Wall->SetRootComponent(WallBox);
	WallBox->RegisterComponent();
	Wall->SetActorLocation(FVector(130.0f, 0.0f, 90.0f));
	F.World->InitializeActorsForPlay(FURL());
	F.World->BeginPlay();
	F.World->GetWorldSettings()->NotifyBeginPlay();

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EmbermereInteractionCoverControl), false);
	QueryParams.AddIgnoredActor(F.Character);
	QueryParams.AddIgnoredActor(Service);
	FHitResult Hit;
	TestTrue(TEXT("Registered wall blocks the F visibility channel"),
		F.World->LineTraceSingleByChannel(Hit,
			F.Character->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			Service->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			ECC_Visibility, QueryParams) && Hit.GetActor() == Wall);
	const int32 ChatBefore = F.Hud->GetChatMessageCount();
	const int32 CopperBefore = F.Character->Wallet->Copper;
	const int32 ExperienceBefore = F.Character->Stats->CurrentExperience;
	TestFalse(TEXT("F cannot open a service through solid cover"), F.Interact());
	TestFalse(TEXT("Blocked service panel remains closed"), F.Hud->IsVendorPanelVisible());
	TestEqual(TEXT("Blocked F posts one explanation"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	UTextBlock* Feedback = F.Hud->WidgetTree->FindWidget<UTextBlock>(TEXT("ChatMessageText_0"));
	if (TestNotNull(TEXT("Blocked feedback row exists"), Feedback))
	{
		TestEqual(TEXT("Blocked F explains the obstacle"), Feedback->GetText().ToString(),
			FString(TEXT("Move around the obstacle to interact.")));
	}
	TestFalse(TEXT("Repeated blocked F remains rejected"), F.Interact());
	TestEqual(TEXT("Repeated blocked F does not flood chat"), F.Hud->GetChatMessageCount(), ChatBefore + 1);
	TestEqual(TEXT("Blocked F preserves copper"), F.Character->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Blocked F preserves XP"), F.Character->Stats->CurrentExperience, ExperienceBefore);
	TestEqual(TEXT("Blocked F preserves inventory"), F.Character->Inventory->Stacks.Num(), 0);
	TestEqual(TEXT("Blocked F preserves quest records"), F.Character->QuestLog->QuestStates.Num(), 0);
	F.Hud->CloseVendor();

	ClearService->SetActorLocation(FVector(100.0f, 280.0f, 90.0f));
	TestTrue(TEXT("F can choose a clear alternate owner over a blocked nearer owner"), F.Interact());
	TestFalse(TEXT("Blocked nearer vendor still stays closed"), F.Hud->IsVendorPanelVisible());
	UTextBlock* Speaker = F.Hud->WidgetTree->FindWidget<UTextBlock>(TEXT("DialogueSpeaker"));
	if (TestNotNull(TEXT("Alternate dialogue speaker exists"), Speaker))
	{
		TestEqual(TEXT("The clear alternate owner receives F"), Speaker->GetText().ToString(),
			FString(TEXT("Side service")));
	}
	ClearService->SetActorLocation(FVector(100.0f, 500.0f, 90.0f));
	Wall->SetActorLocation(FVector(130.0f, 400.0f, 90.0f));
	TestFalse(TEXT("Moving wall clears the F visibility channel"),
		F.World->LineTraceSingleByChannel(Hit,
			F.Character->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			Service->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			ECC_Visibility, QueryParams));
	TestTrue(TEXT("Clear in-range F opens the same service"), F.Interact());
	TestTrue(TEXT("Clear service panel opens"), F.Hud->IsVendorPanelVisible());
	TestEqual(TEXT("Opening the service still spends no copper"), F.Character->Wallet->Copper, CopperBefore);
	return !HasAnyErrors();
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
	AActor* Wall = F.World->SpawnActor<AActor>();
	UBoxComponent* WallBox = NewObject<UBoxComponent>(Wall);
	Wall->SetRootComponent(WallBox);
	WallBox->SetBoxExtent(FVector(10.0f, 100.0f, 150.0f));
	WallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallBox->SetCollisionProfileName(TEXT("BlockAll"));
	WallBox->RegisterComponent();
	Wall->SetActorLocation(FVector(50.0f, 0.0f, 0.0f));
	FCollisionQueryParams SightlineParams(SCENE_QUERY_STAT(EmbermereHotbarSightlineTest), false);
	SightlineParams.AddIgnoredActor(F.Character);
	SightlineParams.AddIgnoredActor(Target);
	FHitResult SightlineHit;
	TestTrue(TEXT("Solid test wall blocks the hotbar sightline"),
		F.World->LineTraceSingleByChannel(SightlineHit, FVector(0.0f, 0.0f, 90.0f),
			FVector(100.0f, 0.0f, 90.0f), ECC_Visibility, SightlineParams));
	F.ActivateAbilitySlot(0);
	TestEqual(TEXT("Obstructed request produces one explanation"), F.Hud->GetChatMessageCount(), InitialMessages + 4);
	TestEqual(TEXT("Obstructed copy"), ChatLine(3), FString(TEXT("Target is out of sight.")));
	TestEqual(TEXT("Obstructed request spends no mana"), F.Character->Stats->CurrentMana, InitialMana);
	TestEqual(TEXT("Obstructed request deals no damage"), Target->Stats->CurrentHealth, TargetHealth);
	TestEqual(TEXT("Obstructed request starts no cooldown"), F.Character->Hotbar->GetSlotCooldownRemaining(0), 0.0f);
	Wall->SetActorLocation(FVector(50.0f, 300.0f, 0.0f));
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereFullResourceRecoveryAbilityTest,
	"Embermere.Combat.FullResourceRecoveryRejection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFullResourceRecoveryAbilityTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = LoadObject<UEmbermereRulesData>(nullptr,
		TEXT("/Game/Data/DA_EmbermereRules.DA_EmbermereRules"));
	FEmbermereAbilityDefinition Meditate;
	if (!TestNotNull(TEXT("Saved rules data exists"), Rules) ||
		!TestTrue(TEXT("Saved Wizard Meditate resolves"), Rules->GetAbilityDefinition(TEXT("Meditate"), Meditate)))
	{
		return false;
	}

	FEmbermereServiceKeyboardFixture F;
	F.Controller->SetPawn(F.Character);
	F.Character->Hotbar->SetAbilityInSlot(3, Meditate);
	const float FullMana = F.Character->Stats->CurrentMana;
	const float FullHealth = F.Character->Stats->CurrentHealth;
	const int32 InitialMessages = F.Hud->GetChatMessageCount();
	int32 CommittedResults = 0;
	const FDelegateHandle ResultHandle = F.Character->Combat->OnCombatResult.AddLambda(
		[&CommittedResults](const FEmbermereCombatResult&) { ++CommittedResults; });
	auto ChatLine = [&F](int32 RowIndex)
	{
		const UTextBlock* Row = F.Hud->WidgetTree->FindWidget<UTextBlock>(
			*FString::Printf(TEXT("ChatMessageText_%d"), RowIndex));
		return Row ? Row->GetText().ToString() : FString();
	};

	F.ActivateAbilitySlot(3);
	TestEqual(TEXT("Full-mana Meditate gives one useful explanation"),
		F.Hud->GetChatMessageCount(), InitialMessages + 1);
	TestEqual(TEXT("Full-mana copy"), ChatLine(0), FString(TEXT("Mana is already full.")));
	TestEqual(TEXT("Full-mana Meditate leaves mana unchanged"), F.Character->Stats->CurrentMana, FullMana);
	TestEqual(TEXT("Full-mana Meditate starts no cooldown"), F.Character->Hotbar->GetSlotCooldownRemaining(3), 0.0f);
	TestEqual(TEXT("Full-mana Meditate commits no result"), CommittedResults, 0);

	TestTrue(TEXT("Mana can be spent before valid Meditate"), F.Character->Stats->SpendMana(20.0f));
	F.ActivateAbilitySlot(3);
	TestEqual(TEXT("Valid Meditate restores exact missing mana"), F.Character->Stats->CurrentMana, FullMana - 2.0f);
	TestTrue(TEXT("Valid Meditate starts its normal cooldown"), F.Character->Hotbar->GetSlotCooldownRemaining(3) > 0.0f);
	TestEqual(TEXT("Valid Meditate publishes one committed result"), CommittedResults, 1);

	FEmbermereAbilityDefinition SelfHeal;
	SelfHeal.AbilityId = TEXT("FullResourceHealCheck");
	SelfHeal.DisplayName = FText::FromString(TEXT("Self Heal"));
	SelfHeal.TargetKind = EEmbermereAbilityTargetKind::Self;
	SelfHeal.EffectType = EEmbermereAbilityEffectType::Heal;
	SelfHeal.Power = 15.0f;
	SelfHeal.ManaCost = 5.0f;
	SelfHeal.Cooldown = 3.0f;
	F.Character->Hotbar->SetAbilityInSlot(4, SelfHeal);
	const float ManaBeforeHeal = F.Character->Stats->CurrentMana;
	const int32 MessagesBeforeHeal = F.Hud->GetChatMessageCount();
	F.ActivateAbilitySlot(4);
	TestEqual(TEXT("Full-health self-heal gives one useful explanation"),
		F.Hud->GetChatMessageCount(), MessagesBeforeHeal + 1);
	TestEqual(TEXT("Full-health copy"), ChatLine(MessagesBeforeHeal), FString(TEXT("Health is already full.")));
	TestEqual(TEXT("Full-health self-heal spends no mana"), F.Character->Stats->CurrentMana, ManaBeforeHeal);
	TestEqual(TEXT("Full-health self-heal starts no cooldown"), F.Character->Hotbar->GetSlotCooldownRemaining(4), 0.0f);
	TestEqual(TEXT("Full-health self-heal commits no result"), CommittedResults, 1);

	F.Character->Stats->ApplyDamage(10.0f);
	F.ActivateAbilitySlot(4);
	TestEqual(TEXT("Valid self-heal restores missing health"), F.Character->Stats->CurrentHealth, FullHealth);
	TestEqual(TEXT("Valid self-heal spends exact mana"), F.Character->Stats->CurrentMana, ManaBeforeHeal - 5.0f);
	TestTrue(TEXT("Valid self-heal starts its normal cooldown"), F.Character->Hotbar->GetSlotCooldownRemaining(4) > 0.0f);
	TestEqual(TEXT("Valid self-heal publishes one more result"), CommittedResults, 2);
	F.Character->Combat->OnCombatResult.Remove(ResultHandle);
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereInventoryChronicleEscapeTest,
	"Embermere.UI.InventoryChronicleEscape",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereInventoryChronicleEscapeTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->PrototypeSaveSlotName = TEXT("EmbermereInventoryChronicleEscapeTest");
	F.Controller->Possess(F.Character);
	TestTrue(TEXT("Inventory begins open"), F.Hud->IsInventoryPanelVisible());
	F.Press(EKeys::Escape);
	TestFalse(TEXT("Game-input Escape closes Inventory"), F.Hud->IsInventoryPanelVisible());

	F.Hud->CloseInteractivePanels();
	F.Press(EKeys::I);
	TestTrue(TEXT("I reopens Inventory"), F.Hud->IsInventoryPanelVisible());
	UEmbermereItemData* First = NewObject<UEmbermereItemData>();
	First->ItemId = TEXT("EscapeFirst");
	First->MaxStack = 1;
	UEmbermereItemData* Second = NewObject<UEmbermereItemData>();
	Second->ItemId = TEXT("EscapeSecond");
	Second->MaxStack = 1;
	TestTrue(TEXT("First transient item fits"), F.Character->Inventory->AddItem(First, 1, false));
	TestTrue(TEXT("Second transient item fits"), F.Character->Inventory->AddItem(Second, 1, false));
	UButton* InventorySort = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("InventorySortButton")));
	if (!TestNotNull(TEXT("Inventory sort button exists"), InventorySort))
	{
		return false;
	}
	{
		FFocusedServiceWindow Window(F);
		TestTrue(TEXT("Focused Inventory button accepts native focus"),
			FSlateApplication::Get().SetKeyboardFocus(InventorySort->TakeWidget()));
		FFocusedServiceWindow::Key(EKeys::Escape);
		TestFalse(TEXT("Focused Escape closes Inventory"), F.Hud->IsInventoryPanelVisible());
	}
	TestEqual(TEXT("Escape does not reorder or consume first item"),
		F.Character->Inventory->Stacks[0].Item.Get(), First);
	TestEqual(TEXT("Escape does not reorder or consume second item"),
		F.Character->Inventory->Stacks[1].Item.Get(), Second);

	F.Press(EKeys::M);
	TestTrue(TEXT("M opens disposable-slot Chronicle"), F.Hud->IsSaveLoadPanelVisible());
	F.Press(EKeys::Escape);
	TestFalse(TEXT("Game-input Escape closes Chronicle"), F.Hud->IsSaveLoadPanelVisible());
	F.Hud->CloseSaveLoadPanel();
	F.Press(EKeys::M);
	UButton* ChronicleClose = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("SaveLoadCloseButton")));
	if (!TestNotNull(TEXT("Chronicle close button exists"), ChronicleClose))
	{
		return false;
	}
	{
		FFocusedServiceWindow Window(F);
		TestTrue(TEXT("Focused Chronicle button accepts native focus"),
			FSlateApplication::Get().SetKeyboardFocus(ChronicleClose->TakeWidget()));
		FFocusedServiceWindow::Key(EKeys::Escape);
		TestFalse(TEXT("Focused Escape closes Chronicle"), F.Hud->IsSaveLoadPanelVisible());
	}
	TestFalse(TEXT("Escape never saves the disposable slot"), F.Controller->DoesPrototypeSaveExist());
	TestEqual(TEXT("Escape never changes copper"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Escape never grants XP"), F.Character->Stats->CurrentExperience, 0);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereInventoryCloseButtonTest,
	"Embermere.UI.InventoryCloseButton",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FEmbermereInventoryCloseButtonTest::RunTest(const FString& Parameters)
{
	FEmbermereServiceKeyboardFixture F;
	F.Controller->Possess(F.Character);
	F.RefreshInputMode();
	TestTrue(TEXT("Inventory begins open"), F.Hud->IsInventoryPanelVisible());
	TestTrue(TEXT("Open Inventory shows the cursor"), F.Controller->bShowMouseCursor);
	UButton* Close = Cast<UButton>(F.Hud->GetWidgetFromName(TEXT("InventoryCloseButton")));
	if (!TestNotNull(TEXT("Inventory has a native close button"), Close))
	{
		return false;
	}

	{
		FFocusedServiceWindow Window(F);
		TestTrue(TEXT("Close button takes native focus"),
			FSlateApplication::Get().SetKeyboardFocus(Close->TakeWidget()));
		FFocusedServiceWindow::Key(EKeys::Enter);
		TestFalse(TEXT("Native Enter closes Inventory"), F.Hud->IsInventoryPanelVisible());
	}
	F.RefreshInputMode();
	TestFalse(TEXT("Controller restores game cursor policy"), F.Controller->bShowMouseCursor);
	F.Press(EKeys::I);
	TestTrue(TEXT("I still reopens Inventory"), F.Hud->IsInventoryPanelVisible());
	{
		FFocusedServiceWindow Window(F);
		TestTrue(TEXT("Close button can take focus again"),
			FSlateApplication::Get().SetKeyboardFocus(Close->TakeWidget()));
		FFocusedServiceWindow::Key(EKeys::SpaceBar);
		TestFalse(TEXT("Native Space closes Inventory"), F.Hud->IsInventoryPanelVisible());
	}
	F.RefreshInputMode();
	TestFalse(TEXT("Closed Inventory leaves gameplay cursor hidden"), F.Controller->bShowMouseCursor);
	TestEqual(TEXT("Close cannot spend copper"), F.Character->Wallet->Copper, 40);
	TestEqual(TEXT("Close cannot grant XP"), F.Character->Stats->CurrentExperience, 0);
	TestEqual(TEXT("Close cannot deliver items"), F.Character->Inventory->Stacks.Num(), 0);
	return !HasAnyErrors();
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
