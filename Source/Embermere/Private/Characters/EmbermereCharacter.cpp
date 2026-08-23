#include "Characters/EmbermereCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTargetingComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/InputComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Data/EmbermereRulesData.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "Game/EmbermerePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
constexpr int32 StatusEffectVfxSegmentCount = 8;
constexpr float StatusEffectVfxBeneficialRadius = 54.0f;
constexpr float StatusEffectVfxHarmfulRadius = 66.0f;
constexpr float StatusEffectVfxHarmfulFootprintScale = 0.70f;
constexpr float StatusEffectVfxHarmfulMaximumRadius = 128.0f;
constexpr float StatusEffectVfxSurfaceClearance = 21.0f;
constexpr float StatusEffectVfxSegmentThickness = 3.5f;
}

AEmbermereCharacter::AEmbermereCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeedCmPerSecond;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 450.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	Stats = CreateDefaultSubobject<UEmbermereStatsComponent>(TEXT("Stats"));
	Combat = CreateDefaultSubobject<UEmbermereCombatComponent>(TEXT("Combat"));
	Targeting = CreateDefaultSubobject<UEmbermereTargetingComponent>(TEXT("Targeting"));
	Hotbar = CreateDefaultSubobject<UEmbermereHotbarComponent>(TEXT("Hotbar"));
	Inventory = CreateDefaultSubobject<UEmbermereInventoryComponent>(TEXT("Inventory"));
	Equipment = CreateDefaultSubobject<UEmbermereEquipmentComponent>(TEXT("Equipment"));
	QuestLog = CreateDefaultSubobject<UEmbermereQuestLogComponent>(TEXT("QuestLog"));
	Wallet = CreateDefaultSubobject<UEmbermereWalletComponent>(TEXT("Wallet"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> StatusSegmentMeshFinder(
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> StatusMaterialFinder(
		TEXT("/Game/Art/Embermere/Targeting/M_EmbermereTargetRing.M_EmbermereTargetRing"));
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> FallbackStatusMaterialFinder(
		TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	for (int32 SegmentIndex = 0; SegmentIndex < StatusEffectVfxSegmentCount; ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = CreateDefaultSubobject<UStaticMeshComponent>(
			*FString::Printf(TEXT("StatusEffectVfxSegment_%02d"), SegmentIndex));
		Segment->SetupAttachment(RootComponent);
		if (StatusSegmentMeshFinder.Succeeded())
		{
			Segment->SetStaticMesh(StatusSegmentMeshFinder.Object);
		}
		if (StatusMaterialFinder.Succeeded())
		{
			Segment->SetMaterial(0, StatusMaterialFinder.Object);
		}
		else if (FallbackStatusMaterialFinder.Succeeded())
		{
			Segment->SetMaterial(0, FallbackStatusMaterialFinder.Object);
		}
		Segment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Segment->SetCanEverAffectNavigation(false);
		Segment->SetCastShadow(false);
		Segment->SetVisibility(false);
		Segment->SetHiddenInGame(true);
		StatusEffectVfxSegments.Add(Segment);
	}

	CharacterName = FText::FromString(TEXT("Embermere Adventurer"));
}

void AEmbermereCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (Equipment)
	{
		Equipment->OnEquipmentChanged.AddUniqueDynamic(this, &AEmbermereCharacter::HandleEquipmentChanged);
	}
	RefreshEquipmentStats();
	PrimeStarterHotbar();
}

void AEmbermereCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Stats && GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed =
			BaseWalkSpeedCmPerSecond * Stats->GetMovementSpeedMultiplier();
	}
	UpdateStatusEffectVfx(DeltaSeconds);
}

void AEmbermereCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AEmbermereCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEmbermereCharacter::MoveRight);
}

void AEmbermereCharacter::ApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass)
{
	TryApplyRaceAndClass(NewRace, NewClass);
}

bool AEmbermereCharacter::TryApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass)
{
	if (bHasDeliberateCharacterChoice)
	{
		return false;
	}
	return Stats && ApplyValidatedRaceAndClass(
		NewRace,
		NewClass,
		Stats->CurrentExperience);
}

bool AEmbermereCharacter::CanRestoreRaceAndClassForSaveGame(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass) const
{
	int32 ResolvedLevel = 1;
	return Stats && CanRestoreCharacterProgressionForSaveGame(
		NewRace,
		NewClass,
		Stats->CurrentExperience,
		ResolvedLevel);
}

bool AEmbermereCharacter::TryRestoreRaceAndClassForSaveGame(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass)
{
	return Stats && TryRestoreCharacterProgressionForSaveGame(
		NewRace,
		NewClass,
		Stats->CurrentExperience);
}

