#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/World.h"
#include "GameFramework/WorldSettings.h"
#include "Misc/AutomationTest.h"

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
