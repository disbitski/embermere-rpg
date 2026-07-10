#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Engine.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"
#include "UI/EmbermereEnemyNameplateWidget.h"
#include "UI/EmbermereGameplayMessageLibrary.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
constexpr int32 TargetRingSegmentCount = 24;
}

AEmbermereEnemyCharacter::AEmbermereEnemyCharacter()
{
	EnemyName = FText::FromString(TEXT("Marsh Prowler"));
	Tags.AddUnique("Hostile");

	NameplateText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectedTargetNameplate"));
	NameplateText->SetupAttachment(RootComponent);
	NameplateText->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
	NameplateText->SetText(FText::FromString(TEXT("Marsh Prowler\nHP 100/100")));
	NameplateText->SetTextRenderColor(FColor(255, 210, 118));
	NameplateText->SetHorizontalAlignment(EHTA_Center);
	NameplateText->SetVerticalAlignment(EVRTA_TextCenter);
	NameplateText->SetWorldSize(24.0f);
	NameplateText->SetCastShadow(false);
	NameplateText->SetVisibility(false);

	TargetMarkerText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("SelectedTargetMarker"));
	TargetMarkerText->SetupAttachment(RootComponent);
	TargetMarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, TargetMarkerHeight));
	TargetMarkerText->SetText(FText::FromString(TEXT("v")));
	TargetMarkerText->SetTextRenderColor(FColor(255, 226, 76));
	TargetMarkerText->SetHorizontalAlignment(EHTA_Center);
	TargetMarkerText->SetVerticalAlignment(EVRTA_TextCenter);
	TargetMarkerText->SetWorldSize(42.0f);
	TargetMarkerText->SetCastShadow(false);
	TargetMarkerText->SetVisibility(false);

	NameplateWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SelectedTargetWidgetNameplate"));
	NameplateWidgetComponent->SetupAttachment(RootComponent);
	NameplateWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
	NameplateWidgetComponent->SetWidgetClass(UEmbermereEnemyNameplateWidget::StaticClass());
	NameplateWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	NameplateWidgetComponent->SetDrawSize(FVector2D(196.0f, 78.0f));
	NameplateWidgetComponent->SetPivot(FVector2D(0.5f, 0.5f));
	NameplateWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NameplateWidgetComponent->SetCanEverAffectNavigation(false);
	NameplateWidgetComponent->SetVisibility(false);
	NameplateWidgetComponent->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> RingSegmentMeshFinder(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> RingMaterialFinder(
		TEXT("/Game/Art/Embermere/Targeting/M_EmbermereTargetRing.M_EmbermereTargetRing"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FallbackRingMaterialFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	for (int32 SegmentIndex = 0; SegmentIndex < TargetRingSegmentCount; ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = CreateDefaultSubobject<UStaticMeshComponent>(
			*FString::Printf(TEXT("SelectedTargetRingSegment_%02d"), SegmentIndex));
		Segment->SetupAttachment(RootComponent);
		if (RingSegmentMeshFinder.Succeeded())
		{
			Segment->SetStaticMesh(RingSegmentMeshFinder.Object);
		}
		if (RingMaterialFinder.Succeeded())
		{
			Segment->SetMaterial(0, RingMaterialFinder.Object);
		}
		else if (FallbackRingMaterialFinder.Succeeded())
		{
			Segment->SetMaterial(0, FallbackRingMaterialFinder.Object);
		}
		Segment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Segment->SetCanEverAffectNavigation(false);
		Segment->SetCastShadow(false);
		Segment->SetVisibility(false);
		Segment->SetHiddenInGame(true);
		TargetRingSegments.Add(Segment);
	}
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

bool AEmbermereEnemyCharacter::HasNameplateWidget() const
{
	return NameplateWidgetComponent != nullptr;
}

int32 AEmbermereEnemyCharacter::GetTargetRingSegmentCount() const
{
	return TargetRingSegments.Num();
}

FString AEmbermereEnemyCharacter::GetTargetRingMaterialPath() const
{
	if (TargetRingSegments.Num() <= 0 || !TargetRingSegments[0])
	{
		return FString();
	}

	const UMaterialInterface* RingMaterial = TargetRingSegments[0]->GetMaterial(0);
	return RingMaterial ? RingMaterial->GetPathName() : FString();
}

bool AEmbermereEnemyCharacter::IsLocationOutsideLeashRadius(const FVector& Location) const
{
	return FVector::DistSquared(SpawnTransform.GetLocation(), Location) > FMath::Square(FMath::Max(0.0f, LeashRadius));
}

bool AEmbermereEnemyCharacter::ShouldReturnHomeFromLocation(const FVector& Location) const
{
	return FVector::DistSquared(SpawnTransform.GetLocation(), Location) > FMath::Square(FMath::Max(0.0f, ReturnHomeRadius));
}

FText AEmbermereEnemyCharacter::GetTargetPresentationText() const
{
	if (!Stats)
	{
		return EnemyName;
	}

	return FText::FromString(FString::Printf(
		TEXT("%s\nHP %.0f/%.0f"),
		*EnemyName.ToString(),
		Stats->CurrentHealth,
		Stats->MaxHealth));
}

FLinearColor AEmbermereEnemyCharacter::GetTargetPresentationColor() const
{
	if (!Stats || Stats->MaxHealth <= 0.0f)
	{
		return FLinearColor(1.0f, 0.82f, 0.46f, 1.0f);
	}

	const float HealthPercent = Stats->CurrentHealth / Stats->MaxHealth;
	if (HealthPercent <= 0.25f)
	{
		return FLinearColor(1.0f, 0.28f, 0.18f, 1.0f);
	}
	if (HealthPercent <= 0.55f)
	{
		return FLinearColor(1.0f, 0.72f, 0.24f, 1.0f);
	}
	return FLinearColor(1.0f, 0.82f, 0.46f, 1.0f);
}

void AEmbermereEnemyCharacter::HandleDeath()
{
	AggroTarget.Reset();
	bReturningHome = false;
	bSelectedByPlayer = false;
	UpdatePrototypeTargetPresentation();

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("%s defeated"), *EnemyName.ToString())),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));

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
	AggroTarget.Reset();
	bReturningHome = false;
	LastAttackTimeSeconds = -1000.0;

	if (Stats)
	{
		Stats->InitializeVitals();
	}
	UpdatePrototypeTargetPresentation();

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("%s respawned"), *EnemyName.ToString())),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
}

