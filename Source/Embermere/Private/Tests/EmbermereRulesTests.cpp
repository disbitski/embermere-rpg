#if WITH_DEV_AUTOMATION_TESTS

#include <limits>

#include "Animation/AnimSequence.h"
#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Characters/EmbermereNpcPresentationActor.h"
#include "Characters/EmbermerePracticeTargetActor.h"
#include "Characters/EmbermereRestPresentationActor.h"
#include "Characters/EmbermereRestQuestServiceActor.h"
#include "Characters/EmbermereRestServiceActor.h"
#include "Characters/EmbermereTrainerServiceActor.h"
#include "Characters/EmbermereVendorServiceActor.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereRestServiceComponent.h"
#include "Components/EmbermereRestQuestObjectiveRouterComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SizeBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetComponent.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Data/EmbermereRestServiceData.h"
#include "Data/EmbermereRulesData.h"
#include "Data/EmbermereTrainerOfferingsData.h"
#include "Data/EmbermereUiIconSet.h"
#include "Data/EmbermereVendorStockData.h"
#include "Engine/Blueprint.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/SaveGame.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/EmbermereTargetableDispatch.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "Misc/AutomationTest.h"
#include "PhysicsEngine/BodySetup.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Save/EmbermereSaveGame.h"
#include "UI/EmbermereCombatFeedbackWidget.h"
#include "UI/EmbermereCharacterCreationWidget.h"
#include "UI/EmbermereEnemyNameplateWidget.h"
#include "UI/EmbermereItemDragDropOperation.h"
#include "UI/EmbermereLevelUpWidget.h"
#include "UI/EmbermereNpcGreetingWidget.h"
#include "UI/EmbermerePlayerHudWidget.h"
#include "UI/EmbermereQuestLedgerRowButton.h"
#include "UObject/Package.h"

namespace
{
	UEmbermereQuestData* CreateAutomationQuestAsset(
		const TCHAR* AssetName,
		FName QuestId,
		FName ObjectiveId,
		int32 RequiredObjectiveCount,
		int32 RewardExperience = 0,
		int32 RewardCopper = 0)
	{
		const FString PackageName = FString::Printf(TEXT("/Game/Automation/%s"), AssetName);
		UPackage* Package = CreatePackage(*PackageName);
		if (!Package)
		{
			return nullptr;
		}

		UEmbermereQuestData* Quest = FindObject<UEmbermereQuestData>(Package, AssetName);
		if (!Quest)
		{
			Quest = NewObject<UEmbermereQuestData>(
				Package,
				FName(AssetName),
				RF_Public | RF_Standalone);
		}
		if (!Quest)
		{
			return nullptr;
		}

		Quest->QuestId = QuestId;
		Quest->Title = FText::FromName(QuestId);
		Quest->ObjectiveId = ObjectiveId;
		Quest->RequiredObjectiveCount = RequiredObjectiveCount;
		Quest->RewardExperience = RewardExperience;
		Quest->RewardCopper = RewardCopper;
		Quest->RewardItem.Reset();
		return Quest;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTrainerTransactionRulesTest,
	"Embermere.Trainer.TransactionRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerTransactionRulesTest::RunTest(const FString& Parameters)
{
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>();
	UEmbermereTrainerOfferingsData* Offerings = NewObject<UEmbermereTrainerOfferingsData>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereWalletComponent* Wallet = NewObject<UEmbermereWalletComponent>();
	if (!Trainer || !Offerings || !Stats || !Wallet)
	{
		AddError(TEXT("Could not create trainer transaction fixtures"));
		return false;
	}

	FEmbermereTrainerOffering CombatDrills;
	CombatDrills.OfferingId = TEXT("CombatDrills");
	CombatDrills.DisplayName = FText::FromString(TEXT("Combat Drills"));
	CombatDrills.Description = FText::FromString(TEXT("Practice the Fenwatch guard forms."));
	CombatDrills.CopperCost = 10;
	CombatDrills.RequiredLevel = 1;
	CombatDrills.ExperienceReward = 25;
	CombatDrills.bRepeatable = true;
	Offerings->TrainerName = FText::FromString(TEXT("Fenwatch Training"));
	Offerings->Offerings.Add(CombatDrills);
	Trainer->SetOfferingsData(Offerings);
	Wallet->SetCopperForPrototype(40);

	TestEqual(
		TEXT("Eligible training passes preflight"),
		Trainer->CanTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(
		TEXT("Training commits transactionally"),
		Trainer->TryTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(TEXT("Training charges exact copper"), Wallet->Copper, 30);
	TestEqual(TEXT("Training grants exact experience"), Stats->CurrentExperience, 25);

	Wallet->SetCopperForPrototype(9);
	TestEqual(
		TEXT("Insufficient funds reject before progression mutation"),
		Trainer->TryTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::InsufficientFunds);
	TestEqual(TEXT("Insufficient-funds rejection preserves copper"), Wallet->Copper, 9);
	TestEqual(TEXT("Insufficient-funds rejection preserves experience"), Stats->CurrentExperience, 25);

	Offerings->Offerings[0].RequiredLevel = 2;
	Wallet->SetCopperForPrototype(40);
	TestEqual(
		TEXT("Level requirement rejects before wallet mutation"),
		Trainer->TryTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::LevelTooLow);
	TestEqual(TEXT("Level rejection preserves copper"), Wallet->Copper, 40);
	TestEqual(TEXT("Level rejection preserves experience"), Stats->CurrentExperience, 25);

	Offerings->Offerings[0].RequiredLevel = 1;
	Stats->RestoreExperienceForSaveGame(MAX_int32 - 10);
	TestEqual(
		TEXT("Experience overflow rejects before wallet mutation"),
		Trainer->TryTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::ProgressionCap);
	TestEqual(TEXT("Progression-cap rejection preserves copper"), Wallet->Copper, 40);
	TestEqual(TEXT("Progression-cap rejection preserves experience"), Stats->CurrentExperience, MAX_int32 - 10);

	Offerings->Offerings[0].CopperCost = 0;
	TestEqual(
		TEXT("Malformed offerings are rejected"),
		Trainer->TryTrain(0, Stats, Wallet),
		EEmbermereTrainingResult::InvalidRequest);

	Offerings->Offerings[0].CopperCost = 1;
	Offerings->Offerings[0].ExperienceReward = 1;
	Offerings->Offerings[0].bRepeatable = false;
	Stats->RestoreExperienceForSaveGame(0);
	Wallet->SetCopperForPrototype(5);
	TestEqual(TEXT("A non-repeatable lesson may commit once"),
		Trainer->TryTrain(0, Stats, Wallet), EEmbermereTrainingResult::Success);
	TestEqual(TEXT("One-time lesson charges once"), Wallet->Copper, 4);
	TestEqual(TEXT("One-time lesson grants XP once"), Stats->CurrentExperience, 1);
	TestEqual(TEXT("A completed one-time lesson is rejected"),
		Trainer->TryTrain(0, Stats, Wallet), EEmbermereTrainingResult::AlreadyCompleted);
	TestEqual(TEXT("Repeated one-time rejection preserves copper"), Wallet->Copper, 4);
	TestEqual(TEXT("Repeated one-time rejection preserves XP"), Stats->CurrentExperience, 1);

	FEmbermereTrainerOffering DuplicateId = Offerings->Offerings[0];
	DuplicateId.DisplayName = FText::FromString(TEXT("Duplicate Lesson"));
	Offerings->Offerings.Add(DuplicateId);
	TestEqual(TEXT("Duplicate stable offering IDs invalidate the complete data set"),
		Trainer->TryTrain(0, Stats, Wallet), EEmbermereTrainingResult::InvalidRequest);
	TestEqual(TEXT("Duplicate-ID rejection preserves copper"), Wallet->Copper, 4);
	TestEqual(TEXT("Duplicate-ID rejection preserves XP"), Stats->CurrentExperience, 1);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTrainerServiceContractTest,
	"Embermere.Trainer.ServiceContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerServiceContractTest::RunTest(const FString& Parameters)
{
	AEmbermereTrainerServiceActor* Service = NewObject<AEmbermereTrainerServiceActor>();
	TestNotNull(TEXT("Trainer service actor can be created"), Service);
	if (!Service)
	{
		return false;
	}

	TestNotNull(TEXT("Trainer service owns interaction"), Service->Interactable.Get());
	TestNotNull(TEXT("Trainer service owns offering and transaction behavior"), Service->Trainer.Get());
	TestNull(TEXT("Trainer service owns no static presentation"), Service->FindComponentByClass<UStaticMeshComponent>());
	TestNull(TEXT("Trainer service owns no skeletal presentation"), Service->FindComponentByClass<USkeletalMeshComponent>());
	TestEqual(
		TEXT("Trainer interaction uses the armsmaster display name"),
		Service->Interactable->DisplayName.ToString(),
		FString(TEXT("Fenwatch Armsmaster")));
	TestTrue(TEXT("Trainer interaction supplies a world marker"), Service->Interactable->bShowWorldMarker);

	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNull(
		TEXT("Art-only NPC wrapper remains free of trainer behavior"),
		Presentation ? Presentation->FindComponentByClass<UEmbermereTrainerComponent>() : nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTrainerPanelTest,
	"Embermere.UI.TrainerPanel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerPanelTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>();
	UEmbermereTrainerOfferingsData* Offerings = NewObject<UEmbermereTrainerOfferingsData>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereWalletComponent* Wallet = NewObject<UEmbermereWalletComponent>();
	if (!Hud || !Trainer || !Offerings || !Stats || !Wallet)
	{
		AddError(TEXT("Could not create trainer panel fixtures"));
		return false;
	}

	FEmbermereTrainerOffering CombatDrills;
	CombatDrills.OfferingId = TEXT("CombatDrills");
	CombatDrills.DisplayName = FText::FromString(TEXT("Combat Drills"));
	CombatDrills.Description = FText::FromString(TEXT("Practice the Fenwatch guard forms."));
	CombatDrills.CopperCost = 10;
	CombatDrills.RequiredLevel = 1;
	CombatDrills.ExperienceReward = 25;
	FEmbermereTrainerOffering AdvancedForms = CombatDrills;
	AdvancedForms.OfferingId = TEXT("AdvancedCombatDrills");
	AdvancedForms.DisplayName = FText::FromString(TEXT("Advanced Combat Drills"));
	AdvancedForms.RequiredLevel = 2;
	AdvancedForms.CopperCost = 20;
	AdvancedForms.ExperienceReward = 50;
	AdvancedForms.bRepeatable = true;
	Offerings->TrainerName = FText::FromString(TEXT("Fenwatch Training"));
	Offerings->Offerings = {CombatDrills, AdvancedForms};
	Trainer->SetOfferingsData(Offerings);
	Wallet->SetCopperForPrototype(40);
	Hud->Stats = Stats;
	Hud->Wallet = Wallet;

	TestTrue(TEXT("Trainer panel opens for configured offerings"), Hud->ShowTrainer(Trainer));
	TestTrue(TEXT("Trainer panel reports visible"), Hud->IsTrainerPanelVisible());
	TestFalse(TEXT("Opening trainer hides inventory to avoid overlap"), Hud->IsInventoryPanelVisible());
	TestEqual(TEXT("Trainer panel keeps fixed dimensions"), Hud->GetTrainerPanelDimensions(), FVector2D(500.0f, 300.0f));
	const FString TrainerText = Hud->GetTrainerDisplayText().ToString();
	TestTrue(TEXT("Trainer display reports its data-driven name"), TrainerText.Contains(TEXT("Fenwatch Training")));
	TestTrue(TEXT("Trainer display reports purse and progression"), TrainerText.Contains(TEXT("Copper: 40")) && TrainerText.Contains(TEXT("XP: 0")));
	TestTrue(TEXT("Trainer display reports offering level, cost, and reward"),
		TrainerText.Contains(TEXT("Combat Drills - Level 1 - 10 copper - +25 XP")));
	TestTrue(TEXT("Level-two offering remains visible at level one"), TrainerText.Contains(TEXT("Advanced Combat Drills")));
	TestTrue(TEXT("Locked offering exposes exact level rejection"), TrainerText.Contains(TEXT("Advanced Combat Drills requires level 2.")));
	TestEqual(TEXT("Trainer selection starts at first offering"), Hud->GetSelectedTrainerOfferingIndex(), 0);
	TestTrue(TEXT("Bracket-style selection advances offerings"), Hud->SelectNextTrainerOffering(1));
	TestEqual(TEXT("Selection advances to the second offering"), Hud->GetSelectedTrainerOfferingIndex(), 1);
	TestFalse(TEXT("Locked selected offering cannot train"), Hud->TrainSelectedOffering());
	TestEqual(TEXT("Locked UI action preserves wallet"), Wallet->Copper, 40);
	TestEqual(TEXT("Locked UI action preserves XP"), Stats->CurrentExperience, 0);
	TestTrue(TEXT("Selection wraps backward"), Hud->SelectNextTrainerOffering(-1));
	TestEqual(TEXT("Wrapped selection returns to first offering"), Hud->GetSelectedTrainerOfferingIndex(), 0);
	TestTrue(TEXT("Trainer panel completes selected training"), Hud->TrainSelectedOffering());
	TestEqual(TEXT("Panel training updates wallet"), Wallet->Copper, 30);
	TestEqual(TEXT("Panel training updates experience"), Stats->CurrentExperience, 25);
	Stats->Level = 2;
	Wallet->SetCopperForPrototype(50);
	const FString UnlockedTrainerText = Hud->GetTrainerDisplayText().ToString();
	TestTrue(TEXT("Derived level refresh exposes the advanced offering as ready"),
		UnlockedTrainerText.Contains(TEXT("Advanced Combat Drills - Level 2 - 20 copper - +50 XP - Repeatable - Ready")));
	TestFalse(TEXT("Derived level refresh clears the stale level rejection"),
		UnlockedTrainerText.Contains(TEXT("Advanced Combat Drills requires level 2.")));
	TestTrue(TEXT("Level-two selection remains reachable"), Hud->SelectTrainerOffering(1));
	TestTrue(TEXT("Unlocked advanced training commits"), Hud->TrainSelectedOffering());
	TestEqual(TEXT("Advanced panel training charges exact copper"), Wallet->Copper, 30);
	TestEqual(TEXT("Advanced panel training grants exact XP"), Stats->CurrentExperience, 75);
	TestTrue(TEXT("Repeatable advanced training commits again"), Hud->TrainSelectedOffering());
	TestEqual(TEXT("Repeated advanced training charges again"), Wallet->Copper, 10);
	TestEqual(TEXT("Repeated advanced training grants again"), Stats->CurrentExperience, 125);
	TestFalse(TEXT("Insufficient funds reject a further repeat"), Hud->TrainSelectedOffering());
	TestEqual(TEXT("Rejected repeat preserves wallet"), Wallet->Copper, 10);
	TestEqual(TEXT("Rejected repeat preserves XP"), Stats->CurrentExperience, 125);
	TestTrue(TEXT("Inventory toggle hands off from trainer"), Hud->ToggleInventoryPanel());
	TestFalse(TEXT("Inventory handoff closes trainer"), Hud->IsTrainerPanelVisible());
	TestTrue(TEXT("Inventory handoff shows inventory"), Hud->IsInventoryPanelVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchTrainerOfferingsDataTest,
	"Embermere.Trainer.FenwatchOfferingsData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchTrainerOfferingsDataTest::RunTest(const FString& Parameters)
{
	const UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(
		nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	TestNotNull(TEXT("Fenwatch armsmaster offerings data loads"), Offerings);
	if (!Offerings)
	{
		return false;
	}

	TestEqual(
		TEXT("Trainer data keeps the reviewed service name"),
		Offerings->TrainerName.ToString(),
		FString(TEXT("Fenwatch Training")));
	TestEqual(TEXT("Trainer data exposes two bounded progression offerings"), Offerings->Offerings.Num(), 2);
	if (Offerings->Offerings.Num() == 2)
	{
		const FEmbermereTrainerOffering& CombatDrills = Offerings->Offerings[0];
		TestEqual(TEXT("Offering identity remains stable"), CombatDrills.OfferingId, FName(TEXT("CombatDrills")));
		TestEqual(
			TEXT("Offering keeps its player-facing name"),
			CombatDrills.DisplayName.ToString(),
			FString(TEXT("Combat Drills")));
		TestEqual(TEXT("Combat Drills costs ten copper"), CombatDrills.CopperCost, 10);
		TestEqual(TEXT("Combat Drills remains available at level one"), CombatDrills.RequiredLevel, 1);
		TestEqual(
			TEXT("Combat Drills uses the experience effect lane"),
			CombatDrills.EffectType,
			EEmbermereTrainingEffectType::Experience);
		TestEqual(TEXT("Combat Drills grants twenty-five experience"), CombatDrills.ExperienceReward, 25);
		TestTrue(TEXT("Combat Drills is explicitly repeatable"), CombatDrills.bRepeatable);
		TestTrue(TEXT("Saved Combat Drills data passes the native validity contract"), CombatDrills.IsValid());

		const FEmbermereTrainerOffering& AdvancedCombatDrills = Offerings->Offerings[1];
		TestEqual(TEXT("Advanced offering identity remains stable"),
			AdvancedCombatDrills.OfferingId, FName(TEXT("AdvancedCombatDrills")));
		TestEqual(TEXT("Advanced offering keeps its player-facing name"),
			AdvancedCombatDrills.DisplayName.ToString(), FString(TEXT("Advanced Combat Drills")));
		TestEqual(TEXT("Advanced Combat Drills costs twenty copper"), AdvancedCombatDrills.CopperCost, 20);
		TestEqual(TEXT("Advanced Combat Drills requires derived level two"), AdvancedCombatDrills.RequiredLevel, 2);
		TestEqual(TEXT("Advanced Combat Drills grants fifty experience"), AdvancedCombatDrills.ExperienceReward, 50);
		TestTrue(TEXT("Advanced Combat Drills is explicitly repeatable"), AdvancedCombatDrills.bRepeatable);
		TestTrue(TEXT("Saved Advanced Combat Drills data passes the native validity contract"),
			AdvancedCombatDrills.IsValid());
		TestTrue(TEXT("Saved trainer data has valid unique offerings"), Offerings->HasValidOfferings());
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTrainerLevelGatedProgressionTest,
	"Embermere.Trainer.LevelGatedProgression",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerLevelGatedProgressionTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>();
	UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(
		nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	if (!Character || !Trainer || !Offerings || !Character->Stats || !Character->Wallet)
	{
		AddError(TEXT("Could not create level-gated trainer fixtures"));
		return false;
	}

	TestTrue(TEXT("Trainer progression fixture confirms Human Warrior"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Trainer->SetOfferingsData(Offerings);
	Character->Wallet->SetCopperForPrototype(50);
	TestEqual(TEXT("Advanced training is locked below its derived level"),
		Trainer->CanTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::LevelTooLow);
	TestEqual(TEXT("Locked training returns exact player-facing copy"),
		Trainer->GetTrainingResultText(EEmbermereTrainingResult::LevelTooLow, 1).ToString(),
		FString(TEXT("Advanced Combat Drills requires level 2.")));
	TestEqual(TEXT("Locked training rejects without mutation"),
		Trainer->TryTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::LevelTooLow);
	TestEqual(TEXT("Locked training preserves copper"), Character->Wallet->Copper, 50);
	TestEqual(TEXT("Locked training preserves XP"), Character->Stats->CurrentExperience, 0);

	TestTrue(TEXT("Authoritative XP reaches derived level two"), Character->Stats->TryAddExperience(150));
	TestEqual(TEXT("Level gate consumes the derived Stats result"), Character->Stats->Level, 2);
	TestEqual(TEXT("Advanced training unlocks at the exact threshold"),
		Trainer->CanTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(TEXT("First advanced transaction succeeds"),
		Trainer->TryTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(TEXT("First advanced transaction charges twenty copper"), Character->Wallet->Copper, 30);
	TestEqual(TEXT("First advanced transaction grants fifty XP"), Character->Stats->CurrentExperience, 200);
	TestEqual(TEXT("First advanced transaction stays level two"), Character->Stats->Level, 2);
	TestEqual(TEXT("Repeatable advanced transaction succeeds again"),
		Trainer->TryTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(TEXT("Second advanced transaction charges exact copper"), Character->Wallet->Copper, 10);
	TestEqual(TEXT("Second advanced transaction reaches level-three threshold"), Character->Stats->CurrentExperience, 250);
	TestEqual(TEXT("Second advanced transaction derives level three"), Character->Stats->Level, 3);
	TestEqual(TEXT("Insufficient funds reject a third repeat"),
		Trainer->TryTrain(1, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::InsufficientFunds);
	TestEqual(TEXT("Rejected third repeat preserves copper"), Character->Wallet->Copper, 10);
	TestEqual(TEXT("Rejected third repeat preserves XP"), Character->Stats->CurrentExperience, 250);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTrainerLevelGatedPersistenceTest,
	"Embermere.Trainer.LevelGatedPersistence",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTrainerLevelGatedPersistenceTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>();
	UEmbermereTrainerOfferingsData* Offerings = LoadObject<UEmbermereTrainerOfferingsData>(
		nullptr,
		TEXT("/Game/Data/Trainers/DA_FenwatchArmsmasterOfferings.DA_FenwatchArmsmasterOfferings"));
	if (!Source || !Target || !Trainer || !Offerings || !Source->Stats || !Source->Wallet)
	{
		AddError(TEXT("Could not create trainer persistence fixtures"));
		return false;
	}

	TestTrue(TEXT("Source confirms Human Warrior"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Source->Wallet->SetCopperForPrototype(50);
	TestTrue(TEXT("Source reaches level two through authoritative XP"), Source->Stats->TryAddExperience(150));
	Trainer->SetOfferingsData(Offerings);
	TestEqual(TEXT("First advanced training commits before capture"),
		Trainer->TryTrain(1, Source->Stats, Source->Wallet), EEmbermereTrainingResult::Success);
	TestEqual(TEXT("Second repeat commits before capture"),
		Trainer->TryTrain(1, Source->Stats, Source->Wallet), EEmbermereTrainingResult::Success);
	TestEqual(TEXT("Captured source has exact trainer-produced copper"), Source->Wallet->Copper, 10);
	TestEqual(TEXT("Captured source has exact trainer-produced XP"), Source->Stats->CurrentExperience, 250);
	TestEqual(TEXT("Captured source derives level three"), Source->Stats->Level, 3);

	UEmbermereSaveGame* CapturedSave = nullptr;
	FText PersistenceMessage;
	TestEqual(TEXT("Trainer-produced state captures through existing owners"),
		UEmbermerePersistenceLibrary::CaptureGameState(Source, {}, CapturedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Trainer-produced capture creates save data"), CapturedSave);
	if (!CapturedSave)
	{
		return false;
	}
	TestEqual(TEXT("Trainer progression requires no save schema expansion"),
		CapturedSave->FormatVersion, EmbermereSaveGameVersion::Current);
	TestEqual(TEXT("Save captures existing copper owner"), CapturedSave->Copper, 10);
	TestEqual(TEXT("Save captures existing XP owner"), CapturedSave->CurrentExperience, 250);

	TArray<uint8> SerializedBytes;
	TestTrue(TEXT("Trainer progression serializes through the Chronicle save format"),
		UGameplayStatics::SaveGameToMemory(CapturedSave, SerializedBytes));
	UEmbermereSaveGame* LoadedSave = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromMemory(SerializedBytes));
	TestNotNull(TEXT("Trainer progression reloads from serialized bytes"), LoadedSave);
	if (!LoadedSave)
	{
		return false;
	}

	TestTrue(TEXT("Target begins as a genuinely different legal identity"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	Target->Wallet->SetCopperForPrototype(99);
	Target->Stats->RestoreExperienceForSaveGame(100);
	TestEqual(TEXT("Trainer-produced save applies"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Load restores trainer-produced copper exactly"), Target->Wallet->Copper, 10);
	TestEqual(TEXT("Load restores trainer-produced XP exactly"), Target->Stats->CurrentExperience, 250);
	TestEqual(TEXT("Load derives trainer-produced level exactly"), Target->Stats->Level, 3);
	TestEqual(TEXT("Load restores saved race"), Target->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Load restores saved class"), Target->Class, EEmbermereClass::Warrior);
	TestEqual(TEXT("Repeated load remains valid"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated load does not replay trainer XP"), Target->Stats->CurrentExperience, 250);
	TestEqual(TEXT("Repeated load does not replay trainer cost"), Target->Wallet->Copper, 10);

	UEmbermereTrainerComponent* LoadedTrainer = NewObject<UEmbermereTrainerComponent>();
	LoadedTrainer->SetOfferingsData(Offerings);
	TestEqual(TEXT("Post-load training still obeys current affordability"),
		LoadedTrainer->TryTrain(1, Target->Stats, Target->Wallet),
		EEmbermereTrainingResult::InsufficientFunds);
	TestEqual(TEXT("Rejected post-load training preserves XP"), Target->Stats->CurrentExperience, 250);
	TestEqual(TEXT("Rejected post-load training preserves copper"), Target->Wallet->Copper, 10);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereVendorTransactionRulesTest,
	"Embermere.Vendor.TransactionRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorTransactionRulesTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereVendorStockData* Stock = NewObject<UEmbermereVendorStockData>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* Wallet = NewObject<UEmbermereWalletComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	UEmbermereItemData* RecruitPack = NewObject<UEmbermereItemData>();
	if (!Vendor || !Stock || !Inventory || !Wallet || !Tonic || !RecruitPack)
	{
		AddError(TEXT("Could not create vendor transaction fixtures"));
		return false;
	}

	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 5;
	RecruitPack->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RecruitPack->MaxStack = 1;
	FEmbermereVendorStockEntry TonicEntry;
	TonicEntry.Item = Tonic;
	TonicEntry.UnitPriceCopper = 8;
	TonicEntry.InitialQuantity = -1;
	FEmbermereVendorStockEntry PackEntry;
	PackEntry.Item = RecruitPack;
	PackEntry.UnitPriceCopper = 30;
	PackEntry.InitialQuantity = 1;
	Stock->Entries = {TonicEntry, PackEntry};
	Vendor->SetStockData(Stock);
	Wallet->SetCopperForPrototype(40);

	TestEqual(
		TEXT("Affordable purchase passes preflight"),
		Vendor->CanPurchase(0, 1, Inventory, Wallet),
		EEmbermereVendorPurchaseResult::Success);
	TestEqual(
		TEXT("Purchase succeeds transactionally"),
		Vendor->TryPurchase(0, 1, Inventory, Wallet),
		EEmbermereVendorPurchaseResult::Success);
	TestEqual(TEXT("Successful purchase charges exact copper"), Wallet->Copper, 32);
	TestEqual(TEXT("Successful purchase grants exact inventory quantity"), Inventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Unlimited stock remains unlimited"), Vendor->GetRemainingQuantity(0), -1);

	TestEqual(
		TEXT("Finite-stock purchase succeeds"),
		Vendor->TryPurchase(1, 1, Inventory, Wallet),
		EEmbermereVendorPurchaseResult::Success);
	TestEqual(TEXT("Finite purchase charges exact copper"), Wallet->Copper, 2);
	TestEqual(TEXT("Finite purchase grants the pack"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestEqual(TEXT("Finite stock decrements exactly once"), Vendor->GetRemainingQuantity(1), 0);
	TestEqual(
		TEXT("Depleted stock rejects later purchases"),
		Vendor->TryPurchase(1, 1, Inventory, Wallet),
		EEmbermereVendorPurchaseResult::OutOfStock);
	TestEqual(TEXT("Out-of-stock rejection preserves copper"), Wallet->Copper, 2);
	TestEqual(TEXT("Out-of-stock rejection does not duplicate inventory"), Inventory->GetItemQuantity(RecruitPack), 1);

	UEmbermereInventoryComponent* FullInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* FullInventoryWallet = NewObject<UEmbermereWalletComponent>();
	UEmbermereItemData* Blocker = NewObject<UEmbermereItemData>();
	FullInventory->MaxSlots = 1;
	Blocker->DisplayName = FText::FromString(TEXT("Packed Supplies"));
	Blocker->MaxStack = 1;
	FullInventoryWallet->SetCopperForPrototype(40);
	TestTrue(TEXT("Full-bag fixture fills its only slot"), FullInventory->AddItem(Blocker, 1));
	TestEqual(
		TEXT("Full inventory rejects purchase before charging"),
		Vendor->TryPurchase(0, 1, FullInventory, FullInventoryWallet),
		EEmbermereVendorPurchaseResult::InventoryFull);
	TestEqual(TEXT("Full-inventory rejection preserves copper"), FullInventoryWallet->Copper, 40);
	TestEqual(TEXT("Full-inventory rejection grants no item"), FullInventory->GetItemQuantity(Tonic), 0);

	UEmbermereInventoryComponent* PoorInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* PoorWallet = NewObject<UEmbermereWalletComponent>();
	PoorWallet->SetCopperForPrototype(7);
	TestEqual(
		TEXT("Insufficient funds reject purchase before inventory mutation"),
		Vendor->TryPurchase(0, 1, PoorInventory, PoorWallet),
		EEmbermereVendorPurchaseResult::InsufficientFunds);
	TestEqual(TEXT("Insufficient-funds rejection preserves copper"), PoorWallet->Copper, 7);
	TestEqual(TEXT("Insufficient-funds rejection grants no item"), PoorInventory->GetItemQuantity(Tonic), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereVendorSellBuybackTransactionsTest,
	"Embermere.Vendor.SellBuybackTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorSellBuybackTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* Wallet = NewObject<UEmbermereWalletComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	UEmbermereItemData* QuestItem = NewObject<UEmbermereItemData>();
	if (!Vendor || !Inventory || !Wallet || !Tonic || !QuestItem)
	{
		AddError(TEXT("Could not create sell and buyback transaction fixtures"));
		return false;
	}

	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 5;
	Tonic->SellValueCopper = 3;
	QuestItem->DisplayName = FText::FromString(TEXT("Fenwatch Seal"));
	QuestItem->MaxStack = 1;
	QuestItem->SellValueCopper = 10;
	QuestItem->Category = EEmbermereItemCategory::Quest;
	Wallet->SetCopperForPrototype(0);
	TestTrue(TEXT("Sell fixture receives two tonics"), Inventory->AddItem(Tonic, 2));

	TestEqual(
		TEXT("Owned sellable item passes preflight"),
		Vendor->CanSell(Tonic, 1, Inventory, Wallet),
		EEmbermereVendorSellResult::Success);
	TestEqual(
		TEXT("Selling commits atomically"),
		Vendor->TrySell(Tonic, 1, Inventory, Wallet),
		EEmbermereVendorSellResult::Success);
	TestEqual(TEXT("Sale removes exactly one item"), Inventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Sale grants exact data-driven copper"), Wallet->Copper, 3);
	TestEqual(TEXT("Sale creates one buyback row"), Vendor->GetBuybackEntryCount(), 1);
	FEmbermereVendorBuybackEntry Buyback;
	TestTrue(TEXT("Most recent buyback entry resolves"), Vendor->GetBuybackEntry(0, Buyback));
	TestTrue(TEXT("Buyback preserves item identity"), Buyback.Item == Tonic);
	TestEqual(TEXT("Buyback preserves quantity"), Buyback.Quantity, 1);
	TestEqual(TEXT("Buyback preserves sale price"), Buyback.UnitPriceCopper, 3);

	TestEqual(
		TEXT("Affordable buyback passes preflight"),
		Vendor->CanBuyback(0, 1, Inventory, Wallet),
		EEmbermereVendorBuybackResult::Success);
	TestEqual(
		TEXT("Buyback commits atomically"),
		Vendor->TryBuyback(0, 1, Inventory, Wallet),
		EEmbermereVendorBuybackResult::Success);
	TestEqual(TEXT("Buyback returns the exact item"), Inventory->GetItemQuantity(Tonic), 2);
	TestEqual(TEXT("Buyback charges the original sale price"), Wallet->Copper, 0);
	TestEqual(TEXT("Depleted buyback row is removed"), Vendor->GetBuybackEntryCount(), 0);

	TestTrue(TEXT("Quest-item fixture enters inventory"), Inventory->AddItem(QuestItem, 1));
	TestEqual(
		TEXT("Quest items remain unsellable even with an accidental value"),
		Vendor->TrySell(QuestItem, 1, Inventory, Wallet),
		EEmbermereVendorSellResult::Unsellable);
	TestEqual(TEXT("Unsellable rejection preserves the item"), Inventory->GetItemQuantity(QuestItem), 1);
	TestEqual(TEXT("Unsellable rejection preserves copper"), Wallet->Copper, 0);

	UEmbermereVendorComponent* OverflowVendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereInventoryComponent* OverflowInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* OverflowWallet = NewObject<UEmbermereWalletComponent>();
	OverflowWallet->SetCopperForPrototype(MAX_int32);
	TestTrue(TEXT("Overflow fixture receives an item"), OverflowInventory->AddItem(Tonic, 1));
	TestEqual(
		TEXT("Full purse rejects sale before item mutation"),
		OverflowVendor->TrySell(Tonic, 1, OverflowInventory, OverflowWallet),
		EEmbermereVendorSellResult::WalletFull);
	TestEqual(TEXT("Full-purse rejection preserves inventory"), OverflowInventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Full-purse rejection preserves copper"), OverflowWallet->Copper, MAX_int32);
	TestEqual(TEXT("Rejected sale creates no buyback"), OverflowVendor->GetBuybackEntryCount(), 0);

	UEmbermereVendorComponent* RollbackVendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereInventoryComponent* RollbackInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* RollbackWallet = NewObject<UEmbermereWalletComponent>();
	UEmbermereItemData* Blocker = NewObject<UEmbermereItemData>();
	RollbackInventory->MaxSlots = 1;
	RollbackWallet->SetCopperForPrototype(0);
	Blocker->DisplayName = FText::FromString(TEXT("Packed Supplies"));
	Blocker->MaxStack = 1;
	TestTrue(TEXT("Rollback fixture receives one tonic"), RollbackInventory->AddItem(Tonic, 1));
	TestEqual(
		TEXT("Rollback fixture sells one tonic"),
		RollbackVendor->TrySell(Tonic, 1, RollbackInventory, RollbackWallet),
		EEmbermereVendorSellResult::Success);
	TestTrue(TEXT("Rollback fixture fills the freed slot"), RollbackInventory->AddItem(Blocker, 1));
	TestEqual(
		TEXT("Full bag rejects buyback before charging"),
		RollbackVendor->TryBuyback(0, 1, RollbackInventory, RollbackWallet),
		EEmbermereVendorBuybackResult::InventoryFull);
	TestEqual(TEXT("Full-bag buyback preserves copper"), RollbackWallet->Copper, 3);
	TestEqual(TEXT("Full-bag buyback grants no tonic"), RollbackInventory->GetItemQuantity(Tonic), 0);
	TestTrue(TEXT("Rejected buyback remains available"), RollbackVendor->GetBuybackEntry(0, Buyback));
	TestEqual(TEXT("Rejected buyback preserves quantity"), Buyback.Quantity, 1);

	RollbackInventory->RemoveItem(Blocker, 1);
	RollbackWallet->SetCopperForPrototype(2);
	TestEqual(
		TEXT("Insufficient copper rejects buyback before inventory mutation"),
		RollbackVendor->TryBuyback(0, 1, RollbackInventory, RollbackWallet),
		EEmbermereVendorBuybackResult::InsufficientFunds);
	TestEqual(TEXT("Insufficient buyback preserves copper"), RollbackWallet->Copper, 2);
	TestEqual(TEXT("Insufficient buyback grants no tonic"), RollbackInventory->GetItemQuantity(Tonic), 0);
	TestTrue(TEXT("Insufficient buyback remains available"), RollbackVendor->GetBuybackEntry(0, Buyback));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereVendorServiceContractTest,
	"Embermere.Vendor.ServiceContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorServiceContractTest::RunTest(const FString& Parameters)
{
	AEmbermereVendorServiceActor* Service = NewObject<AEmbermereVendorServiceActor>();
	TestNotNull(TEXT("Vendor service actor can be created"), Service);
	if (!Service)
	{
		return false;
	}

	TestNotNull(TEXT("Vendor service owns interaction"), Service->Interactable.Get());
	TestNotNull(TEXT("Vendor service owns stock and transaction behavior"), Service->Vendor.Get());
	TestNull(TEXT("Vendor service owns no static presentation"), Service->FindComponentByClass<UStaticMeshComponent>());
	TestNull(TEXT("Vendor service owns no skeletal presentation"), Service->FindComponentByClass<USkeletalMeshComponent>());
	TestEqual(
		TEXT("Vendor interaction uses the quartermaster display name"),
		Service->Interactable->DisplayName.ToString(),
		FString(TEXT("Fenwatch Quartermaster")));
	TestTrue(TEXT("Vendor interaction supplies a world marker"), Service->Interactable->bShowWorldMarker);
	TestEqual(
		TEXT("Vendor service owns a stable persistence ID"),
		Service->Vendor->PersistenceId,
		FName(TEXT("FenwatchQuartermaster")));

	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNull(
		TEXT("Art-only NPC wrapper remains free of vendor behavior"),
		Presentation ? Presentation->FindComponentByClass<UEmbermereVendorComponent>() : nullptr);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereVendorPanelTest,
	"Embermere.UI.VendorPanel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVendorPanelTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereVendorStockData* Stock = NewObject<UEmbermereVendorStockData>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereWalletComponent* Wallet = NewObject<UEmbermereWalletComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	if (!Hud || !Vendor || !Stock || !Inventory || !Wallet || !Tonic)
	{
		AddError(TEXT("Could not create vendor panel fixtures"));
		return false;
	}

	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->Description = FText::FromString(TEXT("A sharp herbal tonic."));
	Tonic->MaxStack = 5;
	Tonic->SellValueCopper = 3;
	FEmbermereVendorStockEntry Entry;
	Entry.Item = Tonic;
	Entry.UnitPriceCopper = 8;
	Entry.InitialQuantity = -1;
	Stock->VendorName = FText::FromString(TEXT("Fenwatch Supplies"));
	Stock->Entries.Add(Entry);
	Vendor->SetStockData(Stock);
	Wallet->SetCopperForPrototype(40);
	Hud->Inventory = Inventory;
	Hud->Wallet = Wallet;

	TestTrue(TEXT("Vendor panel opens for configured stock"), Hud->ShowVendor(Vendor));
	TestTrue(TEXT("Vendor panel reports visible"), Hud->IsVendorPanelVisible());
	TestFalse(TEXT("Opening vendor hides inventory to avoid overlap"), Hud->IsInventoryPanelVisible());
	const FString VendorText = Hud->GetVendorDisplayText().ToString();
	TestTrue(TEXT("Vendor display reports its data-driven name"), VendorText.Contains(TEXT("Fenwatch Supplies")));
	TestTrue(TEXT("Vendor display reports player currency"), VendorText.Contains(TEXT("Copper: 40")));
	TestTrue(TEXT("Vendor display reports stock and price"), VendorText.Contains(TEXT("Marsh Tonic - 8 copper")));
	TestEqual(TEXT("Vendor selection starts at first stock row"), Hud->GetSelectedVendorStockIndex(), 0);
	TestTrue(TEXT("Vendor panel can buy selected stock"), Hud->PurchaseSelectedVendorItem());
	TestEqual(TEXT("Panel purchase updates wallet"), Wallet->Copper, 32);
	TestEqual(TEXT("Panel purchase updates inventory"), Inventory->GetItemQuantity(Tonic), 1);
	TestTrue(TEXT("Vendor panel sells the selected bag item"), Hud->SellSelectedInventoryItem());
	TestEqual(TEXT("Panel sale updates wallet"), Wallet->Copper, 35);
	TestEqual(TEXT("Panel sale removes the selected item"), Inventory->GetItemQuantity(Tonic), 0);
	TestTrue(TEXT("Vendor display reports the buyback row"), Hud->GetVendorDisplayText().ToString().Contains(TEXT("Buyback: Marsh Tonic x1 - 3 copper")));
	TestTrue(TEXT("Vendor panel buys back the most recent item"), Hud->BuyBackMostRecentVendorItem());
	TestEqual(TEXT("Panel buyback restores the prior wallet balance"), Wallet->Copper, 32);
	TestEqual(TEXT("Panel buyback restores the item"), Inventory->GetItemQuantity(Tonic), 1);
	Hud->CloseVendor();
	TestFalse(TEXT("Close hides vendor panel"), Hud->IsVendorPanelVisible());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchVendorStockDataTest,
	"Embermere.Vendor.FenwatchStockData",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchVendorStockDataTest::RunTest(const FString& Parameters)
{
	const UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(
		nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	TestNotNull(TEXT("Fenwatch quartermaster stock data loads"), Stock);
	if (!Stock)
	{
		return false;
	}

	TestEqual(TEXT("Stock data keeps reviewed vendor name"), Stock->VendorName.ToString(), FString(TEXT("Fenwatch Supplies")));
	TestEqual(TEXT("Stock data exposes two starter wares"), Stock->Entries.Num(), 2);
	if (Stock->Entries.Num() == 2)
	{
		TestTrue(TEXT("First ware is Marsh Tonic"), Stock->Entries[0].Item && Stock->Entries[0].Item->ItemId == FName(TEXT("MarshTonic")));
		TestEqual(TEXT("Marsh Tonic costs eight copper"), Stock->Entries[0].UnitPriceCopper, 8);
		TestEqual(TEXT("Marsh Tonic stock is unlimited"), Stock->Entries[0].InitialQuantity, -1);
		TestTrue(TEXT("Second ware is Recruit Pack"), Stock->Entries[1].Item && Stock->Entries[1].Item->ItemId == FName(TEXT("RecruitPack")));
		TestEqual(TEXT("Recruit Pack costs thirty copper"), Stock->Entries[1].UnitPriceCopper, 30);
		TestEqual(TEXT("Recruit Pack stock is finite"), Stock->Entries[1].InitialQuantity, 1);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchEconomyDataTest,
	"Embermere.Economy.FenwatchRewardsAndValues",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchEconomyDataTest::RunTest(const FString& Parameters)
{
	const UEmbermereItemData* Tonic = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	const UEmbermereItemData* RecruitPack = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"));
	const UEmbermereQuestData* Quest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	TestNotNull(TEXT("Marsh Tonic economy data loads"), Tonic);
	TestNotNull(TEXT("Recruit Pack economy data loads"), RecruitPack);
	TestNotNull(TEXT("Fenwatch starter quest economy data loads"), Quest);
	if (Tonic)
	{
		TestEqual(TEXT("Marsh Tonic sells for three copper"), Tonic->SellValueCopper, 3);
	}
	if (RecruitPack)
	{
		TestEqual(TEXT("Recruit Pack sells for twelve copper"), RecruitPack->SellValueCopper, 12);
	}
	if (Quest)
	{
		TestEqual(TEXT("Starter quest awards twenty copper"), Quest->RewardCopper, 20);
	}
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceRoundTripTest,
	"Embermere.Persistence.RoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceRoundTripTest::RunTest(const FString& Parameters)
{
	UEmbermereItemData* Tonic = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	UEmbermereItemData* RecruitPack = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"));
	UEmbermereQuestData* Quest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(
		nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	UEmbermereVendorComponent* SourceVendor = NewObject<UEmbermereVendorComponent>();
	UEmbermereVendorComponent* TargetVendor = NewObject<UEmbermereVendorComponent>();
	if (!Tonic || !RecruitPack || !Quest || !Stock || !Source || !Target ||
		!SourceVendor || !TargetVendor)
	{
		AddError(TEXT("Could not create persistence round-trip fixtures"));
		return false;
	}

	SourceVendor->PersistenceId = TEXT("FenwatchQuartermaster");
	TargetVendor->PersistenceId = TEXT("FenwatchQuartermaster");
	SourceVendor->SetStockData(Stock);
	TargetVendor->SetStockData(Stock);
	SourceVendor->RestoreStockForSaveGame({-1, 0});
	TestTrue(
		TEXT("Source confirms its Human Warrior identity before capture"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Source->Wallet->SetCopperForPrototype(22);
	Source->Stats->RestoreExperienceForSaveGame(125);
	TestTrue(TEXT("Source receives two tonic items"), Source->Inventory->AddItem(Tonic, 2));
	TestTrue(TEXT("Source receives the Recruit Pack"), Source->Inventory->AddItem(RecruitPack, 1));
	TestTrue(
		TEXT("Source equips Recruit Pack from the bag"),
		Source->Equipment->EquipFromInventory(RecruitPack, Source->Stats->Level, Source->Inventory));
	FEmbermereQuestState CompletedQuest;
	CompletedQuest.Quest = Quest;
	CompletedQuest.CurrentObjectiveCount = Quest->RequiredObjectiveCount;
	CompletedQuest.bCompleted = true;
	Source->QuestLog->RestoreQuestStateForSaveGame(CompletedQuest);
	FEmbermereVendorBuybackEntry SourceBuyback;
	SourceBuyback.Item = Tonic;
	SourceBuyback.Quantity = 1;
	SourceBuyback.UnitPriceCopper = 3;
	SourceVendor->BuybackEntries.Add(SourceBuyback);

	UEmbermereSaveGame* CapturedSave = nullptr;
	FText PersistenceMessage;
	TestEqual(
		TEXT("Live progression captures successfully"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source,
			{SourceVendor},
			CapturedSave,
			PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Capture creates an Embermere save object"), CapturedSave);
	if (!CapturedSave)
	{
		return false;
	}
	TestEqual(TEXT("Save uses the current format version"), CapturedSave->FormatVersion, EmbermereSaveGameVersion::Current);
	TestEqual(TEXT("Save captures stable Human race ID"), CapturedSave->RaceId, FName(TEXT("Human")));
	TestEqual(TEXT("Save captures stable Warrior class ID"), CapturedSave->ClassId, FName(TEXT("Warrior")));
	TestEqual(TEXT("Save captures wallet copper"), CapturedSave->Copper, 22);
	TestEqual(TEXT("Save captures XP"), CapturedSave->CurrentExperience, 125);
	TestEqual(TEXT("Equipped item is absent from bag records"), CapturedSave->InventoryStacks.Num(), 1);
	TestEqual(TEXT("Save captures one equipment record"), CapturedSave->EquippedItems.Num(), 1);
	TestEqual(TEXT("Save captures one quest-ledger record"), CapturedSave->QuestStates.Num(), 1);
	if (CapturedSave->QuestStates.Num() == 1)
	{
		TestTrue(TEXT("Save captures completed quest state"), CapturedSave->QuestStates[0].bCompleted);
		TestEqual(TEXT("Save captures stable quest objective ID"), CapturedSave->QuestStates[0].ObjectiveId, Quest->ObjectiveId);
	}
	TestFalse(TEXT("Version 3 leaves the legacy singular quest field empty"), CapturedSave->QuestState.bHasActiveQuest);
	TestEqual(TEXT("Save captures one persistent vendor"), CapturedSave->VendorStocks.Num(), 1);
	TestEqual(TEXT("Save captures exhausted finite stock"), CapturedSave->VendorStocks[0].RemainingQuantities[1], 0);

	TArray<uint8> SerializedBytes;
	TestTrue(
		TEXT("Save serializes through Unreal's SaveGame archive"),
		UGameplayStatics::SaveGameToMemory(CapturedSave, SerializedBytes));
	USaveGame* LoadedBase = UGameplayStatics::LoadGameFromMemory(SerializedBytes);
	UEmbermereSaveGame* LoadedSave = Cast<UEmbermereSaveGame>(LoadedBase);
	TestNotNull(TEXT("Serialized bytes reload as Embermere save data"), LoadedSave);
	if (!LoadedSave)
	{
		return false;
	}

	Target->Wallet->SetCopperForPrototype(5);
	Target->Stats->RestoreExperienceForSaveGame(9);
	TargetVendor->BuybackEntries.Add(SourceBuyback);
	TestEqual(
		TEXT("Resolved save applies successfully"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target,
			{TargetVendor},
			LoadedSave,
			PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Load restores wallet copper"), Target->Wallet->Copper, 22);
	TestEqual(TEXT("Load restores Human identity"), Target->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Load restores Warrior identity"), Target->Class, EEmbermereClass::Warrior);
	TestTrue(TEXT("Loaded identity is deliberate"), Target->bHasDeliberateCharacterChoice);
	TestEqual(TEXT("Load restores XP"), Target->Stats->CurrentExperience, 125);
	TestEqual(TEXT("Load derives level two from saved XP"), Target->Stats->Level, 2);
	TestEqual(TEXT("Load restores exact tonic quantity"), Target->Inventory->GetItemQuantity(Tonic), 2);
	TestTrue(
		TEXT("Load restores Recruit Pack to Back slot"),
		Target->Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);
	TestEqual(TEXT("Progression and equipment apply once to maximum health"), Target->Stats->MaxHealth, 115.0f);
	TestEqual(TEXT("Equipment bonuses apply once to armor"), Target->Stats->Armor, 1.0f);
	TestEqual(TEXT("Load resets health to the equipped maximum"), Target->Stats->CurrentHealth, 115.0f);
	TestTrue(TEXT("Load restores completed quest identity"), Target->QuestLog->ActiveQuest.Quest == Quest);
	TestTrue(TEXT("Load restores completed quest state"), Target->QuestLog->ActiveQuest.bCompleted);
	TestEqual(
		TEXT("Load restores exact completed objective count"),
		Target->QuestLog->ActiveQuest.CurrentObjectiveCount,
		Quest->RequiredObjectiveCount);
	TestEqual(TEXT("Load restores exhausted finite vendor stock"), TargetVendor->GetRemainingQuantity(1), 0);
	TestEqual(TEXT("Successful load clears session-only buyback"), TargetVendor->GetBuybackEntryCount(), 0);
	TestFalse(TEXT("Loaded completed quest cannot pay rewards again"), Target->QuestLog->TryCompleteActiveQuest());
	TestEqual(TEXT("Rejected repeat completion preserves loaded copper"), Target->Wallet->Copper, 22);

	TestEqual(
		TEXT("Applying the same save twice remains valid"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target,
			{TargetVendor},
			LoadedSave,
			PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeat load does not duplicate bag items"), Target->Inventory->GetItemQuantity(Tonic), 2);
	TestEqual(TEXT("Repeat load does not double progression or equipment health"), Target->Stats->MaxHealth, 115.0f);
	TestEqual(TEXT("Repeat load does not double equipment armor"), Target->Stats->Armor, 1.0f);
	TestEqual(TEXT("Repeat load does not duplicate XP"), Target->Stats->CurrentExperience, 125);
	TestEqual(TEXT("Repeat load does not duplicate copper"), Target->Wallet->Copper, 22);
	TestEqual(TEXT("Repeat load preserves exact race"), Target->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Repeat load preserves exact class"), Target->Class, EEmbermereClass::Warrior);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceCharacterIdentityRoundTripTest,
	"Embermere.Persistence.CharacterIdentityRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceCharacterIdentityRoundTripTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Unconfirmed = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	if (!Unconfirmed || !Source || !Target || !Source->Stats || !Source->Hotbar ||
		!Target->Stats || !Target->Hotbar)
	{
		AddError(TEXT("Could not create character identity persistence fixtures"));
		return false;
	}

	UEmbermereSaveGame* RejectedSave = nullptr;
	FText PersistenceMessage;
	TestEqual(
		TEXT("Unconfirmed fallback identity cannot be captured as a deliberate v2 identity"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Unconfirmed, {}, RejectedSave, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	TestNull(TEXT("Rejected capture produces no save object"), RejectedSave);

	TestTrue(
		TEXT("Source confirms Elf Wizard through character authority"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	Source->Wallet->SetCopperForPrototype(31);
	Source->Stats->RestoreExperienceForSaveGame(44);

	UEmbermereSaveGame* CapturedSave = nullptr;
	TestEqual(
		TEXT("Confirmed Elf Wizard captures successfully"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, CapturedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Identity capture creates a save"), CapturedSave);
	if (!CapturedSave)
	{
		return false;
	}
	TestEqual(TEXT("Identity save uses the current v3 format"), CapturedSave->FormatVersion, EmbermereSaveGameVersion::Current);
	TestEqual(TEXT("Elf uses explicit stable ID"), CapturedSave->RaceId, FName(TEXT("Elf")));
	TestEqual(TEXT("Wizard uses explicit stable ID"), CapturedSave->ClassId, FName(TEXT("Wizard")));

	TArray<uint8> SerializedBytes;
	TestTrue(TEXT("Identity save serializes through SaveGame archive"),
		UGameplayStatics::SaveGameToMemory(CapturedSave, SerializedBytes));
	UEmbermereSaveGame* LoadedSave = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromMemory(SerializedBytes));
	TestNotNull(TEXT("Serialized identity save reloads"), LoadedSave);
	if (!LoadedSave)
	{
		return false;
	}

	TestTrue(
		TEXT("Target begins as a different legal identity"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Ogre, EEmbermereClass::Warrior));
	TestEqual(
		TEXT("V2 identity applies atomically"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Race restores exactly"), Target->Race, EEmbermereRace::Elf);
	TestEqual(TEXT("Class restores exactly"), Target->Class, EEmbermereClass::Wizard);
	TestTrue(TEXT("Restored identity remains deliberate"), Target->bHasDeliberateCharacterChoice);
	TestEqual(TEXT("Wizard base health restores exactly"), Target->Stats->MaxHealth, 80.0f);
	TestEqual(TEXT("Wizard base mana restores exactly"), Target->Stats->MaxMana, 110.0f);
	TestEqual(TEXT("Wizard attack power restores exactly"), Target->Stats->AttackPower, 6.0f);
	TestEqual(TEXT("Copper restores with identity"), Target->Wallet->Copper, 31);
	TestEqual(TEXT("XP restores with identity"), Target->Stats->CurrentExperience, 44);
	const TArray<FName> ExpectedAbilities = {
		TEXT("SparkBolt"), TEXT("FrostRoot"), TEXT("ArcaneBurst"), TEXT("Meditate")};
	for (int32 Index = 0; Index < ExpectedAbilities.Num(); ++Index)
	{
		TestEqual(
			*FString::Printf(TEXT("Restored Wizard slot %d matches rules"), Index + 1),
			Target->Hotbar->Slots[Index].AbilityId,
			ExpectedAbilities[Index]);
	}

	TestEqual(
		TEXT("Repeated v2 identity load remains valid"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated load does not stack health"), Target->Stats->MaxHealth, 80.0f);
	TestEqual(TEXT("Repeated load does not stack mana"), Target->Stats->MaxMana, 110.0f);
	TestEqual(TEXT("Repeated load keeps exact first ability"), Target->Hotbar->Slots[0].AbilityId, FName(TEXT("SparkBolt")));
	TestEqual(TEXT("Repeated load does not duplicate XP"), Target->Stats->CurrentExperience, 44);
	TestEqual(TEXT("Repeated load does not drift copper"), Target->Wallet->Copper, 31);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceCharacterIdentityRollbackTest,
	"Embermere.Persistence.CharacterIdentityRollback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceCharacterIdentityRollbackTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	if (!Source || !Target || !Source->Stats || !Target->Stats || !Target->Hotbar)
	{
		AddError(TEXT("Could not create identity rollback fixtures"));
		return false;
	}

	TestTrue(TEXT("Rollback source confirms Elf Wizard"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	Source->Wallet->SetCopperForPrototype(31);
	Source->Stats->RestoreExperienceForSaveGame(44);
	UEmbermereSaveGame* GoodSave = nullptr;
	FText PersistenceMessage;
	TestEqual(
		TEXT("Rollback source captures a valid v2 record"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, GoodSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	if (!GoodSave)
	{
		AddError(TEXT("Identity rollback baseline save is unavailable"));
		return false;
	}

	TestTrue(TEXT("Rollback target confirms Lizardman Ranger"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Lizardman, EEmbermereClass::Ranger));
	Target->Wallet->SetCopperForPrototype(17);
	Target->Stats->RestoreExperienceForSaveGame(9);
	const FName BaselineFirstAbility = Target->Hotbar->Slots[0].AbilityId;
	auto AssertIdentityStateUnchanged = [this, Target, BaselineFirstAbility]()
	{
		TestEqual(TEXT("Rejected identity preserves race"), Target->Race, EEmbermereRace::Lizardman);
		TestEqual(TEXT("Rejected identity preserves class"), Target->Class, EEmbermereClass::Ranger);
		TestEqual(TEXT("Rejected identity preserves max health"), Target->Stats->MaxHealth, 100.0f);
		TestEqual(TEXT("Rejected identity preserves max mana"), Target->Stats->MaxMana, 60.0f);
		TestEqual(TEXT("Rejected identity preserves first ability"), Target->Hotbar->Slots[0].AbilityId, BaselineFirstAbility);
		TestEqual(TEXT("Rejected identity preserves copper"), Target->Wallet->Copper, 17);
		TestEqual(TEXT("Rejected identity preserves XP"), Target->Stats->CurrentExperience, 9);
	};

	UEmbermereSaveGame* UnknownRace = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	UnknownRace->RaceId = TEXT("Moonkin");
	TestEqual(TEXT("Unknown stable race ID rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, UnknownRace, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertIdentityStateUnchanged();

	UEmbermereSaveGame* UnknownClass = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	UnknownClass->ClassId = TEXT("Bard");
	TestEqual(TEXT("Unknown stable class ID rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, UnknownClass, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertIdentityStateUnchanged();

	UEmbermereSaveGame* IllegalPair = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	IllegalPair->RaceId = TEXT("Dwarf");
	IllegalPair->ClassId = TEXT("Ranger");
	TestEqual(TEXT("Known but illegal Dwarf Ranger rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, IllegalPair, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertIdentityStateUnchanged();

	UEmbermereSaveGame* MissingIdentity = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MissingIdentity->RaceId = NAME_None;
	TestEqual(TEXT("Malformed v2 identity rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, MissingIdentity, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertIdentityStateUnchanged();

	UEmbermereSaveGame* NegativeExperience = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	NegativeExperience->CurrentExperience = -1;
	TestEqual(TEXT("Negative saved XP rejects before progression mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, NegativeExperience, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertIdentityStateUnchanged();

	TestEqual(TEXT("Valid identity still applies after rejected candidates"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, GoodSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Valid identity replaces race"), Target->Race, EEmbermereRace::Elf);
	TestEqual(TEXT("Valid identity replaces class"), Target->Class, EEmbermereClass::Wizard);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceLegacyIdentityFallbackTest,
	"Embermere.Persistence.LegacyV1CharacterFallback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceLegacyIdentityFallbackTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	if (!Source || !Target || !Target->Stats || !Target->Hotbar)
	{
		AddError(TEXT("Could not create legacy identity fixtures"));
		return false;
	}

	TestTrue(TEXT("Legacy source begins from a confirmed identity"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	Source->Wallet->SetCopperForPrototype(23);
	Source->Stats->RestoreExperienceForSaveGame(125);
	UEmbermereSaveGame* LegacySave = nullptr;
	FText PersistenceMessage;
	TestEqual(TEXT("Legacy fixture first captures valid current progression"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, LegacySave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	if (!LegacySave)
	{
		return false;
	}
	LegacySave->FormatVersion = EmbermereSaveGameVersion::ProgressionOnly;
	LegacySave->RaceId = NAME_None;
	LegacySave->ClassId = NAME_None;

	TestTrue(TEXT("Target starts as a different Lizardman Ranger"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Lizardman, EEmbermereClass::Ranger));
	TestEqual(TEXT("Version 1 progression remains loadable"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LegacySave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("V1 uses explicit Human fallback"), Target->Race, EEmbermereRace::Human);
	TestEqual(TEXT("V1 uses explicit Warrior fallback"), Target->Class, EEmbermereClass::Warrior);
	TestTrue(TEXT("V1 fallback becomes deliberate runtime identity"), Target->bHasDeliberateCharacterChoice);
	TestEqual(TEXT("V1 fallback derives Warrior level from legacy XP"), Target->Stats->Level, 2);
	TestEqual(TEXT("V1 fallback rebuilds level-two Warrior health"), Target->Stats->MaxHealth, 110.0f);
	TestEqual(TEXT("V1 fallback rebuilds level-two Warrior mana"), Target->Stats->MaxMana, 53.0f);
	TestEqual(TEXT("V1 fallback rebuilds Warrior first ability"), Target->Hotbar->Slots[0].AbilityId, FName(TEXT("Strike")));
	TestEqual(TEXT("V1 keeps legacy copper"), Target->Wallet->Copper, 23);
	TestEqual(TEXT("V1 keeps legacy XP"), Target->Stats->CurrentExperience, 125);

	TestEqual(TEXT("Repeated v1 fallback remains idempotent"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, LegacySave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated v1 does not stack health"), Target->Stats->MaxHealth, 110.0f);
	TestEqual(TEXT("Repeated v1 does not duplicate XP"), Target->Stats->CurrentExperience, 125);
	TestEqual(TEXT("Compatibility load never rewrites source format"), LegacySave->FormatVersion, EmbermereSaveGameVersion::ProgressionOnly);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceValidationRollbackTest,
	"Embermere.Persistence.ValidationRollback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceValidationRollbackTest::RunTest(const FString& Parameters)
{
	UEmbermereItemData* Tonic = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	UEmbermereQuestData* Quest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereVendorStockData* Stock = LoadObject<UEmbermereVendorStockData>(
		nullptr,
		TEXT("/Game/Data/Vendors/DA_FenwatchQuartermasterStock.DA_FenwatchQuartermasterStock"));
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereVendorComponent* Vendor = NewObject<UEmbermereVendorComponent>();
	if (!Tonic || !Quest || !Stock || !Character || !Vendor)
	{
		AddError(TEXT("Could not create persistence rollback fixtures"));
		return false;
	}

	Vendor->PersistenceId = TEXT("FenwatchQuartermaster");
	Vendor->SetStockData(Stock);
	TestTrue(
		TEXT("Rollback fixture confirms Human Warrior before capture"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Character->Wallet->SetCopperForPrototype(17);
	TestTrue(TEXT("Rollback fixture receives one tonic"), Character->Inventory->AddItem(Tonic, 1));
	FEmbermereVendorBuybackEntry ExistingBuyback;
	ExistingBuyback.Item = Tonic;
	ExistingBuyback.Quantity = 1;
	ExistingBuyback.UnitPriceCopper = 3;
	Vendor->BuybackEntries.Add(ExistingBuyback);

	UEmbermereSaveGame* GoodSave = nullptr;
	FText PersistenceMessage;
	TestEqual(
		TEXT("Rollback baseline captures successfully"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Character,
			{Vendor},
			GoodSave,
			PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	if (!GoodSave || GoodSave->InventoryStacks.IsEmpty() || GoodSave->VendorStocks.IsEmpty())
	{
		AddError(TEXT("Rollback baseline save is incomplete"));
		return false;
	}

	auto AssertLiveStateUnchanged = [this, Character, Vendor, Tonic]()
	{
		TestEqual(TEXT("Rejected load preserves copper"), Character->Wallet->Copper, 17);
		TestEqual(TEXT("Rejected load preserves exact inventory"), Character->Inventory->GetItemQuantity(Tonic), 1);
		TestEqual(TEXT("Rejected load preserves finite stock"), Vendor->GetRemainingQuantity(1), 1);
		TestEqual(TEXT("Rejected load preserves session buyback"), Vendor->GetBuybackEntryCount(), 1);
	};

	UEmbermereSaveGame* WrongVersion = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	WrongVersion->FormatVersion = EmbermereSaveGameVersion::Current + 1;
	TestEqual(
		TEXT("Unsupported versions are rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, WrongVersion, PersistenceMessage),
		EEmbermerePersistenceResult::UnsupportedVersion);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* MissingAsset = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MissingAsset->InventoryStacks[0].ItemAsset = FSoftObjectPath(
		TEXT("/Game/Data/Items/DI_MissingItem.DI_MissingItem"));
	TestEqual(
		TEXT("Missing item assets are rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, MissingAsset, PersistenceMessage),
		EEmbermerePersistenceResult::MissingAsset);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* InvalidQuantity = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	InvalidQuantity->InventoryStacks[0].Quantity = Tonic->MaxStack + 1;
	TestEqual(
		TEXT("Invalid stack quantities are rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, InvalidQuantity, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* CapacityConflict = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	const FEmbermereSavedInventoryStack OneTonic = CapacityConflict->InventoryStacks[0];
	CapacityConflict->InventoryStacks.Reset();
	for (int32 Index = 0; Index < Character->Inventory->MaxSlots + 1; ++Index)
	{
		CapacityConflict->InventoryStacks.Add(OneTonic);
	}
	TestEqual(
		TEXT("Over-capacity bags are rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, CapacityConflict, PersistenceMessage),
		EEmbermerePersistenceResult::CapacityConflict);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* InvalidQuest = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	FEmbermereSavedQuestRecord InvalidQuestRecord;
	InvalidQuestRecord.QuestId = Quest->QuestId;
	InvalidQuestRecord.QuestAsset = FSoftObjectPath(Quest);
	InvalidQuestRecord.ObjectiveId = Quest->ObjectiveId;
	InvalidQuestRecord.CurrentObjectiveCount = Quest->RequiredObjectiveCount + 1;
	InvalidQuest->QuestStates.Add(InvalidQuestRecord);
	TestEqual(
		TEXT("Out-of-range quest progress is rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, InvalidQuest, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* VendorMismatch = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	VendorMismatch->VendorStocks[0].VendorId = TEXT("UnknownQuartermaster");
	TestEqual(
		TEXT("Unknown vendor IDs are rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, VendorMismatch, PersistenceMessage),
		EEmbermerePersistenceResult::VendorMismatch);
	AssertLiveStateUnchanged();

	UEmbermereSaveGame* InvalidStock = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	InvalidStock->VendorStocks[0].RemainingQuantities[1] = 2;
	TestEqual(
		TEXT("Stock above its finite initial quantity is rejected before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, InvalidStock, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveStateUnchanged();

	TestEqual(
		TEXT("Validated baseline applies after all rejected candidates"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {Vendor}, GoodSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Successful load keeps captured copper"), Character->Wallet->Copper, 17);
	TestEqual(TEXT("Successful load keeps captured inventory"), Character->Inventory->GetItemQuantity(Tonic), 1);
	TestEqual(TEXT("Successful load resets session buyback"), Vendor->GetBuybackEntryCount(), 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereMultiQuestPersistenceRoundTripTest,
	"Embermere.Persistence.MultiQuestRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereMultiQuestPersistenceRoundTripTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* SecondQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationStillWatersRoundTrip"),
		TEXT("FenwatchStillWatersRoundTrip"),
		TEXT("FenwatchRestCompletedRoundTrip"),
		1,
		50,
		10);
	AEmbermereCharacter* EmptySource = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	if (!MaraQuest || !SecondQuest || !EmptySource || !Source || !Target)
	{
		AddError(TEXT("Could not create multi-quest round-trip fixtures"));
		return false;
	}

	FText PersistenceMessage;
	UEmbermereSaveGame* EmptySave = nullptr;
	TestTrue(TEXT("Empty-ledger fixture confirms a deliberate identity"),
		EmptySource->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestEqual(TEXT("An empty quest ledger captures successfully"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			EmptySource, {}, EmptySave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Empty-ledger capture creates a save"), EmptySave);
	if (EmptySave)
	{
		TestEqual(TEXT("Empty-ledger capture writes zero records"), EmptySave->QuestStates.Num(), 0);
	}

	TestTrue(TEXT("Multi-quest source confirms a deliberate identity"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	TestTrue(TEXT("Mara quest enters the source ledger"), Source->QuestLog->AcceptQuest(MaraQuest));
	TestTrue(TEXT("Second quest enters the source ledger"), Source->QuestLog->AcceptQuest(SecondQuest));
	TestTrue(TEXT("Mara receives exact partial progress"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			MaraQuest->QuestId,
			MaraQuest->ObjectiveId,
			2));
	TestTrue(TEXT("Second quest reaches its exact objective"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			SecondQuest->QuestId,
			SecondQuest->ObjectiveId,
			1));
	TestTrue(TEXT("Second quest commits its reward once"),
		Source->QuestLog->TryCompleteQuest(SecondQuest));
	TestTrue(TEXT("Source focus can deliberately point at completed second quest"),
		Source->QuestLog->FocusQuest(SecondQuest->QuestId));

	UEmbermereSaveGame* CapturedSave = nullptr;
	TestEqual(TEXT("Two quest records capture under version 3"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, CapturedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Multi-quest capture creates a save"), CapturedSave);
	if (!CapturedSave)
	{
		return false;
	}
	TestEqual(TEXT("Multi-quest capture uses version 3"),
		CapturedSave->FormatVersion, EmbermereSaveGameVersion::MultiQuestLedger);
	TestEqual(TEXT("Multi-quest capture writes two records"), CapturedSave->QuestStates.Num(), 2);
	TestFalse(TEXT("Version 3 writes no singular legacy quest"), CapturedSave->QuestState.bHasActiveQuest);
	for (const FEmbermereSavedQuestRecord& SavedQuest : CapturedSave->QuestStates)
	{
		TestFalse(TEXT("Every version-3 record writes a stable objective ID"), SavedQuest.ObjectiveId.IsNone());
	}

	TArray<uint8> SerializedBytes;
	TestTrue(TEXT("Multi-quest version 3 serializes through SaveGame memory"),
		UGameplayStatics::SaveGameToMemory(CapturedSave, SerializedBytes));
	UEmbermereSaveGame* LoadedSave = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromMemory(SerializedBytes));
	TestNotNull(TEXT("Serialized multi-quest bytes reload"), LoadedSave);
	if (!LoadedSave)
	{
		return false;
	}

	Target->Wallet->SetCopperForPrototype(3);
	Target->Stats->RestoreExperienceForSaveGame(7);
	TestEqual(TEXT("Version-3 ledger applies atomically"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	FEmbermereQuestState MaraState;
	FEmbermereQuestState SecondState;
	TestTrue(TEXT("Load restores Mara by stable quest ID"),
		Target->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState));
	TestEqual(TEXT("Load restores exact Mara progress"), MaraState.CurrentObjectiveCount, 2);
	TestFalse(TEXT("Mara remains incomplete"), MaraState.bCompleted);
	TestTrue(TEXT("Load restores second quest by stable ID"),
		Target->QuestLog->GetQuestStateById(SecondQuest->QuestId, SecondState));
	TestTrue(TEXT("Load restores second completed history"), SecondState.bCompleted);
	TestEqual(TEXT("Load restores committed second-quest XP"), Target->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Load restores committed second-quest copper"), Target->Wallet->Copper, 50);
	TestTrue(TEXT("Transient focus is reselected from current incomplete state"),
		Target->QuestLog->ActiveQuest.Quest == MaraQuest);
	TestFalse(TEXT("Loaded completion cannot replay second reward"),
		Target->QuestLog->TryCompleteQuest(SecondQuest));

	TestEqual(TEXT("Applying the same version-3 ledger twice remains valid"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated load preserves two unique records"), Target->QuestLog->QuestStates.Num(), 2);
	TestEqual(TEXT("Repeated load does not duplicate XP"), Target->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Repeated load does not duplicate copper"), Target->Wallet->Copper, 50);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereLegacyQuestCompatibilityTest,
	"Embermere.Persistence.LegacyQuestCompatibility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLegacyQuestCompatibilityTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	AEmbermereCharacter* VersionTwoTarget = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* VersionOneTarget = NewObject<AEmbermereCharacter>();
	UEmbermereSaveGame* VersionTwoSave = NewObject<UEmbermereSaveGame>();
	if (!MaraQuest || !VersionTwoTarget || !VersionOneTarget || !VersionTwoSave)
	{
		AddError(TEXT("Could not create legacy quest compatibility fixtures"));
		return false;
	}

	VersionTwoSave->FormatVersion = EmbermereSaveGameVersion::CharacterIdentity;
	VersionTwoSave->RaceId = TEXT("Elf");
	VersionTwoSave->ClassId = TEXT("Wizard");
	VersionTwoSave->Copper = 22;
	VersionTwoSave->CurrentExperience = 125;
	VersionTwoSave->QuestState.bHasActiveQuest = true;
	VersionTwoSave->QuestState.QuestId = MaraQuest->QuestId;
	VersionTwoSave->QuestState.QuestAsset = FSoftObjectPath(MaraQuest);
	VersionTwoSave->QuestState.CurrentObjectiveCount = MaraQuest->RequiredObjectiveCount;
	VersionTwoSave->QuestState.bCompleted = true;

	FText PersistenceMessage;
	TestEqual(TEXT("Version 2 singular quest adapts into the runtime ledger"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			VersionTwoTarget, {}, VersionTwoSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	FEmbermereQuestState VersionTwoQuestState;
	TestTrue(TEXT("Version 2 adapter restores exact completed quest"),
		VersionTwoTarget->QuestLog->GetQuestStateById(
			MaraQuest->QuestId, VersionTwoQuestState) && VersionTwoQuestState.bCompleted);
	TestEqual(TEXT("Reading version 2 never rewrites its format"),
		VersionTwoSave->FormatVersion, EmbermereSaveGameVersion::CharacterIdentity);
	const int32 VersionTwoCopper = VersionTwoTarget->Wallet->Copper;
	TestFalse(TEXT("Adapted version-2 completion cannot replay reward"),
		VersionTwoTarget->QuestLog->TryCompleteQuest(MaraQuest));
	TestEqual(TEXT("Rejected version-2 replay preserves copper"),
		VersionTwoTarget->Wallet->Copper, VersionTwoCopper);
	TestEqual(TEXT("Repeated version-2 adapter load remains valid"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			VersionTwoTarget, {}, VersionTwoSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated version-2 load keeps one ledger record"),
		VersionTwoTarget->QuestLog->QuestStates.Num(), 1);

	UEmbermereSaveGame* VersionOneSave = DuplicateObject<UEmbermereSaveGame>(
		VersionTwoSave,
		GetTransientPackage());
	VersionOneSave->FormatVersion = EmbermereSaveGameVersion::ProgressionOnly;
	VersionOneSave->RaceId = NAME_None;
	VersionOneSave->ClassId = NAME_None;
	TestEqual(TEXT("Version 1 singular quest uses the same compatibility adapter"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			VersionOneTarget, {}, VersionOneSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	FEmbermereQuestState VersionOneQuestState;
	TestTrue(TEXT("Version 1 adapter retains completed quest history"),
		VersionOneTarget->QuestLog->GetQuestStateById(
			MaraQuest->QuestId, VersionOneQuestState) && VersionOneQuestState.bCompleted);
	TestEqual(TEXT("Version 1 keeps explicit Human fallback"),
		VersionOneTarget->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Reading version 1 never rewrites its format"),
		VersionOneSave->FormatVersion, EmbermereSaveGameVersion::ProgressionOnly);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereMultiQuestValidationRollbackTest,
	"Embermere.Persistence.MultiQuestValidationRollback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereMultiQuestValidationRollbackTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* SecondQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationStillWatersRollback"),
		TEXT("FenwatchStillWatersRollback"),
		TEXT("FenwatchRestCompletedRollback"),
		1);
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!MaraQuest || !SecondQuest || !Character)
	{
		AddError(TEXT("Could not create multi-quest rollback fixtures"));
		return false;
	}

	TestTrue(TEXT("Rollback fixture confirms a deliberate identity"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestTrue(TEXT("Rollback fixture accepts Mara"), Character->QuestLog->AcceptQuest(MaraQuest));
	TestTrue(TEXT("Rollback fixture accepts second quest"), Character->QuestLog->AcceptQuest(SecondQuest));
	TestTrue(TEXT("Rollback fixture records exact Mara progress"),
		Character->QuestLog->AddObjectiveProgressForQuest(
			MaraQuest->QuestId, MaraQuest->ObjectiveId, 1));

	UEmbermereSaveGame* GoodSave = nullptr;
	FText PersistenceMessage;
	TestEqual(TEXT("Valid two-record ledger captures"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Character, {}, GoodSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	if (!GoodSave || GoodSave->QuestStates.Num() != 2)
	{
		AddError(TEXT("Multi-quest rollback baseline save is incomplete"));
		return false;
	}

	auto AssertLiveLedgerUnchanged = [this, Character, MaraQuest, SecondQuest]()
	{
		FEmbermereQuestState MaraState;
		FEmbermereQuestState SecondState;
		TestEqual(TEXT("Rejected ledger preserves record count"), Character->QuestLog->QuestStates.Num(), 2);
		TestTrue(TEXT("Rejected ledger preserves Mara progress"),
			Character->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState) &&
			MaraState.CurrentObjectiveCount == 1 && !MaraState.bCompleted);
		TestTrue(TEXT("Rejected ledger preserves second quest state"),
			Character->QuestLog->GetQuestStateById(SecondQuest->QuestId, SecondState) &&
			SecondState.CurrentObjectiveCount == 0 && !SecondState.bCompleted);
		TestEqual(TEXT("Rejected ledger preserves wallet"), Character->Wallet->Copper, 40);
		TestEqual(TEXT("Rejected ledger preserves experience"), Character->Stats->CurrentExperience, 0);
	};

	UEmbermereSaveGame* DuplicateIds = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	const FEmbermereSavedQuestRecord DuplicateRecord = DuplicateIds->QuestStates[0];
	DuplicateIds->QuestStates.Add(DuplicateRecord);
	TestEqual(TEXT("Duplicate quest IDs reject the complete candidate"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, DuplicateIds, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* MissingAsset = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MissingAsset->QuestStates[0].QuestAsset = FSoftObjectPath(
		TEXT("/Game/Automation/DQ_MissingQuest.DQ_MissingQuest"));
	TestEqual(TEXT("Missing quest assets reject before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, MissingAsset, PersistenceMessage),
		EEmbermerePersistenceResult::MissingAsset);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* MismatchedId = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MismatchedId->QuestStates[0].QuestId = TEXT("MismatchedQuestId");
	TestEqual(TEXT("Mismatched stable quest IDs reject before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, MismatchedId, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* MismatchedObjective = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MismatchedObjective->QuestStates[0].ObjectiveId = TEXT("WrongObjective");
	TestEqual(TEXT("Mismatched objective IDs reject before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, MismatchedObjective, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* InvalidProgress = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	InvalidProgress->QuestStates[0].CurrentObjectiveCount = 100;
	TestEqual(TEXT("Out-of-range quest progress rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, InvalidProgress, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* ContradictoryCompletion = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	ContradictoryCompletion->QuestStates[0].bCompleted = true;
	ContradictoryCompletion->QuestStates[0].CurrentObjectiveCount = 0;
	TestEqual(TEXT("Contradictory completion rejects before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, ContradictoryCompletion, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* CapacityOverflow = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	const FEmbermereSavedQuestRecord RepeatedRecord = CapacityOverflow->QuestStates[0];
	CapacityOverflow->QuestStates.Reset();
	for (int32 Index = 0; Index < UEmbermereQuestLogComponent::MaxTrackedQuests + 1; ++Index)
	{
		CapacityOverflow->QuestStates.Add(RepeatedRecord);
	}
	TestEqual(TEXT("Over-capacity quest ledgers reject before mutation"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, CapacityOverflow, PersistenceMessage),
		EEmbermerePersistenceResult::CapacityConflict);
	AssertLiveLedgerUnchanged();

	UEmbermereSaveGame* MixedLegacyState = DuplicateObject<UEmbermereSaveGame>(GoodSave, GetTransientPackage());
	MixedLegacyState->QuestState.bHasActiveQuest = true;
	MixedLegacyState->QuestState.QuestId = MaraQuest->QuestId;
	MixedLegacyState->QuestState.QuestAsset = FSoftObjectPath(MaraQuest);
	TestEqual(TEXT("Version 3 rejects contradictory singular legacy state"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, MixedLegacyState, PersistenceMessage),
		EEmbermerePersistenceResult::InvalidData);
	AssertLiveLedgerUnchanged();

	TestEqual(TEXT("Valid ledger still applies after every rejected candidate"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Character, {}, GoodSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Validated restore retains two records"), Character->QuestLog->QuestStates.Num(), 2);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePersistenceSlotInspectionTest,
	"Embermere.Persistence.SlotInspection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePersistenceSlotInspectionTest::RunTest(const FString& Parameters)
{
	const FString SlotName = TEXT("EmbermereAutomationSlotInspection");
	constexpr int32 UserIndex = 87;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
	}

	FText Summary;
	TestEqual(
		TEXT("Missing local slot reports an explicit empty state"),
		UEmbermerePersistenceLibrary::InspectSaveSlot(SlotName, UserIndex, Summary),
		EEmbermerePersistenceResult::SlotUnavailable);
	TestTrue(
		TEXT("Empty-state copy identifies the absent journey"),
		Summary.ToString().Contains(TEXT("No saved journey")));

	UEmbermereSaveGame* SaveGame = Cast<UEmbermereSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UEmbermereSaveGame::StaticClass()));
	if (!SaveGame)
	{
		AddError(TEXT("Could not create save-slot inspection fixture"));
		return false;
	}
	SaveGame->Copper = 22;
	SaveGame->CurrentExperience = 125;
	SaveGame->RaceId = TEXT("Elf");
	SaveGame->ClassId = TEXT("Wizard");
	FEmbermereSavedInventoryStack InventoryStack;
	InventoryStack.ItemId = TEXT("MarshTonic");
	InventoryStack.ItemAsset = FSoftObjectPath(TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	InventoryStack.Quantity = 1;
	SaveGame->InventoryStacks.Add(InventoryStack);
	FEmbermereSavedEquipmentItem EquipmentItem;
	EquipmentItem.Slot = EEmbermereEquipmentSlot::Back;
	EquipmentItem.ItemId = TEXT("RecruitPack");
	EquipmentItem.ItemAsset = FSoftObjectPath(
		TEXT("/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"));
	SaveGame->EquippedItems.Add(EquipmentItem);
	FEmbermereSavedQuestRecord SavedQuest;
	SavedQuest.QuestId = TEXT("FirstSignsAtTheRuin");
	SavedQuest.QuestAsset = FSoftObjectPath(
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	SavedQuest.ObjectiveId = TEXT("StarterEnemyDefeated");
	SavedQuest.bCompleted = true;
	SavedQuest.CurrentObjectiveCount = 3;
	SaveGame->QuestStates.Add(SavedQuest);
	TestTrue(
		TEXT("Valid inspection fixture saves through the one-slot lifecycle"),
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex));

	TestEqual(
		TEXT("Current-version local slot inspects successfully"),
		UEmbermerePersistenceLibrary::InspectSaveSlot(SlotName, UserIndex, Summary),
		EEmbermerePersistenceResult::Success);
	const FString ValidSummary = Summary.ToString();
	TestTrue(TEXT("Slot summary reports read-only identity"), ValidSummary.Contains(TEXT("Elf Wizard")));
	TestTrue(TEXT("Slot summary reports derived level"), ValidSummary.Contains(TEXT("Level 2")));
	TestTrue(TEXT("Slot summary reports exact copper"), ValidSummary.Contains(TEXT("22 copper")));
	TestTrue(TEXT("Slot summary reports exact XP"), ValidSummary.Contains(TEXT("125 XP")));
	TestTrue(TEXT("Slot summary reports bag stack count"), ValidSummary.Contains(TEXT("1 bag stacks")));
	TestTrue(TEXT("Slot summary reports equipment count"), ValidSummary.Contains(TEXT("1 equipped")));
	TestTrue(TEXT("Slot summary reports completed quest state"), ValidSummary.Contains(TEXT("Quest complete")));

	SaveGame->FormatVersion = EmbermereSaveGameVersion::ProgressionOnly;
	SaveGame->RaceId = NAME_None;
	SaveGame->ClassId = NAME_None;
	SaveGame->QuestStates.Reset();
	SaveGame->QuestState.bHasActiveQuest = true;
	SaveGame->QuestState.QuestId = SavedQuest.QuestId;
	SaveGame->QuestState.QuestAsset = SavedQuest.QuestAsset;
	SaveGame->QuestState.CurrentObjectiveCount = SavedQuest.CurrentObjectiveCount;
	SaveGame->QuestState.bCompleted = SavedQuest.bCompleted;
	TestTrue(
		TEXT("Legacy version fixture overwrites the test slot"),
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex));
	TestEqual(
		TEXT("Version 1 local slot remains inspectable"),
		UEmbermerePersistenceLibrary::InspectSaveSlot(SlotName, UserIndex, Summary),
		EEmbermerePersistenceResult::Success);
	TestTrue(
		TEXT("Chronicle labels the version 1 identity fallback"),
		Summary.ToString().Contains(TEXT("Human Warrior  |  legacy v1 fallback")));

	SaveGame->FormatVersion = EmbermereSaveGameVersion::Current + 1;
	TestTrue(
		TEXT("Unsupported-version fixture overwrites the test slot"),
		UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex));
	TestEqual(
		TEXT("Unsupported local versions are rejected before player-facing load"),
		UEmbermerePersistenceLibrary::InspectSaveSlot(SlotName, UserIndex, Summary),
		EEmbermerePersistenceResult::UnsupportedVersion);
	TestTrue(
		TEXT("Rejected-version copy identifies the format mismatch"),
		Summary.ToString().Contains(TEXT("unsupported")));

	TestTrue(
		TEXT("Slot inspection test removes its local save artifact"),
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereAutorunCancellationTest,
	"Embermere.Input.AutorunCancellation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereAutorunCancellationTest::RunTest(const FString& Parameters)
{
	AEmbermerePlayerController* Controller = NewObject<AEmbermerePlayerController>();
	TestNotNull(TEXT("Player controller can be created"), Controller);
	if (!Controller)
	{
		return false;
	}

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(1.0f);
	TestFalse(TEXT("Forward input cancels autorun"), Controller->bAutorunEnabled);

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(-1.0f);
	TestFalse(TEXT("Backward input cancels autorun"), Controller->bAutorunEnabled);

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(0.0f);
	TestTrue(TEXT("Idle axis input leaves autorun enabled"), Controller->bAutorunEnabled);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereOutOfBoundsRecoveryTest,
	"Embermere.Player.OutOfBoundsRecovery",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereOutOfBoundsRecoveryTest::RunTest(const FString& Parameters)
{
	AEmbermerePlayerController* Controller = NewObject<AEmbermerePlayerController>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Player controller can be created"), Controller);
	TestNotNull(TEXT("Player character can be created"), Character);
	if (!Controller || !Character || !Character->Stats)
	{
		return false;
	}

	Controller->OutOfBoundsRecoveryZ = -1000.0f;
	Character->Stats->InitializeVitals();
	Character->SetActorLocation(FVector(0.0f, 0.0f, -999.0f));
	Controller->bAutorunEnabled = true;

	TestFalse(
		TEXT("Character above the recovery plane remains in play"),
		Controller->TriggerOutOfBoundsRecoveryIfNeeded(Character));
	TestFalse(TEXT("Safe character remains alive"), Character->Stats->IsDead());
	TestTrue(TEXT("Safe character keeps autorun state"), Controller->bAutorunEnabled);

	Character->SetActorLocation(FVector(0.0f, 0.0f, -1001.0f));
	Character->Stats->GrantDamageImmunity(10.0f);
	TestTrue(
		TEXT("Character below the recovery plane enters recovery"),
		Controller->TriggerOutOfBoundsRecoveryIfNeeded(Character));
	TestTrue(TEXT("Out-of-bounds recovery forces death"), Character->Stats->IsDead());
	TestFalse(TEXT("Out-of-bounds recovery clears damage immunity"), Character->Stats->IsDamageImmune());
	TestFalse(TEXT("Out-of-bounds recovery cancels autorun"), Controller->bAutorunEnabled);
	TestFalse(
		TEXT("Dead character cannot retrigger out-of-bounds recovery"),
		Controller->TriggerOutOfBoundsRecoveryIfNeeded(Character));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRaceClassRulesTest,
	"Embermere.Rules.RaceClassMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRaceClassRulesTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	TestNotNull(TEXT("Rules data can be created"), Rules);
	if (!Rules)
	{
		return false;
	}

	TestTrue(TEXT("Human can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestTrue(TEXT("Human can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Cleric));
	TestTrue(TEXT("Human can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Ranger));
	TestTrue(TEXT("Human can be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Wizard));

	TestTrue(TEXT("Dwarf can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
	TestTrue(TEXT("Dwarf can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Cleric));
	TestFalse(TEXT("Dwarf cannot be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Ranger));
	TestFalse(TEXT("Dwarf cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Wizard));

	TestTrue(TEXT("Bullywug can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Warrior));
	TestTrue(TEXT("Bullywug can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Cleric));
	TestTrue(TEXT("Bullywug can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Ranger));
	TestFalse(TEXT("Bullywug cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Wizard));

	FEmbermereClassDefinition WarriorDefinition;
	TestTrue(TEXT("Warrior class definition exists"), Rules->GetClassDefinition(EEmbermereClass::Warrior, WarriorDefinition));
	TestEqual(TEXT("Warrior starts with four abilities"), WarriorDefinition.StarterAbilityIds.Num(), 4);
	TestEqual(TEXT("Warrior first hotbar ability is Strike"), WarriorDefinition.StarterAbilityIds[0], FName("Strike"));

	FEmbermereAbilityDefinition StrikeDefinition;
	TestTrue(TEXT("Strike ability definition exists"), Rules->GetAbilityDefinition("Strike", StrikeDefinition));
	TestEqual(TEXT("Strike belongs to Warrior"), StrikeDefinition.OwningClass, EEmbermereClass::Warrior);
	TestEqual(TEXT("Strike targets enemies"), StrikeDefinition.TargetKind, EEmbermereAbilityTargetKind::Enemy);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereLevelProgressionRulesTest,
	"Embermere.Progression.LevelRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLevelProgressionRulesTest::RunTest(const FString& Parameters)
{
	UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	if (!Rules)
	{
		AddError(TEXT("Could not create level progression rules fixture"));
		return false;
	}

	TestTrue(TEXT("Native progression definition is valid"), Rules->IsProgressionDefinitionValid());
	const TArray<TPair<int32, int32>> ThresholdCases = {
		{0, 1}, {99, 1}, {100, 2}, {249, 2}, {250, 3},
		{449, 3}, {450, 4}, {699, 4}, {700, 5}, {MAX_int32, 5}};
	for (const TPair<int32, int32>& ThresholdCase : ThresholdCases)
	{
		FEmbermereAttributeBlock Attributes;
		int32 Level = 0;
		TestTrue(
			*FString::Printf(TEXT("XP %d resolves"), ThresholdCase.Key),
			Rules->ResolveProgressionAttributes(
				EEmbermereRace::Human,
				EEmbermereClass::Warrior,
				ThresholdCase.Key,
				Attributes,
				Level));
		TestEqual(
			*FString::Printf(TEXT("XP %d derives exact level"), ThresholdCase.Key),
			Level,
			ThresholdCase.Value);
	}

	FEmbermereAttributeBlock HumanWarriorAttributes;
	FEmbermereAttributeBlock ElfWizardAttributes;
	int32 HumanWarriorLevel = 0;
	int32 ElfWizardLevel = 0;
	TestTrue(TEXT("Human Warrior level-two attributes resolve"),
		Rules->ResolveProgressionAttributes(
			EEmbermereRace::Human,
			EEmbermereClass::Warrior,
			100,
			HumanWarriorAttributes,
			HumanWarriorLevel));
	TestTrue(TEXT("Elf Wizard level-two attributes resolve"),
		Rules->ResolveProgressionAttributes(
			EEmbermereRace::Elf,
			EEmbermereClass::Wizard,
			100,
			ElfWizardAttributes,
			ElfWizardLevel));
	TestEqual(TEXT("Human Warrior reaches level two"), HumanWarriorLevel, 2);
	TestEqual(TEXT("Human Warrior gains combined health growth"), HumanWarriorAttributes.MaxHealth, 110.0f);
	TestEqual(TEXT("Human Warrior gains combined strength growth"), HumanWarriorAttributes.Strength, 12.0f);
	TestEqual(TEXT("Elf Wizard reaches level two"), ElfWizardLevel, 2);
	TestEqual(TEXT("Elf Wizard uses its distinct health growth"), ElfWizardAttributes.MaxHealth, 84.0f);
	TestEqual(TEXT("Elf Wizard uses its distinct mana growth"), ElfWizardAttributes.MaxMana, 122.0f);

	UEmbermereRulesData* NonMonotonic = DuplicateObject<UEmbermereRulesData>(Rules, GetTransientPackage());
	NonMonotonic->ExperienceThresholds = {0, 100, 100};
	TestFalse(TEXT("Duplicate thresholds invalidate progression"), NonMonotonic->IsProgressionDefinitionValid());
	FEmbermereAttributeBlock RejectedAttributes;
	int32 RejectedLevel = 0;
	TestFalse(TEXT("Malformed thresholds cannot resolve attributes"),
		NonMonotonic->ResolveProgressionAttributes(
			EEmbermereRace::Human,
			EEmbermereClass::Warrior,
			100,
			RejectedAttributes,
			RejectedLevel));

	UEmbermereRulesData* NegativeGrowth = DuplicateObject<UEmbermereRulesData>(Rules, GetTransientPackage());
	NegativeGrowth->Classes[0].LevelGrowth.Strength = -1.0f;
	TestFalse(TEXT("Negative class growth invalidates progression"), NegativeGrowth->IsProgressionDefinitionValid());
	UEmbermereRulesData* NonFiniteGrowth = DuplicateObject<UEmbermereRulesData>(Rules, GetTransientPackage());
	NonFiniteGrowth->Races[0].LevelGrowth.MaxHealth = std::numeric_limits<float>::infinity();
	TestFalse(TEXT("Non-finite race growth invalidates progression"), NonFiniteGrowth->IsProgressionDefinitionValid());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereLiveLevelProgressionTest,
	"Embermere.Progression.LiveExperienceAndEquipment",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLiveLevelProgressionTest::RunTest(const FString& Parameters)
{
	UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	if (!Rules || !Stats)
	{
		AddError(TEXT("Could not create live level progression fixtures"));
		return false;
	}

	FEmbermereProgressionProfile Profile;
	TestTrue(TEXT("Human Warrior progression profile resolves"),
		Rules->GetProgressionProfile(
			EEmbermereRace::Human,
			EEmbermereClass::Warrior,
			Profile));
	TestTrue(TEXT("Stats accepts the validated profile"), Stats->ConfigureProgression(Profile, 0, true));
	TestTrue(TEXT("Stats records configured progression"), Stats->IsProgressionConfigured());
	TestEqual(TEXT("Fresh profile starts at level one"), Stats->Level, 1);

	FEmbermereItemStatBonuses EquipmentBonuses;
	EquipmentBonuses.MaxHealth = 5.0f;
	EquipmentBonuses.Power = 2.0f;
	EquipmentBonuses.Armor = 1.0f;
	Stats->ApplyEquipmentBonuses(EquipmentBonuses);
	Stats->CurrentHealth = Stats->MaxHealth - 10.0f;
	TestTrue(TEXT("One grant may cross three thresholds"), Stats->TryAddExperience(450));
	TestEqual(TEXT("Multi-level grant reaches level four"), Stats->Level, 4);
	TestEqual(TEXT("Level-four health includes equipment once"), Stats->MaxHealth, 135.0f);
	TestEqual(TEXT("Level-up preserves absolute missing health"), Stats->CurrentHealth, 125.0f);
	TestEqual(TEXT("Level-four attack includes equipment once"), Stats->AttackPower, 18.0f);
	TestEqual(TEXT("Armor remains a single equipment layer"), Stats->Armor, 1.0f);

	TestTrue(TEXT("XP beyond the first cap remains safe"), Stats->TryAddExperience(1000));
	TestEqual(TEXT("Progression remains at the explicit level-five cap"), Stats->Level, 5);
	TestEqual(TEXT("Capped health still includes equipment once"), Stats->MaxHealth, 145.0f);
	Stats->ApplyEquipmentBonuses(EquipmentBonuses);
	TestEqual(TEXT("Reapplying identical equipment does not stack health"), Stats->MaxHealth, 145.0f);
	TestEqual(TEXT("Reapplying identical equipment does not stack power"), Stats->AttackPower, 20.0f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereExperienceProgressPresentationTest,
	"Embermere.UI.ExperienceProgressPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereExperienceProgressPresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	if (!Character || !Character->Stats || !Hud)
	{
		AddError(TEXT("Could not create experience presentation fixtures"));
		return false;
	}

	TestTrue(TEXT("Experience fixture confirms Human Warrior"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Hud->BindToCharacter(Character);
	TestEqual(TEXT("XP bar keeps fixed reviewed dimensions"), Hud->GetProgressionBarDimensions(), FVector2D(260.0f, 8.0f));
	TestEqual(TEXT("Fresh progress names the next authoritative threshold"),
		Hud->GetProgressionDisplayText().ToString(), FString(TEXT("Level 1   XP 0 / 100")));
	TestTrue(TEXT("Fresh level begins with an empty progress bar"), FMath::IsNearlyZero(Hud->GetProgressionPercent()));

	TestTrue(TEXT("Threshold-approach XP commits"), Character->Stats->TryAddExperience(99));
	TestEqual(TEXT("Threshold approach preserves the exact next total"),
		Hud->GetProgressionDisplayText().ToString(), FString(TEXT("Level 1   XP 99 / 100")));
	TestTrue(TEXT("Threshold approach reports exact within-level progress"),
		FMath::IsNearlyEqual(Hud->GetProgressionPercent(), 0.99f, KINDA_SMALL_NUMBER));

	TestTrue(TEXT("Exact threshold XP commits"), Character->Stats->TryAddExperience(1));
	TestEqual(TEXT("Exact threshold advances and exposes the following total"),
		Hud->GetProgressionDisplayText().ToString(), FString(TEXT("Level 2   XP 100 / 250")));
	TestTrue(TEXT("Exact threshold begins the new level at zero progress"),
		FMath::IsNearlyZero(Hud->GetProgressionPercent()));

	TestTrue(TEXT("Mid-level XP commits"), Character->Stats->TryAddExperience(50));
	TestTrue(TEXT("Level-two progress is normalized between rules-owned thresholds"),
		FMath::IsNearlyEqual(Hud->GetProgressionPercent(), 1.0f / 3.0f, KINDA_SMALL_NUMBER));
	TestEqual(TEXT("Mid-level copy keeps total XP and next threshold"),
		Hud->GetProgressionDisplayText().ToString(), FString(TEXT("Level 2   XP 150 / 250")));

	TestTrue(TEXT("Cap-reaching XP commits"), Character->Stats->TryAddExperience(550));
	TestEqual(TEXT("Cap copy is explicit and stable"),
		Hud->GetProgressionDisplayText().ToString(), FString(TEXT("Level 5   XP 700   CAP")));
	TestTrue(TEXT("Cap presentation reports a full bar"),
		FMath::IsNearlyEqual(Hud->GetProgressionPercent(), 1.0f, KINDA_SMALL_NUMBER));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereLevelUpPresentationTest,
	"Embermere.UI.LevelUpPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLevelUpPresentationTest::RunTest(const FString& Parameters)
{
	UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereLevelUpWidget* Widget = NewObject<UEmbermereLevelUpWidget>();
	if (!Rules || !Stats || !Widget)
	{
		AddError(TEXT("Could not create level-up presentation fixtures"));
		return false;
	}

	FEmbermereProgressionProfile Profile;
	TestTrue(TEXT("Level-up fixture resolves Human Warrior progression"),
		Rules->GetProgressionProfile(EEmbermereRace::Human, EEmbermereClass::Warrior, Profile));
	TestTrue(TEXT("Level-up fixture configures progression"), Stats->ConfigureProgression(Profile, 0, true));
	Widget->BindToStats(Stats);
	TestFalse(TEXT("Observer starts hidden"), Widget->IsLevelUpVisible());
	TestEqual(TEXT("Level-up panel keeps fixed reviewed dimensions"),
		Widget->GetPanelDimensions(), FVector2D(360.0f, 76.0f));
	TestTrue(TEXT("Level-up lifetime remains deterministic"),
		FMath::IsNearlyEqual(Widget->GetPresentationLifetimeSeconds(), 2.75f, KINDA_SMALL_NUMBER));

	Stats->RestoreExperienceForSaveGame(100);
	TestFalse(TEXT("Silent restore never replays level-up presentation"), Widget->IsLevelUpVisible());
	Stats->RestoreExperienceForSaveGame(0);
	TestFalse(TEXT("Silent rollback-style restore also remains presentation-free"), Widget->IsLevelUpVisible());

	TestTrue(TEXT("One live grant may cross three levels"), Stats->TryAddExperience(450));
	TestTrue(TEXT("Live level transition opens presentation"), Widget->IsLevelUpVisible());
	TestEqual(TEXT("Multi-level title reports the authoritative result"),
		Widget->GetLevelUpTitle().ToString(), FString(TEXT("LEVEL 4")));
	TestEqual(TEXT("Multi-level detail reports exact levels advanced and XP"),
		Widget->GetLevelUpDetail().ToString(), FString(TEXT("Advanced 3 levels  |  450 XP")));
	Widget->AdvancePresentation(2.74f);
	TestTrue(TEXT("Presentation remains until its full lifetime"), Widget->IsLevelUpVisible());
	Widget->AdvancePresentation(0.02f);
	TestFalse(TEXT("Presentation expires deterministically"), Widget->IsLevelUpVisible());

	Stats->RestoreExperienceForSaveGame(0);
	TestTrue(TEXT("Live cap grant commits"), Stats->TryAddExperience(700));
	TestEqual(TEXT("Cap title reports the final derived level"),
		Widget->GetLevelUpTitle().ToString(), FString(TEXT("LEVEL 5")));
	TestEqual(TEXT("Cap detail uses explicit cap copy"),
		Widget->GetLevelUpDetail().ToString(), FString(TEXT("700 XP  |  LEVEL CAP")));

	Widget->BindToStats(nullptr);
	TestFalse(TEXT("Unbinding tears down active presentation"), Widget->IsLevelUpVisible());
	Stats->RestoreExperienceForSaveGame(0);
	TestTrue(TEXT("Stats remain independently usable after observer teardown"), Stats->TryAddExperience(100));
	TestFalse(TEXT("Torn-down observer receives no later transition"), Widget->IsLevelUpVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereLevelUpWorldVfxPresentationTest,
	"Embermere.UI.LevelUpWorldVfxPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereLevelUpWorldVfxPresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!Character || !Character->Stats)
	{
		AddError(TEXT("Could not create level-up world VFX fixtures"));
		return false;
	}

	TestTrue(
		TEXT("World VFX fixture confirms Elf Wizard"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	Character->BindLevelUpWorldVfx();
	TestTrue(TEXT("World observer binds to the authoritative live transition"), Character->Stats->OnLevelChanged.IsBound());
	TestFalse(TEXT("World observer starts hidden"), Character->IsLevelUpWorldVfxVisible());
	TestEqual(TEXT("Transient geometry is deferred until a real level-up"), Character->GetLevelUpWorldVfxSegmentCount(), 0);

	Character->Stats->RestoreExperienceForSaveGame(100);
	TestFalse(TEXT("Silent restore never starts world VFX"), Character->IsLevelUpWorldVfxVisible());
	Character->Stats->RestoreExperienceForSaveGame(0);
	TestFalse(TEXT("Silent rollback-style restore remains presentation-free"), Character->IsLevelUpWorldVfxVisible());

	TestTrue(TEXT("One live grant may cross three levels for world VFX"), Character->Stats->TryAddExperience(450));
	TestEqual(TEXT("Authoritative live grant reaches level four"), Character->Stats->Level, 4);
	Character->HandleLevelChangedForWorldVfx(1, 4);
	TestTrue(TEXT("Post-commit level result starts world VFX"), Character->IsLevelUpWorldVfxVisible());
	TestEqual(TEXT("World VFX uses twelve stable segments"), Character->GetLevelUpWorldVfxSegmentCount(), 12);
	TestEqual(TEXT("Every world VFX segment becomes visible"), Character->GetVisibleLevelUpWorldVfxSegmentCount(), 12);
	TestEqual(TEXT("World VFX records the exact multi-level span"), Character->GetLevelUpWorldVfxLevelsGained(), 3);
	TestTrue(
		TEXT("World VFX uses the project-owned emissive material"),
		Character->GetLevelUpWorldVfxMaterialPath().Contains(TEXT("M_EmbermereTargetRing")));
	TestTrue(TEXT("World VFX geometry is presentation-only"), Character->AreLevelUpWorldVfxSegmentsNonColliding());
	TestTrue(
		TEXT("Wizard world VFX resolves to a cyan-blue class palette"),
		Character->GetLevelUpWorldVfxColor().B > Character->GetLevelUpWorldVfxColor().R &&
			Character->GetLevelUpWorldVfxColor().G > Character->GetLevelUpWorldVfxColor().R);
	TestTrue(
		TEXT("World VFX lifetime remains deterministic"),
		FMath::IsNearlyEqual(Character->GetLevelUpWorldVfxLifetimeSeconds(), 1.6f, KINDA_SMALL_NUMBER));
	const float StartingRadius = Character->GetLevelUpWorldVfxRadius();
	Character->AdvanceLevelUpWorldVfx(0.8f);
	TestTrue(TEXT("World VFX remains visible halfway through its lifetime"), Character->IsLevelUpWorldVfxVisible());
	TestTrue(TEXT("World VFX expands without growing layout or gameplay state"), Character->GetLevelUpWorldVfxRadius() > StartingRadius);
	Character->AdvanceLevelUpWorldVfx(0.79f);
	TestTrue(TEXT("World VFX remains until its full lifetime"), Character->IsLevelUpWorldVfxVisible());
	Character->AdvanceLevelUpWorldVfx(0.02f);
	TestFalse(TEXT("World VFX expires deterministically"), Character->IsLevelUpWorldVfxVisible());
	TestEqual(TEXT("Expiry hides every segment"), Character->GetVisibleLevelUpWorldVfxSegmentCount(), 0);

	Character->Stats->RestoreExperienceForSaveGame(0);
	Character->UnbindLevelUpWorldVfx();
	TestFalse(TEXT("World observer removes its authoritative delegate"), Character->Stats->OnLevelChanged.IsBound());
	TestTrue(TEXT("Stats remain independently usable after world observer teardown"), Character->Stats->TryAddExperience(100));
	TestFalse(TEXT("Torn-down world observer receives no later transition"), Character->IsLevelUpWorldVfxVisible());

	Character->Stats->RestoreExperienceForSaveGame(0);
	Character->BindLevelUpWorldVfx();
	TestTrue(TEXT("World observer may bind again without duplication"), Character->Stats->OnLevelChanged.IsBound());
	TestTrue(TEXT("Rebound observer receives a later live level-up"), Character->Stats->TryAddExperience(100));
	Character->HandleLevelChangedForWorldVfx(1, 2);
	TestTrue(TEXT("Rebound world observer becomes visible"), Character->IsLevelUpWorldVfxVisible());
	Character->Stats->ForceDeath();
	Character->AdvanceLevelUpWorldVfx(0.0f);
	TestFalse(TEXT("Death clears world VFX immediately"), Character->IsLevelUpWorldVfxVisible());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereProgressionRewardOwnersTest,
	"Embermere.Progression.RewardOwners",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereProgressionRewardOwnersTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereTrainerComponent* Trainer = NewObject<UEmbermereTrainerComponent>();
	UEmbermereTrainerOfferingsData* Offerings = NewObject<UEmbermereTrainerOfferingsData>();
	UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
	if (!Character || !Trainer || !Offerings || !Quest || !Character->Stats ||
		!Character->Wallet || !Character->QuestLog)
	{
		AddError(TEXT("Could not create progression reward-owner fixtures"));
		return false;
	}

	TestTrue(TEXT("Reward owner fixture confirms Human Warrior"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Character->Wallet->SetCopperForPrototype(40);
	FEmbermereTrainerOffering CombatDrills;
	CombatDrills.OfferingId = TEXT("CombatDrills");
	CombatDrills.DisplayName = FText::FromString(TEXT("Combat Drills"));
	CombatDrills.CopperCost = 10;
	CombatDrills.RequiredLevel = 1;
	CombatDrills.ExperienceReward = 25;
	Offerings->TrainerName = FText::FromString(TEXT("Fenwatch Training"));
	Offerings->Offerings.Add(CombatDrills);
	Trainer->SetOfferingsData(Offerings);
	TestEqual(TEXT("Trainer remains the owner of its atomic XP transaction"),
		Trainer->TryTrain(0, Character->Stats, Character->Wallet),
		EEmbermereTrainingResult::Success);
	TestEqual(TEXT("One Combat Drills grant remains level one"), Character->Stats->Level, 1);
	TestEqual(TEXT("Combat Drills grants exact XP"), Character->Stats->CurrentExperience, 25);

	Quest->QuestId = TEXT("ProgressionOwnerProof");
	Quest->Title = FText::FromString(TEXT("Progression Owner Proof"));
	Quest->ObjectiveId = TEXT("ProofComplete");
	Quest->RequiredObjectiveCount = 1;
	Quest->RewardExperience = 125;
	Quest->RewardCopper = 20;
	TestTrue(TEXT("Progression proof quest can be accepted"), Character->QuestLog->AcceptQuest(Quest));
	TestTrue(TEXT("Progression proof objective advances"),
		Character->QuestLog->AddObjectiveProgress(TEXT("ProofComplete"), 1));
	TestTrue(TEXT("Quest remains the owner of its completion XP"),
		Character->QuestLog->TryCompleteActiveQuest());
	TestEqual(TEXT("Trainer plus quest XP totals exactly 150"), Character->Stats->CurrentExperience, 150);
	TestEqual(TEXT("Quest XP crosses the level-two threshold"), Character->Stats->Level, 2);
	TestEqual(TEXT("Level-two Warrior health uses progression rules"), Character->Stats->MaxHealth, 110.0f);
	TestEqual(TEXT("Quest and trainer currency remains exact"), Character->Wallet->Copper, 50);
	TestFalse(TEXT("Completed quest cannot replay its level transition"),
		Character->QuestLog->TryCompleteActiveQuest());
	TestEqual(TEXT("Rejected replay preserves exact level"), Character->Stats->Level, 2);
	TestEqual(TEXT("Rejected replay preserves exact XP"), Character->Stats->CurrentExperience, 150);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereProgressionRollbackTest,
	"Embermere.Progression.ValidationRollback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereProgressionRollbackTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!Character || !Character->Stats || !Character->Hotbar)
	{
		AddError(TEXT("Could not create progression rollback fixture"));
		return false;
	}
	TestTrue(TEXT("Rollback fixture confirms Human Warrior"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Character->Stats->RestoreExperienceForSaveGame(100);
	const float BaselineHealth = Character->Stats->MaxHealth;
	const FName BaselineAbility = Character->Hotbar->Slots[0].AbilityId;

	UEmbermereRulesData* MalformedRules = NewObject<UEmbermereRulesData>(Character);
	MalformedRules->ExperienceThresholds = {0, 100, 90};
	Character->RulesData = MalformedRules;
	int32 CandidateLevel = 0;
	TestFalse(TEXT("Malformed rules reject save progression preflight"),
		Character->CanRestoreCharacterProgressionForSaveGame(
			EEmbermereRace::Elf,
			EEmbermereClass::Wizard,
			250,
			CandidateLevel));
	TestFalse(TEXT("Malformed rules reject progression commit"),
		Character->TryRestoreCharacterProgressionForSaveGame(
			EEmbermereRace::Elf,
			EEmbermereClass::Wizard,
			250));
	TestEqual(TEXT("Rejected progression preserves race"), Character->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Rejected progression preserves class"), Character->Class, EEmbermereClass::Warrior);
	TestEqual(TEXT("Rejected progression preserves derived level"), Character->Stats->Level, 2);
	TestEqual(TEXT("Rejected progression preserves health"), Character->Stats->MaxHealth, BaselineHealth);
	TestEqual(TEXT("Rejected progression preserves hotbar"), Character->Hotbar->Slots[0].AbilityId, BaselineAbility);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCharacterCreationInitialStateTest,
	"Embermere.UI.CharacterCreationInitialState",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCharacterCreationInitialStateTest::RunTest(const FString& Parameters)
{
	UEmbermereCharacterCreationWidget* Widget = NewObject<UEmbermereCharacterCreationWidget>();
	TestNotNull(TEXT("Native character creation widget can be created"), Widget);
	if (!Widget)
	{
		return false;
	}

	TestEqual(TEXT("Picker exposes all eight races"), Widget->GetRaceOptionCount(), 8);
	TestEqual(TEXT("Picker exposes all four starting classes"), Widget->GetClassOptionCount(), 4);
	TestEqual(TEXT("Picker keeps fixed reviewed dimensions"), Widget->GetPanelDimensions(), FVector2D(940.0f, 560.0f));
	TestEqual(TEXT("Picker starts on the reversible Human fallback"), Widget->SelectedRace, EEmbermereRace::Human);
	TestEqual(TEXT("Picker starts on the reversible Warrior fallback"), Widget->SelectedClass, EEmbermereClass::Warrior);
	TestTrue(TEXT("Initial Human Warrior choice is valid"), Widget->IsCurrentChoiceValid());
	TestFalse(TEXT("Initial choice has not been committed"), Widget->IsConfirmationComplete());
	const FString Summary = Widget->GetSelectionSummary().ToString();
	TestTrue(TEXT("Summary identifies the current race and class"), Summary.Contains(TEXT("Human Warrior")));
	TestTrue(TEXT("Summary exposes starter health and mana"), Summary.Contains(TEXT("Health 100")) && Summary.Contains(TEXT("Mana 50")));
	TestTrue(TEXT("Summary exposes data-driven starter abilities"), Summary.Contains(TEXT("Strike")) && Summary.Contains(TEXT("Battle Shout")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCharacterCreationRestrictionsTest,
	"Embermere.UI.CharacterCreationRestrictions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCharacterCreationRestrictionsTest::RunTest(const FString& Parameters)
{
	UEmbermereCharacterCreationWidget* Widget = NewObject<UEmbermereCharacterCreationWidget>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!Widget || !Character)
	{
		AddError(TEXT("Could not create character creation restriction fixtures"));
		return false;
	}

	TestTrue(TEXT("Human may stage Ranger before changing race"), Widget->SetPendingClass(EEmbermereClass::Ranger));
	TestTrue(TEXT("Race selection can change to Dwarf"), Widget->SetPendingRace(EEmbermereRace::Dwarf));
	TestEqual(TEXT("Race change never silently corrects the pending class"), Widget->SelectedClass, EEmbermereClass::Ranger);
	TestFalse(TEXT("Dwarf Ranger remains visibly invalid"), Widget->IsCurrentChoiceValid());
	TestFalse(TEXT("Dwarf Ranger cannot be confirmed"), Widget->TryConfirmChoice(Character));
	TestFalse(TEXT("Rejected Dwarf Ranger leaves the character unconfirmed"), Character->bHasDeliberateCharacterChoice);
	TestEqual(TEXT("Rejected choice preserves fallback race"), Character->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Rejected choice preserves fallback class"), Character->Class, EEmbermereClass::Warrior);

	TestTrue(TEXT("Human can stage Wizard"), Widget->SetPendingRace(EEmbermereRace::Human));
	TestTrue(TEXT("Wizard is available to Human"), Widget->SetPendingClass(EEmbermereClass::Wizard));
	TestTrue(TEXT("Race selection can change to Bullywug"), Widget->SetPendingRace(EEmbermereRace::Bullywug));
	TestFalse(TEXT("Bullywug Wizard remains disabled"), Widget->IsClassAvailable(EEmbermereClass::Wizard));
	TestFalse(TEXT("Bullywug Wizard cannot be confirmed"), Widget->TryConfirmChoice(Character));
	TestFalse(TEXT("Atomic pair setter also rejects Dwarf Ranger"), Widget->SetRaceAndClass(EEmbermereRace::Dwarf, EEmbermereClass::Ranger));
	TestTrue(TEXT("Dwarf Warrior remains a valid deliberate choice"), Widget->SetRaceAndClass(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
	TestTrue(TEXT("Dwarf Warrior can be confirmed after rejected pairs"), Widget->TryConfirmChoice(Character));
	TestEqual(
		TEXT("Dwarf Warrior publishes the current Chronicle identity"),
		Character->GetJourneyIdentitySummary().ToString(),
		FString(TEXT("Dwarf Warrior  |  Level 1")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCharacterCreationConfirmationLoadoutTest,
	"Embermere.CharacterCreation.ConfirmationLoadout",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCharacterCreationConfirmationLoadoutTest::RunTest(const FString& Parameters)
{
	UEmbermereCharacterCreationWidget* Widget = NewObject<UEmbermereCharacterCreationWidget>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!Widget || !Character || !Character->Stats || !Character->Hotbar)
	{
		AddError(TEXT("Could not create character creation confirmation fixtures"));
		return false;
	}

	Widget->BindToCharacter(Character);
	TestTrue(TEXT("Elf Wizard is a legal data-driven pair"), Widget->SetRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	TestTrue(TEXT("Valid choice confirms once"), Widget->TryConfirmChoice());
	TestTrue(TEXT("Character records a deliberate choice"), Character->bHasDeliberateCharacterChoice);
	TestEqual(TEXT("Confirmed race reaches character authority"), Character->Race, EEmbermereRace::Elf);
	TestEqual(TEXT("Confirmed class reaches character authority"), Character->Class, EEmbermereClass::Wizard);
	TestEqual(TEXT("Wizard starts with class health"), Character->Stats->MaxHealth, 80.0f);
	TestEqual(TEXT("Wizard starts with class mana"), Character->Stats->MaxMana, 110.0f);
	TestEqual(TEXT("Wizard Strength becomes base Attack Power"), Character->Stats->AttackPower, 6.0f);
	TestEqual(TEXT("Wizard Spirit is retained"), Character->Stats->Spirit, 12.0f);
	TestEqual(TEXT("Wizard Agility is retained"), Character->Stats->Agility, 8.0f);
	TestEqual(TEXT("Wizard Intellect is retained"), Character->Stats->Intellect, 16.0f);
	TestEqual(
		TEXT("Character publishes its current Chronicle identity"),
		Character->GetJourneyIdentitySummary().ToString(),
		FString(TEXT("Elf Wizard  |  Level 1")));
	TestEqual(TEXT("Confirmation fills health"), Character->Stats->CurrentHealth, 80.0f);
	TestEqual(TEXT("Confirmation fills mana"), Character->Stats->CurrentMana, 110.0f);
	const TArray<FName> ExpectedAbilities = {TEXT("SparkBolt"), TEXT("FrostRoot"), TEXT("ArcaneBurst"), TEXT("Meditate")};
	for (int32 Index = 0; Index < ExpectedAbilities.Num(); ++Index)
	{
		TestEqual(
			*FString::Printf(TEXT("Wizard hotbar slot %d uses class data"), Index + 1),
			Character->Hotbar->Slots[Index].AbilityId,
			ExpectedAbilities[Index]);
	}

	const float AcceptedHealth = Character->Stats->MaxHealth;
	const FName AcceptedFirstAbility = Character->Hotbar->Slots[0].AbilityId;
	TestFalse(TEXT("Widget rejects duplicate confirmation"), Widget->TryConfirmChoice());
	TestFalse(TEXT("Character authority rejects a second identity application"), Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestEqual(TEXT("Duplicate rejection preserves accepted health"), Character->Stats->MaxHealth, AcceptedHealth);
	TestEqual(TEXT("Duplicate rejection preserves accepted hotbar"), Character->Hotbar->Slots[0].AbilityId, AcceptedFirstAbility);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCharacterCreationControllerLifecycleTest,
	"Embermere.CharacterCreation.ControllerLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCharacterCreationControllerLifecycleTest::RunTest(const FString& Parameters)
{
	AEmbermerePlayerController* Controller = NewObject<AEmbermerePlayerController>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	if (!Controller || !Character)
	{
		AddError(TEXT("Could not create character creation lifecycle fixtures"));
		return false;
	}

	TestTrue(TEXT("Fresh unconfirmed character requests pre-play selection"), Controller->ShouldPresentCharacterCreation(Character));
	TestFalse(TEXT("Detached controller reports no visible modal"), Controller->IsCharacterCreationPanelVisible());
	Controller->bShowCharacterCreationOnFirstPlay = false;
	TestFalse(TEXT("Disabling the surface preserves the fallback start"), Controller->ShouldPresentCharacterCreation(Character));
	TestEqual(TEXT("Fallback remains Human"), Character->Race, EEmbermereRace::Human);
	TestEqual(TEXT("Fallback remains Warrior"), Character->Class, EEmbermereClass::Warrior);
	TestEqual(TEXT("Fallback health remains established baseline"), Character->Stats->MaxHealth, 100.0f);
	TestEqual(TEXT("Fallback mana remains established baseline"), Character->Stats->MaxMana, 50.0f);

	TestFalse(TEXT("Creation input starts unsuppressed"), Controller->bCharacterCreationInputSuppressed);
	Controller->SetCharacterCreationInputSuppressed(true);
	TestTrue(TEXT("Creation presentation suppresses movement"), Controller->IsMoveInputIgnored());
	TestTrue(TEXT("Creation presentation suppresses camera look"), Controller->IsLookInputIgnored());
	Controller->SetCharacterCreationInputSuppressed(true);
	Controller->SetCharacterCreationInputSuppressed(false);
	TestFalse(TEXT("Repeated presentation needs only one movement release"), Controller->IsMoveInputIgnored());
	TestFalse(TEXT("Repeated presentation needs only one look release"), Controller->IsLookInputIgnored());
	TestFalse(TEXT("Creation input ownership is released"), Controller->bCharacterCreationInputSuppressed);

	Controller->bShowCharacterCreationOnFirstPlay = true;
	TestTrue(TEXT("Valid deliberate fallback choice can still be confirmed"), Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestFalse(TEXT("Confirmed character never reopens first-play creation"), Controller->ShouldPresentCharacterCreation(Character));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatTargetSelectionPresentationTest,
	"Embermere.Combat.TargetSelectionPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatTargetSelectionPresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* FirstEnemy = NewObject<AEmbermereEnemyCharacter>();
	AEmbermereEnemyCharacter* SecondEnemy = NewObject<AEmbermereEnemyCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	TestNotNull(TEXT("First enemy can be created"), FirstEnemy);
	TestNotNull(TEXT("Second enemy can be created"), SecondEnemy);
	if (!Character || !FirstEnemy || !SecondEnemy)
	{
		return false;
	}

	TestNotNull(TEXT("Character has combat"), Character->Combat.Get());
	if (!Character->Combat)
	{
		return false;
	}

	TestFalse(TEXT("First enemy starts unselected"), FirstEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("Second enemy starts unselected"), SecondEnemy->IsSelectedByPlayer());
	TestTrue(TEXT("First enemy has a widget nameplate component"), FirstEnemy->HasNameplateWidget());
	TestEqual(TEXT("Enemy uses a smooth 48-segment target circle"), FirstEnemy->GetTargetRingSegmentCount(), 48);
	TestTrue(
		TEXT("Enemy target ring uses the Embermere emissive material"),
		FirstEnemy->GetTargetRingMaterialPath().Contains(TEXT("M_EmbermereTargetRing")));
	const FLinearColor TargetRingColor = FirstEnemy->GetTargetRingColor();
	TestTrue(TEXT("Target ring keeps red restrained"), TargetRingColor.R < 0.1f);
	TestTrue(TEXT("Target ring uses a strong cyan green channel"), TargetRingColor.G > 0.6f);
	TestTrue(TEXT("Target ring uses a bright blue channel"), TargetRingColor.B > 0.9f);
	TestTrue(TEXT("Target ring segments overlap into a complete circle"), FirstEnemy->TargetRingArcCoverage >= 1.0f);
	TestTrue(TEXT("Target ring uses a restrained pulse"), FirstEnemy->TargetRingPulseAmount <= 0.05f);
	TestTrue(
		TEXT("Target ring clears the prototype terrain surface"),
		FirstEnemy->GetEffectiveTargetRingSurfaceClearance() >= 16.0f);
	TestEqual(TEXT("Complete target circle does not rotate"), FirstEnemy->TargetRingRotationSpeedDegreesPerSecond, 0.0f);
	TestTrue(TEXT("Target ring remains presentation-only collision"), FirstEnemy->AreTargetRingSegmentsNonColliding());
	TestFalse(TEXT("Target ring starts hidden"), FirstEnemy->IsTargetRingVisible());
	TestTrue(
		TEXT("Enemy target presentation includes name and HP"),
		FirstEnemy->GetTargetPresentationText().ToString().Contains(TEXT("Marsh Prowler\nHP 100/100")));
	const FLinearColor HealthyPresentationColor = FirstEnemy->GetTargetPresentationColor();
	FirstEnemy->Stats->ApplyDamage(80.0f);
	const FLinearColor LowHealthPresentationColor = FirstEnemy->GetTargetPresentationColor();
	TestTrue(TEXT("Enemy target presentation color changes at low health"), LowHealthPresentationColor.R >= HealthyPresentationColor.R);
	TestTrue(TEXT("Enemy low-health presentation color has less green"), LowHealthPresentationColor.G < HealthyPresentationColor.G);

	Character->Combat->SetTarget(FirstEnemy);
	TestTrue(TEXT("First enemy becomes current target"), Character->Combat->CurrentTarget == FirstEnemy);
	TestTrue(TEXT("First enemy presentation is selected"), FirstEnemy->IsSelectedByPlayer());
	TestTrue(TEXT("First enemy target circle appears on selection"), FirstEnemy->IsTargetRingVisible());
	TestFalse(TEXT("Second enemy remains unselected"), SecondEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("Second enemy target circle remains hidden"), SecondEnemy->IsTargetRingVisible());

	Character->Combat->SetTarget(SecondEnemy);
	TestTrue(TEXT("Second enemy becomes current target"), Character->Combat->CurrentTarget == SecondEnemy);
	TestFalse(TEXT("First enemy presentation clears when target changes"), FirstEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("First enemy target circle clears when target changes"), FirstEnemy->IsTargetRingVisible());
	TestTrue(TEXT("Second enemy presentation is selected"), SecondEnemy->IsSelectedByPlayer());
	TestTrue(TEXT("Second enemy target circle appears when selected"), SecondEnemy->IsTargetRingVisible());

	Character->Combat->SetTarget(nullptr);
	TestNull(TEXT("Current target clears when target is cleared"), Character->Combat->CurrentTarget.Get());
	TestFalse(TEXT("Second enemy presentation clears when target clears"), SecondEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("Second enemy target circle clears with the target"), SecondEnemy->IsTargetRingVisible());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatResultContractTest,
	"Embermere.Combat.ResultContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatResultContractTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	if (!Character || !Enemy || !Character->Combat || !Character->Stats || !Enemy->Stats)
	{
		AddError(TEXT("Could not create combat-result fixtures"));
		return false;
	}

	Character->Stats->InitializeVitals();
	Enemy->Stats->InitializeVitals();
	Enemy->Stats->Armor = 100.0f;
	TArray<FEmbermereCombatResult> Results;
	const FDelegateHandle ResultHandle = Character->Combat->OnCombatResult.AddLambda(
		[&Results](const FEmbermereCombatResult& Result)
		{
			Results.Add(Result);
		});

	FEmbermereAbilityDefinition Strike;
	Strike.AbilityId = TEXT("ResultContractStrike");
	Strike.DisplayName = FText::FromString(TEXT("Contract Strike"));
	Strike.TargetKind = EEmbermereAbilityTargetKind::Enemy;
	Strike.EffectType = EEmbermereAbilityEffectType::Damage;
	Strike.Power = 18.0f;
	Strike.Range = 0.0f;
	Character->Combat->SetTarget(Enemy);

	TestTrue(TEXT("Committed damage executes"), Character->Combat->ExecuteAbility(Strike));
	TestEqual(TEXT("One committed outcome publishes one result"), Results.Num(), 1);
	if (Results.IsValidIndex(0))
	{
		TestEqual(TEXT("Result preserves ability identity"), Results[0].AbilityId, Strike.AbilityId);
		TestTrue(TEXT("Result preserves source identity"), Results[0].Source == Character);
		TestTrue(TEXT("Result preserves target identity"), Results[0].Target == Enemy);
		TestEqual(TEXT("Result classifies authoritative damage"), Results[0].Kind, EEmbermereCombatResultKind::Damage);
		TestEqual(TEXT("Result reports post-armor applied damage"), Results[0].AppliedAmount, 14.0f);
		TestFalse(TEXT("First result is not lethal"), Results[0].bTargetDefeated);
	}

	Enemy->Stats->CurrentHealth = 10.0f;
	TestTrue(TEXT("Lethal committed damage executes"), Character->Combat->ExecuteAbility(Strike));
	TestEqual(TEXT("Second committed outcome publishes once"), Results.Num(), 2);
	if (Results.IsValidIndex(1))
	{
		TestEqual(TEXT("Lethal result reports clamped applied damage"), Results[1].AppliedAmount, 10.0f);
		TestTrue(TEXT("Lethal result carries defeated state"), Results[1].bTargetDefeated);
	}
	TestNull(TEXT("Existing lethal targeting behavior remains authoritative"), Character->Combat->CurrentTarget.Get());
	TestFalse(TEXT("Rejected ability does not publish a result"), Character->Combat->ExecuteAbility(Strike));
	TestEqual(TEXT("Rejected ability leaves result count unchanged"), Results.Num(), 2);

	Character->Combat->OnCombatResult.Remove(ResultHandle);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatFeedbackPresentationTest,
	"Embermere.UI.CombatFeedbackPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatFeedbackPresentationTest::RunTest(const FString& Parameters)
{
	UEmbermereCombatFeedbackWidget* Feedback = NewObject<UEmbermereCombatFeedbackWidget>();
	AActor* Target = NewObject<AActor>();
	if (!Feedback || !Target)
	{
		AddError(TEXT("Could not create combat-feedback fixtures"));
		return false;
	}

	Feedback->TakeWidget();
	TestEqual(TEXT("Feedback uses exactly three bounded entries"), Feedback->GetMaximumFeedbackEntries(), 3);
	TestEqual(TEXT("Feedback slots have stable dimensions"), Feedback->GetFeedbackSlotDimensions(), FVector2D(112.0f, 32.0f));
	TestEqual(TEXT("Feedback uses a short deterministic lifetime"), Feedback->GetFeedbackLifetimeSeconds(), 1.25f);

	FEmbermereCombatResult Result;
	Result.Target = Target;
	Result.Kind = EEmbermereCombatResultKind::Damage;
	Result.AppliedAmount = 12.0f;
	Feedback->PresentCombatResult(Result);
	TestEqual(TEXT("Damage adds one feedback entry"), Feedback->GetActiveFeedbackCount(), 1);
	TestEqual(TEXT("Damage shows the exact rounded amount"), Feedback->GetFeedbackDisplayText(0).ToString(), FString(TEXT("12")));

	Result.Kind = EEmbermereCombatResultKind::Miss;
	Result.AppliedAmount = 0.0f;
	Feedback->PresentCombatResult(Result);
	TestEqual(TEXT("Future authoritative misses use the same observer"), Feedback->GetFeedbackDisplayText(0).ToString(), FString(TEXT("MISS")));

	Result.Kind = EEmbermereCombatResultKind::Damage;
	Result.AppliedAmount = 34.0f;
	Feedback->PresentCombatResult(Result);
	Result.AppliedAmount = 56.0f;
	Feedback->PresentCombatResult(Result);
	TestEqual(TEXT("Rapid outcomes never grow beyond fixed slots"), Feedback->GetActiveFeedbackCount(), 3);
	TestEqual(TEXT("Newest rapid outcome appears first"), Feedback->GetFeedbackDisplayText(0).ToString(), FString(TEXT("56")));
	TestEqual(TEXT("Second rapid outcome remains ordered"), Feedback->GetFeedbackDisplayText(1).ToString(), FString(TEXT("34")));
	TestEqual(TEXT("Oldest overflow outcome is evicted"), Feedback->GetFeedbackDisplayText(2).ToString(), FString(TEXT("MISS")));

	FEmbermereCombatResult HealingResult;
	HealingResult.Target = Target;
	HealingResult.Kind = EEmbermereCombatResultKind::Healing;
	HealingResult.AppliedAmount = 20.0f;
	Feedback->PresentCombatResult(HealingResult);
	TestEqual(TEXT("Resource and status results do not duplicate existing HUD rows"), Feedback->GetActiveFeedbackCount(), 3);

	Feedback->ClearForTarget(Target);
	TestEqual(TEXT("Target clearing removes every related entry"), Feedback->GetActiveFeedbackCount(), 0);
	Result.AppliedAmount = 28.0f;
	Feedback->PresentCombatResult(Result);
	Feedback->AdvancePresentation(Feedback->GetFeedbackLifetimeSeconds());
	TestEqual(TEXT("Lifetime expiry clears feedback deterministically"), Feedback->GetActiveFeedbackCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePracticeTargetPolicyTest,
	"Embermere.Combat.PracticeTargetPolicy",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePracticeTargetPolicyTest::RunTest(const FString& Parameters)
{
	AEmbermerePracticeTargetActor* PracticeTarget = NewObject<AEmbermerePracticeTargetActor>();
	AEmbermereEnemyCharacter* OrdinaryEnemy = NewObject<AEmbermereEnemyCharacter>();
	AEmbermereCharacter* Viewer = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Practice target can be created"), PracticeTarget);
	TestNotNull(TEXT("Ordinary enemy can be created"), OrdinaryEnemy);
	TestNotNull(TEXT("Practice-target viewer can be created"), Viewer);
	if (!PracticeTarget || !OrdinaryEnemy || !Viewer || !PracticeTarget->Stats)
	{
		return false;
	}

	TestTrue(
		TEXT("Practice target remains eligible for tab targeting"),
		PracticeTarget->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()));
	const FVector FeedbackAnchor =
		EmbermereTargetableDispatch::GetCombatFeedbackAnchorLocation(PracticeTarget);
	TestTrue(TEXT("Practice target feedback anchor remains finite"), !FeedbackAnchor.ContainsNaN());
	TestTrue(TEXT("Practice target feedback anchor clears the dummy body"), FeedbackAnchor.Z >= 200.0f);
	TestTrue(TEXT("Practice target feedback anchor ignores screen-space nameplate bounds"), FeedbackAnchor.Z < 500.0f);
	TestTrue(
		TEXT("Practice target is selectable through the hostile target lane"),
		EmbermereTargetableDispatch::IsHostileTo(PracticeTarget, Viewer));
	TestFalse(
		TEXT("Practice target never grants defeat credit"),
		EmbermereTargetableDispatch::ShouldGrantDefeatCredit(PracticeTarget));
	TestTrue(
		TEXT("Ordinary enemies retain defeat credit"),
		EmbermereTargetableDispatch::ShouldGrantDefeatCredit(OrdinaryEnemy));
	TestEqual(
		TEXT("Practice target exposes its readable name"),
		EmbermereTargetableDispatch::GetDisplayName(PracticeTarget).ToString(),
		FString(TEXT("Fenwatch Practice Target")));

	TestFalse(TEXT("Practice target owns no prototype AI"), PracticeTarget->bPrototypeAiEnabled);
	TestFalse(TEXT("Practice target owns no loot policy"), PracticeTarget->bLootEnabled);
	TestFalse(TEXT("Practice target owns no gameplay collision"), PracticeTarget->bGameplayCollisionEnabled);
	TestFalse(TEXT("Practice target does not trace against its separate art"), PracticeTarget->bTraceTargetRingSurface);
	TestEqual(TEXT("Practice target cannot retaliate"), PracticeTarget->AttackDamage, 0.0f);
	TestEqual(TEXT("Practice target cannot chase"), PracticeTarget->GetEffectiveMoveSpeedCmPerSecond(), 0.0f);
	TestEqual(TEXT("Practice target resets quickly"), PracticeTarget->RespawnDelaySeconds, 3.0f);
	TestFalse(TEXT("Practice target cannot roll loot"), PracticeTarget->ShouldDropLoot(0.0f));
	TestFalse(TEXT("Practice target cannot grant loot"), PracticeTarget->GrantLootTo(Viewer));

	TestNotNull(TEXT("Practice target retains health authority"), PracticeTarget->Stats.Get());
	TestEqual(TEXT("Practice target has a durable training health pool"), PracticeTarget->Stats->MaxHealth, 150.0f);
	TestTrue(TEXT("Practice target has no configured skeletal art"), PracticeTarget->VisualSkeletalMesh.IsNull());
	TestNull(TEXT("Practice target renders no inherited skeletal mesh"), PracticeTarget->GetMesh()->GetSkeletalMeshAsset());
	TestNull(TEXT("Practice target creates no interactable authority"), PracticeTarget->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(TEXT("Practice target creates no trainer authority"), PracticeTarget->FindComponentByClass<UEmbermereTrainerComponent>());
	TestNull(TEXT("Practice target creates no vendor authority"), PracticeTarget->FindComponentByClass<UEmbermereVendorComponent>());
	TestEqual(
		TEXT("Practice target capsule stays non-colliding"),
		PracticeTarget->GetCapsuleComponent()->GetCollisionEnabled(),
		ECollisionEnabled::NoCollision);
	TestNotNull(TEXT("Practice target retains a movement component"), PracticeTarget->GetCharacterMovement());
	TestEqual(TEXT("Practice target gravity is disabled"), PracticeTarget->GetCharacterMovement()->GravityScale, 0.0f);
	TestEqual(
		TEXT("Practice target stays fixed at its authored transform"),
		PracticeTarget->GetCharacterMovement()->MovementMode,
		MOVE_None);
	TestTrue(TEXT("Practice target reuses the accepted nameplate"), PracticeTarget->HasNameplateWidget());
	TestEqual(TEXT("Practice target reuses the complete cyan target circle"), PracticeTarget->GetTargetRingSegmentCount(), 48);
	TestTrue(TEXT("Practice target circle clears the dummy footprint"), PracticeTarget->TargetRingRadius >= 150.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePracticeTargetCombatResetTest,
	"Embermere.Combat.PracticeTargetCombatReset",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePracticeTargetCombatResetTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermerePracticeTargetActor* PracticeTarget = NewObject<AEmbermerePracticeTargetActor>();
	UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
	if (!Character || !PracticeTarget || !Quest || !Character->Combat ||
		!Character->Stats || !Character->QuestLog || !Character->Inventory ||
		!PracticeTarget->Stats)
	{
		AddError(TEXT("Could not create practice-target combat fixtures"));
		return false;
	}

	Quest->QuestId = TEXT("PracticeTargetCreditExclusion");
	Quest->Title = FText::FromString(TEXT("Do Not Count the Dummy"));
	Quest->ObjectiveId = TEXT("StarterEnemyDefeated");
	Quest->RequiredObjectiveCount = 1;
	Character->Stats->InitializeVitals();
	PracticeTarget->Stats->InitializeVitals();
	TestTrue(TEXT("Credit-exclusion quest can be accepted"), Character->QuestLog->AcceptQuest(Quest));

	FEmbermereAbilityDefinition TrainingSnare;
	TrainingSnare.AbilityId = TEXT("TrainingSnare");
	TrainingSnare.DisplayName = FText::FromString(TEXT("Training Snare"));
	TrainingSnare.TargetKind = EEmbermereAbilityTargetKind::Enemy;
	TrainingSnare.EffectType = EEmbermereAbilityEffectType::Damage;
	TrainingSnare.Power = 1.0f;
	TrainingSnare.Range = 225.0f;
	TrainingSnare.Duration = 6.0f;
	TrainingSnare.MovementSpeedMultiplier = 0.5f;

	Character->Combat->SetTarget(PracticeTarget);
	TestTrue(TEXT("Practice target can be selected for combat"), Character->Combat->CurrentTarget == PracticeTarget);
	TestTrue(TEXT("Practice target shows its cyan circle while selected"), PracticeTarget->IsTargetRingVisible());
	TestTrue(TEXT("Control abilities can affect the practice target"), Character->Combat->ExecuteAbility(TrainingSnare));
	TestEqual(TEXT("Practice target accepts damage"), PracticeTarget->Stats->CurrentHealth, 139.0f);
	TestEqual(TEXT("Practice target accepts control-effect state"), PracticeTarget->Stats->GetMovementSpeedMultiplier(), 0.5f);

	PracticeTarget->ResetPracticeTarget();
	TestEqual(TEXT("Manual training reset restores full health"), PracticeTarget->Stats->CurrentHealth, 150.0f);
	TestEqual(TEXT("Manual training reset clears control effects"), PracticeTarget->Stats->GetMovementSpeedMultiplier(), 1.0f);
	TestTrue(TEXT("Manual training reset preserves an active selection"), PracticeTarget->IsTargetRingVisible());
	TestEqual(
		TEXT("Manual training reset preserves no collision"),
		PracticeTarget->GetCapsuleComponent()->GetCollisionEnabled(),
		ECollisionEnabled::NoCollision);
	TestEqual(
		TEXT("Manual training reset preserves the stationary movement mode"),
		PracticeTarget->GetCharacterMovement()->MovementMode,
		MOVE_None);

	FEmbermereAbilityDefinition FinishingStrike;
	FinishingStrike.AbilityId = TEXT("PracticeFinisher");
	FinishingStrike.DisplayName = FText::FromString(TEXT("Practice Finisher"));
	FinishingStrike.TargetKind = EEmbermereAbilityTargetKind::Enemy;
	FinishingStrike.EffectType = EEmbermereAbilityEffectType::Damage;
	FinishingStrike.Power = 200.0f;
	FinishingStrike.Range = 225.0f;
	Character->Combat->SetTarget(PracticeTarget);
	TestTrue(TEXT("Practice target can be defeated"), Character->Combat->ExecuteAbility(FinishingStrike));
	TestTrue(TEXT("Practice target reaches its defeated state"), PracticeTarget->Stats->IsDead());
	TestNull(TEXT("Defeating the practice target clears combat target"), Character->Combat->CurrentTarget.Get());
	TestEqual(TEXT("Practice defeat grants no quest progress"), Character->QuestLog->ActiveQuest.CurrentObjectiveCount, 0);
	TestEqual(TEXT("Practice defeat grants no XP"), Character->Stats->CurrentExperience, 0);
	TestEqual(TEXT("Practice defeat grants no inventory loot"), Character->Inventory->Stacks.Num(), 0);

	PracticeTarget->ResetPracticeTarget();
	TestTrue(TEXT("Practice target is alive for another training cycle"), PracticeTarget->IsAlive_Implementation());
	TestEqual(TEXT("Post-defeat reset restores exact health"), PracticeTarget->Stats->CurrentHealth, 150.0f);
	TestEqual(TEXT("Repeated reset still grants no quest progress"), Character->QuestLog->ActiveQuest.CurrentObjectiveCount, 0);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereStarterAbilityEffectsTest,
	"Embermere.Combat.StarterAbilityEffects",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStarterAbilityEffectsTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = LoadObject<UEmbermereRulesData>(
		nullptr,
		TEXT("/Game/Data/DA_EmbermereRules.DA_EmbermereRules"));
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	TestNotNull(TEXT("Saved rules data resolves"), Rules);
	TestNotNull(TEXT("Character can be created for starter effects"), Character);
	TestNotNull(TEXT("Enemy can be created for starter effects"), Enemy);
	if (!Rules || !Character || !Enemy || !Character->Combat || !Character->Stats || !Enemy->Stats)
	{
		return false;
	}

	auto GetAbility = [this, Rules](FName AbilityId, FEmbermereAbilityDefinition& OutAbility)
	{
		const bool bFound = Rules->GetAbilityDefinition(AbilityId, OutAbility);
		TestTrue(*FString::Printf(TEXT("%s resolves from saved rules"), *AbilityId.ToString()), bFound);
		return bFound;
	};

	FEmbermereAbilityDefinition BattleShout;
	FEmbermereAbilityDefinition Ward;
	FEmbermereAbilityDefinition LesserHeal;
	FEmbermereAbilityDefinition Snare;
	FEmbermereAbilityDefinition NaturesFocus;
	FEmbermereAbilityDefinition FrostRoot;
	FEmbermereAbilityDefinition Meditate;
	if (!GetAbility("BattleShout", BattleShout) ||
		!GetAbility("Ward", Ward) ||
		!GetAbility("LesserHeal", LesserHeal) ||
		!GetAbility("Snare", Snare) ||
		!GetAbility("NaturesFocus", NaturesFocus) ||
		!GetAbility("FrostRoot", FrostRoot) ||
		!GetAbility("Meditate", Meditate))
	{
		return false;
	}

	TestEqual(TEXT("Battle Shout is data-driven attack power"), BattleShout.EffectType, EEmbermereAbilityEffectType::AttackPowerBuff);
	TestEqual(TEXT("Battle Shout lasts ten seconds"), BattleShout.Duration, 10.0f);
	TestEqual(TEXT("Ward is data-driven armor"), Ward.EffectType, EEmbermereAbilityEffectType::ArmorBuff);
	TestEqual(TEXT("Lesser Heal uses the healing effect"), LesserHeal.EffectType, EEmbermereAbilityEffectType::Heal);
	TestEqual(TEXT("Snare uses a half-speed multiplier"), Snare.MovementSpeedMultiplier, 0.5f);
	TestEqual(TEXT("Snare lasts six seconds"), Snare.Duration, 6.0f);
	TestEqual(TEXT("Snare targets an enemy"), Snare.TargetKind, EEmbermereAbilityTargetKind::Enemy);
	TestEqual(TEXT("Snare keeps its eight-meter range"), Snare.Range, 800.0f);
	TestEqual(TEXT("Nature's Focus is data-driven attack power"), NaturesFocus.EffectType, EEmbermereAbilityEffectType::AttackPowerBuff);
	TestEqual(TEXT("Frost Root stops movement"), FrostRoot.MovementSpeedMultiplier, 0.0f);
	TestEqual(TEXT("Frost Root lasts four seconds"), FrostRoot.Duration, 4.0f);
	TestEqual(TEXT("Frost Root targets an enemy"), FrostRoot.TargetKind, EEmbermereAbilityTargetKind::Enemy);
	TestEqual(TEXT("Frost Root keeps its eight-meter range"), FrostRoot.Range, 800.0f);
	TestEqual(TEXT("Meditate restores mana"), Meditate.EffectType, EEmbermereAbilityEffectType::RestoreMana);

	Character->Stats->InitializeVitals();
	TestTrue(TEXT("Battle Shout activates"), Character->Combat->ExecuteAbility(BattleShout));
	TestEqual(TEXT("Battle Shout preserves base attack power"), Character->Stats->AttackPower, 10.0f);
	TestEqual(TEXT("Battle Shout grants eight effective attack power"), Character->Stats->GetEffectiveAttackPower(), 18.0f);
	Character->Stats->ClearTemporaryEffects();
	TestEqual(TEXT("Clearing Battle Shout restores effective attack power"), Character->Stats->GetEffectiveAttackPower(), 10.0f);

	TestTrue(TEXT("Nature's Focus activates through the same effect contract"), Character->Combat->ExecuteAbility(NaturesFocus));
	TestEqual(TEXT("Nature's Focus grants eight effective attack power"), Character->Stats->GetEffectiveAttackPower(), 18.0f);
	Character->Stats->ClearTemporaryEffects();

	Character->Stats->InitializeVitals();
	TestTrue(TEXT("Ward activates"), Character->Combat->ExecuteAbility(Ward));
	TestEqual(TEXT("Ward preserves base armor"), Character->Stats->Armor, 0.0f);
	TestEqual(TEXT("Ward grants ten effective armor"), Character->Stats->GetEffectiveArmor(), 10.0f);
	TestTrue(
		TEXT("Ward mitigates damage through effective armor"),
		FMath::IsNearlyEqual(Character->Stats->ApplyDamage(11.0f), 10.0f, 0.01f));
	Character->Stats->ClearTemporaryEffects();

	Character->Stats->InitializeVitals();
	TestTrue(TEXT("Mana can be spent before Meditate"), Character->Stats->SpendMana(20.0f));
	TestTrue(TEXT("Meditate activates"), Character->Combat->ExecuteAbility(Meditate));
	TestEqual(TEXT("Meditate restores eighteen mana"), Character->Stats->CurrentMana, 48.0f);

	Character->Stats->InitializeVitals();
	Enemy->Stats->InitializeVitals();
	Character->Combat->SetTarget(Enemy);
	TestTrue(TEXT("Enemy is alive before Snare"), Enemy->IsAlive_Implementation());
	TestTrue(
		TEXT("Enemy class exposes the targetable interface"),
		Enemy->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()));
	TestNotNull(TEXT("Enemy exposes its native targetable implementation"), Cast<IEmbermereTargetable>(Enemy));
	TestNotNull(
		TEXT("Enemy stats resolve through actor component lookup"),
		Enemy->FindComponentByClass<UEmbermereStatsComponent>());
	TestTrue(TEXT("Character is alive before Snare"), !Character->Stats->IsDead());
	TestTrue(TEXT("Snare target is retained"), Character->Combat->CurrentTarget == Enemy);
	TestTrue(TEXT("Snare target is in range"), Character->Combat->IsTargetInRange(Snare.Range));
	TestTrue(TEXT("Character has enough mana for Snare"), Character->Stats->CurrentMana >= Snare.ManaCost);
	TestTrue(TEXT("Snare activates on a live target"), Character->Combat->ExecuteAbility(Snare));
	TestEqual(TEXT("Snare applies its light damage plus base attack power"), Enemy->Stats->CurrentHealth, 82.0f);
	TestEqual(TEXT("Snare halves movement"), Enemy->Stats->GetMovementSpeedMultiplier(), 0.5f);
	TestTrue(
		TEXT("Enemy chase speed consumes Snare"),
		FMath::IsNearlyEqual(Enemy->GetEffectiveMoveSpeedCmPerSecond(), Enemy->MoveSpeedCmPerSecond * 0.5f));

	Character->Stats->InitializeVitals();
	Enemy->Stats->InitializeVitals();
	Enemy->Stats->ClearTemporaryEffects();
	TestTrue(TEXT("Frost Root activates on a live target"), Character->Combat->ExecuteAbility(FrostRoot));
	TestEqual(TEXT("Frost Root stops movement"), Enemy->Stats->GetMovementSpeedMultiplier(), 0.0f);
	TestEqual(TEXT("Rooted enemy chase speed is zero"), Enemy->GetEffectiveMoveSpeedCmPerSecond(), 0.0f);

	Enemy->Stats->InitializeVitals();
	TestEqual(TEXT("Respawn-style vital initialization clears control effects"), Enemy->Stats->GetMovementSpeedMultiplier(), 1.0f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereTimedStatusPresentationTest,
	"Embermere.UI.TimedStatusPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereTimedStatusPresentationTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = LoadObject<UEmbermereRulesData>(
		nullptr,
		TEXT("/Game/Data/DA_EmbermereRules.DA_EmbermereRules"));
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("Saved rules resolve for timed statuses"), Rules);
	TestNotNull(TEXT("Character can be created for timed statuses"), Character);
	TestNotNull(TEXT("Enemy can be created for timed statuses"), Enemy);
	TestNotNull(TEXT("HUD can be created for timed statuses"), HudWidget);
	if (!Rules || !Character || !Enemy || !HudWidget ||
		!Character->Combat || !Character->Stats || !Enemy->Stats)
	{
		return false;
	}

	FEmbermereAbilityDefinition BattleShout;
	FEmbermereAbilityDefinition Snare;
	TestTrue(
		TEXT("Battle Shout resolves for timed status presentation"),
		Rules->GetAbilityDefinition("BattleShout", BattleShout));
	TestTrue(
		TEXT("Snare resolves for timed status presentation"),
		Rules->GetAbilityDefinition("Snare", Snare));

	Character->Stats->InitializeVitals();
	HudWidget->BindToCharacter(Character);
	TestTrue(TEXT("Battle Shout activates for timed status presentation"), Character->Combat->ExecuteAbility(BattleShout));
	const TArray<FEmbermereActiveStatusEffect> PlayerEffects = Character->Stats->GetActiveStatusEffects();
	TestEqual(TEXT("Battle Shout creates one active player status"), PlayerEffects.Num(), 1);
	if (PlayerEffects.IsValidIndex(0))
	{
		TestEqual(TEXT("Player status preserves the source ability"), PlayerEffects[0].Ability.AbilityId, FName("BattleShout"));
		TestTrue(TEXT("Battle Shout status is beneficial"), PlayerEffects[0].bBeneficial);
		TestTrue(TEXT("Battle Shout status preserves its saved icon"), !PlayerEffects[0].Ability.Icon.IsNull());
		TestTrue(
			TEXT("Battle Shout status exposes a live countdown"),
			PlayerEffects[0].RemainingSeconds > 9.0f && PlayerEffects[0].RemainingSeconds <= 10.0f);
	}
	TestEqual(TEXT("HUD sees one player status"), HudWidget->GetPlayerStatusEffectCount(), 1);
	TestTrue(
		TEXT("Player status copy includes ability name and countdown"),
		HudWidget->GetPlayerStatusEffectDisplayText(0).ToString().Contains(TEXT("Battle Shout")) &&
			HudWidget->GetPlayerStatusEffectDisplayText(0).ToString().Contains(TEXT("10s")));
	TestTrue(
		TEXT("Reapplying a status refreshes its existing record"),
		Character->Stats->RegisterTimedStatusEffect(BattleShout, true));
	TestEqual(
		TEXT("Reapplying a status does not duplicate its presentation"),
		Character->Stats->GetActiveStatusEffects().Num(),
		1);

	Enemy->Stats->InitializeVitals();
	Character->Combat->SetTarget(Enemy);
	TestTrue(TEXT("Snare activates for target status presentation"), Character->Combat->ExecuteAbility(Snare));
	const TArray<FEmbermereActiveStatusEffect> TargetEffects = Enemy->Stats->GetActiveStatusEffects();
	TestEqual(TEXT("Snare creates one active target status"), TargetEffects.Num(), 1);
	if (TargetEffects.IsValidIndex(0))
	{
		TestEqual(TEXT("Target status preserves the source ability"), TargetEffects[0].Ability.AbilityId, FName("Snare"));
		TestFalse(TEXT("Snare status is harmful"), TargetEffects[0].bBeneficial);
		TestTrue(
			TEXT("Snare status exposes a live countdown"),
			TargetEffects[0].RemainingSeconds > 5.0f && TargetEffects[0].RemainingSeconds <= 6.0f);
	}
	TestEqual(TEXT("HUD sees one target status"), HudWidget->GetTargetStatusEffectCount(), 1);
	TestTrue(
		TEXT("Target status copy includes ability name and countdown"),
		HudWidget->GetTargetStatusEffectDisplayText(0).ToString().Contains(TEXT("Snare")) &&
			HudWidget->GetTargetStatusEffectDisplayText(0).ToString().Contains(TEXT("6s")));
	Character->Combat->SetTarget(nullptr);
	TestEqual(TEXT("Clearing the target hides target statuses immediately"), HudWidget->GetTargetStatusEffectCount(), 0);
	Character->Combat->SetTarget(Enemy);
	TestEqual(TEXT("Retargeting restores the active target status"), HudWidget->GetTargetStatusEffectCount(), 1);

	TestEqual(
		TEXT("Status icon dimensions stay fixed"),
		HudWidget->GetStatusEffectIconDimensions(),
		FVector2D(22.0f, 22.0f));
	TestEqual(
		TEXT("Status slot dimensions stay fixed"),
		HudWidget->GetStatusEffectSlotDimensions(),
		FVector2D(128.0f, 32.0f));

	Character->Stats->ClearTemporaryEffects();
	Enemy->Stats->InitializeVitals();
	TestEqual(TEXT("Clearing player effects clears presentation snapshots"), HudWidget->GetPlayerStatusEffectCount(), 0);
	TestEqual(TEXT("Respawn-style initialization clears target presentation snapshots"), HudWidget->GetTargetStatusEffectCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereWorldStatusVfxPresentationTest,
	"Embermere.UI.WorldStatusVfxPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereWorldStatusVfxPresentationTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = LoadObject<UEmbermereRulesData>(
		nullptr,
		TEXT("/Game/Data/DA_EmbermereRules.DA_EmbermereRules"));
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	USkeletalMesh* ProwlerMesh = LoadObject<USkeletalMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/Enemies/MarshProwler/"
			"SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"));
	TestNotNull(TEXT("Saved rules resolve for world status VFX"), Rules);
	TestNotNull(TEXT("Character can be created for world status VFX"), Character);
	TestNotNull(TEXT("Enemy can be created for world status VFX"), Enemy);
	TestNotNull(TEXT("Marsh Prowler mesh resolves for world status VFX"), ProwlerMesh);
	if (!Rules || !Character || !Enemy || !ProwlerMesh ||
		!Character->Combat || !Character->Stats || !Enemy->Stats)
	{
		return false;
	}
	Enemy->GetMesh()->SetSkeletalMeshAsset(ProwlerMesh);
	Enemy->GetMesh()->SetRelativeScale3D(FVector(0.65f));

	FEmbermereAbilityDefinition BattleShout;
	FEmbermereAbilityDefinition Ward;
	FEmbermereAbilityDefinition Snare;
	FEmbermereAbilityDefinition FrostRoot;
	TestTrue(TEXT("Battle Shout resolves for world status VFX"), Rules->GetAbilityDefinition("BattleShout", BattleShout));
	TestTrue(TEXT("Ward resolves for world status VFX"), Rules->GetAbilityDefinition("Ward", Ward));
	TestTrue(TEXT("Snare resolves for world status VFX"), Rules->GetAbilityDefinition("Snare", Snare));
	TestTrue(TEXT("Frost Root resolves for world status VFX"), Rules->GetAbilityDefinition("FrostRoot", FrostRoot));

	Character->Stats->InitializeVitals();
	Enemy->Stats->InitializeVitals();
	Character->RefreshStatusEffectVfx();
	TestEqual(TEXT("Status VFX uses eight stable rune segments"), Character->GetStatusEffectVfxSegmentCount(), 8);
	TestEqual(TEXT("Status VFX starts hidden"), Character->GetVisibleStatusEffectVfxSegmentCount(), 0);
	TestTrue(
		TEXT("Status VFX uses the project-owned emissive material"),
		Character->GetStatusEffectVfxMaterialPath().Contains(TEXT("M_EmbermereTargetRing")));

	TestTrue(TEXT("Battle Shout activates for world status VFX"), Character->Combat->ExecuteAbility(BattleShout));
	Character->RefreshStatusEffectVfx();
	TestEqual(TEXT("Battle Shout shows all world-status segments"), Character->GetVisibleStatusEffectVfxSegmentCount(), 8);
	TestTrue(TEXT("Battle Shout resolves as beneficial world VFX"), Character->IsStatusEffectVfxBeneficial());
	const FLinearColor BattleShoutColor = Character->GetStatusEffectVfxColor();
	TestTrue(
		TEXT("Attack-power VFX resolves to ember gold"),
		BattleShoutColor.R > BattleShoutColor.G && BattleShoutColor.G > BattleShoutColor.B);

	Character->Stats->ClearTemporaryEffects();
	TestTrue(TEXT("Ward activates for world status VFX"), Character->Combat->ExecuteAbility(Ward));
	Character->RefreshStatusEffectVfx();
	const FLinearColor WardColor = Character->GetStatusEffectVfxColor();
	TestTrue(
		TEXT("Armor VFX resolves to blue-white"),
		WardColor.B > WardColor.R && WardColor.B > WardColor.G);

	Character->Stats->ApplyDamage(Character->Stats->MaxHealth * 2.0f);
	Character->RefreshStatusEffectVfx();
	TestEqual(TEXT("Death hides world status VFX"), Character->GetVisibleStatusEffectVfxSegmentCount(), 0);

	Character->Stats->ClearTemporaryEffects();
	Character->RefreshStatusEffectVfx();
	TestEqual(TEXT("Clearing buffs hides world status VFX"), Character->GetVisibleStatusEffectVfxSegmentCount(), 0);

	Character->Stats->InitializeVitals();
	Enemy->Stats->InitializeVitals();
	Character->Combat->SetTarget(Enemy);
	TestTrue(TEXT("Snare activates for world status VFX"), Character->Combat->ExecuteAbility(Snare));
	Enemy->RefreshStatusEffectVfx();
	TestFalse(TEXT("Snare resolves as harmful world VFX"), Enemy->IsStatusEffectVfxBeneficial());
	TestEqual(TEXT("Snare shows all enemy world-status segments"), Enemy->GetVisibleStatusEffectVfxSegmentCount(), 8);
	TestTrue(
		TEXT("Harmful Prowler VFX scales beyond the generic character radius"),
		Enemy->GetStatusEffectVfxRadius() > 66.0f);
	TestTrue(
		TEXT("Harmful Prowler VFX remains inside the target identity circle"),
		Enemy->GetStatusEffectVfxRadius() < Enemy->GetResolvedTargetRingRadius());
	TestTrue(
		TEXT("Harmful Prowler VFX resolves near the capsule base instead of the torso"),
		Enemy->GetStatusEffectVfxRelativeHeight() < -40.0f);
	const FLinearColor SnareColor = Enemy->GetStatusEffectVfxColor();
	TestTrue(
		TEXT("Snare resolves to marsh green"),
		SnareColor.G > SnareColor.R && SnareColor.G > SnareColor.B);

	Enemy->Stats->InitializeVitals();
	Character->Stats->InitializeVitals();
	TestTrue(TEXT("Frost Root activates for world status VFX"), Character->Combat->ExecuteAbility(FrostRoot));
	Enemy->RefreshStatusEffectVfx();
	const FLinearColor RootColor = Enemy->GetStatusEffectVfxColor();
	TestTrue(
		TEXT("Root resolves to brighter frost blue than Snare"),
		RootColor.B > RootColor.G && RootColor.B > SnareColor.B);
	TestTrue(
		TEXT("Root stays visibly lighter than the marsh-green Snare palette"),
		RootColor.R > SnareColor.R && RootColor.G > SnareColor.G);

	Enemy->Stats->InitializeVitals();
	Enemy->RefreshStatusEffectVfx();
	TestEqual(TEXT("Respawn-style initialization hides world status VFX"), Enemy->GetVisibleStatusEffectVfxSegmentCount(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatDeadCasterRejectedTest,
	"Embermere.Combat.DeadCasterRejected",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatDeadCasterRejectedTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	if (!Character || !Character->Combat || !Character->Stats)
	{
		return false;
	}

	FEmbermereAbilityDefinition RecoveryFocus;
	RecoveryFocus.AbilityId = "RecoveryFocus";
	RecoveryFocus.DisplayName = FText::FromString(TEXT("Recovery Focus"));
	RecoveryFocus.TargetKind = EEmbermereAbilityTargetKind::Self;
	RecoveryFocus.Power = 1.0f;
	RecoveryFocus.ManaCost = 5.0f;
	RecoveryFocus.Cooldown = 1.0f;

	Character->Stats->InitializeVitals();
	const float ManaBeforeDeadCast = Character->Stats->CurrentMana;
	Character->Stats->ApplyDamage(Character->Stats->MaxHealth);
	TestTrue(TEXT("Character is dead before rejected cast"), Character->Stats->IsDead());
	TestFalse(TEXT("Dead character cannot execute a self ability"), Character->Combat->ExecuteAbility(RecoveryFocus));
	TestEqual(TEXT("Dead character does not spend mana"), Character->Stats->CurrentMana, ManaBeforeDeadCast);

	Character->Stats->InitializeVitals();
	TestFalse(TEXT("Character is alive after recovery"), Character->Stats->IsDead());
	TestTrue(TEXT("Recovered character can execute a self ability"), Character->Combat->ExecuteAbility(RecoveryFocus));
	TestEqual(TEXT("Recovered character spends mana"), Character->Stats->CurrentMana, Character->Stats->MaxMana - RecoveryFocus.ManaCost);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereStatsDamageImmunityTest,
	"Embermere.Stats.DamageImmunity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStatsDamageImmunityTest::RunTest(const FString& Parameters)
{
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Stats component can be created"), Stats);
	if (!Stats)
	{
		return false;
	}

	Stats->InitializeVitals();
	Stats->GrantDamageImmunity(3.0f);
	TestTrue(TEXT("Damage immunity is active after grant"), Stats->IsDamageImmune());
	TestEqual(TEXT("Damage immunity prevents damage"), Stats->ApplyDamage(25.0f), 0.0f);
	TestEqual(TEXT("Health stays full during immunity"), Stats->CurrentHealth, Stats->MaxHealth);

	Stats->ClearDamageImmunity();
	TestFalse(TEXT("Damage immunity clears"), Stats->IsDamageImmune());
	TestEqual(TEXT("Damage applies after immunity clears"), Stats->ApplyDamage(25.0f), 25.0f);
	TestEqual(TEXT("Health changes after immunity clears"), Stats->CurrentHealth, Stats->MaxHealth - 25.0f);

	Stats->GrantDamageImmunity(3.0f);
	TestTrue(TEXT("Damage immunity can be granted again"), Stats->IsDamageImmune());
	Stats->InitializeVitals();
	TestFalse(TEXT("InitializeVitals clears old immunity"), Stats->IsDamageImmune());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyNameplateWidgetTest,
	"Embermere.UI.EnemyNameplateWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyNameplateWidgetTest::RunTest(const FString& Parameters)
{
	UEmbermereEnemyNameplateWidget* NameplateWidget = NewObject<UEmbermereEnemyNameplateWidget>();
	TestNotNull(TEXT("Enemy nameplate widget can be created"), NameplateWidget);
	if (!NameplateWidget)
	{
		return false;
	}

	TestFalse(TEXT("Nameplate starts unselected"), NameplateWidget->IsNameplateSelected());
	NameplateWidget->SetNameplateState(
		FText::FromString(TEXT("Marsh Prowler")),
		45.0f,
		100.0f,
		FLinearColor(1.0f, 0.72f, 0.24f, 1.0f),
		true);
	TestTrue(TEXT("Nameplate reports selected after selected state"), NameplateWidget->IsNameplateSelected());

	NameplateWidget->SetNameplateState(
		FText::FromString(TEXT("Marsh Prowler")),
		0.0f,
		100.0f,
		FLinearColor(1.0f, 0.28f, 0.18f, 1.0f),
		false);
	TestFalse(TEXT("Nameplate reports unselected after hidden state"), NameplateWidget->IsNameplateSelected());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyLeashRulesTest,
	"Embermere.Enemy.LeashRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyLeashRulesTest::RunTest(const FString& Parameters)
{
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	TestNotNull(TEXT("Enemy can be created"), Enemy);
	if (!Enemy)
	{
		return false;
	}

	TestEqual(TEXT("Starter enemy default aggro supports separated pulls"), Enemy->AggroRadius, 525.0f);
	TestEqual(TEXT("Starter enemy keeps first-pass attack damage"), Enemy->AttackDamage, 6.0f);
	TestEqual(TEXT("Starter enemy keeps first-pass attack cadence"), Enemy->AttackCooldownSeconds, 2.0f);

	Enemy->LeashRadius = 1000.0f;
	Enemy->ReturnHomeRadius = 120.0f;

	TestFalse(TEXT("Enemy location inside leash stays valid"), Enemy->IsLocationOutsideLeashRadius(FVector(500.0f, 0.0f, 0.0f)));
	TestTrue(TEXT("Enemy location outside leash is detected"), Enemy->IsLocationOutsideLeashRadius(FVector(1200.0f, 0.0f, 0.0f)));
	TestFalse(TEXT("Enemy at home does not need return"), Enemy->ShouldReturnHomeFromLocation(FVector(80.0f, 0.0f, 0.0f)));
	TestTrue(TEXT("Enemy away from home should return"), Enemy->ShouldReturnHomeFromLocation(FVector(180.0f, 0.0f, 0.0f)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryHudToggleTest,
	"Embermere.UI.InventoryToggle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryHudToggleTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	if (!HudWidget)
	{
		return false;
	}

	TestTrue(TEXT("Inventory panel starts visible"), HudWidget->IsInventoryPanelVisible());
	TestFalse(TEXT("First toggle hides inventory"), HudWidget->ToggleInventoryPanel());
	TestFalse(TEXT("Inventory panel reports hidden"), HudWidget->IsInventoryPanelVisible());
	TestTrue(TEXT("Second toggle shows inventory"), HudWidget->ToggleInventoryPanel());
	TestTrue(TEXT("Inventory panel reports visible"), HudWidget->IsInventoryPanelVisible());
	const FString EmptyInventoryText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory empty state is explicit"), EmptyInventoryText.Contains(TEXT("Empty")));
	TestTrue(TEXT("Inventory empty state points rewards into the panel"), EmptyInventoryText.Contains(TEXT("Quest rewards appear here")));
	TestTrue(TEXT("Inventory empty state reports slot capacity"), EmptyInventoryText.Contains(TEXT("Slots 0 / 24")));
	TestTrue(TEXT("Inventory empty state includes close control"), EmptyInventoryText.Contains(TEXT("I Close")));

	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* InventoryOwnerStats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created for inventory actions"), Equipment);
	TestNotNull(TEXT("Stats component can be created for inventory actions"), InventoryOwnerStats);
	if (!Inventory || !Equipment || !InventoryOwnerStats)
	{
		return false;
	}

	UEmbermereItemData* FirstItem = NewObject<UEmbermereItemData>();
	FirstItem->ItemId = "RecruitPack";
	FirstItem->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	FirstItem->Description = FText::FromString(TEXT("A small bundle for new Embermere adventurers."));
	FirstItem->MaxStack = 5;
	FirstItem->Category = EEmbermereItemCategory::Armor;
	FirstItem->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	FirstItem->RequiredLevel = 2;
	FirstItem->StatBonuses.MaxHealth = 5.0f;
	FirstItem->StatBonuses.Armor = 1.0f;

	UEmbermereItemData* SecondItem = NewObject<UEmbermereItemData>();
	SecondItem->ItemId = "MarshReed";
	SecondItem->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	SecondItem->Description = FText::FromString(TEXT("A damp reed gathered near the prowler marsh."));
	SecondItem->MaxStack = 10;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = InventoryOwnerStats;
	TestTrue(TEXT("First inventory item can be added"), Inventory->AddItem(FirstItem, 1));
	TestTrue(TEXT("Second inventory item can be added"), Inventory->AddItem(SecondItem, 2));
	TestEqual(TEXT("Inventory selection starts at the first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	FString FirstItemDisplayText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory display reports occupied slot capacity"), FirstItemDisplayText.Contains(TEXT("Slots 2 / 24")));
	TestTrue(TEXT("Inventory display shows first stack position"), FirstItemDisplayText.Contains(TEXT("Inspecting 1/2")));
	TestTrue(TEXT("Inventory display marks selected first stack"), FirstItemDisplayText.Contains(TEXT("> Recruit Pack x1")));
	TestTrue(TEXT("Inventory display shows first item stack detail"), FirstItemDisplayText.Contains(TEXT("Stack: 1 / 5")));
	TestTrue(TEXT("Inventory display shows first item description"), FirstItemDisplayText.Contains(TEXT("new Embermere adventurers")));
	TestTrue(TEXT("Inventory display shows item category"), FirstItemDisplayText.Contains(TEXT("Armor")));
	TestTrue(TEXT("Inventory display shows primary action"), FirstItemDisplayText.Contains(TEXT("Action: Equip")));
	TestTrue(TEXT("Inventory display shows equipment slot"), FirstItemDisplayText.Contains(TEXT("Slot: Back")));
	TestTrue(TEXT("Inventory display shows required level"), FirstItemDisplayText.Contains(TEXT("Required level: 2")));
	TestTrue(TEXT("Inventory display shows item stat summary"), FirstItemDisplayText.Contains(TEXT("+5 HP")) && FirstItemDisplayText.Contains(TEXT("+1 Armor")));
	TestTrue(TEXT("Inventory selection advances to the next stack"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection reports second stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);
	FString SecondItemDisplayText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory display shows second stack position"), SecondItemDisplayText.Contains(TEXT("Inspecting 2/2")));
	TestTrue(TEXT("Inventory display marks selected second stack"), SecondItemDisplayText.Contains(TEXT("> Marsh Reed x2")));
	TestTrue(TEXT("Inventory display shows second item stack detail"), SecondItemDisplayText.Contains(TEXT("Stack: 2 / 10")));
	TestTrue(TEXT("Inventory display shows bracket cycling hint for multiple stacks"), SecondItemDisplayText.Contains(TEXT("Use [ ] to inspect")));
	TestTrue(TEXT("Inventory selection wraps forward"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection wraps to first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestTrue(TEXT("Inventory selection wraps backward"), HudWidget->SelectNextInventoryItem(-1));
	TestEqual(TEXT("Inventory selection wraps to last stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);
	TestTrue(TEXT("Direct inventory selection supports mouse-driven rows"), HudWidget->SelectInventoryItem(0));
	TestEqual(TEXT("Direct inventory selection updates the inspected stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestFalse(TEXT("Direct inventory selection rejects an invalid row"), HudWidget->SelectInventoryItem(Inventory->Stacks.Num()));
	TestEqual(TEXT("Invalid direct selection preserves the inspected stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestFalse(TEXT("Equip action respects item level requirement"), HudWidget->ActivateSelectedInventoryItem());
	InventoryOwnerStats->Level = 2;
	TestTrue(TEXT("Eligible selected item can be equipped"), HudWidget->ActivateSelectedInventoryItem());
	TestTrue(TEXT("Inventory action equips selected item"), Equipment->IsItemEquipped(FirstItem));
	TestEqual(TEXT("Equipped item leaves the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Remaining bag stack is preserved"), Inventory->Stacks[0].Item == SecondItem);
	const FString EquipmentText = HudWidget->GetEquipmentDisplayText().ToString();
	TestTrue(TEXT("Equipment panel shows Back-slot item"), EquipmentText.Contains(TEXT("Back")) && EquipmentText.Contains(TEXT("Recruit Pack")));
	TestTrue(TEXT("Equipment panel shows aggregate health bonus"), EquipmentText.Contains(TEXT("HP +5")));
	TestTrue(TEXT("Equipment panel shows aggregate armor bonus"), EquipmentText.Contains(TEXT("Armor +1")));
	TestTrue(TEXT("Occupied Back slot can be activated to unequip"), HudWidget->ActivateEquipmentSlot(EEmbermereEquipmentSlot::Back));
	TestFalse(TEXT("Equipment slot action clears equipped state"), Equipment->IsItemEquipped(FirstItem));
	TestEqual(TEXT("Unequipped item returns to the bag"), Inventory->Stacks.Num(), 2);
	TestTrue(TEXT("Returned item is appended to the bag"), Inventory->Stacks[1].Item == FirstItem);
	TestTrue(TEXT("Returned item can be selected"), HudWidget->SelectInventoryItem(1));
	TestEqual(TEXT("Unequipped item action returns to Equip"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Equip")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereSaveLoadPanelTest,
	"Embermere.UI.SaveLoadPanel",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereSaveLoadPanelTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	if (!Hud)
	{
		AddError(TEXT("Could not create Chronicle panel fixture"));
		return false;
	}
	Hud->TakeWidget();
	TestTrue(TEXT("Chronicle button is fixed to the bottom-right viewport corner"), Hud->IsChronicleButtonBottomRightAnchored());
	TestEqual(
		TEXT("Chronicle button uses larger reviewed dimensions"),
		Hud->GetChronicleButtonDimensions(),
		FVector2D(140.0f, 38.0f));
	TestEqual(
		TEXT("Chronicle button keeps a stable screen-edge margin"),
		Hud->GetChronicleButtonViewportOffset(),
		FVector2D(-24.0f, -24.0f));

	TestFalse(TEXT("Chronicle panel starts hidden"), Hud->IsSaveLoadPanelVisible());
	TestTrue(TEXT("Chronicle toggle opens the panel"), Hud->ToggleSaveLoadPanel());
	TestTrue(TEXT("Chronicle panel reports visible"), Hud->IsSaveLoadPanelVisible());
	TestFalse(TEXT("Opening Chronicle hides inventory"), Hud->IsInventoryPanelVisible());
	TestFalse(TEXT("Opening Chronicle leaves vendor hidden"), Hud->IsVendorPanelVisible());
	TestEqual(
		TEXT("Chronicle uses stable reviewed dimensions"),
		Hud->GetSaveLoadPanelDimensions(),
		FVector2D(500.0f, 320.0f));
	UTextBlock* CurrentJourneyText = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("SaveLoadCurrentText")));
	UTextBlock* SavedJourneyText = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("SaveLoadSlotText")));
	UTextBlock* SaveActionText = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("SaveLoadSaveText")));
	UTextBlock* LoadActionText = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("SaveLoadLoadText")));
	USizeBox* SavedSummarySize = Cast<USizeBox>(Hud->GetWidgetFromName(TEXT("SaveLoadSummarySize")));
	UVerticalBox* ChronicleStack = Cast<UVerticalBox>(Hud->GetWidgetFromName(TEXT("SaveLoadStack")));
	UWidget* ActionRow = Hud->GetWidgetFromName(TEXT("SaveLoadActionRow"));
	TestNotNull(TEXT("Chronicle exposes an explicit current-journey row"), CurrentJourneyText);
	TestNotNull(TEXT("Chronicle exposes an explicit saved-journey label"), SavedJourneyText);
	TestNotNull(TEXT("Chronicle reserves a fixed saved-summary region"), SavedSummarySize);
	TestTrue(
		TEXT("Current identity is labeled separately from saved identity"),
		CurrentJourneyText && CurrentJourneyText->GetText().ToString().StartsWith(TEXT("Current Journey")));
	TestTrue(
		TEXT("Slot identity is labeled as the saved journey"),
		SavedJourneyText && SavedJourneyText->GetText().ToString().StartsWith(TEXT("Saved Journey")));
	TestEqual(
		TEXT("Chronicle makes save direction explicit"),
		SaveActionText ? SaveActionText->GetText().ToString() : FString(),
		FString(TEXT("Save Current")));
	TestEqual(
		TEXT("Chronicle makes load direction explicit"),
		LoadActionText ? LoadActionText->GetText().ToString() : FString(),
		FString(TEXT("Load Saved")));
	TestEqual(
		TEXT("Saved summary has room for all three explicit lines"),
		SavedSummarySize ? SavedSummarySize->GetHeightOverride() : 0.0f,
		78.0f);
	TestTrue(
		TEXT("Action row follows the complete saved-summary region"),
		ChronicleStack && SavedSummarySize && ActionRow &&
		ChronicleStack->GetChildIndex(SavedSummarySize) < ChronicleStack->GetChildIndex(ActionRow));

	TestTrue(TEXT("Inventory command opens inventory from Chronicle"), Hud->ToggleInventoryPanel());
	TestFalse(TEXT("Inventory command closes Chronicle"), Hud->IsSaveLoadPanelVisible());
	TestTrue(TEXT("Inventory remains visible after handoff"), Hud->IsInventoryPanelVisible());
	TestTrue(TEXT("Chronicle reopens after inventory handoff"), Hud->ToggleSaveLoadPanel());
	Hud->CloseSaveLoadPanel();
	TestFalse(TEXT("Explicit close hides Chronicle"), Hud->IsSaveLoadPanelVisible());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereUiIconPresentationTest,
	"Embermere.UI.IconPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereUiIconPresentationTest::RunTest(const FString& Parameters)
{
	const UEmbermereUiIconSet* IconSet = LoadObject<UEmbermereUiIconSet>(
		nullptr,
		TEXT("/Game/UI/Icons/DA_EmbermereUiIconSet.DA_EmbermereUiIconSet"));
	const UEmbermereItemData* RecruitPack = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"));
	const UEmbermereItemData* MarshTonic = LoadObject<UEmbermereItemData>(
		nullptr,
		TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	TestNotNull(TEXT("UI icon set asset resolves"), IconSet);
	TestNotNull(TEXT("Recruit Pack data resolves"), RecruitPack);
	TestNotNull(TEXT("Marsh Tonic data resolves"), MarshTonic);
	if (!IconSet || !RecruitPack || !MarshTonic)
	{
		return false;
	}

	TestEqual(TEXT("Icon set maps all ten equipment slots"), IconSet->EquipmentSlotIcons.Num(), 10);
	TestEqual(TEXT("Icon set maps all five item category fallbacks"), IconSet->CategoryFallbackIcons.Num(), 5);
	TestFalse(TEXT("Icon set has a missing-item fallback"), IconSet->MissingItemIcon.IsNull());
	TestFalse(TEXT("Icon set has a missing-slot fallback"), IconSet->MissingSlotIcon.IsNull());
	TestFalse(TEXT("Recruit Pack owns an explicit icon reference"), RecruitPack->Icon.IsNull());
	TestFalse(TEXT("Marsh Tonic owns an explicit icon reference"), MarshTonic->Icon.IsNull());

	auto TestIconDimensions = [this](const FString& Label, UTexture2D* Texture)
	{
		TestNotNull(*FString::Printf(TEXT("%s resolves"), *Label), Texture);
		if (Texture)
		{
			TestEqual(*FString::Printf(TEXT("%s source width is stable"), *Label), static_cast<int32>(Texture->Source.GetSizeX()), 128);
			TestEqual(*FString::Printf(TEXT("%s source height is stable"), *Label), static_cast<int32>(Texture->Source.GetSizeY()), 128);
		}
	};

	UTexture2D* RecruitPackIcon = IconSet->ResolveItemIcon(RecruitPack);
	UTexture2D* MarshTonicIcon = IconSet->ResolveItemIcon(MarshTonic);
	TestIconDimensions(TEXT("Recruit Pack icon"), RecruitPackIcon);
	TestIconDimensions(TEXT("Marsh Tonic icon"), MarshTonicIcon);
	TestTrue(TEXT("Recruit Pack resolves its direct icon"), RecruitPackIcon == RecruitPack->Icon.LoadSynchronous());
	TestTrue(TEXT("Marsh Tonic resolves its direct icon"), MarshTonicIcon == MarshTonic->Icon.LoadSynchronous());

	static const EEmbermereEquipmentSlot EquipmentSlots[] = {
		EEmbermereEquipmentSlot::MainHand,
		EEmbermereEquipmentSlot::OffHand,
		EEmbermereEquipmentSlot::Head,
		EEmbermereEquipmentSlot::Chest,
		EEmbermereEquipmentSlot::Hands,
		EEmbermereEquipmentSlot::Legs,
		EEmbermereEquipmentSlot::Feet,
		EEmbermereEquipmentSlot::Back,
		EEmbermereEquipmentSlot::Neck,
		EEmbermereEquipmentSlot::Ring,
	};
	for (const EEmbermereEquipmentSlot Slot : EquipmentSlots)
	{
		TestIconDimensions(
			FString::Printf(TEXT("Equipment slot %d icon"), static_cast<int32>(Slot)),
			IconSet->ResolveEquipmentSlotIcon(Slot));
	}

	UEmbermereItemData* FallbackWeapon = NewObject<UEmbermereItemData>();
	FallbackWeapon->DisplayName = FText::FromString(TEXT("Fallback Weapon"));
	FallbackWeapon->Category = EEmbermereItemCategory::Weapon;
	TestTrue(
		TEXT("Item without direct art resolves its category fallback"),
		IconSet->ResolveItemIcon(FallbackWeapon) == IconSet->ResolveEquipmentSlotIcon(EEmbermereEquipmentSlot::MainHand));
	TestTrue(
		TEXT("Null item resolves explicit missing-item art"),
		IconSet->ResolveItemIcon(nullptr) == IconSet->MissingItemIcon.LoadSynchronous());
	TestTrue(
		TEXT("Unmapped slot resolves explicit missing-slot art"),
		IconSet->ResolveEquipmentSlotIcon(EEmbermereEquipmentSlot::None) == IconSet->MissingSlotIcon.LoadSynchronous());

	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD can be created for icon presentation"), HudWidget);
	if (HudWidget)
	{
		TestFalse(TEXT("HUD defaults to the data-driven icon set"), HudWidget->UiIconSet.IsNull());
		TestTrue(TEXT("HUD resolves Recruit Pack through item data"), HudWidget->ResolveItemIconForUi(RecruitPack) == RecruitPackIcon);
		TestEqual(TEXT("Bag row icon dimensions stay fixed"), HudWidget->GetInventoryRowIconDimensions(), FVector2D(18.0f, 18.0f));
		TestEqual(TEXT("Detail icon dimensions stay fixed"), HudWidget->GetInventoryDetailIconDimensions(), FVector2D(42.0f, 42.0f));
		TestEqual(TEXT("Equipment icon dimensions stay fixed"), HudWidget->GetEquipmentSlotIconDimensions(), FVector2D(18.0f, 18.0f));
		TestEqual(TEXT("Loot popup icon dimensions stay fixed"), HudWidget->GetLootPopupIconDimensions(), FVector2D(32.0f, 32.0f));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermerePaperDollPresentationTest,
	"Embermere.UI.PaperDollPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermerePaperDollPresentationTest::RunTest(const FString& Parameters)
{
	const UEmbermereUiIconSet* IconSet = LoadObject<UEmbermereUiIconSet>(
		nullptr,
		TEXT("/Game/UI/Icons/DA_EmbermereUiIconSet.DA_EmbermereUiIconSet"));
	TestNotNull(TEXT("UI icon set resolves for paper-doll presentation"), IconSet);
	if (!IconSet)
	{
		return false;
	}

	TestFalse(TEXT("Icon set owns a paper-doll backdrop reference"), IconSet->PaperDollBackdrop.IsNull());
	UTexture2D* PaperDollBackdrop = IconSet->ResolvePaperDollBackdrop();
	TestNotNull(TEXT("Paper-doll backdrop resolves"), PaperDollBackdrop);
	if (PaperDollBackdrop)
	{
		TestEqual(
			TEXT("Paper-doll source width is stable"),
			static_cast<int32>(PaperDollBackdrop->Source.GetSizeX()),
			128);
		TestEqual(
			TEXT("Paper-doll source height is stable"),
			static_cast<int32>(PaperDollBackdrop->Source.GetSizeY()),
			160);
	}

	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD can be created for paper-doll presentation"), HudWidget);
	if (HudWidget)
	{
		TestTrue(
			TEXT("HUD resolves the shared paper-doll backdrop"),
			HudWidget->ResolvePaperDollBackdropForUi() == PaperDollBackdrop);
		TestEqual(
			TEXT("Paper-doll presentation dimensions stay fixed"),
			HudWidget->GetPaperDollBackdropDimensions(),
			FVector2D(128.0f, 160.0f));
		TestEqual(
			TEXT("Equipment slot dimensions remain unchanged"),
			HudWidget->GetEquipmentSlotIconDimensions(),
			FVector2D(18.0f, 18.0f));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereAbilityIconPresentationTest,
	"Embermere.UI.AbilityIconPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereAbilityIconPresentationTest::RunTest(const FString& Parameters)
{
	const UEmbermereUiIconSet* IconSet = LoadObject<UEmbermereUiIconSet>(
		nullptr,
		TEXT("/Game/UI/Icons/DA_EmbermereUiIconSet.DA_EmbermereUiIconSet"));
	const UEmbermereRulesData* SavedRules = LoadObject<UEmbermereRulesData>(
		nullptr,
		TEXT("/Game/Data/DA_EmbermereRules.DA_EmbermereRules"));
	const UEmbermereRulesData* DefaultRules = NewObject<UEmbermereRulesData>();
	TestNotNull(TEXT("UI icon set resolves for abilities"), IconSet);
	TestNotNull(TEXT("Saved rules data resolves for abilities"), SavedRules);
	TestNotNull(TEXT("Native rules defaults resolve for abilities"), DefaultRules);
	if (!IconSet || !SavedRules || !DefaultRules)
	{
		return false;
	}

	TestFalse(TEXT("Icon set has a missing-ability fallback"), IconSet->MissingAbilityIcon.IsNull());
	TestEqual(TEXT("Saved rules contain all starter abilities"), SavedRules->Abilities.Num(), 16);
	TestEqual(TEXT("Native defaults contain all starter abilities"), DefaultRules->Abilities.Num(), 16);

	TSet<FString> SavedAbilityIds;
	TSet<FString> SavedIconPaths;
	for (const FEmbermereAbilityDefinition& Ability : SavedRules->Abilities)
	{
		const FString AbilityId = Ability.AbilityId.ToString();
		SavedAbilityIds.Add(AbilityId);
		TestFalse(
			*FString::Printf(TEXT("%s owns a saved icon reference"), *AbilityId),
			Ability.Icon.IsNull());

		UTexture2D* AbilityIcon = IconSet->ResolveAbilityIcon(Ability);
		TestNotNull(*FString::Printf(TEXT("%s icon resolves"), *AbilityId), AbilityIcon);
		if (AbilityIcon)
		{
			TestEqual(
				*FString::Printf(TEXT("%s source width is stable"), *AbilityId),
				static_cast<int32>(AbilityIcon->Source.GetSizeX()),
				128);
			TestEqual(
				*FString::Printf(TEXT("%s source height is stable"), *AbilityId),
				static_cast<int32>(AbilityIcon->Source.GetSizeY()),
				128);
		}
		SavedIconPaths.Add(Ability.Icon.ToSoftObjectPath().ToString());
	}

	TestEqual(TEXT("All sixteen saved ability IDs are distinct"), SavedAbilityIds.Num(), 16);
	TestEqual(TEXT("All sixteen saved ability icons are distinct"), SavedIconPaths.Num(), 16);

	for (const FEmbermereAbilityDefinition& Ability : DefaultRules->Abilities)
	{
		TestFalse(
			*FString::Printf(TEXT("%s native default owns an icon reference"), *Ability.AbilityId.ToString()),
			Ability.Icon.IsNull());
		TestTrue(
			*FString::Printf(TEXT("%s native default icon resolves"), *Ability.AbilityId.ToString()),
			IconSet->ResolveAbilityIcon(Ability) != nullptr);
	}

	FEmbermereAbilityDefinition MissingArtAbility;
	MissingArtAbility.AbilityId = "MissingArt";
	TestTrue(
		TEXT("Ability without direct art resolves the explicit fallback"),
		IconSet->ResolveAbilityIcon(MissingArtAbility) == IconSet->MissingAbilityIcon.LoadSynchronous());

	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD can be created for ability icon presentation"), HudWidget);
	if (HudWidget)
	{
		TestEqual(TEXT("Hotbar ability icon dimensions stay fixed"), HudWidget->GetHotbarSlotIconDimensions(), FVector2D(32.0f, 32.0f));
		TestTrue(
			TEXT("HUD resolves saved Strike art through ability data"),
			HudWidget->ResolveAbilityIconForUi(SavedRules->Abilities[0]) ==
				IconSet->ResolveAbilityIcon(SavedRules->Abilities[0]));
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryItemComparisonTest,
	"Embermere.UI.ItemComparison",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryItemComparisonTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	if (!HudWidget || !Inventory || !Equipment)
	{
		return false;
	}

	UEmbermereItemData* WeatheredCloak = NewObject<UEmbermereItemData>();
	WeatheredCloak->ItemId = "WeatheredCloak";
	WeatheredCloak->DisplayName = FText::FromString(TEXT("Weathered Cloak"));
	WeatheredCloak->Category = EEmbermereItemCategory::Armor;
	WeatheredCloak->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	WeatheredCloak->StatBonuses.MaxHealth = 2.0f;
	WeatheredCloak->StatBonuses.Armor = 2.0f;

	UEmbermereItemData* FenwatchMantle = NewObject<UEmbermereItemData>();
	FenwatchMantle->ItemId = "FenwatchMantle";
	FenwatchMantle->DisplayName = FText::FromString(TEXT("Fenwatch Mantle"));
	FenwatchMantle->Description = FText::FromString(TEXT("A moss-green mantle issued to trusted Fenwatch scouts."));
	FenwatchMantle->Category = EEmbermereItemCategory::Armor;
	FenwatchMantle->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	FenwatchMantle->RequiredLevel = 2;
	FenwatchMantle->StatBonuses.MaxHealth = 5.0f;
	FenwatchMantle->StatBonuses.MaxMana = 1.0f;
	FenwatchMantle->StatBonuses.Armor = 1.0f;

	UEmbermereItemData* MarshTonic = NewObject<UEmbermereItemData>();
	MarshTonic->ItemId = "MarshTonic";
	MarshTonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	MarshTonic->Description = FText::FromString(TEXT("A sharp herbal draught."));
	MarshTonic->Category = EEmbermereItemCategory::Consumable;
	MarshTonic->MaxStack = 5;
	MarshTonic->ConsumableEffects.HealHealth = 25.0f;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	TestTrue(TEXT("Baseline cloak can be equipped"), Equipment->EquipItem(WeatheredCloak, 1));
	TestTrue(TEXT("Comparison candidate can be added"), Inventory->AddItem(FenwatchMantle, 1));
	TestTrue(TEXT("Consumable can be added"), Inventory->AddItem(MarshTonic, 2));

	const FString ComparisonText = HudWidget->GetSelectedInventoryComparisonText().ToString();
	TestTrue(TEXT("Comparison names the equipped item"), ComparisonText.Contains(TEXT("Vs Weathered Cloak")));
	TestTrue(TEXT("Comparison reports a health upgrade"), ComparisonText.Contains(TEXT("HP +3")));
	TestTrue(TEXT("Comparison reports a mana upgrade"), ComparisonText.Contains(TEXT("Mana +1")));
	TestTrue(TEXT("Comparison reports an armor downgrade"), ComparisonText.Contains(TEXT("Armor -1")));

	const FString TooltipText = HudWidget->GetSelectedInventoryTooltipText().ToString();
	TestTrue(TEXT("Tooltip names the inspected item"), TooltipText.Contains(TEXT("Fenwatch Mantle x1")));
	TestTrue(TEXT("Tooltip includes slot and level"), TooltipText.Contains(TEXT("Back | Level 2")));
	TestTrue(TEXT("Tooltip includes the item's effects"), TooltipText.Contains(TEXT("+5 HP")));
	TestTrue(TEXT("Tooltip includes equipment comparison"), TooltipText.Contains(TEXT("Vs Weathered Cloak")));
	TestTrue(TEXT("Tooltip includes item description"), TooltipText.Contains(TEXT("trusted Fenwatch scouts")));
	TestTrue(TEXT("Inventory detail text includes comparison"), HudWidget->GetInventoryDisplayText().ToString().Contains(TEXT("Armor -1")));

	TestNotNull(TEXT("Equipped item can be removed for empty-slot comparison"), Equipment->UnequipItem(EEmbermereEquipmentSlot::Back));
	const FString EmptySlotComparison = HudWidget->GetSelectedInventoryComparisonText().ToString();
	TestTrue(TEXT("Empty-slot comparison names the destination slot"), EmptySlotComparison.Contains(TEXT("Vs empty Back")));
	TestTrue(TEXT("Empty-slot comparison reports candidate armor"), EmptySlotComparison.Contains(TEXT("Armor +1")));

	TestTrue(TEXT("Consumable can be selected"), HudWidget->SelectInventoryItem(1));
	TestTrue(TEXT("Consumable has no equipment comparison"), HudWidget->GetSelectedInventoryComparisonText().IsEmpty());
	const FString ConsumableTooltip = HudWidget->GetSelectedInventoryTooltipText().ToString();
	TestTrue(TEXT("Consumable tooltip reports quantity"), ConsumableTooltip.Contains(TEXT("Marsh Tonic x2")));
	TestTrue(TEXT("Consumable tooltip reports its effect"), ConsumableTooltip.Contains(TEXT("Restores 25 HP")));
	TestFalse(TEXT("Consumable tooltip omits equipment comparison"), ConsumableTooltip.Contains(TEXT("Vs ")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryIdentityActionsTest,
	"Embermere.Inventory.IdentityActions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryIdentityActionsTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	TestNotNull(TEXT("Stats component can be created"), Stats);
	if (!HudWidget || !Inventory || !Equipment || !Stats)
	{
		return false;
	}

	UEmbermereItemData* RecruitPack = NewObject<UEmbermereItemData>();
	RecruitPack->ItemId = "RecruitPack";
	RecruitPack->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RecruitPack->Category = EEmbermereItemCategory::Armor;
	RecruitPack->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	RecruitPack->RequiredLevel = 1;
	RecruitPack->StatBonuses.MaxHealth = 5.0f;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = Stats;
	Stats->Level = 1;
	TestTrue(TEXT("Identity-action item can be added"), Inventory->AddItem(RecruitPack, 1));

	TestFalse(
		TEXT("Wrong-slot request is rejected"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Chest));
	TestEqual(TEXT("Wrong-slot rejection preserves bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestNull(TEXT("Wrong-slot rejection preserves empty Back slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back));

	TestTrue(
		TEXT("Matching-slot request equips through the identity action"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Successful identity action removes bag item"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Successful identity action equips requested item"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	TestFalse(
		TEXT("Stale item payload is rejected after item leaves bag"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Stale payload does not duplicate item into bag"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Stale payload leaves equipment intact"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	TestTrue(TEXT("A second copy of equipped gear can remain in the bag"), Inventory->AddItem(RecruitPack, 1));
	TestFalse(
		TEXT("Already-equipped item definition is a rejected no-op"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Rejected duplicate equip preserves bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestTrue(TEXT("Rejected duplicate equip preserves equipped item"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryDragDropTest,
	"Embermere.UI.InventoryDragDrop",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryDragDropTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereItemData* RecruitPack = NewObject<UEmbermereItemData>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	TestNotNull(TEXT("Stats component can be created"), Stats);
	TestNotNull(TEXT("Dragged item can be created"), RecruitPack);
	if (!HudWidget || !Inventory || !Equipment || !Stats || !RecruitPack)
	{
		return false;
	}

	RecruitPack->ItemId = "RecruitPack";
	RecruitPack->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RecruitPack->Category = EEmbermereItemCategory::Armor;
	RecruitPack->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	RecruitPack->RequiredLevel = 2;
	RecruitPack->MaxStack = 1;
	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = Stats;
	Stats->Level = 1;
	TestTrue(TEXT("Dragged gear can be added to the bag"), Inventory->AddItem(RecruitPack, 1));

	UEmbermereItemDragDropOperation* Operation = NewObject<UEmbermereItemDragDropOperation>();
	Operation->Item = RecruitPack;
	Operation->Source = EEmbermereItemDragSource::Inventory;
	Operation->ResolvedIcon = HudWidget->ResolveItemIconForUi(RecruitPack);
	TestTrue(TEXT("Typed drag payload preserves item identity"), Operation->Item == RecruitPack);
	TestEqual(TEXT("Typed drag payload records bag source"), Operation->Source, EEmbermereItemDragSource::Inventory);
	TestNotNull(TEXT("Fantasy drag token receives the shared resolved item icon"), Operation->ResolvedIcon.Get());
	TestNotNull(TEXT("Fantasy drag token can build with resolved icon art"), Operation->CreateDragVisual());
	TestEqual(TEXT("Armor drag visual uses an armor sigil"), Operation->GetVisualSigilText().ToString(), FString(TEXT("ARM")));
	TestEqual(
		TEXT("Armor drag visual includes slot and level context"),
		Operation->GetVisualContextText().ToString(),
		FString(TEXT("Back | Level 2")));
	TestTrue(TEXT("Armor drag visual uses a warm fantasy accent"), Operation->GetVisualAccentColor().R > 0.9f);

	TestFalse(
		TEXT("Wrong equipment slot is not a valid drop target"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Chest));
	TestFalse(
		TEXT("Under-level character sees the matching slot as invalid"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	Stats->Level = 2;
	TestTrue(
		TEXT("Eligible matching slot is a valid drop target"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Valid drop equips through the atomic transaction"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Equipped drag removes bag quantity"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Equipped drag occupies Back slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);
	TestFalse(
		TEXT("Stale bag payload is rejected after the item moves"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));

	UEmbermereItemData* StaleItem = NewObject<UEmbermereItemData>();
	StaleItem->Category = EEmbermereItemCategory::Armor;
	StaleItem->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	TestFalse(
		TEXT("Mismatched equipped-item identity cannot return another item"),
		HudWidget->ReturnEquipmentItemToInventory(StaleItem, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Equipped item can return to an available bag"),
		HudWidget->CanReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Equipment-to-bag drop uses the atomic unequip transaction"),
		HudWidget->ReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Returned item restores bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestNull(TEXT("Returned item clears equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back));

	UEmbermerePlayerHudWidget* FullBagHud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* FullBag = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* FullBagEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereItemData* Filler = NewObject<UEmbermereItemData>();
	Filler->DisplayName = FText::FromString(TEXT("Filler"));
	Filler->MaxStack = 1;
	FullBag->MaxSlots = 1;
	FullBagHud->Inventory = FullBag;
	FullBagHud->Equipment = FullBagEquipment;
	TestTrue(TEXT("Full-bag fixture equips gear"), FullBagEquipment->EquipItem(RecruitPack, 2));
	TestTrue(TEXT("Full-bag fixture fills its only slot"), FullBag->AddItem(Filler, 1));
	TestFalse(
		TEXT("Full bag rejects equipment-to-bag drop preflight"),
		FullBagHud->CanReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestFalse(
		TEXT("Full bag rejects the actual equipment-to-bag drop"),
		FullBagHud->ReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Rejected full-bag drop preserves equipped item"),
		FullBagEquipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryStableSortingTest,
	"Embermere.Inventory.StableSorting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryStableSortingTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	TestNotNull(TEXT("HUD widget can be created for inventory sorting"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created for sorting"), Inventory);
	if (!HudWidget || !Inventory)
	{
		return false;
	}

	auto MakeItem = [](const TCHAR* ItemId, const TCHAR* DisplayName, EEmbermereItemCategory Category)
	{
		UEmbermereItemData* Item = NewObject<UEmbermereItemData>();
		Item->ItemId = FName(ItemId);
		Item->DisplayName = FText::FromString(DisplayName);
		Item->Category = Category;
		Item->MaxStack = 1;
		return Item;
	};

	UEmbermereItemData* AncientPebble = MakeItem(TEXT("AncientPebble"), TEXT("Ancient Pebble"), EEmbermereItemCategory::Misc);
	UEmbermereItemData* MarshTonic = MakeItem(TEXT("MarshTonic"), TEXT("Marsh Tonic"), EEmbermereItemCategory::Consumable);
	UEmbermereItemData* FenwatchSeal = MakeItem(TEXT("FenwatchSeal"), TEXT("Fenwatch Seal"), EEmbermereItemCategory::Quest);
	UEmbermereItemData* RecruitPack = MakeItem(TEXT("RecruitPack"), TEXT("Recruit Pack"), EEmbermereItemCategory::Armor);
	UEmbermereItemData* AshenBlade = MakeItem(TEXT("AshenBlade"), TEXT("Ashen Blade"), EEmbermereItemCategory::Weapon);
	UEmbermereItemData* BitterDraught = MakeItem(TEXT("BitterDraught"), TEXT("Bitter Draught"), EEmbermereItemCategory::Consumable);

	HudWidget->Inventory = Inventory;
	TestTrue(TEXT("Misc item enters the unsorted bag"), Inventory->AddItem(AncientPebble, 1));
	TestTrue(TEXT("Two identical consumable stacks enter the unsorted bag"), Inventory->AddItem(MarshTonic, 2));
	TestTrue(TEXT("Quest item enters the unsorted bag"), Inventory->AddItem(FenwatchSeal, 1));
	TestTrue(TEXT("Armor enters the unsorted bag"), Inventory->AddItem(RecruitPack, 1));
	TestTrue(TEXT("Weapon enters the unsorted bag"), Inventory->AddItem(AshenBlade, 1));
	TestTrue(TEXT("Alphabetically earlier consumable enters last"), Inventory->AddItem(BitterDraught, 1));
	TestEqual(TEXT("Sorting fixture has seven stacks"), Inventory->Stacks.Num(), 7);

	TestTrue(TEXT("Second duplicate stack can be selected"), HudWidget->SelectInventoryItem(2));
	TestTrue(TEXT("Mixed bag reports a changed order"), HudWidget->SortInventory());

	const TArray<UEmbermereItemData*> ExpectedOrder = {
		AshenBlade,
		RecruitPack,
		BitterDraught,
		MarshTonic,
		MarshTonic,
		FenwatchSeal,
		AncientPebble
	};
	for (int32 Index = 0; Index < ExpectedOrder.Num(); ++Index)
	{
		TestTrue(
			*FString::Printf(TEXT("Sorted stack %d has expected category/name order"), Index),
			Inventory->Stacks[Index].Item == ExpectedOrder[Index]);
	}

	TestEqual(
		TEXT("Selection follows the second identical stack by item occurrence"),
		HudWidget->GetSelectedInventoryStackIndex(),
		4);
	TestTrue(
		TEXT("Selected stack still points to the same item identity"),
		Inventory->Stacks[HudWidget->GetSelectedInventoryStackIndex()].Item == MarshTonic);
	TestTrue(
		TEXT("Stable sorting preserves the first identical stack before the selected duplicate"),
		Inventory->Stacks[3].Item == MarshTonic);

	TestFalse(TEXT("Sorting an already ordered bag is a no-op"), HudWidget->SortInventory());
	TestEqual(
		TEXT("No-op sorting preserves selected duplicate occurrence"),
		HudWidget->GetSelectedInventoryStackIndex(),
		4);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEquipmentSlotRulesTest,
	"Embermere.Equipment.SlotRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentSlotRulesTest::RunTest(const FString& Parameters)
{
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereItemData* RecruitBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshReed = NewObject<UEmbermereItemData>();
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	if (!Equipment || !RecruitBlade || !MarshBlade || !MarshReed)
	{
		return false;
	}

	RecruitBlade->DisplayName = FText::FromString(TEXT("Recruit Blade"));
	RecruitBlade->Category = EEmbermereItemCategory::Weapon;
	RecruitBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	RecruitBlade->RequiredLevel = 2;
	RecruitBlade->StatBonuses.Power = 3.0f;

	MarshBlade->DisplayName = FText::FromString(TEXT("Marsh Blade"));
	MarshBlade->Category = EEmbermereItemCategory::Weapon;
	MarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;

	MarshReed->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	MarshReed->Category = EEmbermereItemCategory::Misc;

	TestTrue(TEXT("Weapon data reports equippable"), RecruitBlade->IsEquippable());
	TestEqual(TEXT("Weapon primary action is Equip"), RecruitBlade->GetPrimaryActionLabel().ToString(), FString(TEXT("Equip")));
	TestFalse(TEXT("Level requirement rejects low-level character"), Equipment->CanEquip(RecruitBlade, 1));
	TestTrue(TEXT("Level requirement accepts eligible character"), Equipment->CanEquip(RecruitBlade, 2));
	TestFalse(TEXT("Low-level equip attempt fails"), Equipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Eligible equip attempt succeeds"), Equipment->EquipItem(RecruitBlade, 2));
	TestTrue(TEXT("Main-hand slot stores equipped weapon"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestTrue(TEXT("Equipped item query finds weapon"), Equipment->IsItemEquipped(RecruitBlade));
	TestEqual(TEXT("Equipment aggregates weapon power"), Equipment->GetTotalStatBonuses().Power, 3.0f);

	TestTrue(TEXT("Equipping a replacement weapon succeeds"), Equipment->EquipItem(MarshBlade, 2));
	TestTrue(TEXT("Replacement occupies the same slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestFalse(TEXT("Replaced weapon is no longer equipped"), Equipment->IsItemEquipped(RecruitBlade));
	TestFalse(TEXT("Miscellaneous item cannot be equipped"), Equipment->EquipItem(MarshReed, 2));
	TestTrue(TEXT("Unequip returns the removed weapon"), Equipment->UnequipItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestNull(TEXT("Main-hand slot clears after unequip"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand));
	TestNull(TEXT("Unequipping an empty slot is harmless"), Equipment->UnequipItem(EEmbermereEquipmentSlot::MainHand));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEquipmentInventoryTransactionsTest,
	"Embermere.Equipment.InventoryTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentInventoryTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* RecruitBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshBlade = NewObject<UEmbermereItemData>();
	if (!Equipment || !Inventory || !RecruitBlade || !MarshBlade)
	{
		AddError(TEXT("Could not create equipment transaction test objects"));
		return false;
	}

	RecruitBlade->DisplayName = FText::FromString(TEXT("Recruit Blade"));
	RecruitBlade->Category = EEmbermereItemCategory::Weapon;
	RecruitBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	MarshBlade->DisplayName = FText::FromString(TEXT("Marsh Blade"));
	MarshBlade->Category = EEmbermereItemCategory::Weapon;
	MarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;

	TestTrue(TEXT("Recruit weapon enters the bag"), Inventory->AddItem(RecruitBlade, 1));
	TestTrue(TEXT("Replacement weapon enters the bag"), Inventory->AddItem(MarshBlade, 1));
	TestTrue(TEXT("Equipping transfers the recruit weapon out of the bag"), Equipment->EquipFromInventory(RecruitBlade, 1, Inventory));
	TestTrue(TEXT("Recruit weapon occupies its equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Only the replacement remains in the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Replacement stack is preserved"), Inventory->Stacks[0].Item == MarshBlade);

	TestTrue(TEXT("Equipping a replacement succeeds transactionally"), Equipment->EquipFromInventory(MarshBlade, 1, Inventory));
	TestTrue(TEXT("Replacement occupies the slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestEqual(TEXT("Replaced item returns to the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Returned bag item is the recruit weapon"), Inventory->Stacks[0].Item == RecruitBlade);
	TestTrue(TEXT("Unequip transfers the replacement back to the bag"), Equipment->UnequipToInventory(EEmbermereEquipmentSlot::MainHand, Inventory));
	TestNull(TEXT("Unequip clears the equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand));
	TestEqual(TEXT("Both items are in the bag after unequip"), Inventory->Stacks.Num(), 2);

	UEmbermereEquipmentComponent* FullBagEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* FullBagInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* Blocker = NewObject<UEmbermereItemData>();
	FullBagInventory->MaxSlots = 1;
	Blocker->DisplayName = FText::FromString(TEXT("Packed Supplies"));
	TestTrue(TEXT("Full-bag fixture equips a weapon directly"), FullBagEquipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Full-bag fixture fills its only slot"), FullBagInventory->AddItem(Blocker, 1));
	TestFalse(TEXT("Full bag rejects unequip"), FullBagEquipment->UnequipToInventory(EEmbermereEquipmentSlot::MainHand, FullBagInventory));
	TestTrue(TEXT("Rejected unequip leaves the weapon equipped"), FullBagEquipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Rejected unequip leaves the bag unchanged"), FullBagInventory->Stacks.Num(), 1);

	UEmbermereEquipmentComponent* RollbackEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* RollbackInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* StackableMarshBlade = NewObject<UEmbermereItemData>();
	RollbackInventory->MaxSlots = 1;
	StackableMarshBlade->DisplayName = FText::FromString(TEXT("Stackable Marsh Blade"));
	StackableMarshBlade->Category = EEmbermereItemCategory::Weapon;
	StackableMarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	StackableMarshBlade->MaxStack = 2;
	TestTrue(TEXT("Rollback fixture equips the original weapon"), RollbackEquipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Rollback fixture fills its bag stack"), RollbackInventory->AddItem(StackableMarshBlade, 2));
	TestFalse(TEXT("Replacement fails when the old item cannot return to the bag"), RollbackEquipment->EquipFromInventory(StackableMarshBlade, 1, RollbackInventory));
	TestTrue(TEXT("Failed replacement preserves the original equipment"), RollbackEquipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Failed replacement restores the candidate stack"), RollbackInventory->Stacks[0].Quantity, 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEquipmentStatApplicationTest,
	"Embermere.Equipment.StatApplication",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentStatApplicationTest::RunTest(const FString& Parameters)
{
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Stats component can be created for equipment bonuses"), Stats);
	if (!Stats)
	{
		return false;
	}

	Stats->InitializeVitals();
	FEmbermereItemStatBonuses Bonuses;
	Bonuses.MaxHealth = 20.0f;
	Bonuses.MaxMana = 10.0f;
	Bonuses.Armor = 25.0f;
	Bonuses.Power = 3.0f;
	Stats->ApplyEquipmentBonuses(Bonuses);

	TestEqual(TEXT("Equipment increases maximum health"), Stats->MaxHealth, 120.0f);
	TestEqual(TEXT("Equipment increases current health while preserving missing health"), Stats->CurrentHealth, 120.0f);
	TestEqual(TEXT("Equipment increases maximum mana"), Stats->MaxMana, 60.0f);
	TestEqual(TEXT("Equipment increases attack power"), Stats->AttackPower, 13.0f);
	TestEqual(TEXT("Equipment applies armor"), Stats->Armor, 25.0f);

	Stats->ApplyEquipmentBonuses(Bonuses);
	TestEqual(TEXT("Reapplying identical equipment bonuses is idempotent"), Stats->MaxHealth, 120.0f);
	TestTrue(TEXT("Armor mitigates incoming damage"), FMath::IsNearlyEqual(Stats->ApplyDamage(25.0f), 20.0f));

	Stats->ApplyEquipmentBonuses(FEmbermereItemStatBonuses());
	TestEqual(TEXT("Removing gear restores base maximum health"), Stats->MaxHealth, 100.0f);
	TestEqual(TEXT("Removing gear preserves missing health"), Stats->CurrentHealth, 80.0f);
	TestEqual(TEXT("Removing gear restores base attack power"), Stats->AttackPower, 10.0f);
	TestEqual(TEXT("Removing gear clears armor"), Stats->Armor, 0.0f);

	Stats->ApplyDamage(Stats->CurrentHealth);
	TestTrue(TEXT("Character is dead before equipment change"), Stats->IsDead());
	Stats->ApplyEquipmentBonuses(Bonuses);
	TestTrue(TEXT("Maximum-health gear does not resurrect a dead character"), Stats->IsDead());
	TestEqual(TEXT("Dead character remains at zero health after gear change"), Stats->CurrentHealth, 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyLootRulesTest,
	"Embermere.Enemy.LootRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyLootRulesTest::RunTest(const FString& Parameters)
{
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	AEmbermereCharacter* Recipient = NewObject<AEmbermereCharacter>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	if (!Enemy || !Recipient || !Recipient->Inventory || !Tonic)
	{
		AddError(TEXT("Could not create enemy loot test objects"));
		return false;
	}

	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 5;
	Enemy->LootItem = Tonic;
	Enemy->LootQuantity = 2;
	Enemy->LootDropChance = 0.5f;

	TestTrue(TEXT("Loot roll inside chance succeeds"), Enemy->ShouldDropLoot(0.25f));
	TestFalse(TEXT("Loot roll outside chance fails"), Enemy->ShouldDropLoot(0.75f));
	Enemy->LootDropChance = 0.0f;
	TestFalse(TEXT("Zero drop chance never succeeds"), Enemy->ShouldDropLoot(0.0f));
	Enemy->LootDropChance = 1.0f;
	TestTrue(TEXT("Guaranteed drop chance accepts the highest runtime roll"), Enemy->ShouldDropLoot(0.999999f));
	TestTrue(TEXT("Loot can be granted to an inventory owner"), Enemy->GrantLootTo(Recipient));
	TestEqual(TEXT("Loot grant creates one stack"), Recipient->Inventory->Stacks.Num(), 1);
	TestEqual(TEXT("Loot grant uses configured quantity"), Recipient->Inventory->Stacks[0].Quantity, 2);
	TestTrue(TEXT("Loot grant stores configured item"), Recipient->Inventory->Stacks[0].Item == Tonic);

	Enemy->LootQuantity = 0;
	TestFalse(TEXT("Zero-quantity loot is rejected"), Enemy->GrantLootTo(Recipient));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereConsumableUseTest,
	"Embermere.Inventory.ConsumableUse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereConsumableUseTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	if (!HudWidget || !Inventory || !Stats || !Tonic)
	{
		AddError(TEXT("Could not create consumable test objects"));
		return false;
	}

	Tonic->ItemId = "MarshTonic";
	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->Category = EEmbermereItemCategory::Consumable;
	Tonic->MaxStack = 5;
	Tonic->ConsumableEffects.HealHealth = 25.0f;
	Tonic->ConsumableEffects.RestoreMana = 10.0f;
	HudWidget->Inventory = Inventory;
	HudWidget->Stats = Stats;
	Stats->InitializeVitals();
	Stats->ApplyDamage(40.0f);
	Stats->SpendMana(20.0f);
	TestTrue(TEXT("Consumable stack can be added"), Inventory->AddItem(Tonic, 2));
	TestEqual(TEXT("Consumable primary action is Use"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Use")));
	TestTrue(TEXT("Consumable summary reports recovery"), Tonic->GetEffectSummary().ToString().Contains(TEXT("Restores 25 HP")));

	TestTrue(TEXT("Consumable applies when health or mana is missing"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Consumable restores health"), Stats->CurrentHealth, 85.0f);
	TestEqual(TEXT("Consumable restores mana"), Stats->CurrentMana, 40.0f);
	TestEqual(TEXT("Successful use consumes one item"), Inventory->Stacks[0].Quantity, 1);

	TestTrue(TEXT("Second consumable can finish recovery"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Second consumable caps health"), Stats->CurrentHealth, Stats->MaxHealth);
	TestEqual(TEXT("Second consumable caps mana"), Stats->CurrentMana, Stats->MaxMana);
	TestEqual(TEXT("Empty consumable stack is removed"), Inventory->Stacks.Num(), 0);

	TestTrue(TEXT("Fresh consumable can be added at full resources"), Inventory->AddItem(Tonic, 1));
	TestFalse(TEXT("Consumable cannot be wasted at full resources"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Failed use preserves inventory"), Inventory->Stacks[0].Quantity, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryCapacityTransactionsTest,
	"Embermere.Inventory.CapacityTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryCapacityTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	UEmbermereItemData* Reed = NewObject<UEmbermereItemData>();
	if (!Inventory || !Tonic || !Reed)
	{
		AddError(TEXT("Could not create inventory capacity transaction test objects"));
		return false;
	}

	Inventory->MaxSlots = 1;
	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 2;
	Reed->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	Reed->MaxStack = 10;

	TestTrue(TEXT("A stack that fits can be added"), Inventory->AddItem(Tonic, 2));
	TestEqual(TEXT("Quantity query sums the stored stack"), Inventory->GetItemQuantity(Tonic), 2);
	TestFalse(TEXT("Over-capacity add is rejected"), Inventory->AddItem(Tonic, 1));
	TestEqual(TEXT("Rejected stack growth leaves quantity unchanged"), Inventory->GetItemQuantity(Tonic), 2);
	TestFalse(TEXT("Different item cannot enter a full bag"), Inventory->AddItem(Reed, 1));
	TestEqual(TEXT("Rejected different item creates no partial stack"), Inventory->GetItemQuantity(Reed), 0);
	TestFalse(TEXT("Over-quantity removal is rejected"), Inventory->RemoveItem(Tonic, 3));
	TestEqual(TEXT("Rejected removal leaves quantity unchanged"), Inventory->GetItemQuantity(Tonic), 2);
	TestTrue(TEXT("Available quantity can be removed"), Inventory->RemoveItem(Tonic, 1));
	TestEqual(TEXT("Successful removal decrements exactly once"), Inventory->GetItemQuantity(Tonic), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereHotbarCooldownDisplayTest,
	"Embermere.UI.HotbarCooldownDisplay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereHotbarCooldownDisplayTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereHotbarComponent* Hotbar = NewObject<UEmbermereHotbarComponent>();
	TestNotNull(TEXT("HUD widget can be created for hotbar display"), HudWidget);
	TestNotNull(TEXT("Hotbar component can be created for display"), Hotbar);
	if (!HudWidget || !Hotbar)
	{
		return false;
	}

	FEmbermereAbilityDefinition Strike;
	const UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	TestNotNull(TEXT("Native rules resolve for hotbar display"), Rules);
	if (!Rules || !Rules->GetAbilityDefinition("Strike", Strike))
	{
		AddError(TEXT("Strike ability definition did not resolve for hotbar display"));
		return false;
	}
	Hotbar->SetAbilityInSlot(0, Strike);
	HudWidget->Hotbar = Hotbar;

	const FString ReadyText = HudWidget->GetHotbarSlotDisplayText(0, 0.0f).ToString();
	TestTrue(TEXT("Ready hotbar text includes key and ability"), ReadyText.Contains(TEXT("1\nStrike")));
	TestEqual(TEXT("Ready hotbar text has no countdown line"), ReadyText, FString(TEXT("1\nStrike")));
	TestNotNull(TEXT("Ready hotbar ability resolves its icon"), HudWidget->ResolveAbilityIconForUi(Strike));
	TestEqual(TEXT("Hotbar ability icon dimensions stay fixed"), HudWidget->GetHotbarSlotIconDimensions(), FVector2D(32.0f, 32.0f));
	const FString StrikeTooltip = HudWidget->GetHotbarSlotTooltipText(0).ToString();
	TestTrue(TEXT("Ability tooltip keeps its description"), StrikeTooltip.Contains(TEXT("A simple weapon attack.")));
	TestTrue(TEXT("Ability tooltip includes power"), StrikeTooltip.Contains(TEXT("Power 18")));
	TestTrue(TEXT("Ability tooltip includes no-mana state"), StrikeTooltip.Contains(TEXT("No mana")));
	TestTrue(TEXT("Ability tooltip converts range to meters"), StrikeTooltip.Contains(TEXT("2.2m Range")));
	TestTrue(TEXT("Ability tooltip includes cooldown"), StrikeTooltip.Contains(TEXT("1.5s Cooldown")));

	const FString CoolingText = HudWidget->GetHotbarSlotDisplayText(0, 1.26f).ToString();
	TestTrue(TEXT("Cooling hotbar text keeps ability label"), CoolingText.Contains(TEXT("Strike")));
	TestTrue(TEXT("Cooling hotbar text rounds countdown for display"), CoolingText.Contains(TEXT("1.3s")));
	TestTrue(TEXT("Interact slot keeps its command label"), HudWidget->GetHotbarSlotDisplayText(9, 0.0f).ToString().Contains(TEXT("F\nInteract")));
	TestTrue(TEXT("Interact slot keeps an accessible tooltip"), HudWidget->GetHotbarSlotTooltipText(9).ToString().Contains(TEXT("Interact")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereHudChatLogTest,
	"Embermere.UI.ChatLog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereHudChatLogTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	if (!HudWidget)
	{
		return false;
	}

	TestEqual(TEXT("Chat log starts empty"), HudWidget->GetChatMessageCount(), 0);
	HudWidget->AddChatMessage(FText::FromString(TEXT("Marsh Prowler hits you for 4")), FLinearColor::Red);
	TestEqual(TEXT("Chat log stores a posted combat line"), HudWidget->GetChatMessageCount(), 1);

	for (int32 Index = 0; Index < 10; ++Index)
	{
		HudWidget->AddChatMessage(FText::FromString(FString::Printf(TEXT("Message %d"), Index)), FLinearColor::White);
	}
	TestEqual(TEXT("Chat log keeps the configured number of recent messages"), HudWidget->GetChatMessageCount(), HudWidget->GetChatMessageLimit());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereQuestRewardTest,
	"Embermere.Quests.CompletionRewards",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestRewardTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	if (!Character)
	{
		return false;
	}

	TestNotNull(TEXT("Character has stats"), Character->Stats.Get());
	TestNotNull(TEXT("Character has inventory"), Character->Inventory.Get());
	TestNotNull(TEXT("Character has quest log"), Character->QuestLog.Get());
	TestNotNull(TEXT("Character has wallet"), Character->Wallet.Get());
	if (!Character->Stats || !Character->Inventory || !Character->QuestLog || !Character->Wallet)
	{
		return false;
	}

	UEmbermereItemData* RewardItem = NewObject<UEmbermereItemData>();
	RewardItem->ItemId = "RecruitPack";
	RewardItem->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RewardItem->MaxStack = 5;

	UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
	Quest->QuestId = "FirstSignsAtTheRuin";
	Quest->Title = FText::FromString(TEXT("First Signs at the Ruin"));
	Quest->ObjectiveId = "StarterEnemyDefeated";
	Quest->RequiredObjectiveCount = 2;
	Quest->RewardExperience = 75;
	Quest->RewardCopper = 20;
	Quest->RewardItem = RewardItem;

	Character->Stats->InitializeVitals();

	TestTrue(TEXT("Quest can be accepted"), Character->QuestLog->AcceptQuest(Quest));
	TestTrue(TEXT("First objective progress is accepted"), Character->QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1));
	TestFalse(TEXT("Quest cannot complete early"), Character->QuestLog->TryCompleteActiveQuest());
	TestTrue(TEXT("Second objective progress is accepted"), Character->QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1));
	TestTrue(TEXT("Quest completes when objective count is met"), Character->QuestLog->TryCompleteActiveQuest());

	TestTrue(TEXT("Quest state is completed"), Character->QuestLog->ActiveQuest.bCompleted);
	TestEqual(TEXT("XP reward is granted"), Character->Stats->CurrentExperience, 75);
	TestEqual(TEXT("Copper reward is granted exactly once"), Character->Wallet->Copper, 60);
	TestEqual(TEXT("Reward item creates one inventory stack"), Character->Inventory->Stacks.Num(), 1);
	if (Character->Inventory->Stacks.Num() > 0)
	{
		TestTrue(TEXT("Reward stack contains reward item"), Character->Inventory->Stacks[0].Item == RewardItem);
		TestEqual(TEXT("Reward stack quantity is one"), Character->Inventory->Stacks[0].Quantity, 1);
	}

	TestFalse(TEXT("Completed quest cannot complete again"), Character->QuestLog->TryCompleteActiveQuest());
	TestEqual(TEXT("Completed quest cannot duplicate copper"), Character->Wallet->Copper, 60);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereMultiQuestRuntimeTest,
	"Embermere.Quests.MultiQuestRuntime",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereMultiQuestRuntimeTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AActor* SecondQuestGiver = NewObject<AActor>();
	UEmbermereInteractableComponent* SecondInteractable =
		NewObject<UEmbermereInteractableComponent>(SecondQuestGiver);
	UEmbermereQuestData* SecondQuest = NewObject<UEmbermereQuestData>();
	if (!MaraQuest || !Source || !SecondQuestGiver ||
		!SecondInteractable || !SecondQuest)
	{
		AddError(TEXT("Could not create second-quest compatibility fixtures"));
		return false;
	}

	SecondQuest->QuestId = TEXT("FenwatchStillWaters");
	SecondQuest->Title = FText::FromString(TEXT("Still Waters"));
	SecondQuest->ObjectiveId = TEXT("FenwatchRestCompleted");
	SecondQuest->RequiredObjectiveCount = 1;
	SecondQuest->RewardExperience = 50;
	SecondQuest->RewardCopper = 10;
	SecondInteractable->QuestToOffer = SecondQuest;

	TestEqual(
		TEXT("A valid first quest passes acceptance preflight"),
		Source->QuestLog->EvaluateQuestAcceptance(MaraQuest),
		EEmbermereQuestAcceptanceResult::Success);
	TestTrue(TEXT("Mara quest enters the bounded runtime ledger"),
		Source->QuestLog->AcceptQuest(MaraQuest));
	TestEqual(
		TEXT("A different valid quest can coexist in version 3"),
		Source->QuestLog->EvaluateQuestAcceptance(SecondQuest),
		EEmbermereQuestAcceptanceResult::Success);
	TestTrue(TEXT("Second quest enters the same ledger"),
		Source->QuestLog->AcceptQuest(SecondQuest));
	TestEqual(TEXT("Ledger tracks both stable quest IDs"),
		Source->QuestLog->QuestStates.Num(), 2);
	TestTrue(TEXT("Mara quest reaches ready-to-turn-in state"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			MaraQuest->QuestId,
			MaraQuest->ObjectiveId,
			MaraQuest->RequiredObjectiveCount));

	const int32 CopperBeforeWrongGiver = Source->Wallet->Copper;
	const int32 ExperienceBeforeWrongGiver = Source->Stats->CurrentExperience;
	SecondInteractable->Interact(Source);
	FEmbermereQuestState MaraState;
	FEmbermereQuestState SecondState;
	TestTrue(TEXT("Mara state remains queryable by stable ID"),
		Source->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState));
	TestFalse(TEXT("A different quest giver cannot complete Mara's ready quest"),
		MaraState.bCompleted);
	TestEqual(TEXT("Wrong-giver rejection preserves copper"),
		Source->Wallet->Copper, CopperBeforeWrongGiver);
	TestEqual(TEXT("Wrong-giver rejection preserves experience"),
		Source->Stats->CurrentExperience, ExperienceBeforeWrongGiver);

	TestTrue(TEXT("Only Mara's matching quest turn-in may complete the quest"),
		Source->QuestLog->TryCompleteQuest(MaraQuest));
	TestTrue(TEXT("Mara completion remains keyed in durable history"),
		Source->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState) &&
		MaraState.bCompleted);
	TestTrue(TEXT("Completing Mara does not remove the second quest"),
		Source->QuestLog->GetQuestStateById(SecondQuest->QuestId, SecondState) &&
		!SecondState.bCompleted && SecondState.CurrentObjectiveCount == 0);

	TestTrue(TEXT("Exact second objective routing advances only the second quest"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			SecondQuest->QuestId,
			SecondQuest->ObjectiveId,
			1));
	TestTrue(TEXT("Transient focus follows the exact progressed quest"),
		Source->QuestLog->ActiveQuest.Quest == SecondQuest);
	TestTrue(TEXT("Mara progress remains unchanged after second-quest progress"),
		Source->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState) &&
		MaraState.CurrentObjectiveCount == MaraQuest->RequiredObjectiveCount);

	Source->Wallet->SetCopperForPrototype(MAX_int32 - 5);
	const int32 ExperienceBeforeRejectedReward = Source->Stats->CurrentExperience;
	TestFalse(TEXT("Reward overflow rejects the complete turn-in before mutation"),
		Source->QuestLog->TryCompleteQuest(SecondQuest));
	TestTrue(TEXT("Rejected reward leaves the exact quest ready but incomplete"),
		Source->QuestLog->GetQuestStateById(SecondQuest->QuestId, SecondState) &&
		!SecondState.bCompleted && SecondState.CurrentObjectiveCount == 1);
	TestEqual(TEXT("Rejected reward preserves experience"),
		Source->Stats->CurrentExperience, ExperienceBeforeRejectedReward);

	Source->Wallet->SetCopperForPrototype(60);
	TestTrue(TEXT("Second quest completes once after reward preflight succeeds"),
		Source->QuestLog->TryCompleteQuestById(SecondQuest->QuestId));
	TestFalse(TEXT("Second quest cannot pay rewards twice"),
		Source->QuestLog->TryCompleteQuestById(SecondQuest->QuestId));
	TestEqual(TEXT("Both quest rewards grant exact copper once"), Source->Wallet->Copper, 70);
	TestEqual(TEXT("Both quest rewards grant exact XP once"), Source->Stats->CurrentExperience, 175);
	TestTrue(TEXT("Explicit focus can return to Mara without mutating either record"),
		Source->QuestLog->FocusQuest(MaraQuest->QuestId) &&
		Source->QuestLog->ActiveQuest.Quest == MaraQuest);
	TestEqual(TEXT("Duplicate quest acceptance remains rejected"),
		Source->QuestLog->EvaluateQuestAcceptance(MaraQuest),
		EEmbermereQuestAcceptanceResult::AlreadyTracked);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchKeeperPresentationTest,
	"Embermere.NPC.FenwatchKeeperPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchKeeperPresentationTest::RunTest(const FString& Parameters)
{
	UStaticMesh* KeeperMesh = LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper/SM_EmbermereFenwatchKeeper_Mara_01.SM_EmbermereFenwatchKeeper_Mara_01"));
	UEmbermereQuestData* StarterQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UBlueprint* QuestGiverBlueprint = LoadObject<UBlueprint>(
		nullptr,
		TEXT("/Game/Blueprints/BP_QuestGiver.BP_QuestGiver"));
	TestNotNull(TEXT("Accepted Fenwatch keeper static fallback loads"), KeeperMesh);
	TestNotNull(TEXT("Mara's starter quest data loads"), StarterQuest);
	TestNotNull(TEXT("Quest-giver Blueprint loads"), QuestGiverBlueprint);
	if (!KeeperMesh || !StarterQuest || !QuestGiverBlueprint || !QuestGiverBlueprint->GeneratedClass)
	{
		return false;
	}

	TestNotNull(
		TEXT("Quest giver retains its Simple Construction Script"),
		QuestGiverBlueprint->SimpleConstructionScript.Get());
	if (!QuestGiverBlueprint->SimpleConstructionScript)
	{
		return false;
	}

	UStaticMeshComponent* KeeperVisual = nullptr;
	UEmbermereInteractableComponent* MaraInteraction = nullptr;
	for (USCS_Node* Node : QuestGiverBlueprint->SimpleConstructionScript->GetAllNodes())
	{
		if (Node)
		{
			if (!KeeperVisual)
			{
				KeeperVisual = Cast<UStaticMeshComponent>(Node->ComponentTemplate);
			}
			if (!MaraInteraction)
			{
				MaraInteraction = Cast<UEmbermereInteractableComponent>(Node->ComponentTemplate);
			}
		}
	}
	TestNotNull(TEXT("Quest giver retains its reversible static component template"), KeeperVisual);
	TestNotNull(TEXT("Quest giver retains its interaction authority"), MaraInteraction);
	if (!KeeperVisual || !MaraInteraction)
	{
		return false;
	}

	TestNull(
		TEXT("Quest Blueprint releases render art to the separate presentation wrapper"),
		KeeperVisual->GetStaticMesh());
	TestTrue(
		TEXT("Reversible static template preserves the accepted ground offset"),
		KeeperVisual->GetRelativeLocation().Equals(FVector(0.0f, 0.0f, -140.0f), KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Reversible static template preserves the accepted visual yaw"),
		FMath::IsNearlyEqual(KeeperVisual->GetRelativeRotation().Yaw, 100.0f, KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Reversible static template keeps unit authored scale"),
		KeeperVisual->GetRelativeScale3D().Equals(FVector::OneVector, KINDA_SMALL_NUMBER));
	TestEqual(
		TEXT("Quest Blueprint's dormant art template remains non-colliding"),
		KeeperVisual->GetCollisionEnabled(),
		ECollisionEnabled::NoCollision);
	TestEqual(TEXT("Static fallback keeps its reviewed triangle count"), KeeperMesh->GetNumTriangles(0), 3280);
	TestEqual(
		TEXT("Mara retains her exact interaction name"),
		MaraInteraction->DisplayName.ToString(),
		FString(TEXT("Mara Fenwatch")));
	TestEqual(
		TEXT("Mara retains her exact dialogue"),
		MaraInteraction->DialogueText.ToString(),
		FString(TEXT("You picked a lively day to arrive. Something is stirring near the old stones east of town. Cull a few of those things and come back whole.")));
	TestTrue(TEXT("Mara retains the exact starter quest"), MaraInteraction->QuestToOffer == StarterQuest);
	TestTrue(TEXT("Mara retains her gold world marker"), MaraInteraction->bShowWorldMarker);
	TestTrue(
		TEXT("Mara retains the accepted marker height"),
		FMath::IsNearlyEqual(MaraInteraction->MarkerHeight, 185.0f, 0.1f));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchKeeperIdlePresentationTest,
	"Embermere.NPC.FenwatchKeeperIdlePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchKeeperIdlePresentationTest::RunTest(const FString& Parameters)
{
	const FString ArtRoot = TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchKeeper");
	UStaticMesh* StaticFallback = LoadObject<UStaticMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SM_EmbermereFenwatchKeeper_Mara_01.SM_EmbermereFenwatchKeeper_Mara_01")));
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SK_EmbermereFenwatchKeeper_Mara_01.SK_EmbermereFenwatchKeeper_Mara_01")));
	UAnimSequence* IdleAnimation = LoadObject<UAnimSequence>(
		nullptr,
		*(ArtRoot + TEXT("/Animations/A_EmbermereFenwatchKeeper_Mara_Idle.A_EmbermereFenwatchKeeper_Mara_Idle")));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Accepted keeper static fallback loads"), StaticFallback);
	TestNotNull(TEXT("Rigged keeper skeletal mesh loads"), SkeletalMesh);
	TestNotNull(TEXT("Rigged keeper Idle loads"), IdleAnimation);
	TestNotNull(TEXT("Rigged keeper presentation can be created"), Presentation);
	if (!StaticFallback || !SkeletalMesh || !IdleAnimation || !Presentation)
	{
		return false;
	}

	const FVector Size = SkeletalMesh->GetBounds().BoxExtent * 2.0f;
	TestTrue(TEXT("Rigged keeper width retains the accepted silhouette"), FMath::IsNearlyEqual(Size.X, 107.45f, 1.0f));
	TestTrue(TEXT("Rigged keeper depth retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Y, 71.0f, 1.0f));
	TestTrue(TEXT("Rigged keeper height retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Z, 207.5f, 1.0f));
	TestEqual(TEXT("Rigged keeper keeps six project materials"), SkeletalMesh->GetMaterials().Num(), 6);
	const FReferenceSkeleton& ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
	TestEqual(
		TEXT("Classic FBX keeps one Armature root plus the nine reviewed keeper bones"),
		ReferenceSkeleton.GetRawBoneNum(),
		10);
	const TArray<FName> AuthoredBoneNames = {
		TEXT("root"),
		TEXT("pelvis"),
		TEXT("spine"),
		TEXT("neck"),
		TEXT("head"),
		TEXT("upper_arm_l"),
		TEXT("forearm_l"),
		TEXT("upper_arm_r"),
		TEXT("forearm_r"),
	};
	for (const FName BoneName : AuthoredBoneNames)
	{
		TestTrue(
			*FString::Printf(TEXT("Rigged keeper retains authored bone %s"), *BoneName.ToString()),
			ReferenceSkeleton.FindBoneIndex(BoneName) != INDEX_NONE);
	}
	const int32 AuthoredRootIndex = ReferenceSkeleton.FindBoneIndex(TEXT("root"));
	TestTrue(
		TEXT("Keeper authored root remains beneath the classic FBX Armature root"),
		AuthoredRootIndex > 0 && ReferenceSkeleton.GetParentIndex(AuthoredRootIndex) == 0);
	TestTrue(
		TEXT("Idle animation uses the rigged keeper Skeleton"),
		IdleAnimation->GetSkeleton() == SkeletalMesh->GetSkeleton());
	TestTrue(
		TEXT("Idle animation keeps its reviewed 3.6-second cycle"),
		FMath::IsNearlyEqual(IdleAnimation->GetPlayLength(), 3.6f, 0.12f));

	Presentation->StaticVisualMesh = StaticFallback;
	Presentation->SkeletalVisualMesh = SkeletalMesh;
	Presentation->IdleAnimation = IdleAnimation;
	Presentation->bLoopIdleAnimation = true;
	Presentation->IdleAnimationPlayRate = 1.0f;
	Presentation->bPreferSkeletalVisual = true;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Saved-style keeper configuration resolves the skeletal lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::SkeletalMesh);
	TestEqual(
		TEXT("Saved-style keeper configuration resolves looping single-node Idle"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::SingleNodeIdle);
	TestTrue(
		TEXT("Keeper Idle is the exact serialized animation"),
		Presentation->SkeletalVisual->AnimationData.AnimToPlay == IdleAnimation);
	TestTrue(
		TEXT("Keeper Idle remains marked playing and looping"),
		Presentation->SkeletalVisual->AnimationData.bSavedPlaying &&
			Presentation->SkeletalVisual->AnimationData.bSavedLooping);
	TestTrue(TEXT("Rigged keeper remains non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Rigged keeper presentation owns no interaction or quest authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Rigged keeper presentation owns no vendor authority"),
		Presentation->FindComponentByClass<UEmbermereVendorComponent>());
	TestNull(
		TEXT("Rigged keeper presentation owns no trainer authority"),
		Presentation->FindComponentByClass<UEmbermereTrainerComponent>());

	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Disabling skeletal preference restores the accepted keeper fallback"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(
		TEXT("Keeper static fallback remains the exact accepted mesh"),
		Presentation->StaticVisual->GetStaticMesh() == StaticFallback);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereNpcContextGreetingPresentationTest,
	"Embermere.NPC.ContextGreetingPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereNpcContextGreetingPresentationTest::RunTest(const FString& Parameters)
{
	const FString AvailableCopy = TEXT("The eastern stones are restless.");
	const FString ActiveCopy = TEXT("Keep to the road, then watch the reeds.");
	const FString ReadyCopy = TEXT("You have done enough. Come speak with me.");
	const FString CompletedCopy = TEXT("Fenwatch remembers a steady hand.");

	UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
	UEmbermereQuestData* OtherQuest = NewObject<UEmbermereQuestData>();
	UEmbermereNpcGreetingWidget* GreetingWidget = NewObject<UEmbermereNpcGreetingWidget>();
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	if (!Quest || !OtherQuest || !GreetingWidget || !Presentation)
	{
		AddError(TEXT("Could not create contextual greeting fixtures"));
		return false;
	}

	Quest->RequiredObjectiveCount = 3;
	Quest->AvailableGreeting = FText::FromString(AvailableCopy);
	Quest->ActiveGreeting = FText::FromString(ActiveCopy);
	Quest->ReadyGreeting = FText::FromString(ReadyCopy);
	Quest->CompletedGreeting = FText::FromString(CompletedCopy);

	FEmbermereQuestState QuestState;
	TestEqual(
		TEXT("No active quest resolves the available greeting"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(Quest, QuestState),
		EEmbermereNpcGreetingState::Available);
	TestEqual(
		TEXT("Available state resolves quest-owned copy"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingText(
			Quest,
			EEmbermereNpcGreetingState::Available).ToString(),
		AvailableCopy);

	QuestState.Quest = Quest;
	QuestState.CurrentObjectiveCount = 1;
	TestEqual(
		TEXT("Incomplete matching quest resolves active"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(Quest, QuestState),
		EEmbermereNpcGreetingState::Active);
	TestEqual(
		TEXT("Active state resolves quest-owned copy"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingText(
			Quest,
			EEmbermereNpcGreetingState::Active).ToString(),
		ActiveCopy);

	QuestState.CurrentObjectiveCount = Quest->RequiredObjectiveCount;
	TestEqual(
		TEXT("Satisfied objective resolves ready-to-turn-in"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(Quest, QuestState),
		EEmbermereNpcGreetingState::ReadyToTurnIn);
	TestEqual(
		TEXT("Ready state resolves quest-owned copy"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingText(
			Quest,
			EEmbermereNpcGreetingState::ReadyToTurnIn).ToString(),
		ReadyCopy);

	QuestState.bCompleted = true;
	TestEqual(
		TEXT("Completed matching quest resolves completed"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(Quest, QuestState),
		EEmbermereNpcGreetingState::Completed);
	TestEqual(
		TEXT("Completed state resolves quest-owned copy"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingText(
			Quest,
			EEmbermereNpcGreetingState::Completed).ToString(),
		CompletedCopy);

	QuestState.Quest = OtherQuest;
	QuestState.bCompleted = false;
	TestEqual(
		TEXT("An unrelated active quest hides this greeting"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(Quest, QuestState),
		EEmbermereNpcGreetingState::Hidden);
	TestEqual(
		TEXT("Missing quest authority hides contextual presentation"),
		AEmbermereNpcPresentationActor::ResolveContextGreetingState(nullptr, QuestState),
		EEmbermereNpcGreetingState::Hidden);

	GreetingWidget->SetGreetingState(EEmbermereNpcGreetingState::ReadyToTurnIn, FText::FromString(ReadyCopy));
	TestEqual(
		TEXT("Native greeting widget retains resolved state"),
		GreetingWidget->GetGreetingState(),
		EEmbermereNpcGreetingState::ReadyToTurnIn);
	TestEqual(TEXT("Native greeting widget retains copy"), GreetingWidget->GetGreetingText().ToString(), ReadyCopy);
	TestTrue(
		TEXT("Greeting panel uses fixed 320 by 56 bounds"),
		GreetingWidget->GetPanelDimensions().Equals(FVector2D(320.0f, 56.0f), KINDA_SMALL_NUMBER));
	TestEqual(
		TEXT("Visible greeting is hit-test-invisible"),
		GreetingWidget->GetVisibility(),
		ESlateVisibility::HitTestInvisible);
	GreetingWidget->SetGreetingState(EEmbermereNpcGreetingState::Hidden, FText::GetEmpty());
	TestEqual(
		TEXT("Hidden greeting collapses without layout participation"),
		GreetingWidget->GetVisibility(),
		ESlateVisibility::Collapsed);

	TestNotNull(TEXT("NPC wrapper owns a dedicated greeting widget component"), Presentation->ContextGreetingWidget.Get());
	TestTrue(TEXT("Greeting widget remains presentation-only"), Presentation->IsContextGreetingPresentationOnly());
	TestTrue(
		TEXT("Greeting widget uses fixed draw dimensions"),
		Presentation->ContextGreetingWidget->GetDrawSize().Equals(
			FVector2D(320.0f, 56.0f),
			KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Greeting widget resolves the project-owned native class"),
		Presentation->ContextGreetingWidget->GetWidgetClass() == UEmbermereNpcGreetingWidget::StaticClass());
	TestFalse(TEXT("Context greeting remains opt-in"), Presentation->bEnableContextGreeting);
	TestNull(TEXT("Context greeting defaults to no authority actor"), Presentation->ContextAuthorityActor.Get());
	TestNull(
		TEXT("Greeting presentation owns no interaction or quest authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Greeting presentation owns no vendor authority"),
		Presentation->FindComponentByClass<UEmbermereVendorComponent>());
	TestNull(
		TEXT("Greeting presentation owns no trainer authority"),
		Presentation->FindComponentByClass<UEmbermereTrainerComponent>());

	UEmbermereQuestData* StarterQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	TestNotNull(TEXT("Saved starter quest data loads for greeting copy"), StarterQuest);
	if (StarterQuest)
	{
		TestEqual(TEXT("Starter quest retains available copy"), StarterQuest->AvailableGreeting.ToString(), AvailableCopy);
		TestEqual(TEXT("Starter quest retains active copy"), StarterQuest->ActiveGreeting.ToString(), ActiveCopy);
		TestEqual(TEXT("Starter quest retains ready copy"), StarterQuest->ReadyGreeting.ToString(), ReadyCopy);
		TestEqual(TEXT("Starter quest retains completed copy"), StarterQuest->CompletedGreeting.ToString(), CompletedCopy);
	}

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereNpcPresentationContractTest,
	"Embermere.NPC.PresentationContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereNpcPresentationContractTest::RunTest(const FString& Parameters)
{
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/Enemies/MarshProwler/SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Static presentation fixture loads"), StaticMesh);
	TestNotNull(TEXT("Skeletal presentation fixture loads"), SkeletalMesh);
	TestNotNull(TEXT("NPC presentation actor can be created"), Presentation);
	if (!StaticMesh || !SkeletalMesh || !Presentation)
	{
		return false;
	}

	const FTransform AuthoredTransform(
		FRotator(0.0f, 25.0f, 0.0f),
		FVector(3.0f, 4.0f, 5.0f),
		FVector::OneVector);
	Presentation->StaticVisualMesh = StaticMesh;
	Presentation->VisualRelativeTransform = AuthoredTransform;
	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Static art resolves when no skeletal art is preferred"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(TEXT("Static lane owns the resolved mesh"), Presentation->StaticVisual->GetStaticMesh() == StaticMesh);
	TestTrue(TEXT("Static lane keeps the authored local transform"), Presentation->StaticVisual->GetRelativeTransform().Equals(AuthoredTransform));
	TestTrue(TEXT("Both art lanes remain non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Presentation wrapper does not invent interaction or service behavior"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());

	Presentation->SkeletalVisualMesh = SkeletalMesh;
	Presentation->bPreferSkeletalVisual = true;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Skeletal art can replace static art without changing the actor contract"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::SkeletalMesh);
	TestTrue(TEXT("Skeletal lane owns the resolved mesh"), Presentation->SkeletalVisual->GetSkeletalMeshAsset() == SkeletalMesh);
	TestNull(TEXT("Inactive static lane releases its mesh"), Presentation->StaticVisual->GetStaticMesh());
	TestTrue(TEXT("Skeletal lane keeps the same authored local transform"), Presentation->SkeletalVisual->GetRelativeTransform().Equals(AuthoredTransform));
	TestTrue(TEXT("Skeletal swap keeps both art lanes non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestEqual(
		TEXT("Skeletal art without animation remains an explicit unanimated lane"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::None);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereNpcSkeletalIdlePresentationTest,
	"Embermere.NPC.SkeletalIdlePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereNpcSkeletalIdlePresentationTest::RunTest(const FString& Parameters)
{
	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/Enemies/MarshProwler/SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"));
	UAnimSequence* IdleAnimation = LoadObject<UAnimSequence>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/Enemies/MarshProwler/Animations/A_EmbermereMarshProwler_Idle.A_EmbermereMarshProwler_Idle"));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Static fallback fixture loads"), StaticMesh);
	TestNotNull(TEXT("Project-owned skeletal fixture loads"), SkeletalMesh);
	TestNotNull(TEXT("Project-owned Idle animation loads"), IdleAnimation);
	TestNotNull(TEXT("Skeletal Idle presentation actor can be created"), Presentation);
	if (!StaticMesh || !SkeletalMesh || !IdleAnimation || !Presentation)
	{
		return false;
	}

	const FTransform AuthoredTransform(
		FRotator(0.0f, -35.0f, 0.0f),
		FVector(11.0f, 12.0f, 13.0f),
		FVector(0.8f));
	Presentation->StaticVisualMesh = StaticMesh;
	Presentation->SkeletalVisualMesh = SkeletalMesh;
	Presentation->IdleAnimation = IdleAnimation;
	Presentation->bLoopIdleAnimation = true;
	Presentation->IdleAnimationPlayRate = 0.75f;
	Presentation->VisualRelativeTransform = AuthoredTransform;
	Presentation->bPreferSkeletalVisual = true;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Preferred skeletal art resolves through the rigged lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::SkeletalMesh);
	TestEqual(
		TEXT("Compatible project-owned Idle art resolves through the single-node lane"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::SingleNodeIdle);
	TestEqual(
		TEXT("Idle lane uses Unreal single-node animation mode"),
		Presentation->SkeletalVisual->GetAnimationMode(),
		EAnimationMode::AnimationSingleNode);
	TestTrue(
		TEXT("Idle lane serializes the exact project-owned animation"),
		Presentation->SkeletalVisual->AnimationData.AnimToPlay == IdleAnimation);
	TestTrue(
		TEXT("Idle lane serializes looping intent"),
		Presentation->SkeletalVisual->AnimationData.bSavedLooping);
	TestTrue(
		TEXT("Idle lane serializes playing intent"),
		Presentation->SkeletalVisual->AnimationData.bSavedPlaying);
	TestTrue(
		TEXT("Idle lane serializes the authored play rate"),
		FMath::IsNearlyEqual(Presentation->SkeletalVisual->AnimationData.SavedPlayRate, 0.75f));
	TestTrue(
		TEXT("Skeletal Idle lane preserves the shared authored transform"),
		Presentation->SkeletalVisual->GetRelativeTransform().Equals(AuthoredTransform));
	TestTrue(TEXT("Animated presentation remains non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Animated presentation still owns no interaction authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Animated presentation still owns no vendor authority"),
		Presentation->FindComponentByClass<UEmbermereVendorComponent>());

	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Static preference restores the accepted fallback lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestEqual(
		TEXT("Static fallback clears resolved animation state"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::None);
	TestNull(
		TEXT("Static fallback releases the prior serialized Idle clip"),
		Presentation->SkeletalVisual->AnimationData.AnimToPlay.Get());
	TestTrue(
		TEXT("Static fallback preserves the same authored transform"),
		Presentation->StaticVisual->GetRelativeTransform().Equals(AuthoredTransform));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchQuartermasterPresentationTest,
	"Embermere.NPC.FenwatchQuartermasterPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchQuartermasterPresentationTest::RunTest(const FString& Parameters)
{
	UStaticMesh* QuartermasterMesh = LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/SM_EmbermereFenwatchQuartermaster_01.SM_EmbermereFenwatchQuartermaster_01"));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Fenwatch quartermaster mesh loads"), QuartermasterMesh);
	TestNotNull(TEXT("Fenwatch quartermaster presentation can be created"), Presentation);
	if (!QuartermasterMesh || !Presentation)
	{
		return false;
	}

	const FVector Size = QuartermasterMesh->GetBounds().BoxExtent * 2.0f;
	TestTrue(TEXT("Quartermaster width retains its authored contract"), FMath::IsNearlyEqual(Size.X, 120.842f, 1.0f));
	TestTrue(TEXT("Quartermaster depth retains its authored contract"), FMath::IsNearlyEqual(Size.Y, 93.0f, 1.0f));
	TestTrue(TEXT("Quartermaster height retains its authored contract"), FMath::IsNearlyEqual(Size.Z, 217.0f, 1.0f));
	TestEqual(TEXT("Quartermaster keeps its reviewed triangle count"), QuartermasterMesh->GetNumTriangles(0), 3632);
	TestEqual(TEXT("Quartermaster keeps six authored material slots"), QuartermasterMesh->GetStaticMaterials().Num(), 6);

	TSet<FString> MaterialPaths;
	for (const FStaticMaterial& StaticMaterial : QuartermasterMesh->GetStaticMaterials())
	{
		if (StaticMaterial.MaterialInterface)
		{
			MaterialPaths.Add(StaticMaterial.MaterialInterface->GetPathName());
		}
	}
	TestTrue(
		TEXT("Quartermaster keeps its project-owned skin material"),
		MaterialPaths.Contains(TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster/M_FenwatchQuartermasterSkin.M_FenwatchQuartermasterSkin")));
	TestNotNull(TEXT("Quartermaster mesh retains a body setup"), QuartermasterMesh->GetBodySetup());
	if (QuartermasterMesh->GetBodySetup())
	{
		TestEqual(
			TEXT("Quartermaster mesh carries no authored collision shapes"),
			QuartermasterMesh->GetBodySetup()->AggGeom.GetElementCount(),
			0);
	}

	Presentation->StaticVisualMesh = QuartermasterMesh;
	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Quartermaster resolves through the static presentation lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(
		TEXT("Quartermaster presentation resolves the project-owned mesh"),
		Presentation->StaticVisual->GetStaticMesh() == QuartermasterMesh);
	TestTrue(TEXT("Quartermaster visual lanes remain non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Quartermaster presentation does not invent interaction or vendor behavior"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchQuartermasterIdlePresentationTest,
	"Embermere.NPC.FenwatchQuartermasterIdlePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchQuartermasterIdlePresentationTest::RunTest(const FString& Parameters)
{
	const FString ArtRoot = TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchQuartermaster");
	UStaticMesh* StaticFallback = LoadObject<UStaticMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SM_EmbermereFenwatchQuartermaster_01.SM_EmbermereFenwatchQuartermaster_01")));
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SK_EmbermereFenwatchQuartermaster_01.SK_EmbermereFenwatchQuartermaster_01")));
	UAnimSequence* IdleAnimation = LoadObject<UAnimSequence>(
		nullptr,
		*(ArtRoot + TEXT("/Animations/A_EmbermereFenwatchQuartermaster_Idle.A_EmbermereFenwatchQuartermaster_Idle")));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Accepted quartermaster static fallback loads"), StaticFallback);
	TestNotNull(TEXT("Rigged quartermaster skeletal mesh loads"), SkeletalMesh);
	TestNotNull(TEXT("Rigged quartermaster Idle loads"), IdleAnimation);
	TestNotNull(TEXT("Rigged quartermaster presentation can be created"), Presentation);
	if (!StaticFallback || !SkeletalMesh || !IdleAnimation || !Presentation)
	{
		return false;
	}

	const FVector Size = SkeletalMesh->GetBounds().BoxExtent * 2.0f;
	TestTrue(TEXT("Rigged quartermaster width retains the accepted silhouette"), FMath::IsNearlyEqual(Size.X, 120.842f, 1.0f));
	TestTrue(TEXT("Rigged quartermaster depth retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Y, 93.0f, 1.0f));
	TestTrue(TEXT("Rigged quartermaster height retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Z, 217.0f, 1.0f));
	TestEqual(TEXT("Rigged quartermaster keeps six project materials"), SkeletalMesh->GetMaterials().Num(), 6);
	const FReferenceSkeleton& ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
	TestEqual(
		TEXT("Classic FBX keeps one Armature root plus the nine reviewed quartermaster bones"),
		ReferenceSkeleton.GetRawBoneNum(),
		10);
	const TArray<FName> AuthoredBoneNames = {
		TEXT("root"),
		TEXT("pelvis"),
		TEXT("spine"),
		TEXT("neck"),
		TEXT("head"),
		TEXT("upper_arm_l"),
		TEXT("forearm_l"),
		TEXT("upper_arm_r"),
		TEXT("forearm_r"),
	};
	for (const FName BoneName : AuthoredBoneNames)
	{
		TestTrue(
			*FString::Printf(TEXT("Rigged quartermaster retains authored bone %s"), *BoneName.ToString()),
			ReferenceSkeleton.FindBoneIndex(BoneName) != INDEX_NONE);
	}
	const int32 AuthoredRootIndex = ReferenceSkeleton.FindBoneIndex(TEXT("root"));
	TestTrue(
		TEXT("Quartermaster authored root remains beneath the classic FBX Armature root"),
		AuthoredRootIndex > 0 && ReferenceSkeleton.GetParentIndex(AuthoredRootIndex) == 0);
	TestTrue(
		TEXT("Idle animation uses the rigged quartermaster Skeleton"),
		IdleAnimation->GetSkeleton() == SkeletalMesh->GetSkeleton());
	TestTrue(
		TEXT("Idle animation keeps its reviewed four-second cycle"),
		FMath::IsNearlyEqual(IdleAnimation->GetPlayLength(), 4.0f, 0.12f));

	Presentation->StaticVisualMesh = StaticFallback;
	Presentation->SkeletalVisualMesh = SkeletalMesh;
	Presentation->IdleAnimation = IdleAnimation;
	Presentation->bLoopIdleAnimation = true;
	Presentation->IdleAnimationPlayRate = 1.0f;
	Presentation->bPreferSkeletalVisual = true;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Saved-style quartermaster configuration resolves the skeletal lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::SkeletalMesh);
	TestEqual(
		TEXT("Saved-style quartermaster configuration resolves looping single-node Idle"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::SingleNodeIdle);
	TestTrue(
		TEXT("Quartermaster Idle is the exact serialized animation"),
		Presentation->SkeletalVisual->AnimationData.AnimToPlay == IdleAnimation);
	TestTrue(
		TEXT("Quartermaster Idle remains marked playing and looping"),
		Presentation->SkeletalVisual->AnimationData.bSavedPlaying &&
			Presentation->SkeletalVisual->AnimationData.bSavedLooping);
	TestTrue(TEXT("Rigged quartermaster remains non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Rigged quartermaster presentation owns no interaction authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Rigged quartermaster presentation owns no vendor authority"),
		Presentation->FindComponentByClass<UEmbermereVendorComponent>());

	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Disabling skeletal preference restores the accepted quartermaster fallback"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(
		TEXT("Quartermaster static fallback remains the exact accepted mesh"),
		Presentation->StaticVisual->GetStaticMesh() == StaticFallback);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchArmsmasterPresentationTest,
	"Embermere.NPC.FenwatchArmsmasterPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchArmsmasterPresentationTest::RunTest(const FString& Parameters)
{
	UStaticMesh* ArmsmasterMesh = LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/SM_EmbermereFenwatchArmsmaster_01.SM_EmbermereFenwatchArmsmaster_01"));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Fenwatch armsmaster mesh loads"), ArmsmasterMesh);
	TestNotNull(TEXT("Fenwatch armsmaster presentation can be created"), Presentation);
	if (!ArmsmasterMesh || !Presentation)
	{
		return false;
	}

	const FVector Size = ArmsmasterMesh->GetBounds().BoxExtent * 2.0f;
	TestTrue(TEXT("Armsmaster width retains its imported contract"), FMath::IsNearlyEqual(Size.X, 154.5f, 1.0f));
	TestTrue(TEXT("Armsmaster depth retains its authored contract"), FMath::IsNearlyEqual(Size.Y, 87.0f, 1.0f));
	TestTrue(TEXT("Armsmaster height retains its authored contract"), FMath::IsNearlyEqual(Size.Z, 228.0f, 1.0f));
	TestEqual(
		TEXT("Armsmaster keeps the classic-FBX imported triangle count"),
		ArmsmasterMesh->GetNumTriangles(0),
		2800);
	TestEqual(TEXT("Armsmaster keeps six authored material slots"), ArmsmasterMesh->GetStaticMaterials().Num(), 6);

	TSet<FString> MaterialPaths;
	for (const FStaticMaterial& StaticMaterial : ArmsmasterMesh->GetStaticMaterials())
	{
		if (StaticMaterial.MaterialInterface)
		{
			MaterialPaths.Add(StaticMaterial.MaterialInterface->GetPathName());
		}
	}
	TestTrue(
		TEXT("Armsmaster keeps its project-owned skin material"),
		MaterialPaths.Contains(TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster/M_FenwatchArmsmasterSkin.M_FenwatchArmsmasterSkin")));
	TestNotNull(TEXT("Armsmaster mesh retains a body setup"), ArmsmasterMesh->GetBodySetup());
	if (ArmsmasterMesh->GetBodySetup())
	{
		TestEqual(
			TEXT("Armsmaster mesh carries no authored collision shapes"),
			ArmsmasterMesh->GetBodySetup()->AggGeom.GetElementCount(),
			0);
	}

	Presentation->StaticVisualMesh = ArmsmasterMesh;
	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Armsmaster resolves through the static presentation lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(
		TEXT("Armsmaster presentation resolves the project-owned mesh"),
		Presentation->StaticVisual->GetStaticMesh() == ArmsmasterMesh);
	TestTrue(TEXT("Armsmaster visual lanes remain non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Armsmaster presentation owns no interaction authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Armsmaster presentation owns no trainer authority"),
		Presentation->FindComponentByClass<UEmbermereTrainerComponent>());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereFenwatchArmsmasterIdlePresentationTest,
	"Embermere.NPC.FenwatchArmsmasterIdlePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereFenwatchArmsmasterIdlePresentationTest::RunTest(const FString& Parameters)
{
	const FString ArtRoot = TEXT("/Game/Art/Embermere/Characters/NPCs/FenwatchArmsmaster");
	UStaticMesh* StaticFallback = LoadObject<UStaticMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SM_EmbermereFenwatchArmsmaster_01.SM_EmbermereFenwatchArmsmaster_01")));
	USkeletalMesh* SkeletalMesh = LoadObject<USkeletalMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SK_EmbermereFenwatchArmsmaster_01.SK_EmbermereFenwatchArmsmaster_01")));
	UAnimSequence* IdleAnimation = LoadObject<UAnimSequence>(
		nullptr,
		*(ArtRoot + TEXT("/Animations/A_EmbermereFenwatchArmsmaster_Idle.A_EmbermereFenwatchArmsmaster_Idle")));
	AEmbermereNpcPresentationActor* Presentation = NewObject<AEmbermereNpcPresentationActor>();
	TestNotNull(TEXT("Accepted armsmaster static fallback loads"), StaticFallback);
	TestNotNull(TEXT("Rigged armsmaster skeletal mesh loads"), SkeletalMesh);
	TestNotNull(TEXT("Rigged armsmaster Idle loads"), IdleAnimation);
	TestNotNull(TEXT("Rigged armsmaster presentation can be created"), Presentation);
	if (!StaticFallback || !SkeletalMesh || !IdleAnimation || !Presentation)
	{
		return false;
	}

	const FVector Size = SkeletalMesh->GetBounds().BoxExtent * 2.0f;
	TestTrue(TEXT("Rigged armsmaster width retains the accepted silhouette"), FMath::IsNearlyEqual(Size.X, 154.5f, 1.0f));
	TestTrue(TEXT("Rigged armsmaster depth retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Y, 87.0f, 1.0f));
	TestTrue(TEXT("Rigged armsmaster height retains the accepted silhouette"), FMath::IsNearlyEqual(Size.Z, 228.0f, 1.0f));
	TestEqual(TEXT("Rigged armsmaster keeps six project materials"), SkeletalMesh->GetMaterials().Num(), 6);
	const FReferenceSkeleton& ReferenceSkeleton = SkeletalMesh->GetRefSkeleton();
	TestEqual(
		TEXT("Classic FBX keeps one Armature root plus the nine reviewed authored bones"),
		ReferenceSkeleton.GetRawBoneNum(),
		10);
	const TArray<FName> AuthoredBoneNames = {
		TEXT("root"),
		TEXT("pelvis"),
		TEXT("spine"),
		TEXT("neck"),
		TEXT("head"),
		TEXT("upper_arm_l"),
		TEXT("forearm_l"),
		TEXT("upper_arm_r"),
		TEXT("forearm_r"),
	};
	for (const FName BoneName : AuthoredBoneNames)
	{
		TestTrue(
			*FString::Printf(TEXT("Rigged armsmaster retains authored bone %s"), *BoneName.ToString()),
			ReferenceSkeleton.FindBoneIndex(BoneName) != INDEX_NONE);
	}
	const int32 AuthoredRootIndex = ReferenceSkeleton.FindBoneIndex(TEXT("root"));
	TestTrue(
		TEXT("Authored root remains beneath the classic FBX Armature root"),
		AuthoredRootIndex > 0 && ReferenceSkeleton.GetParentIndex(AuthoredRootIndex) == 0);
	TestTrue(
		TEXT("Idle animation uses the rigged armsmaster Skeleton"),
		IdleAnimation->GetSkeleton() == SkeletalMesh->GetSkeleton());
	TestTrue(
		TEXT("Idle animation keeps its reviewed 3.2-second cycle"),
		FMath::IsNearlyEqual(IdleAnimation->GetPlayLength(), 3.2f, 0.12f));

	Presentation->StaticVisualMesh = StaticFallback;
	Presentation->SkeletalVisualMesh = SkeletalMesh;
	Presentation->IdleAnimation = IdleAnimation;
	Presentation->bLoopIdleAnimation = true;
	Presentation->IdleAnimationPlayRate = 1.0f;
	Presentation->bPreferSkeletalVisual = true;
	Presentation->RefreshPresentation();

	TestEqual(
		TEXT("Saved-style armsmaster configuration resolves the skeletal lane"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::SkeletalMesh);
	TestEqual(
		TEXT("Saved-style armsmaster configuration resolves looping single-node Idle"),
		Presentation->GetResolvedAnimationMode(),
		EEmbermereNpcAnimationMode::SingleNodeIdle);
	TestTrue(
		TEXT("Production Idle is the exact serialized animation"),
		Presentation->SkeletalVisual->AnimationData.AnimToPlay == IdleAnimation);
	TestTrue(
		TEXT("Production Idle remains marked playing and looping"),
		Presentation->SkeletalVisual->AnimationData.bSavedPlaying &&
			Presentation->SkeletalVisual->AnimationData.bSavedLooping);
	TestTrue(TEXT("Rigged armsmaster remains non-colliding"), Presentation->IsPresentationCollisionDisabled());
	TestNull(
		TEXT("Rigged armsmaster presentation owns no interaction authority"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(
		TEXT("Rigged armsmaster presentation owns no trainer authority"),
		Presentation->FindComponentByClass<UEmbermereTrainerComponent>());

	Presentation->bPreferSkeletalVisual = false;
	Presentation->RefreshPresentation();
	TestEqual(
		TEXT("Disabling skeletal preference restores the accepted static fallback"),
		Presentation->GetResolvedVisualMode(),
		EEmbermereNpcVisualMode::StaticMesh);
	TestTrue(
		TEXT("Static fallback remains the exact accepted mesh"),
		Presentation->StaticVisual->GetStaticMesh() == StaticFallback);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereMarshProwlerPresentationTest,
	"Embermere.Enemy.MarshProwlerPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereMarshProwlerPresentationTest::RunTest(const FString& Parameters)
{
	const FString ArtRoot = TEXT("/Game/Art/Embermere/Characters/Enemies/MarshProwler");
	USkeletalMesh* ProwlerMesh = LoadObject<USkeletalMesh>(
		nullptr,
		*(ArtRoot + TEXT("/SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01")));
	UBlueprint* EnemyBlueprint = LoadObject<UBlueprint>(
		nullptr,
		TEXT("/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy"));
	TestNotNull(TEXT("Marsh Prowler skeletal mesh loads"), ProwlerMesh);
	TestNotNull(TEXT("Starter enemy Blueprint loads"), EnemyBlueprint);
	if (!ProwlerMesh || !EnemyBlueprint || !EnemyBlueprint->GeneratedClass)
	{
		return false;
	}

	const AEmbermereEnemyCharacter* EnemyDefaults =
		Cast<AEmbermereEnemyCharacter>(EnemyBlueprint->GeneratedClass->GetDefaultObject());
	TestNotNull(TEXT("Starter enemy defaults use the native enemy class"), EnemyDefaults);
	if (!EnemyDefaults || !EnemyDefaults->GetMesh())
	{
		return false;
	}

	TestTrue(
		TEXT("Starter enemy uses the Marsh Prowler mesh"),
		EnemyDefaults->GetMesh()->GetSkeletalMeshAsset() == ProwlerMesh);
	TestEqual(
		TEXT("Starter enemy keeps an asset-agnostic soft mesh reference"),
		EnemyDefaults->VisualSkeletalMesh.ToSoftObjectPath().ToString(),
		ArtRoot + TEXT("/SK_EmbermereMarshProwler_01.SK_EmbermereMarshProwler_01"));
	TestTrue(
		TEXT("Marsh Prowler mesh uses the reviewed 0.65 world scale"),
		EnemyDefaults->GetMesh()->GetRelativeScale3D().Equals(FVector(0.65f), KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Marsh Prowler configured visual scale matches the component"),
		EnemyDefaults->VisualMeshRelativeScale.Equals(FVector(0.65f), KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Marsh Prowler target circle expands beyond the generic minimum radius"),
		EnemyDefaults->GetResolvedTargetRingRadius() > EnemyDefaults->TargetRingRadius);
	TestTrue(
		TEXT("Marsh Prowler target circle remains bounded for starter combat readability"),
		EnemyDefaults->GetResolvedTargetRingRadius() < 220.0f);
	TestTrue(
		TEXT("Starter enemy Blueprint resolves enough surface clearance for the prototype terrain"),
		EnemyDefaults->GetEffectiveTargetRingSurfaceClearance() >= 16.0f);

	AEmbermereEnemyCharacter* RuntimeBlueprintEnemy = NewObject<AEmbermereEnemyCharacter>(
		GetTransientPackage(),
		EnemyBlueprint->GeneratedClass);
	TestNotNull(TEXT("Starter enemy Blueprint can create a runtime presentation instance"), RuntimeBlueprintEnemy);
	if (RuntimeBlueprintEnemy)
	{
		RuntimeBlueprintEnemy->EnsureTargetRingPresentationComponents();
		const FVector CombatFeedbackAnchor =
			EmbermereTargetableDispatch::GetCombatFeedbackAnchorLocation(RuntimeBlueprintEnemy);
		TestEqual(
			TEXT("Runtime starter enemy reconciles stale Blueprint templates to 48 target-ring segments"),
			RuntimeBlueprintEnemy->GetTargetRingSegmentCount(),
			48);
		TestTrue(
			TEXT("Reconciled runtime target ring remains presentation-only collision"),
			RuntimeBlueprintEnemy->AreTargetRingSegmentsNonColliding());
		TestFalse(
			TEXT("Runtime starter enemy resolves a finite combat-feedback anchor"),
			CombatFeedbackAnchor.ContainsNaN());
		TestTrue(
			TEXT("Runtime starter enemy combat-feedback anchor stays above the actor origin"),
			CombatFeedbackAnchor.Z >= RuntimeBlueprintEnemy->GetActorLocation().Z + 10.0f);
		TestTrue(
			TEXT("Runtime starter enemy combat-feedback anchor remains spatially bounded"),
			FVector::DistSquared2D(CombatFeedbackAnchor, RuntimeBlueprintEnemy->GetActorLocation()) <=
				FMath::Square(5000.0f));
	}
	TestTrue(
		TEXT("Marsh Prowler feet align with the gameplay capsule"),
		EnemyDefaults->GetMesh()->GetRelativeLocation().Equals(FVector(0.0f, 0.0f, -95.0f), KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Marsh Prowler configured visual offset matches the component"),
		EnemyDefaults->VisualMeshRelativeLocation.Equals(FVector(0.0f, 0.0f, -95.0f), KINDA_SMALL_NUMBER));
	TestTrue(
		TEXT("Starter enemy resolves all six visual animation roles"),
		EnemyDefaults->HasCompleteVisualAnimationSet());

	const TArray<TPair<FString, TSoftObjectPtr<UAnimSequence>>> AnimationRoles = {
		{TEXT("Idle"), EnemyDefaults->IdleAnimation},
		{TEXT("Walk"), EnemyDefaults->WalkAnimation},
		{TEXT("Run"), EnemyDefaults->RunAnimation},
		{TEXT("Attack"), EnemyDefaults->AttackAnimation},
		{TEXT("Hit"), EnemyDefaults->HitAnimation},
		{TEXT("Death"), EnemyDefaults->DeathAnimation}
	};
	for (const TPair<FString, TSoftObjectPtr<UAnimSequence>>& AnimationRole : AnimationRoles)
	{
		UAnimSequence* Sequence = AnimationRole.Value.LoadSynchronous();
		TestNotNull(*FString::Printf(TEXT("%s animation loads"), *AnimationRole.Key), Sequence);
		if (Sequence)
		{
			TestTrue(
				*FString::Printf(TEXT("%s animation has authored time"), *AnimationRole.Key),
				Sequence->GetPlayLength() > 0.0f);
			TestTrue(
				*FString::Printf(TEXT("%s animation shares the Prowler skeleton"), *AnimationRole.Key),
				Sequence->GetSkeleton() == ProwlerMesh->GetSkeleton());
		}
	}

	return true;
}

namespace
{
UEmbermereRestServiceData* MakeValidRestServiceData()
{
	UEmbermereRestServiceData* Data = NewObject<UEmbermereRestServiceData>();
	if (!Data)
	{
		return nullptr;
	}
	Data->ServiceId = TEXT("FenwatchCommunalWellRest");
	Data->DisplayName = FText::FromString(TEXT("Fenwatch Communal Well"));
	Data->PromptText = FText::FromString(
		TEXT("Cool emberlit water gathers beneath the old stone."));
	Data->RestingText = FText::FromString(
		TEXT("Resting at Fenwatch Communal Well. Remain still."));
	Data->InteractionRadius = 300.0f;
	Data->ChannelDurationSeconds = 1.5f;
	Data->CooldownSeconds = 12.0f;
	Data->MovementInterruptDistance = 35.0f;
	Data->bRestoreHealth = true;
	Data->bRestoreMana = true;
	return Data;
}

void ConfigureRestFixture(
	AEmbermereRestServiceActor* Service,
	AEmbermereCharacter* Character,
	UEmbermereRestServiceData* Data)
{
	Service->SetActorLocation(FVector::ZeroVector);
	Character->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
	Service->RestService->SetRestData(Data);
	Character->Stats->InitializeVitals();
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRestServiceContractTest,
	"Embermere.Rest.ServiceContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRestServiceContractTest::RunTest(const FString& Parameters)
{
	AEmbermereRestServiceActor* Service = NewObject<AEmbermereRestServiceActor>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	const UEmbermereRestServiceData* SavedData = LoadObject<UEmbermereRestServiceData>(
		nullptr,
		TEXT("/Game/Data/Services/DA_FenwatchCommunalWellRest.DA_FenwatchCommunalWellRest"));
	TestNotNull(TEXT("Rest service actor can be created"), Service);
	TestNotNull(TEXT("Rest service character fixture can be created"), Character);
	TestNotNull(TEXT("Saved Fenwatch rest definition loads"), SavedData);
	if (!Service || !Character || !Character->Stats || !SavedData)
	{
		return false;
	}

	TestNotNull(TEXT("Rest service owns standard interaction"), Service->Interactable.Get());
	TestNotNull(TEXT("Rest service owns recovery policy"), Service->RestService.Get());
	TestNull(TEXT("Rest service owns no static art"), Service->FindComponentByClass<UStaticMeshComponent>());
	TestNull(TEXT("Rest service owns no skeletal art"), Service->FindComponentByClass<USkeletalMeshComponent>());
	TestNull(TEXT("Rest service owns no vendor authority"), Service->FindComponentByClass<UEmbermereVendorComponent>());
	TestNull(TEXT("Rest service owns no trainer authority"), Service->FindComponentByClass<UEmbermereTrainerComponent>());
	TestEqual(
		TEXT("Rest interaction uses the reviewed well name"),
		Service->Interactable->DisplayName.ToString(),
		FString(TEXT("Fenwatch Communal Well")));
	TestTrue(TEXT("Rest interaction supplies a world marker"), Service->Interactable->bShowWorldMarker);
	TestEqual(TEXT("Rest marker clears the accepted roof"), Service->Interactable->MarkerHeight, 355.0f);

	TestTrue(TEXT("Saved rest definition passes native validation"), SavedData->HasValidDefinition());
	TestEqual(TEXT("Saved rest identity is stable"), SavedData->ServiceId, FName(TEXT("FenwatchCommunalWellRest")));
	TestEqual(TEXT("Saved rest radius is bounded"), SavedData->InteractionRadius, 300.0f);
	TestEqual(TEXT("Saved rest channel stays deliberate"), SavedData->ChannelDurationSeconds, 1.5f);
	TestEqual(TEXT("Saved rest cooldown stays session-only and bounded"), SavedData->CooldownSeconds, 12.0f);
	TestEqual(TEXT("Saved rest movement threshold stays fixed"), SavedData->MovementInterruptDistance, 35.0f);
	TestTrue(TEXT("Saved rest restores health"), SavedData->bRestoreHealth);
	TestTrue(TEXT("Saved rest restores mana"), SavedData->bRestoreMana);

	ConfigureRestFixture(Service, Character, const_cast<UEmbermereRestServiceData*>(SavedData));
	TestEqual(
		TEXT("Full resources reject without beginning a channel"),
		Service->RestService->CanBeginRest(Character),
		EEmbermereRestResult::ResourcesFull);
	Character->Stats->ApplyDamage(20.0f);
	Character->SetActorLocation(FVector(301.0f, 0.0f, 0.0f));
	TestEqual(
		TEXT("Data-owned range rejects a reachable global interaction edge"),
		Service->RestService->CanBeginRest(Character),
		EEmbermereRestResult::OutOfRange);
	Character->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
	TestEqual(
		TEXT("Missing resources inside range are eligible"),
		Service->RestService->CanBeginRest(Character),
		EEmbermereRestResult::Started);

	UEmbermereRestServiceData* MalformedData = MakeValidRestServiceData();
	MalformedData->ServiceId = NAME_None;
	Service->RestService->SetRestData(MalformedData);
	TestFalse(TEXT("Malformed rest definition fails validation"), MalformedData->HasValidDefinition());
	TestEqual(
		TEXT("Malformed rest definition rejects before mutation"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::InvalidRequest);
	TestEqual(TEXT("Malformed request preserves missing health"), Character->Stats->CurrentHealth, 80.0f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRestRecoveryTransactionsTest,
	"Embermere.Rest.RecoveryTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRestRecoveryTransactionsTest::RunTest(const FString& Parameters)
{
	AEmbermereRestServiceActor* Service = NewObject<AEmbermereRestServiceActor>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereRestServiceData* Data = MakeValidRestServiceData();
	if (!Service || !Character || !Character->Stats || !Data)
	{
		AddError(TEXT("Could not create rest recovery fixtures"));
		return false;
	}
	ConfigureRestFixture(Service, Character, Data);
	Character->Stats->ApplyDamage(30.0f);
	TestTrue(TEXT("Recovery fixture spends mana"), Character->Stats->SpendMana(20.0f));

	TestEqual(
		TEXT("Eligible request begins one channel"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::Started);
	TestTrue(TEXT("Rest channel reports pending"), Service->RestService->IsRestPending());
	TestEqual(
		TEXT("Started outcome uses exact authored copy"),
		Service->RestService->GetRestResultText(Service->RestService->GetLastOutcome()).ToString(),
		FString(TEXT("Resting at Fenwatch Communal Well. Remain still.")));
	TestEqual(
		TEXT("Duplicate request is rejected without restarting the channel"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::AlreadyResting);
	Service->RestService->AdvanceRest(1.49f);
	TestEqual(TEXT("Health does not mutate before channel completion"), Character->Stats->CurrentHealth, 70.0f);
	TestEqual(TEXT("Mana does not mutate before channel completion"), Character->Stats->CurrentMana, 30.0f);
	Service->RestService->AdvanceRest(0.01f);

	const FEmbermereRestOutcome Success = Service->RestService->GetLastOutcome();
	TestEqual(TEXT("Completion publishes success"), Success.Result, EEmbermereRestResult::Success);
	TestEqual(TEXT("Completion reports exact health restored"), Success.HealthRestored, 30.0f);
	TestEqual(TEXT("Completion reports exact mana restored"), Success.ManaRestored, 20.0f);
	TestEqual(TEXT("Completion restores health once"), Character->Stats->CurrentHealth, 100.0f);
	TestEqual(TEXT("Completion restores mana once"), Character->Stats->CurrentMana, 50.0f);
	TestFalse(TEXT("Completion clears pending state"), Service->RestService->IsRestPending());
	TestEqual(TEXT("Successful rest starts exact cooldown"), Service->RestService->GetCooldownRemainingSeconds(), 12.0f);
	TestEqual(
		TEXT("Cooldown rejects a duplicate recovery transaction"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::Cooldown);
	TestEqual(TEXT("Cooldown rejection preserves health"), Character->Stats->CurrentHealth, 100.0f);
	TestEqual(TEXT("Cooldown rejection preserves mana"), Character->Stats->CurrentMana, 50.0f);

	Service->RestService->AdvanceRest(12.0f);
	Character->Stats->ApplyDamage(10.0f);
	TestEqual(TEXT("A later missing-health-only request begins"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Service->RestService->AdvanceRest(1.5f);
	const FEmbermereRestOutcome HealthOnlySuccess = Service->RestService->GetLastOutcome();
	TestEqual(TEXT("Health-only recovery reports exact amount"), HealthOnlySuccess.HealthRestored, 10.0f);
	TestEqual(TEXT("Health-only recovery reports no fake mana"), HealthOnlySuccess.ManaRestored, 0.0f);

	Service->RestService->AdvanceRest(12.0f);
	Character->Stats->CurrentHealth = 50.0f;
	Character->Stats->CurrentMana = std::numeric_limits<float>::quiet_NaN();
	TestEqual(
		TEXT("Malformed vital state rejects before any owner mutates"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::InvalidRequest);
	TestEqual(TEXT("Malformed vital rollback preserves health"), Character->Stats->CurrentHealth, 50.0f);
	TestTrue(TEXT("Malformed vital rollback preserves the invalid mana sentinel"), FMath::IsNaN(Character->Stats->CurrentMana));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRestInterruptionAndCombatTest,
	"Embermere.Rest.InterruptionAndCombat",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRestInterruptionAndCombatTest::RunTest(const FString& Parameters)
{
	AEmbermereRestServiceActor* Service = NewObject<AEmbermereRestServiceActor>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermerePracticeTargetActor* PracticeTarget = NewObject<AEmbermerePracticeTargetActor>();
	AEmbermereEnemyCharacter* Prowler = NewObject<AEmbermereEnemyCharacter>();
	UEmbermereRestServiceData* Data = MakeValidRestServiceData();
	if (!Service || !Character || !Character->Stats || !Character->Combat ||
		!PracticeTarget || !Prowler || !Prowler->Stats || !Data)
	{
		AddError(TEXT("Could not create rest interruption fixtures"));
		return false;
	}
	ConfigureRestFixture(Service, Character, Data);
	Character->Stats->ApplyDamage(25.0f);
	TestEqual(TEXT("Movement-interruption fixture begins"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Character->SetActorLocation(FVector(136.0f, 0.0f, 0.0f));
	Service->RestService->AdvanceRest(0.1f);
	TestEqual(
		TEXT("Moving beyond the data-owned threshold interrupts"),
		Service->RestService->GetLastOutcome().Result,
		EEmbermereRestResult::Interrupted);
	TestEqual(TEXT("Interrupted rest preserves health"), Character->Stats->CurrentHealth, 75.0f);
	TestEqual(TEXT("Interrupted rest starts no cooldown"), Service->RestService->GetCooldownRemainingSeconds(), 0.0f);

	Character->Combat->SetTarget(PracticeTarget);
	TestFalse(TEXT("The stationary practice target is not active combat"), PracticeTarget->bPrototypeAiEnabled);
	TestEqual(
		TEXT("Practice-target selection does not block village recovery"),
		Service->RestService->CanBeginRest(Character),
		EEmbermereRestResult::Started);
	TestEqual(TEXT("Practice-target selection can begin rest"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Service->RestService->AdvanceRest(1.5f);
	TestEqual(TEXT("Practice-target selection still permits exact recovery"), Character->Stats->CurrentHealth, 100.0f);

	Service->RestService->AdvanceRest(12.0f);
	Character->Stats->ApplyDamage(10.0f);
	Character->Combat->SetTarget(nullptr);
	TestEqual(TEXT("A safe request begins before combat changes"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Prowler->Stats->InitializeVitals();
	Prowler->SetActorLocation(Character->GetActorLocation() + FVector(100.0f, 0.0f, 0.0f));
	Character->Combat->SetTarget(Prowler);
	Service->RestService->AdvanceRest(0.1f);
	TestEqual(
		TEXT("Enemy engagement during the channel interrupts before recovery"),
		Service->RestService->GetLastOutcome().Result,
		EEmbermereRestResult::InCombat);
	TestEqual(TEXT("Combat interruption preserves health"), Character->Stats->CurrentHealth, 90.0f);
	TestEqual(TEXT("Combat interruption starts no cooldown"), Service->RestService->GetCooldownRemainingSeconds(), 0.0f);
	TestEqual(
		TEXT("A live nearby prototype enemy blocks rest"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::InCombat);
	TestEqual(TEXT("Combat rejection preserves health"), Character->Stats->CurrentHealth, 90.0f);
	TestEqual(TEXT("Combat rejection starts no cooldown"), Service->RestService->GetCooldownRemainingSeconds(), 0.0f);

	Character->Combat->SetTarget(nullptr);
	Character->Stats->ForceDeath();
	TestEqual(
		TEXT("Dead characters reject before recovery"),
		Service->RestService->TryBeginRest(Character),
		EEmbermereRestResult::Dead);
	TestTrue(TEXT("Death rejection preserves the dead state"), Character->Stats->IsDead());

	Character->Stats->InitializeVitals();
	Character->Stats->ApplyDamage(10.0f);
	TestEqual(TEXT("Teardown fixture begins"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Service->RestService->ResetTransientState();
	TestFalse(TEXT("Component teardown clears pending recovery"), Service->RestService->IsRestPending());
	TestEqual(TEXT("Component teardown clears session cooldown"), Service->RestService->GetCooldownRemainingSeconds(), 0.0f);
	TestEqual(TEXT("Component teardown never commits partial recovery"), Character->Stats->CurrentHealth, 90.0f);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRestWorldPresentationTest,
	"Embermere.UI.RestWorldPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRestWorldPresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereRestServiceActor* Service = NewObject<AEmbermereRestServiceActor>();
	AEmbermereRestPresentationActor* Presentation =
		NewObject<AEmbermereRestPresentationActor>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereRestServiceData* Data = MakeValidRestServiceData();
	if (!Service || !Presentation || !Character || !Character->Stats || !Data)
	{
		AddError(TEXT("Could not create rest presentation fixtures"));
		return false;
	}

	ConfigureRestFixture(Service, Character, Data);
	Presentation->SetObservedRestService(Service);
	TestTrue(TEXT("Presentation observer binds to the separate rest service"),
		Presentation->IsBoundToRestService());
	TestTrue(TEXT("Native rest outcome stream has a live presentation observer"),
		Service->RestService->OnRestOutcomeNative.IsBound());
	TestFalse(TEXT("Presentation starts hidden"), Presentation->IsRestPresentationVisible());
	TestEqual(TEXT("Transient geometry is deferred until a live channel"),
		Presentation->GetRestPresentationSegmentCount(), 0);
	TestNull(TEXT("Presentation owns no interaction"),
		Presentation->FindComponentByClass<UEmbermereInteractableComponent>());
	TestNull(TEXT("Presentation owns no recovery service"),
		Presentation->FindComponentByClass<UEmbermereRestServiceComponent>());
	TestNull(TEXT("Presentation owns no vendor authority"),
		Presentation->FindComponentByClass<UEmbermereVendorComponent>());
	TestNull(TEXT("Presentation owns no trainer authority"),
		Presentation->FindComponentByClass<UEmbermereTrainerComponent>());

	Character->Stats->ApplyDamage(30.0f);
	TestTrue(TEXT("Presentation fixture spends mana"), Character->Stats->SpendMana(20.0f));
	TestEqual(TEXT("Authoritative service begins the observed channel"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	TestEqual(TEXT("Started outcome reaches the presentation observer exactly once"),
		Presentation->ObservedOutcomeCount, 1);
	TestEqual(TEXT("Started outcome selects the channel phase"),
		Presentation->GetPresentationPhase(), EEmbermereRestPresentationPhase::Channeling);
	TestEqual(TEXT("Channel owns twelve fixed visual segments"),
		Presentation->GetRestPresentationSegmentCount(), 12);
	TestEqual(TEXT("Every channel segment is visible"),
		Presentation->GetVisibleRestPresentationSegmentCount(), 12);
	TestTrue(TEXT("Channel geometry is presentation-only"),
		Presentation->AreRestPresentationSegmentsNonColliding());
	TestTrue(TEXT("Channel uses the project target-ring material"),
		Presentation->GetRestPresentationMaterialPath().Contains(TEXT("M_EmbermereTargetRing")));
	TestTrue(TEXT("Channel color is cyan"),
		Presentation->GetRestPresentationColor().B > Presentation->GetRestPresentationColor().R &&
			Presentation->GetRestPresentationColor().G > Presentation->GetRestPresentationColor().R);
	TestTrue(TEXT("Channel remains inside the open well shaft"),
		Presentation->GetRestPresentationRadius() <= 48.5f);

	TestEqual(TEXT("Duplicate request is rejected by authority"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::AlreadyResting);
	Presentation->AdvanceRestPresentation(0.25f);
	TestEqual(TEXT("Duplicate request neither restarts nor clears the channel"),
		Presentation->GetPresentationPhase(), EEmbermereRestPresentationPhase::Channeling);
	Character->SetActorLocation(FVector(136.0f, 0.0f, 0.0f));
	Service->RestService->AdvanceRest(0.1f);
	TestFalse(TEXT("Movement interruption clears the channel immediately"),
		Presentation->IsRestPresentationVisible());
	TestEqual(TEXT("Interruption hides every segment"),
		Presentation->GetVisibleRestPresentationSegmentCount(), 0);
	TestEqual(TEXT("Presentation never commits partial recovery"),
		Character->Stats->CurrentHealth, 70.0f);

	Character->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
	TestEqual(TEXT("A later valid request starts a fresh observed channel"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Service->RestService->AdvanceRest(1.5f);
	TestEqual(TEXT("Only committed success selects completion presentation"),
		Presentation->GetPresentationPhase(), EEmbermereRestPresentationPhase::Completion);
	TestTrue(TEXT("Completion color is mint rather than failure red"),
		Presentation->GetRestPresentationColor().G > Presentation->GetRestPresentationColor().B &&
			Presentation->GetRestPresentationColor().B > Presentation->GetRestPresentationColor().R);
	TestTrue(TEXT("Completion restores health through service authority"),
		FMath::IsNearlyEqual(Character->Stats->CurrentHealth, 100.0f));
	TestTrue(TEXT("Completion restores mana through service authority"),
		FMath::IsNearlyEqual(Character->Stats->CurrentMana, 50.0f));
	TestTrue(TEXT("Completion lifetime is deliberately fixed"),
		FMath::IsNearlyEqual(Presentation->GetCompletionLifetimeSeconds(), 1.1f));
	const float CompletionStartRadius = Presentation->GetRestPresentationRadius();
	Presentation->AdvanceRestPresentation(0.55f);
	TestTrue(TEXT("Completion expands after the committed outcome"),
		Presentation->GetRestPresentationRadius() > CompletionStartRadius);
	TestTrue(TEXT("Completion remains inside its fixed world bound"),
		Presentation->GetRestPresentationRadius() <= 120.0f);
	Presentation->AdvanceRestPresentation(0.54f);
	TestTrue(TEXT("Completion remains visible for its full lifetime"),
		Presentation->IsRestPresentationVisible());
	Presentation->AdvanceRestPresentation(0.02f);
	TestFalse(TEXT("Completion expires deterministically"),
		Presentation->IsRestPresentationVisible());

	Service->RestService->AdvanceRest(12.0f);
	Character->Stats->ApplyDamage(10.0f);
	TestEqual(TEXT("Death-clear fixture begins"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Character->Stats->ForceDeath();
	Presentation->AdvanceRestPresentation(0.0f);
	TestFalse(TEXT("Character death clears presentation immediately"),
		Presentation->IsRestPresentationVisible());
	Service->RestService->ResetTransientState();

	Character->Stats->InitializeVitals();
	Character->Stats->ApplyDamage(10.0f);
	Presentation->UnbindFromRestService();
	TestFalse(TEXT("Unbinding clears presentation and observer state"),
		Presentation->IsBoundToRestService());
	TestEqual(TEXT("Unbound service can still start its authoritative transaction"),
		Service->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	TestFalse(TEXT("Torn-down observer receives no later live outcome"),
		Presentation->IsRestPresentationVisible());
	Service->RestService->ResetTransientState();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereStillWatersServiceContractTest,
	"Embermere.Quests.StillWatersServiceContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStillWatersServiceContractTest::RunTest(const FString& Parameters)
{
	const UEmbermereQuestData* StillWaters = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters"));
	AEmbermereRestQuestServiceActor* QuestService =
		NewObject<AEmbermereRestQuestServiceActor>();
	TestNotNull(TEXT("Still Waters data asset resolves"), StillWaters);
	TestNotNull(TEXT("Art-free notice-board quest service can be created"), QuestService);
	if (!StillWaters || !QuestService || !QuestService->Interactable ||
		!QuestService->RestObjectiveRouter)
	{
		return false;
	}

	TestEqual(TEXT("Still Waters owns its stable quest ID"),
		StillWaters->QuestId, FName(TEXT("FenwatchStillWaters")));
	TestEqual(TEXT("Still Waters owns its stable objective ID"),
		StillWaters->ObjectiveId, FName(TEXT("FenwatchRestCompleted")));
	TestEqual(TEXT("Still Waters requires one committed rest"),
		StillWaters->RequiredObjectiveCount, 1);
	TestEqual(TEXT("Still Waters grants exact XP"), StillWaters->RewardExperience, 50);
	TestEqual(TEXT("Still Waters grants exact copper"), StillWaters->RewardCopper, 10);
	TestTrue(TEXT("Still Waters grants no item"), StillWaters->RewardItem.IsNull());
	TestEqual(TEXT("Still Waters title is exact"),
		StillWaters->Title.ToString(), FString(TEXT("Still Waters")));
	TestEqual(TEXT("Still Waters active copy is exact"),
		StillWaters->ActiveGreeting.ToString(),
		FString(TEXT("Still Waters: Complete one successful rest at the Fenwatch communal well.")));

	TestNotNull(TEXT("Quest service owns standard interaction"), QuestService->Interactable.Get());
	TestNotNull(TEXT("Quest service owns a dedicated rest router"),
		QuestService->RestObjectiveRouter.Get());
	TestNull(TEXT("Quest service owns no static art"),
		QuestService->FindComponentByClass<UStaticMeshComponent>());
	TestNull(TEXT("Quest service owns no skeletal art"),
		QuestService->FindComponentByClass<USkeletalMeshComponent>());
	TestNull(TEXT("Quest service owns no recovery authority"),
		QuestService->FindComponentByClass<UEmbermereRestServiceComponent>());
	TestNull(TEXT("Quest service owns no vendor authority"),
		QuestService->FindComponentByClass<UEmbermereVendorComponent>());
	TestNull(TEXT("Quest service owns no trainer authority"),
		QuestService->FindComponentByClass<UEmbermereTrainerComponent>());
	TestFalse(TEXT("Art-free quest service has no actor collision"),
		QuestService->GetActorEnableCollision());
	TestTrue(TEXT("Notice interaction enables quest-state dialogue"),
		QuestService->Interactable->bUseQuestStateDialogue);
	TestEqual(TEXT("Notice interaction keeps a fixed marker height"),
		QuestService->Interactable->MarkerHeight, 305.0f);
	TestEqual(TEXT("Router targets the exact quest"),
		QuestService->RestObjectiveRouter->QuestId,
		StillWaters->QuestId);
	TestEqual(TEXT("Router targets the exact objective"),
		QuestService->RestObjectiveRouter->ObjectiveId,
		StillWaters->ObjectiveId);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereStillWatersRestRoutingTest,
	"Embermere.Quests.StillWatersRestRouting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStillWatersRestRoutingTest::RunTest(const FString& Parameters)
{
	AEmbermereRestServiceActor* RestService = NewObject<AEmbermereRestServiceActor>();
	AEmbermereRestQuestServiceActor* QuestService =
		NewObject<AEmbermereRestQuestServiceActor>();
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereRestServiceData* RestData = MakeValidRestServiceData();
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* StillWaters = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters"));
	if (!RestService || !QuestService || !Character || !Character->Stats ||
		!Character->QuestLog || !Character->Wallet || !RestData ||
		!MaraQuest || !StillWaters)
	{
		AddError(TEXT("Could not create Still Waters routing fixtures"));
		return false;
	}

	QuestService->Interactable->QuestToOffer = StillWaters;
	QuestService->RestObjectiveRouter->SetObservedRestService(RestService);
	ConfigureRestFixture(RestService, Character, RestData);
	TestTrue(TEXT("Dedicated router binds to the exact rest service"),
		QuestService->RestObjectiveRouter->IsBoundToObservedRestService());
	TestTrue(TEXT("Rest outcome stream has a quest observer"),
		RestService->RestService->OnRestOutcomeNative.IsBound());
	TestEqual(TEXT("Untracked notice resolves available copy"),
		QuestService->Interactable->GetInteractionDialogueText(Character).ToString(),
		StillWaters->AvailableGreeting.ToString());

	TestTrue(TEXT("Mara and Still Waters can coexist"),
		Character->QuestLog->AcceptQuest(MaraQuest));
	QuestService->Interactable->Interact(Character);
	FEmbermereQuestState StillWatersState;
	FEmbermereQuestState MaraState;
	TestTrue(TEXT("Notice owner accepts Still Waters by stable ID"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState));
	TestEqual(TEXT("Both quests occupy independent ledger records"),
		Character->QuestLog->QuestStates.Num(), 2);
	TestEqual(TEXT("Tracked notice resolves active copy"),
		QuestService->Interactable->GetInteractionDialogueText(Character).ToString(),
		StillWaters->ActiveGreeting.ToString());

	TestEqual(TEXT("Full-resource rejection remains rest authority"),
		RestService->RestService->TryBeginRest(Character),
		EEmbermereRestResult::ResourcesFull);
	TestTrue(TEXT("Rejected rest leaves Still Waters at zero"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.CurrentObjectiveCount == 0);

	Character->Stats->ApplyDamage(30.0f);
	TestTrue(TEXT("Routing fixture spends mana"), Character->Stats->SpendMana(20.0f));
	TestEqual(TEXT("Eligible rest starts"),
		RestService->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	Character->SetActorLocation(FVector(136.0f, 0.0f, 0.0f));
	RestService->RestService->AdvanceRest(0.1f);
	TestEqual(TEXT("Movement produces an authoritative interruption"),
		RestService->RestService->GetLastOutcome().Result,
		EEmbermereRestResult::Interrupted);
	TestTrue(TEXT("Interrupted rest cannot advance Still Waters"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.CurrentObjectiveCount == 0);

	Character->SetActorLocation(FVector(100.0f, 0.0f, 0.0f));
	TestEqual(TEXT("Second eligible rest starts"),
		RestService->RestService->TryBeginRest(Character), EEmbermereRestResult::Started);
	RestService->RestService->AdvanceRest(1.5f);
	TestEqual(TEXT("Committed service outcome is Success"),
		RestService->RestService->GetLastOutcome().Result,
		EEmbermereRestResult::Success);
	TestTrue(TEXT("One committed success advances the exact objective once"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.CurrentObjectiveCount == 1 && !StillWatersState.bCompleted);
	TestTrue(TEXT("Committed rest focuses only Still Waters"),
		Character->QuestLog->ActiveQuest.Quest == StillWaters);
	TestTrue(TEXT("Mara remains independently unprogressed"),
		Character->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState) &&
		MaraState.CurrentObjectiveCount == 0 && !MaraState.bCompleted);
	TestEqual(TEXT("Ready notice resolves ready copy"),
		QuestService->Interactable->GetInteractionDialogueText(Character).ToString(),
		StillWaters->ReadyGreeting.ToString());

	const FEmbermereRestOutcome DuplicateSuccess =
		RestService->RestService->GetLastOutcome();
	RestService->RestService->OnRestOutcomeNative.Broadcast(DuplicateSuccess);
	TestTrue(TEXT("Duplicate committed outcome cannot grow a capped objective"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.CurrentObjectiveCount == 1);

	Character->Wallet->SetCopperForPrototype(MAX_int32 - 5);
	QuestService->Interactable->Interact(Character);
	TestTrue(TEXT("Reward overflow leaves Still Waters ready and incomplete"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		!StillWatersState.bCompleted);
	TestEqual(TEXT("Rejected turn-in preserves XP"),
		Character->Stats->CurrentExperience, 0);

	Character->Wallet->SetCopperForPrototype(40);
	QuestService->Interactable->Interact(Character);
	TestTrue(TEXT("Correct notice owner completes Still Waters"),
		Character->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.bCompleted);
	TestEqual(TEXT("Still Waters grants 50 XP exactly once"),
		Character->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Still Waters grants 10 copper exactly once"),
		Character->Wallet->Copper, 50);
	TestEqual(TEXT("Completed notice resolves completed copy"),
		QuestService->Interactable->GetInteractionDialogueText(Character).ToString(),
		StillWaters->CompletedGreeting.ToString());
	QuestService->Interactable->Interact(Character);
	TestEqual(TEXT("Repeated notice interaction cannot replay XP"),
		Character->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Repeated notice interaction cannot replay copper"),
		Character->Wallet->Copper, 50);

	QuestService->RestObjectiveRouter->SetObservedRestService(nullptr);
	TestFalse(TEXT("Router teardown removes its rest subscription"),
		QuestService->RestObjectiveRouter->IsBoundToObservedRestService());
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereStillWatersPersistenceRoundTripTest,
	"Embermere.Persistence.StillWatersRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStillWatersPersistenceRoundTripTest::RunTest(const FString& Parameters)
{
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* StillWaters = LoadObject<UEmbermereQuestData>(
		nullptr,
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters"));
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	if (!MaraQuest || !StillWaters || !Source || !Target)
	{
		AddError(TEXT("Could not create Still Waters persistence fixtures"));
		return false;
	}

	TestTrue(TEXT("Still Waters source confirms identity"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	TestTrue(TEXT("Source accepts Mara"), Source->QuestLog->AcceptQuest(MaraQuest));
	TestTrue(TEXT("Source accepts saved Still Waters asset"),
		Source->QuestLog->AcceptQuest(StillWaters));
	TestTrue(TEXT("Source advances Mara independently"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			MaraQuest->QuestId, MaraQuest->ObjectiveId, 2));
	TestTrue(TEXT("Source completes Still Waters objective"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			StillWaters->QuestId, StillWaters->ObjectiveId, 1));
	TestTrue(TEXT("Source commits Still Waters rewards once"),
		Source->QuestLog->TryCompleteQuest(StillWaters));

	FText PersistenceMessage;
	UEmbermereSaveGame* CapturedSave = nullptr;
	TestEqual(TEXT("Still Waters captures through save version 3"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, CapturedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Still Waters capture creates a save"), CapturedSave);
	if (!CapturedSave)
	{
		return false;
	}
	TestEqual(TEXT("Still Waters capture keeps version 3"),
		CapturedSave->FormatVersion, EmbermereSaveGameVersion::MultiQuestLedger);
	TestEqual(TEXT("Still Waters capture writes both quests"),
		CapturedSave->QuestStates.Num(), 2);

	TArray<uint8> SerializedBytes;
	TestTrue(TEXT("Still Waters save serializes through memory"),
		UGameplayStatics::SaveGameToMemory(CapturedSave, SerializedBytes));
	UEmbermereSaveGame* LoadedSave = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromMemory(SerializedBytes));
	TestNotNull(TEXT("Still Waters save reloads from memory"), LoadedSave);
	if (!LoadedSave)
	{
		return false;
	}

	TestTrue(TEXT("Fresh target begins with a different legal identity"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
	TestEqual(TEXT("First Still Waters load succeeds"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	FEmbermereQuestState MaraState;
	FEmbermereQuestState StillWatersState;
	TestTrue(TEXT("Load restores independent Mara progress"),
		Target->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState) &&
		MaraState.CurrentObjectiveCount == 2 && !MaraState.bCompleted);
	TestTrue(TEXT("Load restores completed Still Waters history"),
		Target->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState) &&
		StillWatersState.CurrentObjectiveCount == 1 && StillWatersState.bCompleted);
	TestEqual(TEXT("Load restores exact Still Waters XP"),
		Target->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Load restores exact Still Waters copper"),
		Target->Wallet->Copper, 50);
	TestFalse(TEXT("Loaded Still Waters cannot replay rewards"),
		Target->QuestLog->TryCompleteQuest(StillWaters));

	TestEqual(TEXT("Second Still Waters load remains idempotent"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, LoadedSave, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated load keeps exactly two quests"),
		Target->QuestLog->QuestStates.Num(), 2);
	TestEqual(TEXT("Repeated load does not duplicate XP"),
		Target->Stats->CurrentExperience, 50);
	TestEqual(TEXT("Repeated load does not duplicate copper"),
		Target->Wallet->Copper, 50);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereQuestLedgerPresentationTest,
	"Embermere.UI.QuestLedgerPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestLedgerPresentationTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereQuestLogComponent* QuestLog = NewObject<UEmbermereQuestLogComponent>();
	if (!Hud || !QuestLog)
	{
		AddError(TEXT("Could not create quest-ledger presentation fixtures"));
		return false;
	}

	Hud->QuestLog = QuestLog;
	Hud->TakeWidget();
	TestTrue(TEXT("Quest-ledger command is bottom-right anchored"),
		Hud->IsQuestLedgerButtonBottomRightAnchored());
	TestEqual(TEXT("Quest-ledger command keeps fixed dimensions"),
		Hud->GetQuestLedgerButtonDimensions(), FVector2D(140.0f, 38.0f));
	TestEqual(TEXT("Quest-ledger command sits above Chronicle with fixed offset"),
		Hud->GetQuestLedgerButtonViewportOffset(), FVector2D(-24.0f, -70.0f));
	TestEqual(TEXT("Quest ledger keeps fixed panel dimensions"),
		Hud->GetQuestLedgerPanelDimensions(), FVector2D(620.0f, 430.0f));
	TestEqual(TEXT("Every quest row keeps fixed dimensions"),
		Hud->GetQuestLedgerRowDimensions(), FVector2D(596.0f, 30.0f));
	TestFalse(TEXT("Quest ledger starts hidden"), Hud->IsQuestLedgerPanelVisible());
	TestTrue(TEXT("Quest ledger opens through its explicit command"),
		Hud->ToggleQuestLedgerPanel());
	TestTrue(TEXT("Opening the ledger hides Inventory"), !Hud->IsInventoryPanelVisible());
	TestEqual(TEXT("Empty ledger reports zero records"),
		Hud->GetQuestLedgerVisibleRecordCount(), 0);
	TestTrue(TEXT("Empty ledger copy is explicit"),
		Hud->GetQuestLedgerDisplayText().ToString().Contains(TEXT("No quests tracked.")));
	UTextBlock* EmptyText = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("QuestLedgerEmptyText")));
	TestTrue(TEXT("Empty ledger keeps its fixed empty-state presentation"),
		EmptyText && EmptyText->GetVisibility() == ESlateVisibility::HitTestInvisible);

	UEmbermereQuestData* ActiveQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationLedgerActive"),
		TEXT("AutomationLedgerActive"),
		TEXT("AutomationLedgerActiveObjective"),
		3);
	UEmbermereQuestData* ReadyQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationLedgerReady"),
		TEXT("AutomationLedgerReady"),
		TEXT("AutomationLedgerReadyObjective"),
		1);
	if (!ActiveQuest || !ReadyQuest)
	{
		AddError(TEXT("Could not create quest-ledger row fixtures"));
		return false;
	}
	ActiveQuest->Title = FText::FromString(TEXT("First Signs at the Ruin"));
	ReadyQuest->Title = FText::FromString(TEXT("Still Waters"));
	TestTrue(TEXT("One active quest enters the ledger"), QuestLog->AcceptQuest(ActiveQuest));
	TestEqual(TEXT("One-record layout reports one row"),
		Hud->GetQuestLedgerVisibleRecordCount(), 1);
	TestEqual(TEXT("Active row uses exact quest-owned title, state, and progress"),
		Hud->GetQuestLedgerRowDisplayText(0).ToString(),
		FString(TEXT("First Signs at the Ruin   |   ACTIVE   |   0/3")));

	TestTrue(TEXT("Second quest enters the ledger independently"), QuestLog->AcceptQuest(ReadyQuest));
	TestTrue(TEXT("Second quest reaches ready state through quest authority"),
		QuestLog->AddObjectiveProgressForQuest(
			ReadyQuest->QuestId, ReadyQuest->ObjectiveId, 1));
	TestEqual(TEXT("Ready row uses exact state copy"),
		Hud->GetQuestLedgerRowDisplayText(1).ToString(),
		FString(TEXT("Still Waters   |   READY   |   1/1")));
	TestTrue(TEXT("Quest authority commits the ready record"),
		QuestLog->TryCompleteQuestById(ReadyQuest->QuestId));
	TestEqual(TEXT("Completed history remains visible with exact copy"),
		Hud->GetQuestLedgerRowDisplayText(1).ToString(),
		FString(TEXT("Still Waters   |   COMPLETED   |   1/1")));
	TestEqual(TEXT("Two-record layout reports both rows"),
		Hud->GetQuestLedgerVisibleRecordCount(), 2);

	for (int32 QuestIndex = 2; QuestIndex < UEmbermereQuestLogComponent::MaxTrackedQuests; ++QuestIndex)
	{
		UEmbermereQuestData* Quest = CreateAutomationQuestAsset(
			*FString::Printf(TEXT("DQ_AutomationLedgerFull_%d"), QuestIndex),
			*FString::Printf(TEXT("AutomationLedgerFull%d"), QuestIndex),
			*FString::Printf(TEXT("AutomationLedgerObjective%d"), QuestIndex),
			QuestIndex + 1);
		TestTrue(
			*FString::Printf(TEXT("Full-ledger fixture accepts row %d"), QuestIndex + 1),
			QuestLog->AcceptQuest(Quest));
	}
	TestEqual(TEXT("Full layout exposes all eight bounded records"),
		Hud->GetQuestLedgerVisibleRecordCount(), UEmbermereQuestLogComponent::MaxTrackedQuests);
	TestTrue(TEXT("Selecting the last record refreshes the bounded row presentation"),
		Hud->SelectQuestLedgerRecord(UEmbermereQuestLogComponent::MaxTrackedQuests - 1));
	UWidget* EighthRow = Hud->GetWidgetFromName(TEXT("QuestLedgerRowButton_7"));
	TestTrue(TEXT("Eighth fixed row remains visible at capacity"),
		EighthRow && EighthRow->GetVisibility() == ESlateVisibility::Visible);
	TestTrue(TEXT("Inventory command cleanly takes ownership from the ledger"),
		Hud->ToggleInventoryPanel());
	TestFalse(TEXT("Inventory handoff closes the ledger"), Hud->IsQuestLedgerPanelVisible());
	TestTrue(TEXT("Ledger can reopen after Inventory handoff"), Hud->ToggleQuestLedgerPanel());
	TestTrue(TEXT("Chronicle opens from the ledger"), Hud->ToggleSaveLoadPanel());
	TestFalse(TEXT("Chronicle handoff closes the ledger"), Hud->IsQuestLedgerPanelVisible());
	TestFalse(TEXT("Chronicle closes explicitly"), Hud->ToggleSaveLoadPanel());
	TestTrue(TEXT("Ledger reopens after Chronicle handoff"), Hud->ToggleQuestLedgerPanel());
	Hud->CloseQuestLedgerPanel();
	TestFalse(TEXT("Explicit close tears down visible ledger state"),
		Hud->IsQuestLedgerPanelVisible());
	TestEqual(TEXT("Close resets only transient row selection"),
		Hud->GetSelectedQuestLedgerIndex(), 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereQuestLedgerFocusLifecycleTest,
	"Embermere.UI.QuestLedgerFocusLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestLedgerFocusLifecycleTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Source = NewObject<AEmbermereCharacter>();
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereQuestData* ActiveQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationLedgerFocusActive"),
		TEXT("AActiveLedgerQuest"),
		TEXT("AActiveLedgerObjective"),
		3);
	UEmbermereQuestData* CompletedQuest = CreateAutomationQuestAsset(
		TEXT("DQ_AutomationLedgerFocusCompleted"),
		TEXT("ZCompletedLedgerQuest"),
		TEXT("ZCompletedLedgerObjective"),
		1);
	if (!Source || !Target || !Hud || !ActiveQuest || !CompletedQuest)
	{
		AddError(TEXT("Could not create quest-ledger focus fixtures"));
		return false;
	}

	ActiveQuest->Title = FText::FromString(TEXT("First Signs at the Ruin"));
	CompletedQuest->Title = FText::FromString(TEXT("Still Waters"));
	TestTrue(TEXT("Focus fixture confirms a deliberate source identity"),
		Source->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestTrue(TEXT("Focus fixture confirms a distinct target identity"),
		Target->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard));
	TestTrue(TEXT("Focus fixture accepts active quest"), Source->QuestLog->AcceptQuest(ActiveQuest));
	TestTrue(TEXT("Focus fixture accepts completion-history quest"),
		Source->QuestLog->AcceptQuest(CompletedQuest));
	TestTrue(TEXT("Focus fixture advances active quest independently"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			ActiveQuest->QuestId, ActiveQuest->ObjectiveId, 1));
	TestTrue(TEXT("Focus fixture reaches completed objective"),
		Source->QuestLog->AddObjectiveProgressForQuest(
			CompletedQuest->QuestId, CompletedQuest->ObjectiveId, 1));
	TestTrue(TEXT("Focus fixture commits completed history once"),
		Source->QuestLog->TryCompleteQuestById(CompletedQuest->QuestId));

	Hud->BindToCharacter(Source);
	Hud->TakeWidget();
	TestTrue(TEXT("Ledger opens aligned to the current focused record"),
		Hud->ToggleQuestLedgerPanel());
	TestEqual(TEXT("Completed quest begins selected because it is focused"),
		Hud->GetSelectedQuestLedgerIndex(), 1);
	UEmbermereQuestLedgerRowButton* ActiveRow = Cast<UEmbermereQuestLedgerRowButton>(
		Hud->GetWidgetFromName(TEXT("QuestLedgerRowButton_0")));
	TestNotNull(TEXT("Mouse-facing active row exists"), ActiveRow);
	if (ActiveRow)
	{
		ActiveRow->OnQuestLedgerRowClicked.Broadcast(0);
	}
	TestEqual(TEXT("Mouse row selection changes only transient selection"),
		Hud->GetSelectedQuestLedgerIndex(), 0);
	TestEqual(TEXT("Row selection alone preserves focused quest"),
		Source->QuestLog->FocusedQuestId, CompletedQuest->QuestId);

	FEmbermereQuestState ActiveBefore;
	FEmbermereQuestState CompletedBefore;
	Source->QuestLog->GetQuestStateById(ActiveQuest->QuestId, ActiveBefore);
	Source->QuestLog->GetQuestStateById(CompletedQuest->QuestId, CompletedBefore);
	const int32 CopperBeforeFocus = Source->Wallet->Copper;
	const int32 ExperienceBeforeFocus = Source->Stats->CurrentExperience;
	const int32 InventoryStacksBeforeFocus = Source->Inventory->Stacks.Num();
	TestTrue(TEXT("Explicit focus projects the selected active quest"), Hud->FocusSelectedQuest());
	TestEqual(TEXT("Focus updates only transient stable ID"),
		Source->QuestLog->FocusedQuestId, ActiveQuest->QuestId);
	TestTrue(TEXT("Compatibility tracker projects the focused active quest"),
		Source->QuestLog->ActiveQuest.Quest == ActiveQuest);
	TestTrue(TEXT("Duplicate focus input remains a safe idempotent request"),
		Hud->FocusSelectedQuest());
	TestTrue(TEXT("Keyboard-style selection wraps backward"),
		Hud->SelectNextQuestLedgerRecord(-1));
	TestEqual(TEXT("Wrapped selection reaches completed history"),
		Hud->GetSelectedQuestLedgerIndex(), 1);
	TestTrue(TEXT("Keyboard-style focus projects completed history"),
		Hud->FocusSelectedQuest());
	TestTrue(TEXT("Compatibility tracker projects completed Still Waters"),
		Source->QuestLog->ActiveQuest.Quest == CompletedQuest &&
		Source->QuestLog->ActiveQuest.bCompleted);

	FEmbermereQuestState ActiveAfter;
	FEmbermereQuestState CompletedAfter;
	Source->QuestLog->GetQuestStateById(ActiveQuest->QuestId, ActiveAfter);
	Source->QuestLog->GetQuestStateById(CompletedQuest->QuestId, CompletedAfter);
	TestEqual(TEXT("Focus preserves active quest progress"),
		ActiveAfter.CurrentObjectiveCount, ActiveBefore.CurrentObjectiveCount);
	TestEqual(TEXT("Focus preserves active completion state"),
		ActiveAfter.bCompleted, ActiveBefore.bCompleted);
	TestEqual(TEXT("Focus preserves completed quest progress"),
		CompletedAfter.CurrentObjectiveCount, CompletedBefore.CurrentObjectiveCount);
	TestEqual(TEXT("Focus preserves completed history"),
		CompletedAfter.bCompleted, CompletedBefore.bCompleted);
	TestEqual(TEXT("Focus preserves wallet"), Source->Wallet->Copper, CopperBeforeFocus);
	TestEqual(TEXT("Focus preserves experience"),
		Source->Stats->CurrentExperience, ExperienceBeforeFocus);
	TestEqual(TEXT("Focus preserves inventory"),
		Source->Inventory->Stacks.Num(), InventoryStacksBeforeFocus);

	FText PersistenceMessage;
	UEmbermereSaveGame* SaveGame = nullptr;
	TestEqual(TEXT("Focused ledger captures through normal version-3 persistence"),
		UEmbermerePersistenceLibrary::CaptureGameState(
			Source, {}, SaveGame, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestNotNull(TEXT("Focused ledger capture creates a save"), SaveGame);
	if (!SaveGame)
	{
		return false;
	}
	TestEqual(TEXT("Focus does not add a durable record or schema field"),
		SaveGame->QuestStates.Num(), 2);
	TestEqual(TEXT("Captured format remains save version 3"),
		SaveGame->FormatVersion, EmbermereSaveGameVersion::MultiQuestLedger);
	TestEqual(TEXT("Version-3 restore succeeds without focus input"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, SaveGame, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Load derives fresh transient focus from the first active record"),
		Target->QuestLog->FocusedQuestId, ActiveQuest->QuestId);
	TestTrue(TEXT("Saved completed focus is not restored"),
		Target->QuestLog->ActiveQuest.Quest == ActiveQuest &&
		!Target->QuestLog->ActiveQuest.bCompleted);
	TestEqual(TEXT("Load preserves both authoritative quest records"),
		Target->QuestLog->QuestStates.Num(), 2);
	TestEqual(TEXT("Repeated load remains focus-idempotent"),
		UEmbermerePersistenceLibrary::ApplyGameState(
			Target, {}, SaveGame, PersistenceMessage),
		EEmbermerePersistenceResult::Success);
	TestEqual(TEXT("Repeated load derives the same active focus"),
		Target->QuestLog->FocusedQuestId, ActiveQuest->QuestId);
	return true;
}

#endif