bool AEmbermereCharacter::CanRestoreCharacterProgressionForSaveGame(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass,
	int32 Experience,
	int32& OutLevel) const
{
	OutLevel = 1;
	if (!ValidateRaceAndClassLoadout(NewRace, NewClass))
	{
		return false;
	}

	const UEmbermereRulesData* EffectiveRules = RulesData.Get()
		? RulesData.Get()
		: GetDefault<UEmbermereRulesData>();
	FEmbermereProgressionProfile Profile;
	return EffectiveRules && Stats &&
		EffectiveRules->GetProgressionProfile(NewRace, NewClass, Profile) &&
		Stats->CanConfigureProgression(Profile, Experience, &OutLevel);
}

bool AEmbermereCharacter::TryRestoreCharacterProgressionForSaveGame(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass,
	int32 Experience)
{
	return ApplyValidatedRaceAndClass(NewRace, NewClass, Experience);
}

bool AEmbermereCharacter::ValidateRaceAndClassLoadout(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass) const
{
	const UEmbermereRulesData* EffectiveRules = RulesData.Get()
		? RulesData.Get()
		: GetDefault<UEmbermereRulesData>();
	FEmbermereProgressionProfile Profile;
	return EffectiveRules && Stats && Hotbar &&
		EffectiveRules->IsCharacterIdentityValid(NewRace, NewClass) &&
		EffectiveRules->GetProgressionProfile(NewRace, NewClass, Profile);
}

bool AEmbermereCharacter::ApplyValidatedRaceAndClass(
	EEmbermereRace NewRace,
	EEmbermereClass NewClass,
	int32 Experience)
{
	if (!ValidateRaceAndClassLoadout(NewRace, NewClass))
	{
		return false;
	}

	const UEmbermereRulesData* EffectiveRules = RulesData.Get()
		? RulesData.Get()
		: GetDefault<UEmbermereRulesData>();
	FEmbermereProgressionProfile Profile;
	if (!EffectiveRules ||
		!EffectiveRules->GetProgressionProfile(NewRace, NewClass, Profile) ||
		!Stats->CanConfigureProgression(Profile, Experience))
	{
		return false;
	}

	if (!Stats->ConfigureProgression(Profile, Experience, true))
	{
		return false;
	}
	Race = NewRace;
	Class = NewClass;
	PrimeStarterHotbar();
	bHasDeliberateCharacterChoice = true;
	return true;
}

void AEmbermereCharacter::MoveForward(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		if (AEmbermerePlayerController* EmbermereController = Cast<AEmbermerePlayerController>(Controller))
		{
			EmbermereController->NotifyManualMoveForwardInput(Value);
		}

		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), Value);
	}
}

void AEmbermereCharacter::MoveRight(float Value)
{
	if (Controller && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
		AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y), Value);
	}
}

void AEmbermereCharacter::RefreshStatusEffectVfx()
{
	UpdateStatusEffectVfx(0.0f);
}

int32 AEmbermereCharacter::GetStatusEffectVfxSegmentCount() const
{
	return StatusEffectVfxSegments.Num();
}

int32 AEmbermereCharacter::GetVisibleStatusEffectVfxSegmentCount() const
{
	return bStatusEffectVfxVisible ? StatusEffectVfxSegments.Num() : 0;
}

FLinearColor AEmbermereCharacter::GetStatusEffectVfxColor() const
{
	return StatusEffectVfxColor;
}

bool AEmbermereCharacter::IsStatusEffectVfxBeneficial() const
{
	return bStatusEffectVfxVisible && bStatusEffectVfxBeneficial;
}

FString AEmbermereCharacter::GetStatusEffectVfxMaterialPath() const
{
	if (StatusEffectVfxSegments.IsEmpty() || !StatusEffectVfxSegments[0])
	{
		return FString();
	}
	const UMaterialInterface* Material = StatusEffectVfxSegments[0]->GetMaterial(0);
	return Material ? Material->GetPathName() : FString();
}

float AEmbermereCharacter::GetStatusEffectVfxRadius() const
{
	return StatusEffectVfxRadius;
}

float AEmbermereCharacter::GetStatusEffectVfxRelativeHeight() const
{
	return StatusEffectVfxRelativeHeight;
}

