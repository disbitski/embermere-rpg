#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereEnemyCharacter.h"
#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermerePracticeTargetActor.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyMeleeCoverSafetyTest,
	"Embermere.Enemy.MeleeCoverSafety",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyMeleeCoverSafetyTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Isolated enemy world exists"), World))
	{
		return false;
	}
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	AEmbermereCharacter* Player = World->SpawnActor<AEmbermereCharacter>(
		FVector(120.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	AEmbermereEnemyCharacter* Enemy = World->SpawnActor<AEmbermereEnemyCharacter>(
		FVector(0.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	AActor* Wall = World->SpawnActor<AActor>();
	if (!Controller || !Player || !Enemy || !Wall)
	{
		AddError(TEXT("Could not spawn enemy cover fixtures"));
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
		return false;
	}
	UBoxComponent* WallBox = NewObject<UBoxComponent>(Wall);
	WallBox->SetBoxExtent(FVector(10.0f, 120.0f, 120.0f));
	WallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallBox->SetCollisionResponseToAllChannels(ECR_Block);
	Wall->SetRootComponent(WallBox);
	WallBox->RegisterComponent();
	Wall->SetActorLocation(FVector(60.0f, 0.0f, 90.0f));
	Controller->Possess(Player);
	World->AddController(Controller);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	World->GetWorldSettings()->NotifyBeginPlay();

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EmbermereEnemyCoverControl), false);
	QueryParams.AddIgnoredActor(Enemy);
	QueryParams.AddIgnoredActor(Player);
	FHitResult Hit;
	TestTrue(TEXT("Solid wall blocks the actual melee visibility channel"),
		World->LineTraceSingleByChannel(Hit,
			Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			Player->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			ECC_Visibility, QueryParams) && Hit.GetActor() == Wall);
	const float HealthBefore = Player->Stats->CurrentHealth;
	static_cast<AActor*>(Enemy)->Tick(1.0f / 60.0f);
	TestEqual(TEXT("In-range enemy cannot hit through solid cover"),
		Player->Stats->CurrentHealth, HealthBefore);
	const float HealthAfterBlockedAttempt = Player->Stats->CurrentHealth;

	Wall->SetActorLocation(FVector(60.0f, 400.0f, 90.0f));
	TestFalse(TEXT("Moved wall leaves the melee visibility channel clear"),
		World->LineTraceSingleByChannel(Hit,
			Enemy->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			Player->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			ECC_Visibility, QueryParams));
	static_cast<AActor*>(Enemy)->Tick(1.0f / 60.0f);
	TestTrue(TEXT("Enemy lands the normal hit immediately after cover clears"),
		Player->Stats->CurrentHealth < HealthAfterBlockedAttempt);

	World->EndPlay(EEndPlayReason::Quit);
	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereObstructedAbilityTest,
	"Embermere.Combat.ObstructedAbility",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereObstructedAbilityTest::RunTest(const FString& Parameters)
{
	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Isolated combat world exists"), World))
	{
		return false;
	}
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	AEmbermereCharacter* Player = World->SpawnActor<AEmbermereCharacter>(
		FVector(0.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	AEmbermerePracticeTargetActor* Target = World->SpawnActor<AEmbermerePracticeTargetActor>(
		FVector(400.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	AActor* Wall = World->SpawnActor<AActor>();
	if (!Controller || !Player || !Target || !Wall)
	{
		AddError(TEXT("Could not spawn line-of-sight fixtures"));
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
		return false;
	}
	UBoxComponent* WallBox = NewObject<UBoxComponent>(Wall);
	WallBox->SetBoxExtent(FVector(20.0f, 150.0f, 120.0f));
	WallBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WallBox->SetCollisionResponseToAllChannels(ECR_Block);
	Wall->SetRootComponent(WallBox);
	WallBox->RegisterComponent();
	Wall->SetActorLocation(FVector(200.0f, 0.0f, 90.0f));
	Controller->Possess(Player);
	World->AddController(Controller);
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	World->GetWorldSettings()->NotifyBeginPlay();

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EmbermereAbilityObstructionControl), false);
	QueryParams.AddIgnoredActor(Player);
	QueryParams.AddIgnoredActor(Target);
	FHitResult Hit;
	TestTrue(TEXT("Registered wall blocks the actual visibility channel"),
		World->LineTraceSingleByChannel(Hit,
			Player->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			Target->GetActorLocation() + FVector(0.0f, 0.0f, 90.0f),
			ECC_Visibility, QueryParams) && Hit.GetActor() == Wall);

	FEmbermereAbilityDefinition Bolt;
	Bolt.AbilityId = TEXT("BlockedBolt");
	Bolt.DisplayName = FText::FromString(TEXT("Blocked Bolt"));
	Bolt.TargetKind = EEmbermereAbilityTargetKind::Enemy;
	Bolt.EffectType = EEmbermereAbilityEffectType::Damage;
	Bolt.Power = 20.0f;
	Bolt.ManaCost = 8.0f;
	Bolt.Range = 600.0f;
	Player->Combat->SetTarget(Target);
	const float HealthBefore = Target->Stats->CurrentHealth;
	const float ManaBefore = Player->Stats->CurrentMana;
	const int32 CopperBefore = Player->Wallet->Copper;
	const int32 ExperienceBefore = Player->Stats->CurrentExperience;
	int32 CommittedResults = 0;
	Player->Combat->OnCombatResult.AddLambda([&CommittedResults](const FEmbermereCombatResult&)
	{
		++CommittedResults;
	});
	TestFalse(TEXT("Solid wall rejects the targeted cast"), Player->Combat->ExecuteAbility(Bolt));
	TestEqual(TEXT("Blocked cast leaves target health unchanged"), Target->Stats->CurrentHealth, HealthBefore);
	TestEqual(TEXT("Blocked cast spends no mana"), Player->Stats->CurrentMana, ManaBefore);
	TestEqual(TEXT("Blocked cast publishes no result"), CommittedResults, 0);
	TestEqual(TEXT("Blocked cast gives no copper"), Player->Wallet->Copper, CopperBefore);
	TestEqual(TEXT("Blocked cast gives no XP"), Player->Stats->CurrentExperience, ExperienceBefore);
	TestEqual(TEXT("Blocked cast gives no quest progress"), Player->QuestLog->QuestStates.Num(), 0);

	Wall->SetActorLocation(FVector(200.0f, 400.0f, 90.0f));
	TestTrue(TEXT("Same in-range ability works after the wall clears"),
		Player->Combat->ExecuteAbility(Bolt));
	TestTrue(TEXT("Clear cast applies damage"), Target->Stats->CurrentHealth < HealthBefore);
	TestEqual(TEXT("Clear cast spends its normal mana cost"), Player->Stats->CurrentMana,
		ManaBefore - Bolt.ManaCost);
	TestEqual(TEXT("Only the clear cast publishes a result"), CommittedResults, 1);

	World->EndPlay(EEndPlayReason::Quit);
	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereSpawnedDefeatLootTest,
	"Embermere.Enemy.SpawnedDefeatLoot",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereSpawnedDefeatLootTest::RunTest(const FString& Parameters)
{
	UClass* EnemyClass = LoadClass<AEmbermereEnemyCharacter>(nullptr,
		TEXT("/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy_C"));
	if (!TestNotNull(TEXT("Saved Prowler class exists"), EnemyClass))
	{
		return false;
	}

	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Isolated world exists"), World))
	{
		return false;
	}
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	AEmbermereCharacter* Player = World->SpawnActor<AEmbermereCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	AEmbermereEnemyCharacter* Enemy = World->SpawnActor<AEmbermereEnemyCharacter>(
		EnemyClass, FVector(300.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	if (Controller && Player && Enemy)
	{
		Controller->Possess(Player);
		World->AddController(Controller);
		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
		World->GetWorldSettings()->NotifyBeginPlay();
		UEmbermereItemData* Tonic = Enemy->LootItem.LoadSynchronous();
		if (TestTrue(TEXT("Saved Prowler entered a started world"), Enemy->HasActorBegunPlay()) &&
			TestNotNull(TEXT("Saved drop resolves"), Tonic))
		{
			const int32 CopperBefore = Player->Wallet->Copper;
			const int32 ExperienceBefore = Player->Stats->CurrentExperience;
			TestEqual(TEXT("Saved drop identity is Marsh Tonic"), Tonic->ItemId, FName(TEXT("MarshTonic")));
			TestEqual(TEXT("Bag starts without a tonic"), Player->Inventory->GetItemQuantity(Tonic), 0);
			TestTrue(TEXT("Authoritative damage defeats the spawned Prowler"),
				Enemy->Stats->ApplyDamage(100000.0f) > 0.0f && Enemy->Stats->IsDead());
			TestEqual(TEXT("Death event grants the configured tonic once"),
				Player->Inventory->GetItemQuantity(Tonic), 1);
			TestEqual(TEXT("Repeated damage cannot grant a second drop"), Enemy->Stats->ApplyDamage(100000.0f), 0.0f);
			TestEqual(TEXT("Drop remains single after repeated damage"), Player->Inventory->GetItemQuantity(Tonic), 1);
			TestEqual(TEXT("Death loot does not grant copper directly"), Player->Wallet->Copper, CopperBefore);
			TestEqual(TEXT("Death loot does not grant XP directly"), Player->Stats->CurrentExperience, ExperienceBefore);
		}
		World->EndPlay(EEndPlayReason::Quit);
	}
	else
	{
		AddError(TEXT("Could not spawn player and Prowler fixtures"));
	}

	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereSpawnedCombatQuestCreditTest,
	"Embermere.Quests.SpawnedCombatCredit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereSpawnedCombatQuestCreditTest::RunTest(const FString& Parameters)
{
	UClass* EnemyClass = LoadClass<AEmbermereEnemyCharacter>(nullptr,
		TEXT("/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy_C"));
	UEmbermereQuestData* MaraQuest = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	UEmbermereQuestData* StillWaters = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters"));
	if (!TestNotNull(TEXT("Saved Prowler class exists"), EnemyClass) ||
		!TestNotNull(TEXT("Saved Mara quest exists"), MaraQuest) ||
		!TestNotNull(TEXT("Saved Still Waters quest exists"), StillWaters))
	{
		return false;
	}

	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Isolated world exists"), World))
	{
		return false;
	}
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	APlayerController* Controller = World->SpawnActor<APlayerController>();
	AEmbermereCharacter* Player = World->SpawnActor<AEmbermereCharacter>(FVector::ZeroVector, FRotator::ZeroRotator);
	AEmbermerePracticeTargetActor* Dummy = World->SpawnActor<AEmbermerePracticeTargetActor>(
		FVector(150.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	AEmbermereEnemyCharacter* Enemy = World->SpawnActor<AEmbermereEnemyCharacter>(
		EnemyClass, FVector(300.0f, 0.0f, 90.0f), FRotator::ZeroRotator);
	if (Controller && Player && Dummy && Enemy)
	{
		Controller->Possess(Player);
		World->AddController(Controller);
		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
		World->GetWorldSettings()->NotifyBeginPlay();
		if (TestTrue(TEXT("Saved Prowler entered a started world"), Enemy->HasActorBegunPlay()) &&
			TestTrue(TEXT("Player has combat and quest owners"), Player->Combat && Player->QuestLog) &&
			TestTrue(TEXT("Mara quest enters the ledger"), Player->QuestLog->AcceptQuest(MaraQuest)) &&
			TestTrue(TEXT("Still Waters enters the ledger"), Player->QuestLog->AcceptQuest(StillWaters)))
		{
			const int32 CopperBefore = Player->Wallet->Copper;
			const int32 ExperienceBefore = Player->Stats->CurrentExperience;
			FEmbermereAbilityDefinition Strike;
			Strike.AbilityId = TEXT("QuestCreditStrike");
			Strike.DisplayName = FText::FromString(TEXT("Quest Credit Strike"));
			Strike.TargetKind = EEmbermereAbilityTargetKind::Enemy;
			Strike.EffectType = EEmbermereAbilityEffectType::Damage;
			Strike.Power = 200.0f;
			Strike.Range = 400.0f;

			Player->Combat->SetTarget(Dummy);
			TestTrue(TEXT("Combat can defeat the practice target"), Player->Combat->ExecuteAbility(Strike));
			FEmbermereQuestState MaraState;
			TestTrue(TEXT("Mara remains tracked after training"),
				Player->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState));
			TestEqual(TEXT("Practice defeat grants no Mara credit"), MaraState.CurrentObjectiveCount, 0);

			Player->Combat->SetTarget(Enemy);
			TestTrue(TEXT("Combat can defeat the saved Prowler"), Player->Combat->ExecuteAbility(Strike));
			TestTrue(TEXT("Saved Prowler is dead after the committed ability"), Enemy->Stats->IsDead());
			TestTrue(TEXT("Mara remains tracked after Prowler defeat"),
				Player->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState));
			TestEqual(TEXT("One combat defeat grants exactly one Mara objective credit"),
				MaraState.CurrentObjectiveCount, 1);
			FEmbermereQuestState StillWatersState;
			TestTrue(TEXT("Still Waters remains tracked"),
				Player->QuestLog->GetQuestStateById(StillWaters->QuestId, StillWatersState));
			TestEqual(TEXT("Combat cannot advance the rest objective"), StillWatersState.CurrentObjectiveCount, 0);
			TestFalse(TEXT("Mara remains incomplete before three defeats"), MaraState.bCompleted);
			TestFalse(TEXT("Rejected repeat cannot credit a defeated target"), Player->Combat->ExecuteAbility(Strike));
			Player->QuestLog->GetQuestStateById(MaraQuest->QuestId, MaraState);
			TestEqual(TEXT("Rejected repeat cannot duplicate quest credit"), MaraState.CurrentObjectiveCount, 1);
			TestEqual(TEXT("Combat credit does not pay Mara rewards"), Player->Wallet->Copper, CopperBefore);
			TestEqual(TEXT("Combat credit does not grant quest XP"), Player->Stats->CurrentExperience, ExperienceBefore);
		}
		World->EndPlay(EEndPlayReason::Quit);
	}
	else
	{
		AddError(TEXT("Could not spawn player, dummy, and Prowler fixtures"));
	}

	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	return !HasAnyErrors();
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyReturnPastGateTest,
	"Embermere.Enemy.ReturnPastGate",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyReturnPastGateTest::RunTest(const FString& Parameters)
{
	UStaticMesh* GateMesh = LoadObject<UStaticMesh>(nullptr,
		TEXT("/Game/Art/Embermere/Environment/PrototypeVillage/SM_EmbermereRoadGate_01.SM_EmbermereRoadGate_01"));
	UClass* EnemyClass = LoadClass<AEmbermereEnemyCharacter>(nullptr,
		TEXT("/Game/Blueprints/BP_StarterEnemy.BP_StarterEnemy_C"));
	if (!TestNotNull(TEXT("Saved gate mesh exists"), GateMesh) ||
		!TestNotNull(TEXT("Saved enemy class exists"), EnemyClass))
	{
		return false;
	}

	UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
	if (!TestNotNull(TEXT("Isolated physics world exists"), World))
	{
		return false;
	}
	GEngine->CreateNewWorldContext(EWorldType::Game).SetCurrentWorld(World);
	AStaticMeshActor* Gate = World->SpawnActor<AStaticMeshActor>(
		FVector(1080.0, 540.0, 0.0), FRotator(0.0, 20.0, 0.0));
	const FVector Home(1700.0, 1100.0, 90.15);
	AEmbermereEnemyCharacter* Enemy = World->SpawnActor<AEmbermereEnemyCharacter>(
		EnemyClass, Home, FRotator::ZeroRotator);
	if (Gate && Enemy)
	{
		Gate->GetStaticMeshComponent()->SetStaticMesh(GateMesh);
		Gate->GetStaticMeshComponent()->SetCollisionProfileName(TEXT("BlockAll"));
		World->InitializeActorsForPlay(FURL());
		World->BeginPlay();
		World->GetWorldSettings()->NotifyBeginPlay();
		TestTrue(TEXT("Enemy participates in a started world"), Enemy->HasActorBegunPlay());
		TestFalse(TEXT("Configured home is captured before displacement"),
			Enemy->ShouldReturnHomeFromLocation(Home));

		// Reproduce September 9's real post-pull capsule contact, not a center-line trace.
		const FVector Contact(1095.200758, 641.289858, 90.150099);
		Enemy->SetActorLocation(Contact, false);
		TestTrue(TEXT("Contact requires a return"), Enemy->ShouldReturnHomeFromLocation(Contact));
		FCollisionQueryParams Query(SCENE_QUERY_STAT(EmbermereReturnGateTest), false, Enemy);
		FHitResult Hit;
		TestFalse(TEXT("Center ray alone misses the footing"),
			World->LineTraceSingleByChannel(Hit, Contact, Home, ECC_Pawn, Query));
		TestTrue(TEXT("Full capsule detects the gate contact"),
			World->SweepSingleByChannel(Hit, Contact, Home, FQuat::Identity, ECC_Pawn,
				Enemy->GetCapsuleComponent()->GetCollisionShape(), Query));
		TestEqual(TEXT("Purposeful gate collision is the obstruction"), Hit.GetActor(), static_cast<AActor*>(Gate));
		Enemy->TickActor(0.0f, LEVELTICK_All, Enemy->PrimaryActorTick);
		TestTrue(TEXT("Zero delta cannot advance return"), Enemy->GetActorLocation().Equals(Contact, 0.01));
		Enemy->bPrototypeAiEnabled = false;
		Enemy->TickActor(1.0f, LEVELTICK_All, Enemy->PrimaryActorTick);
		TestTrue(TEXT("Disabled AI cannot move"), Enemy->GetActorLocation().Equals(Contact, 0.01));
		Enemy->bPrototypeAiEnabled = true;
		Enemy->Stats->GrantMovementSpeedMultiplier(0.0f, 10.0f);
		Enemy->TickActor(1.0f, LEVELTICK_All, Enemy->PrimaryActorTick);
		TestTrue(TEXT("Rooted return cannot slide"), Enemy->GetActorLocation().Equals(Contact, 0.01));
		Enemy->Stats->ClearTemporaryEffects();
		const float HealthBefore = Enemy->Stats->CurrentHealth;
		for (int32 Frame = 0; Frame < 300; ++Frame)
		{
			Enemy->TickActor(1.0f / 30.0f, LEVELTICK_All, Enemy->PrimaryActorTick);
		}
		AddInfo(FString::Printf(TEXT("Return location after ten seconds: %s"),
			*Enemy->GetActorLocation().ToString()));
		TestTrue(TEXT("Return passes the solid gate footing and reaches exact home"),
			Enemy->GetActorLocation().Equals(Home, 1.0));
		TestEqual(TEXT("Return does not mutate health"), Enemy->Stats->CurrentHealth, HealthBefore);
		TestFalse(TEXT("Returned enemy needs no further return"),
			Enemy->ShouldReturnHomeFromLocation(Enemy->GetActorLocation()));
		World->EndPlay(EEndPlayReason::Quit);
	}
	else
	{
		AddError(TEXT("Could not spawn gate and enemy fixtures"));
	}

	GEngine->DestroyWorldContext(World);
	World->DestroyWorld(false);
	return !HasAnyErrors();
}

#endif
