#include "Characters/EmbermereEnemyCharacter.h"
#include "Animation/AnimSequence.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/WidgetComponent.h"
#include "Data/EmbermereItemData.h"
#include "Engine/Engine.h"
#include "Engine/SkeletalMesh.h"
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
constexpr int32 TargetRingSegmentCount = 48;
constexpr float MinimumTargetRingSurfaceClearance = 16.0f;
}

AEmbermereEnemyCharacter::AEmbermereEnemyCharacter()
{
	EnemyName = FText::FromString(TEXT("Marsh Prowler"));
	Tags.AddUnique("Hostile");

	static ConstructorHelpers::FObjectFinder<UEmbermereItemData> MarshTonic(
		TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
	if (MarshTonic.Succeeded())
	{
		LootItem = MarshTonic.Object;
	}

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

void AEmbermereEnemyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	EnsureTargetRingPresentationComponents();
}

void AEmbermereEnemyCharacter::EnsureTargetRingPresentationComponents()
{
	TMap<FName, UStaticMeshComponent*> ExistingSegments;
	TInlineComponentArray<UStaticMeshComponent*> StaticMeshComponents(this);
	for (UStaticMeshComponent* Component : StaticMeshComponents)
	{
		if (Component && Component->GetName().StartsWith(TEXT("SelectedTargetRingSegment_")))
		{
			ExistingSegments.Add(Component->GetFName(), Component);
		}
	}

	UStaticMesh* RingSegmentMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane"));
	UMaterialInterface* RingMaterial = LoadObject<UMaterialInterface>(
		nullptr,
		TEXT("/Game/Art/Embermere/Targeting/M_EmbermereTargetRing.M_EmbermereTargetRing"));
	if (!RingMaterial)
	{
		RingMaterial = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}

	TargetRingSegments.Reset(TargetRingSegmentCount);
	TargetRingMaterials.Reset();
	for (int32 SegmentIndex = 0; SegmentIndex < TargetRingSegmentCount; ++SegmentIndex)
	{
		const FName SegmentName(*FString::Printf(TEXT("SelectedTargetRingSegment_%02d"), SegmentIndex));
		UStaticMeshComponent* Segment = ExistingSegments.FindRef(SegmentName);
		if (!Segment)
		{
			Segment = NewObject<UStaticMeshComponent>(this, SegmentName, RF_Transient);
			Segment->SetupAttachment(RootComponent);
			AddInstanceComponent(Segment);
		}

		if (RingSegmentMesh)
		{
			Segment->SetStaticMesh(RingSegmentMesh);
		}
		if (RingMaterial)
		{
			Segment->SetMaterial(0, RingMaterial);
		}
		Segment->SetMobility(EComponentMobility::Movable);
		Segment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Segment->SetGenerateOverlapEvents(false);
		Segment->SetCanEverAffectNavigation(false);
		Segment->SetCastShadow(false);
		Segment->SetVisibility(false);
		Segment->SetHiddenInGame(true);

		if (!Segment->IsRegistered() && GetWorld())
		{
			Segment->RegisterComponent();
		}
		TargetRingSegments.Add(Segment);
	}
}

void AEmbermereEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	SpawnTransform = GetActorTransform();
	if (!bGameplayCollisionEnabled)
	{
		SetActorEnableCollision(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCapsuleComponent()->SetCanEverAffectNavigation(false);
	}
	ApplyConfiguredVisualPresentation();

	if (Stats)
	{
		Stats->OnDied.AddDynamic(this, &AEmbermereEnemyCharacter::HandleDeath);
		Stats->OnHealthChanged.AddDynamic(this, &AEmbermereEnemyCharacter::HandleHealthChanged);
		LastObservedHealth = Stats->CurrentHealth;
	}

	UpdateVisualAnimation();
	UpdatePrototypeTargetPresentation();
}

void AEmbermereEnemyCharacter::ApplyConfiguredVisualPresentation()
{
	USkeletalMeshComponent* MeshComponent = GetMesh();
	if (!MeshComponent || VisualSkeletalMesh.IsNull())
	{
		return;
	}

	USkeletalMesh* LoadedMesh = VisualSkeletalMesh.LoadSynchronous();
	if (!LoadedMesh)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Enemy '%s' could not resolve visual mesh '%s'"),
			*GetName(),
			*VisualSkeletalMesh.ToSoftObjectPath().ToString());
		return;
	}

	MeshComponent->SetSkeletalMeshAsset(LoadedMesh);
	MeshComponent->SetRelativeLocation(VisualMeshRelativeLocation);
	MeshComponent->SetRelativeRotation(VisualMeshRelativeRotation);
	MeshComponent->SetRelativeScale3D(VisualMeshRelativeScale);
	MeshComponent->SetAnimationMode(EAnimationMode::AnimationSingleNode);
}

void AEmbermereEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	bMovedThisFrame = false;
	UpdatePrototypeAi(DeltaSeconds);
	UpdateVisualAnimation();
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

FVector AEmbermereEnemyCharacter::GetCombatFeedbackAnchorLocation_Implementation() const
{
	const float FeedbackHeight = FMath::Max(60.0f, NameplateHeight - 48.0f);
	return GetActorLocation() + FVector(0.0f, 0.0f, FeedbackHeight);
}

bool AEmbermereEnemyCharacter::ShouldGrantDefeatCredit_Implementation() const
{
	return bGrantsDefeatCredit;
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

FLinearColor AEmbermereEnemyCharacter::GetTargetRingColor() const
{
	return TargetRingColor;
}

float AEmbermereEnemyCharacter::GetResolvedTargetRingRadius() const
{
	float ResolvedRadius = FMath::Max(16.0f, TargetRingRadius);
	const float Padding = FMath::Max(0.0f, TargetRingBoundsPadding);

	if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		ResolvedRadius = FMath::Max(ResolvedRadius, Capsule->GetUnscaledCapsuleRadius() + Padding);
	}

	const USkeletalMeshComponent* MeshComponent = GetMesh();
	const USkeletalMesh* MeshAsset = MeshComponent ? MeshComponent->GetSkeletalMeshAsset() : nullptr;
	if (MeshComponent && MeshAsset)
	{
		const FTransform VisualTransform(
			MeshComponent->GetRelativeRotation(),
			FVector::ZeroVector,
			MeshComponent->GetRelativeScale3D());
		const FBoxSphereBounds VisualBounds = MeshAsset->GetBounds().TransformBy(VisualTransform);
		const float HorizontalExtent = FMath::Max(VisualBounds.BoxExtent.X, VisualBounds.BoxExtent.Y);
		ResolvedRadius = FMath::Max(ResolvedRadius, HorizontalExtent + Padding);
	}

	return ResolvedRadius;
}

float AEmbermereEnemyCharacter::GetEffectiveTargetRingSurfaceClearance() const
{
	return FMath::Max(MinimumTargetRingSurfaceClearance, TargetRingSurfaceClearance);
}

bool AEmbermereEnemyCharacter::IsTargetRingVisible() const
{
	return TargetRingSegments.Num() > 0 && TargetRingSegments[0] && TargetRingSegments[0]->IsVisible();
}

bool AEmbermereEnemyCharacter::AreTargetRingSegmentsNonColliding() const
{
	for (const UStaticMeshComponent* Segment : TargetRingSegments)
	{
		if (!Segment || Segment->GetCollisionEnabled() != ECollisionEnabled::NoCollision)
		{
			return false;
		}
	}

	return TargetRingSegments.Num() > 0;
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

	SetActorEnableCollision(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	const float DeathAnimationDuration = PlayVisualAnimation(DeathAnimation, false, true);

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("%s defeated"), *EnemyName.ToString())),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));

	if (ShouldDropLoot(FMath::FRand()))
	{
		GrantLootTo(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (UWorld* World = GetWorld())
	{
		if (DeathAnimationDuration > KINDA_SMALL_NUMBER)
		{
			const float HideDelay = FMath::Min(
				DeathAnimationDuration,
				FMath::Max(0.1f, RespawnDelaySeconds - 0.1f));
			World->GetTimerManager().SetTimer(
				DeathHideTimerHandle,
				this,
				&AEmbermereEnemyCharacter::HideDeadBody,
				HideDelay,
				false);
		}
		else
		{
			SetActorHiddenInGame(true);
		}

		World->GetTimerManager().SetTimer(
			RespawnTimerHandle,
			this,
			&AEmbermereEnemyCharacter::Respawn,
			FMath::Max(0.1f, RespawnDelaySeconds),
			false);
	}
}

bool AEmbermereEnemyCharacter::ShouldDropLoot(float RandomRoll) const
{
	const float ClampedChance = FMath::Clamp(LootDropChance, 0.0f, 1.0f);
	return bLootEnabled && !LootItem.IsNull() && LootQuantity > 0 && ClampedChance > 0.0f &&
		FMath::Clamp(RandomRoll, 0.0f, 1.0f) < ClampedChance;
}

bool AEmbermereEnemyCharacter::GrantLootTo(AActor* Recipient)
{
	if (!bLootEnabled || !Recipient || LootQuantity <= 0)
	{
		return false;
	}

	UEmbermereItemData* Item = LootItem.LoadSynchronous();
	UEmbermereInventoryComponent* RecipientInventory = Recipient->FindComponentByClass<UEmbermereInventoryComponent>();
	if (!Item || !RecipientInventory || !RecipientInventory->AddItem(Item, LootQuantity))
	{
		return false;
	}

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		Recipient,
		FText::FromString(FString::Printf(TEXT("Looted %s x%d"), *Item->DisplayName.ToString(), LootQuantity)),
		FLinearColor(0.48f, 0.92f, 0.62f, 1.0f));
	return true;
}