void AEmbermereEnemyCharacter::UpdatePrototypeAi(float DeltaSeconds)
{
	if (!Stats || Stats->IsDead() || IsHidden())
	{
		return;
	}

	if (bReturningHome)
	{
		UpdateReturnHome(DeltaSeconds);
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
		if (ShouldReturnHomeFromLocation(GetActorLocation()))
		{
			bReturningHome = true;
			UpdateReturnHome(DeltaSeconds);
		}
		return;
	}

	const float DistanceSquared = FVector::DistSquared(GetActorLocation(), Target->GetActorLocation());
	if (ShouldLeashFromTarget(Target) || DistanceSquared > FMath::Square(AggroRadius * 1.35f))
	{
		DropAggroAndReturnHome();
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
	const bool bUseWidgetNameplate = NameplateWidgetComponent != nullptr;

	if (NameplateWidgetComponent)
	{
		NameplateWidgetComponent->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
		NameplateWidgetComponent->SetVisibility(bShowTargetPresentation);
		NameplateWidgetComponent->SetHiddenInGame(!bShowTargetPresentation);

		if (UEmbermereEnemyNameplateWidget* NameplateWidget = Cast<UEmbermereEnemyNameplateWidget>(NameplateWidgetComponent->GetUserWidgetObject()))
		{
			NameplateWidget->SetNameplateState(
				EnemyName,
				Stats ? Stats->CurrentHealth : 0.0f,
				Stats ? Stats->MaxHealth : 0.0f,
				GetTargetPresentationColor(),
				bShowTargetPresentation);
		}
	}

	if (NameplateText)
	{
		NameplateText->SetRelativeLocation(FVector(0.0f, 0.0f, NameplateHeight));
		NameplateText->SetText(GetTargetPresentationText());
		NameplateText->SetTextRenderColor(GetTargetPresentationColor().ToFColor(true));
		NameplateText->SetVisibility(bShowTargetPresentation && !bUseWidgetNameplate);
	}

	if (TargetMarkerText)
	{
		TargetMarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, TargetMarkerHeight));
		TargetMarkerText->SetVisibility(bShowTargetPresentation && !bUseWidgetNameplate);
	}

	UpdatePrototypeTargetRing(bShowTargetPresentation);

	UWorld* World = GetWorld();
	APawn* PlayerPawn = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
	if (bShowTargetPresentation && PlayerPawn)
	{
		const FVector PresentationLocation = NameplateWidgetComponent
			? NameplateWidgetComponent->GetComponentLocation()
			: (NameplateText ? NameplateText->GetComponentLocation() : GetActorLocation());
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

void AEmbermereEnemyCharacter::UpdatePrototypeTargetRing(bool bIsVisible)
{
	if (TargetRingSegments.Num() == 0)
	{
		return;
	}

	const float RingRadius = FMath::Max(16.0f, TargetRingRadius);
	const float SegmentThickness = FMath::Max(2.0f, TargetRingThickness);
	const float SegmentLength = 2.0f * RingRadius * FMath::Tan(PI / static_cast<float>(TargetRingSegmentCount)) * 0.82f;
	const UWorld* World = GetWorld();
	const float TimeSeconds = World ? World->GetTimeSeconds() : 0.0f;
	const float RotationOffsetDegrees = TimeSeconds * TargetRingRotationSpeedDegreesPerSecond;
	const float PulseAlpha = 0.5f + 0.5f * FMath::Sin(TimeSeconds * 2.4f);
	const float PulseScale = 1.0f + FMath::Max(0.0f, TargetRingPulseAmount) * PulseAlpha;
	FLinearColor AnimatedRingColor = TargetRingColor;
	const float Brightness = 0.82f + 0.18f * PulseAlpha;
	AnimatedRingColor.R *= Brightness;
	AnimatedRingColor.G *= Brightness;
	AnimatedRingColor.B *= Brightness;

	for (int32 SegmentIndex = 0; SegmentIndex < TargetRingSegments.Num(); ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = TargetRingSegments[SegmentIndex];
		if (!Segment)
		{
			continue;
		}

		Segment->SetVisibility(bIsVisible);
		Segment->SetHiddenInGame(!bIsVisible);
		if (!bIsVisible)
		{
			continue;
		}

		if (!TargetRingMaterials.IsValidIndex(SegmentIndex) || !TargetRingMaterials[SegmentIndex])
		{
			UMaterialInstanceDynamic* DynamicMaterial = Segment->CreateDynamicMaterialInstance(0);
			if (DynamicMaterial)
			{
				if (!TargetRingMaterials.IsValidIndex(SegmentIndex))
				{
					TargetRingMaterials.SetNum(TargetRingSegments.Num());
				}
				TargetRingMaterials[SegmentIndex] = DynamicMaterial;
			}
		}
		if (TargetRingMaterials.IsValidIndex(SegmentIndex) && TargetRingMaterials[SegmentIndex])
		{
			TargetRingMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("Color"), AnimatedRingColor);
			TargetRingMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("BaseColor"), AnimatedRingColor);
		}

		const float AngleDegrees =
			(360.0f * static_cast<float>(SegmentIndex)) / static_cast<float>(TargetRingSegments.Num()) + RotationOffsetDegrees;
		const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		const FVector SegmentLocation(
			FMath::Cos(AngleRadians) * RingRadius * PulseScale,
			FMath::Sin(AngleRadians) * RingRadius * PulseScale,
			TargetRingHeightOffset);
		const float TangentYawDegrees = AngleDegrees + 90.0f;

		Segment->SetRelativeLocation(SegmentLocation);
		Segment->SetRelativeRotation(FRotator(0.0f, TangentYawDegrees, 0.0f));
		Segment->SetRelativeScale3D(FVector(SegmentLength / 100.0f, SegmentThickness / 100.0f, 1.0f));
	}
}