void AEmbermereCharacter::UpdateStatusEffectVfx(float DeltaSeconds)
{
	const TArray<FEmbermereActiveStatusEffect> Effects = Stats
		? Stats->GetActiveStatusEffects()
		: TArray<FEmbermereActiveStatusEffect>();
	const FEmbermereActiveStatusEffect* PresentedEffect = nullptr;
	for (const FEmbermereActiveStatusEffect& Effect : Effects)
	{
		if (!Effect.bBeneficial)
		{
			PresentedEffect = &Effect;
			break;
		}
		if (!PresentedEffect)
		{
			PresentedEffect = &Effect;
		}
	}

	bStatusEffectVfxVisible = PresentedEffect && Stats && !Stats->IsDead();
	if (!bStatusEffectVfxVisible)
	{
		StatusEffectVfxColor = FLinearColor::Transparent;
		bStatusEffectVfxBeneficial = false;
		for (UStaticMeshComponent* Segment : StatusEffectVfxSegments)
		{
			if (Segment)
			{
				Segment->SetVisibility(false);
				Segment->SetHiddenInGame(true);
			}
		}
		return;
	}

	bStatusEffectVfxBeneficial = PresentedEffect->bBeneficial;
	float RotationSpeedDegreesPerSecond = bStatusEffectVfxBeneficial ? 34.0f : -20.0f;
	const float RingRadius = ResolveStatusEffectVfxRadius(bStatusEffectVfxBeneficial);
	const float GroundRelativeZ = ResolveStatusEffectVfxHeightOffset();
	StatusEffectVfxRadius = RingRadius;
	StatusEffectVfxRelativeHeight = GroundRelativeZ;
	if (!bStatusEffectVfxBeneficial &&
		PresentedEffect->Ability.MovementSpeedMultiplier < 1.0f)
	{
		const bool bRooted =
			PresentedEffect->Ability.MovementSpeedMultiplier <= KINDA_SMALL_NUMBER;
		StatusEffectVfxColor = bRooted
			? FLinearColor(0.46f, 0.92f, 1.0f, 1.0f)
			: FLinearColor(0.28f, 0.78f, 0.24f, 1.0f);
	}
	else if (PresentedEffect->Ability.EffectType == EEmbermereAbilityEffectType::ArmorBuff)
	{
		StatusEffectVfxColor = FLinearColor(0.32f, 0.64f, 1.0f, 1.0f);
	}
	else if (PresentedEffect->Ability.EffectType == EEmbermereAbilityEffectType::AttackPowerBuff)
	{
		StatusEffectVfxColor = FLinearColor(1.0f, 0.34f, 0.06f, 1.0f);
	}
	else
	{
		StatusEffectVfxColor = bStatusEffectVfxBeneficial
			? FLinearColor(1.0f, 0.72f, 0.18f, 1.0f)
			: FLinearColor(0.92f, 0.12f, 0.08f, 1.0f);
	}

	StatusEffectVfxRotationDegrees = FMath::Fmod(
		StatusEffectVfxRotationDegrees + DeltaSeconds * RotationSpeedDegreesPerSecond,
		360.0f);
	const UWorld* World = GetWorld();
	const float TimeSeconds = World
		? World->GetTimeSeconds()
		: FMath::Abs(StatusEffectVfxRotationDegrees) / FMath::Max(1.0f, FMath::Abs(RotationSpeedDegreesPerSecond));
	const float PulseAlpha = 0.5f + 0.5f * FMath::Sin(TimeSeconds * 3.2f);
	const float PulseScale = 1.0f + 0.07f * PulseAlpha;
	const float Brightness = 0.78f + 0.22f * PulseAlpha;
	FLinearColor AnimatedColor = StatusEffectVfxColor;
	AnimatedColor.R *= Brightness;
	AnimatedColor.G *= Brightness;
	AnimatedColor.B *= Brightness;
	const float SegmentLength =
		2.0f * RingRadius * FMath::Tan(PI / static_cast<float>(StatusEffectVfxSegmentCount)) * 0.56f;

	if (StatusEffectVfxMaterials.Num() != StatusEffectVfxSegments.Num())
	{
		StatusEffectVfxMaterials.SetNum(StatusEffectVfxSegments.Num());
	}
	for (int32 SegmentIndex = 0; SegmentIndex < StatusEffectVfxSegments.Num(); ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = StatusEffectVfxSegments[SegmentIndex];
		if (!Segment)
		{
			continue;
		}
		Segment->SetVisibility(true);
		Segment->SetHiddenInGame(false);
		if (!StatusEffectVfxMaterials[SegmentIndex])
		{
			StatusEffectVfxMaterials[SegmentIndex] = Segment->CreateDynamicMaterialInstance(0);
		}
		if (StatusEffectVfxMaterials[SegmentIndex])
		{
			StatusEffectVfxMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("Color"), AnimatedColor);
			StatusEffectVfxMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("BaseColor"), AnimatedColor);
		}

		const float AngleDegrees =
			(360.0f * static_cast<float>(SegmentIndex)) / static_cast<float>(StatusEffectVfxSegments.Num())
			+ StatusEffectVfxRotationDegrees;
		const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		Segment->SetRelativeLocation(FVector(
			FMath::Cos(AngleRadians) * RingRadius * PulseScale,
			FMath::Sin(AngleRadians) * RingRadius * PulseScale,
			GroundRelativeZ));
		Segment->SetRelativeRotation(FRotator(0.0f, AngleDegrees + 90.0f, 0.0f));
		Segment->SetRelativeScale3D(FVector(
			SegmentLength / 100.0f,
			StatusEffectVfxSegmentThickness / 100.0f,
			1.0f));
	}
}

