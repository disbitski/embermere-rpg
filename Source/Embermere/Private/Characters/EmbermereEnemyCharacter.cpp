#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEmbermereEnemyCharacter::AEmbermereEnemyCharacter()
{
	EnemyName = FText::FromString(TEXT("Marsh Prowler"));
	Tags.AddUnique("Hostile");
}

void AEmbermereEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();

	if (Stats)
	{
		Stats->OnDied.AddDynamic(this, &AEmbermereEnemyCharacter::HandleDeath);
	}
}

void AEmbermereEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdatePrototypeAi(DeltaSeconds);
}

bool AEmbermereEnemyCharacter::IsHostileTo_Implementation(const AActor* Viewer) const
{
	return Viewer != this;
}

FText AEmbermereEnemyCharacter::GetTargetDisplayName_Implementation() const
{
	return EnemyName;
}

void AEmbermereEnemyCharacter::HandleDeath()
{
	AggroTarget.Reset();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.5f,
			FColor::Silver,
			FString::Printf(TEXT("%s defeated"), *EnemyName.ToString()));
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&AEmbermereEnemyCharacter::Respawn,
			FMath::Max(0.1f, RespawnDelaySeconds),
			false);
	}
}

void AEmbermereEnemyCharacter::Respawn()
{
	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	LastAttackTimeSeconds = -1000.0;

	if (Stats)
	{
		Stats->InitializeVitals();
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.5f,
			FColor::Silver,
			FString::Printf(TEXT("%s respawned"), *EnemyName.ToString()));
	}
}

void AEmbermereEnemyCharacter::UpdatePrototypeAi(float DeltaSeconds)
{
	if (!Stats || Stats->IsDead() || IsHidden())
	{
		return;
	}

	AActor* Target = AggroTarget.Get();
	if (!IsValidAggroTarget(Target))
	{
		Target = FindAggroTarget();
		AggroTarget = Target;
	}

	if (!Target)
	{
		return;
	}

	const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	if (DistanceSquared > FMath::Square(AggroRadius * 1.35f))
	{
		AggroTarget.Reset();
		return;
	}

	FaceTarget(Target);

	if (DistanceSquared > FMath::Square(AttackRange))
	{
		MoveTowardTarget(Target, DeltaSeconds);
	}
	else
	{
		TryAttackTarget(Target);
	}
}

AActor* AEmbermereEnemyCharacter::FindAggroTarget() const
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (IsValidAggroTarget(PlayerPawn) &&
		FVector::DistSquared(GetActorLocation(), PlayerPawn->GetActorLocation()) <= FMath::Square(AggroRadius))
	{
		return PlayerPawn;
	}

	return nullptr;
}

bool AEmbermereEnemyCharacter::IsValidAggroTarget(const AActor* Candidate) const
{
	if (!Candidate || Candidate == this)
	{
		return false;
	}

	const UEmbermereStatsComponent* CandidateStats = Candidate->FindComponentByClass<UEmbermereStatsComponent>();
	return CandidateStats && !CandidateStats->IsDead();
}

void AEmbermereEnemyCharacter::FaceTarget(const AActor* Target)
{
	if (!Target)
	{
		return;
	}

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	if (Direction.IsNearlyZero())
	{
		return;
	}

	SetActorRotation(Direction.Rotation());
}

void AEmbermereEnemyCharacter::MoveTowardTarget(AActor* Target, float DeltaSeconds)
{
	if (!Target || DeltaSeconds <= 0.0f || MoveSpeedCmPerSecond <= 0.0f)
	{
		return;
	}

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	if (!Direction.Normalize())
	{
		return;
	}

	const FVector Step = Direction * MoveSpeedCmPerSecond * DeltaSeconds;
	SetActorLocation(GetActorLocation() + Step, true);
}

void AEmbermereEnemyCharacter::TryAttackTarget(AActor* Target)
{
	if (!Target || AttackDamage <= 0.0f)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const double CurrentTimeSeconds = World->GetTimeSeconds();
	if (CurrentTimeSeconds - LastAttackTimeSeconds < AttackCooldownSeconds)
	{
		return;
	}

	UEmbermereStatsComponent* TargetStats = Target->FindComponentByClass<UEmbermereStatsComponent>();
	if (!TargetStats || TargetStats->IsDead())
	{
		return;
	}

	const float AppliedDamage = TargetStats->ApplyDamage(AttackDamage);
	LastAttackTimeSeconds = CurrentTimeSeconds;

	if (GEngine && AppliedDamage > 0.0f)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.5f,
			FColor::Red,
			FString::Printf(TEXT("%s hits you for %.0f"), *EnemyName.ToString(), AppliedDamage));
	}
}