AActor* AEmbermereEnemyCharacter::FindAggroTarget() const
{
	UWorld* World = GetWorld();
	APawn* PlayerPawn = World ? UGameplayStatics::GetPlayerPawn(World, 0) : nullptr;
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

bool AEmbermereEnemyCharacter::ShouldLeashFromTarget(const AActor* Target) const
{
	return IsLocationOutsideLeashRadius(GetActorLocation()) ||
		(Target && IsLocationOutsideLeashRadius(Target->GetActorLocation()));
}

void AEmbermereEnemyCharacter::DropAggroAndReturnHome()
{
	AggroTarget.Reset();
	bReturningHome = ShouldReturnHomeFromLocation(GetActorLocation());
}

void AEmbermereEnemyCharacter::UpdateReturnHome(float DeltaSeconds)
{
	const FVector HomeLocation = SpawnTransform.GetLocation();
	if (!ShouldReturnHomeFromLocation(GetActorLocation()))
	{
		SetActorLocation(HomeLocation, true);
		SetActorRotation(SpawnTransform.GetRotation());
		bReturningHome = false;
		return;
	}

	if (DeltaSeconds <= 0.0f)
	{
		return;
	}

	FVector Direction = HomeLocation - GetActorLocation();
	Direction.Z = 0.0f;
	const float DistanceToHome = Direction.Size();
	if (DistanceToHome <= KINDA_SMALL_NUMBER || !Direction.Normalize())
	{
		SetActorLocation(HomeLocation, true);
		SetActorRotation(SpawnTransform.GetRotation());
		bReturningHome = false;
		return;
	}

	SetActorRotation(Direction.Rotation());
	const float ReturnSpeed = FMath::Max(0.0f, ReturnHomeSpeedCmPerSecond);
	const float StepDistance = FMath::Min(DistanceToHome, ReturnSpeed * DeltaSeconds);
	SetActorLocation(GetActorLocation() + Direction * StepDistance, true);
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

	if (AppliedDamage > 0.0f)
	{
		UEmbermereGameplayMessageLibrary::PostGameplayMessage(
			this,
			FText::FromString(FString::Printf(TEXT("%s hits you for %.0f"), *EnemyName.ToString(), AppliedDamage)),
			FLinearColor(1.0f, 0.18f, 0.12f, 1.0f));
	}
}