void AEmbermereEnemyCharacter::HandleHealthChanged(float CurrentHealth, float MaxHealth)
{
	if (CurrentHealth > 0.0f &&
		LastObservedHealth >= 0.0f &&
		CurrentHealth < LastObservedHealth)
	{
		PlayVisualAnimation(HitAnimation, false, true);
	}
	LastObservedHealth = CurrentHealth;
	UpdatePrototypeTargetPresentation();
}

void AEmbermereEnemyCharacter::Respawn()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DeathHideTimerHandle);
	}

	SetActorTransform(SpawnTransform);
	SetActorHiddenInGame(false);
	SetActorEnableCollision(bGameplayCollisionEnabled);
	GetCapsuleComponent()->SetCollisionEnabled(
		bGameplayCollisionEnabled
			? ECollisionEnabled::QueryAndPhysics
			: ECollisionEnabled::NoCollision);
	AggroTarget.Reset();
	bReturningHome = false;
	LastAttackTimeSeconds = -1000.0;

	if (Stats)
	{
		Stats->InitializeVitals();
		LastObservedHealth = Stats->CurrentHealth;
	}
	CurrentVisualAnimation = nullptr;
	bCurrentVisualAnimationLooping = false;
	VisualAnimationLockUntilSeconds = -1.0;
	UpdateVisualAnimation();
	UpdatePrototypeTargetPresentation();

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("%s respawned"), *EnemyName.ToString())),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
}

void AEmbermereEnemyCharacter::UpdatePrototypeAi(float DeltaSeconds)
{
	if (!bPrototypeAiEnabled || !Stats || Stats->IsDead() || IsHidden())
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

	if (!bIsVisible)
	{
		for (UStaticMeshComponent* Segment : TargetRingSegments)
		{
			if (Segment)
			{
				Segment->SetVisibility(false);
				Segment->SetHiddenInGame(true);
			}
		}
		return;
	}

	const float RingRadius = GetResolvedTargetRingRadius();
	const float SegmentThickness = FMath::Max(2.0f, TargetRingThickness);
	const float SegmentLength =
		2.0f * RingRadius * FMath::Tan(PI / static_cast<float>(TargetRingSegments.Num())) *
		FMath::Max(1.0f, TargetRingArcCoverage);
	const UWorld* World = GetWorld();
	const float TimeSeconds = World ? World->GetTimeSeconds() : 0.0f;
	const float RotationOffsetDegrees = TimeSeconds * TargetRingRotationSpeedDegreesPerSecond;
	const float PulseAlpha = 0.5f + 0.5f * FMath::Sin(TimeSeconds * 2.4f);
	const float PulseScale = 1.0f + FMath::Max(0.0f, TargetRingPulseAmount) * PulseAlpha;
	FLinearColor AnimatedRingColor = TargetRingColor;
	const float Brightness = 0.9f + 0.1f * PulseAlpha;
	AnimatedRingColor.R *= Brightness;
	AnimatedRingColor.G *= Brightness;
	AnimatedRingColor.B *= Brightness;
	const float GroundRelativeZ = ResolveTargetRingHeightOffset();
	const float AnimatedEmissiveStrength =
		FMath::Max(0.0f, TargetRingEmissiveStrength) * (0.9f + 0.1f * PulseAlpha);

	for (int32 SegmentIndex = 0; SegmentIndex < TargetRingSegments.Num(); ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = TargetRingSegments[SegmentIndex];
		if (!Segment)
		{
			continue;
		}

		Segment->SetVisibility(true);
		Segment->SetHiddenInGame(false);

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
			TargetRingMaterials[SegmentIndex]->SetScalarParameterValue(TEXT("EmissiveStrength"), AnimatedEmissiveStrength);
		}

		const float AngleDegrees =
			(360.0f * static_cast<float>(SegmentIndex)) / static_cast<float>(TargetRingSegments.Num()) + RotationOffsetDegrees;
		const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		const FVector SegmentLocation(
			FMath::Cos(AngleRadians) * RingRadius * PulseScale,
			FMath::Sin(AngleRadians) * RingRadius * PulseScale,
			GroundRelativeZ);
		const float TangentYawDegrees = AngleDegrees + 90.0f;

		Segment->SetRelativeLocation(SegmentLocation);
		Segment->SetRelativeRotation(FRotator(0.0f, TangentYawDegrees, 0.0f));
		Segment->SetRelativeScale3D(FVector(SegmentLength / 100.0f, SegmentThickness / 100.0f, 1.0f));
	}
}

