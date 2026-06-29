#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

AEmbermereEnemyCharacter::AEmbermereEnemyCharacter()
{
	EnemyName = FText::FromString(TEXT("Marsh Prowler"));
	Tags.AddUnique("Hostile");

	NameplateText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectedTargetNameplate"));
	NameplateText->SetupAttachment(RootComponent);
	NameplateText->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
	NameplateText->SetText(FText::FromString(TEXT("Marsh Prowler\n100/100 HP")));
	NameplateText->SetTextRenderColor(FColor(255, 92, 82));
	NameplateText->SetHorizontalAlignment(EHTA_Center);
	NameplateText->SetVerticalAlignment(EVRTA_TextCenter);
	NameplateText->SetWorldSize(28.0f);
	NameplateText->SetCastShadow(false);
	NameplateText->SetVisibility(false);

	TargetMarkerText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectedTargetMarker"));
	TargetMarkerText->SetupAttachment(RootComponent);
	TargetMarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, TargetMarkerHeight));
	TargetMarkerText->SetText(FText::FromString(TEXT("TARGET")));
	TargetMarkerText->SetTextRenderColor(FColor(255, 226, 76));
	TargetMarkerText->SetHorizontalAlignment(EHTA_Center);
	TargetMarkerText->SetVerticalAlignment(EVRTA_TextCenter);
	TargetMarkerText->SetWorldSize(34.0f);
	TargetMarkerText->SetCastShadow(false);
	TargetMarkerText->SetVisibility(false);
}

void AEmbermereEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();

	if (Stats)
	{
		Stats->OnDied.AddDynamic(this, &AEmbermereEnemyCharacter::HandleDeath);
		Stats->OnHealthChanged.AddDynamic(this, &AEmbermereEnemyCharacter::HandleHealthChanged);
	}

	UpdatePrototypeTargetPresentation();
}

void AEmbermereEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdatePrototypeAi(DeltaSeconds);
	UpdatePrototypeTargetPresentation();
}

bool AEmbermereEnemyCharacter::IsHostileTo_Implementation(const AActor* Viewer) const
{
	return Viewer != this;
}

FText AEmbermereEnemyCharacter::GetTargetDisplayName_Implementation() const
{
	return EnemyName;
}

void AEmbermereEnemyCharacter::HandleTargetedByPlayer(bool bIsTargeted)
{
	bSelectedByPlayer = bIsTargeted;
	UpdatePrototypeTargetPresentation();
}

bool AEmbermereEnemyCharacter::IsSelectedByPlayer() const
{
	return bSelectedByPlayer;
}

void AEmbermereEnemyCharacter::HandleDeath()
{
	AggroTarget.Reset();
	bSelectedByPlayer = false;
	UpdatePrototypeTargetPresentation();

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

void AEmbermereEnemyCharacter::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	UpdatePrototypeTargetPresentation();
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
	UpdatePrototypeTargetPresentation();

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

void AEmbermereEnemyCharacter::UpdatePrototypeTargetPresentation()
{
	const bool bShowTargetPresentation = bSelectedByPlayer && Stats && !Stats->IsDead() && !IsHidden();

	if (NameplateText)
	{
		NameplateText->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
		if (Stats)
		{
			NameplateText->SetText(FText::FromString(FString::Printf(
				TEXT("%s\n%.0f/%.0f HP"),
				*EnemyName.ToString(),
				Stats->CurrentHealth,
				Stats->MaxHealth)));
		}
		NameplateText->SetVisibility(bShowTargetPresentation);
	}

	if (TargetMarkerText)
	{
		TargetMarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, TargetMarkerHeight));
		TargetMarkerText->SetVisibility(bShowTargetPresentation);
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (bShowTargetPresentation && PlayerPawn)
	{
		const FVector PresentationLocation = NameplateText ? NameplateText->GetComponentLocation() : GetActorLocation();
		FVector DirectionToPlayer = PlayerPawn->GetActorLocation() - PresentationLocation;
		DirectionToPlayer.Z = 0.0f;
		if (!DirectionToPlayer.IsNearlyZero())
		{
			const FRotator FacingRotation(0.0f, DirectionToPlayer.Rotation().Yaw + 180.0f, 0.0f);
			if (NameplateText)
			{
				NameplateText->SetWorldRotation(FacingRotation);
			}
			if (TargetMarkerText)
			{
				TargetMarkerText->SetWorldRotation(FacingRotation);
			}
		}
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