float AEmbermereCharacter::ResolveStatusEffectVfxRadius(bool bBeneficial) const
{
	if (bBeneficial)
	{
		return StatusEffectVfxBeneficialRadius;
	}

	float ResolvedRadius = StatusEffectVfxHarmfulRadius;
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
		ResolvedRadius = FMath::Max(
			ResolvedRadius,
			HorizontalExtent * StatusEffectVfxHarmfulFootprintScale);
	}

	return FMath::Min(ResolvedRadius, StatusEffectVfxHarmfulMaximumRadius);
}

float AEmbermereCharacter::ResolveStatusEffectVfxHeightOffset() const
{
	const FVector ActorLocation = GetActorLocation();
	const UWorld* World = GetWorld();
	if (World)
	{
		const FVector TraceStart = ActorLocation + FVector(0.0f, 0.0f, 64.0f);
		const FVector TraceEnd = ActorLocation - FVector(0.0f, 0.0f, 640.0f);
		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(EmbermereStatusEffectGround), false, this);
		FHitResult GroundHit;
		if (World->LineTraceSingleByChannel(
			GroundHit,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			QueryParams))
		{
			return GroundHit.ImpactPoint.Z - ActorLocation.Z + StatusEffectVfxSurfaceClearance;
		}
	}

	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float CapsuleHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 0.0f;
	return -CapsuleHalfHeight + StatusEffectVfxSurfaceClearance;
}

bool AEmbermereCharacter::IsAlive_Implementation() const
{
	return Stats && !Stats->IsDead();
}

bool AEmbermereCharacter::IsHostileTo_Implementation(const AActor* Viewer) const
{
	return false;
}

FText AEmbermereCharacter::GetTargetDisplayName_Implementation() const
{
	return CharacterName;
}

FVector AEmbermereCharacter::GetCombatFeedbackAnchorLocation_Implementation() const
{
	if (const USkeletalMeshComponent* CharacterMesh = GetMesh())
	{
		if (CharacterMesh->GetSkeletalMeshAsset() && CharacterMesh->IsVisible() && !CharacterMesh->bHiddenInGame)
		{
			return CharacterMesh->Bounds.Origin + FVector(0.0f, 0.0f, CharacterMesh->Bounds.BoxExtent.Z + 30.0f);
		}
	}

	const UCapsuleComponent* Capsule = GetCapsuleComponent();
	const float CapsuleHalfHeight = Capsule ? Capsule->GetScaledCapsuleHalfHeight() : 70.0f;
	return GetActorLocation() + FVector(0.0f, 0.0f, CapsuleHalfHeight + 30.0f);
}

bool AEmbermereCharacter::ShouldGrantDefeatCredit_Implementation() const
{
	return false;
}

void AEmbermereCharacter::SetTargetedByPlayer_Implementation(bool bIsTargeted)
{
	HandleTargetedByPlayer(bIsTargeted);
}

void AEmbermereCharacter::HandleTargetedByPlayer(bool bIsTargeted)
{
}

void AEmbermereCharacter::HandleEquipmentChanged()
{
	RefreshEquipmentStats();
}

void AEmbermereCharacter::RefreshEquipmentStats()
{
	if (Stats && Equipment)
	{
		Stats->ApplyEquipmentBonuses(Equipment->GetTotalStatBonuses());
	}
}

void AEmbermereCharacter::PrimeStarterHotbar()
{
	if (!Hotbar)
	{
		return;
	}

	UEmbermereRulesData* EffectiveRules = RulesData.Get() ? RulesData.Get() : NewObject<UEmbermereRulesData>(this);
	if (!EffectiveRules)
	{
		return;
	}

	FEmbermereClassDefinition ClassDefinition;
	if (!EffectiveRules->GetClassDefinition(Class, ClassDefinition))
	{
		return;
	}

	for (int32 Index = 0; Index < Hotbar->Slots.Num(); ++Index)
	{
		Hotbar->SetAbilityInSlot(Index, FEmbermereAbilityDefinition());
	}

	for (int32 Index = 0; Index < ClassDefinition.StarterAbilityIds.Num() && Index < 4; ++Index)
	{
		FEmbermereAbilityDefinition AbilityDefinition;
		if (EffectiveRules->GetAbilityDefinition(ClassDefinition.StarterAbilityIds[Index], AbilityDefinition))
		{
			Hotbar->SetAbilityInSlot(Index, AbilityDefinition);
		}
	}
}