float AEmbermereEnemyCharacter::ResolveTargetRingHeightOffset() const
{
	const UWorld* World = GetWorld();
	if (!bTraceTargetRingSurface || !World)
	{
		return TargetRingHeightOffset;
	}

	const FVector ActorLocation = GetActorLocation();
	const FVector TraceStart = ActorLocation + FVector(0.0f, 0.0f, 64.0f);
	const FVector TraceEnd = ActorLocation - FVector(0.0f, 0.0f, 640.0f);
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EmbermereTargetRingGround), false, this);
	FHitResult GroundHit;
	if (World->LineTraceSingleByChannel(GroundHit, TraceStart, TraceEnd, ECC_Visibility, QueryParams))
	{
		return GroundHit.ImpactPoint.Z - ActorLocation.Z + GetEffectiveTargetRingSurfaceClearance();
	}

	return TargetRingHeightOffset;
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
	const float ReturnSpeed =
		FMath::Max(0.0f, ReturnHomeSpeedCmPerSecond) *
		(Stats ? Stats->GetMovementSpeedMultiplier() : 1.0f);
	const float StepDistance = FMath::Min(DistanceToHome, ReturnSpeed * DeltaSeconds);
	SetActorLocation(GetActorLocation() + Direction * StepDistance, true);
	bMovedThisFrame = true;
}

float AEmbermereEnemyCharacter::GetEffectiveMoveSpeedCmPerSecond() const
{
	return FMath::Max(0.0f, MoveSpeedCmPerSecond) *
		(Stats ? Stats->GetMovementSpeedMultiplier() : 1.0f);
}

bool AEmbermereEnemyCharacter::IsActivelyEngagedWith(const AActor* Candidate) const
{
	return Candidate && bPrototypeAiEnabled && Stats && !Stats->IsDead() &&
		!IsHidden() && AggroTarget.Get() == Candidate;
}

bool AEmbermereEnemyCharacter::HasCompleteVisualAnimationSet() const
{
	return !IdleAnimation.IsNull() &&
		!WalkAnimation.IsNull() &&
		!RunAnimation.IsNull() &&
		!AttackAnimation.IsNull() &&
		!HitAnimation.IsNull() &&
		!DeathAnimation.IsNull();
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
	const float EffectiveMoveSpeed = GetEffectiveMoveSpeedCmPerSecond();
	if (!Target || DeltaSeconds <= 0.0f || EffectiveMoveSpeed <= 0.0f)
	{
		return;
	}

	FVector Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Z = 0.0f;
	if (!Direction.Normalize())
	{
		return;
	}

	const FVector Step = Direction * EffectiveMoveSpeed * DeltaSeconds;
	SetActorLocation(GetActorLocation() + Step, true);
	bMovedThisFrame = true;
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

	PlayVisualAnimation(AttackAnimation, false, true);
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

void AEmbermereEnemyCharacter::UpdateVisualAnimation()
{
	if (!Stats || Stats->IsDead() || IsHidden())
	{
		return;
	}

	const UWorld* World = GetWorld();
	const double CurrentTimeSeconds = World ? World->GetTimeSeconds() : 0.0;
	if (CurrentTimeSeconds < VisualAnimationLockUntilSeconds)
	{
		return;
	}

	VisualAnimationLockUntilSeconds = -1.0;
	if (bMovedThisFrame)
	{
		PlayVisualAnimation(bReturningHome ? RunAnimation : WalkAnimation, true);
		return;
	}

	PlayVisualAnimation(IdleAnimation, true);
}

float AEmbermereEnemyCharacter::PlayVisualAnimation(
	const TSoftObjectPtr<UAnimSequence>& Animation,
	bool bLooping,
	bool bRestartIfAlreadyPlaying)
{
	UAnimSequence* Sequence = Animation.LoadSynchronous();
	USkeletalMeshComponent* CharacterMesh = GetMesh();
	if (!Sequence || !CharacterMesh || !CharacterMesh->GetSkeletalMeshAsset())
	{
		return 0.0f;
	}

	if (!bRestartIfAlreadyPlaying &&
		CurrentVisualAnimation == Sequence &&
		bCurrentVisualAnimationLooping == bLooping)
	{
		return Sequence->GetPlayLength();
	}

	CharacterMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	CharacterMesh->PlayAnimation(Sequence, bLooping);
	CurrentVisualAnimation = Sequence;
	bCurrentVisualAnimationLooping = bLooping;

	const float PlayLength = FMath::Max(0.0f, Sequence->GetPlayLength());
	if (!bLooping)
	{
		const UWorld* World = GetWorld();
		const double CurrentTimeSeconds = World ? World->GetTimeSeconds() : 0.0;
		VisualAnimationLockUntilSeconds = CurrentTimeSeconds + PlayLength;
	}

	return PlayLength;
}

void AEmbermereEnemyCharacter::HideDeadBody()
{
	if (Stats && Stats->IsDead())
	{
		SetActorHiddenInGame(true);
	}
}
