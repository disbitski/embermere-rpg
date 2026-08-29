#include "Characters/EmbermereRestPresentationActor.h"

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereRestServiceActor.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"

AEmbermereRestPresentationActor::AEmbermereRestPresentationActor()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);
}

void AEmbermereRestPresentationActor::BeginPlay()
{
	Super::BeginPlay();
	BindToRestService();
}

void AEmbermereRestPresentationActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromRestService();
	Super::EndPlay(EndPlayReason);
}

void AEmbermereRestPresentationActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	AdvanceRestPresentation(DeltaSeconds);
}

void AEmbermereRestPresentationActor::SetObservedRestService(
	AEmbermereRestServiceActor* NewRestService)
{
	if (ObservedRestService == NewRestService)
	{
		return;
	}

	UnbindFromRestService();
	ObservedRestService = NewRestService;
	BindToRestService();
}

void AEmbermereRestPresentationActor::BindToRestService()
{
	if (BoundRestService.Get() == ObservedRestService && BoundRestService.IsValid())
	{
		return;
	}

	UnbindFromRestService();
	if (!ObservedRestService || !ObservedRestService->RestService)
	{
		return;
	}

	RestOutcomeHandle = ObservedRestService->RestService->OnRestOutcomeNative.AddUObject(
		this,
		&AEmbermereRestPresentationActor::HandleRestOutcome);
	BoundRestService = ObservedRestService;
}

void AEmbermereRestPresentationActor::UnbindFromRestService()
{
	if (AEmbermereRestServiceActor* BoundService = BoundRestService.Get())
	{
		if (BoundService->RestService)
		{
			if (RestOutcomeHandle.IsValid())
			{
				BoundService->RestService->OnRestOutcomeNative.Remove(RestOutcomeHandle);
			}
		}
	}
	RestOutcomeHandle.Reset();
	BoundRestService.Reset();
	ClearRestPresentation();
}

void AEmbermereRestPresentationActor::AdvanceRestPresentation(float DeltaSeconds)
{
	if (PresentationPhase == EEmbermereRestPresentationPhase::Hidden)
	{
		return;
	}

	AEmbermereCharacter* Character = ActiveCharacter.Get();
	if (!Character || !Character->Stats || Character->Stats->IsDead())
	{
		ClearRestPresentation();
		return;
	}

	if (PresentationPhase == EEmbermereRestPresentationPhase::Channeling)
	{
		AEmbermereRestServiceActor* BoundService = BoundRestService.Get();
		if (!BoundService || !BoundService->RestService ||
			!BoundService->RestService->IsRestPending())
		{
			ClearRestPresentation();
			return;
		}
	}

	const float SafeDeltaSeconds = FMath::IsFinite(DeltaSeconds)
		? FMath::Max(0.0f, DeltaSeconds)
		: 0.0f;
	PresentationAgeSeconds += SafeDeltaSeconds;
	if (PresentationPhase == EEmbermereRestPresentationPhase::Completion &&
		PresentationAgeSeconds >= CompletionLifetimeSeconds)
	{
		ClearRestPresentation();
		return;
	}

	UpdateSegmentPresentation(SafeDeltaSeconds);
}

void AEmbermereRestPresentationActor::ClearRestPresentation()
{
	PresentationPhase = EEmbermereRestPresentationPhase::Hidden;
	PresentationAgeSeconds = 0.0f;
	PresentationRotationDegrees = 0.0f;
	PresentationRadius = 0.0f;
	PresentationColor = FLinearColor::Transparent;
	ActiveCharacter.Reset();
	SetActorTickEnabled(false);

	for (UStaticMeshComponent* Segment : PresentationSegments)
	{
		if (Segment)
		{
			Segment->SetVisibility(false);
			Segment->SetHiddenInGame(true);
		}
	}
}

EEmbermereRestPresentationPhase AEmbermereRestPresentationActor::GetPresentationPhase() const
{
	return PresentationPhase;
}

bool AEmbermereRestPresentationActor::IsRestPresentationVisible() const
{
	return PresentationPhase != EEmbermereRestPresentationPhase::Hidden;
}

bool AEmbermereRestPresentationActor::IsBoundToRestService() const
{
	return BoundRestService.IsValid();
}

int32 AEmbermereRestPresentationActor::GetRestPresentationSegmentCount() const
{
	return PresentationSegments.Num();
}

int32 AEmbermereRestPresentationActor::GetVisibleRestPresentationSegmentCount() const
{
	return IsRestPresentationVisible() ? PresentationSegments.Num() : 0;
}

bool AEmbermereRestPresentationActor::AreRestPresentationSegmentsNonColliding() const
{
	if (PresentationSegments.IsEmpty())
	{
		return false;
	}

	for (const UStaticMeshComponent* Segment : PresentationSegments)
	{
		if (!Segment ||
			Segment->GetCollisionEnabled() != ECollisionEnabled::NoCollision ||
			Segment->GetGenerateOverlapEvents() ||
			Segment->CanEverAffectNavigation() ||
			Segment->CastShadow)
		{
			return false;
		}
	}
	return true;
}

float AEmbermereRestPresentationActor::GetRestPresentationRadius() const
{
	return PresentationRadius;
}

float AEmbermereRestPresentationActor::GetCompletionLifetimeSeconds() const
{
	return CompletionLifetimeSeconds;
}

FLinearColor AEmbermereRestPresentationActor::GetRestPresentationColor() const
{
	return PresentationColor;
}

FString AEmbermereRestPresentationActor::GetRestPresentationMaterialPath() const
{
	return PresentationMaterialPath;
}

void AEmbermereRestPresentationActor::EnsurePresentationComponents()
{
	if (PresentationSegments.Num() == SegmentCount)
	{
		return;
	}

	for (UStaticMeshComponent* Segment : PresentationSegments)
	{
		if (Segment)
		{
			Segment->DestroyComponent();
		}
	}
	PresentationSegments.Reset();
	PresentationMaterials.Reset();

	UStaticMesh* SegmentMesh = LoadObject<UStaticMesh>(
		nullptr,
		TEXT("/Engine/BasicShapes/Plane.Plane"));
	UMaterialInterface* SegmentMaterial = LoadObject<UMaterialInterface>(
		nullptr,
		TEXT("/Game/Art/Embermere/Targeting/M_EmbermereTargetRing.M_EmbermereTargetRing"));
	if (!SegmentMaterial)
	{
		SegmentMaterial = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
	if (!SegmentMesh || !SegmentMaterial || !RootComponent)
	{
		return;
	}
	PresentationMaterialPath = SegmentMaterial->GetPathName();

	for (int32 SegmentIndex = 0; SegmentIndex < SegmentCount; ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = NewObject<UStaticMeshComponent>(
			this,
			*FString::Printf(TEXT("RestPresentationSegment_%02d"), SegmentIndex),
			RF_Transient);
		if (!Segment)
		{
			continue;
		}

		Segment->SetStaticMesh(SegmentMesh);
		Segment->SetMaterial(0, SegmentMaterial);
		Segment->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Segment->SetGenerateOverlapEvents(false);
		Segment->SetCanEverAffectNavigation(false);
		Segment->SetCastShadow(false);
		Segment->SetMobility(EComponentMobility::Movable);
		Segment->SetTranslucentSortPriority(5);
		Segment->SetVisibility(false);
		Segment->SetHiddenInGame(true);
		Segment->SetupAttachment(RootComponent);
		AddInstanceComponent(Segment);
		if (GetWorld())
		{
			Segment->RegisterComponent();
		}
		PresentationSegments.Add(Segment);
	}
	PresentationMaterials.SetNum(PresentationSegments.Num());
}

void AEmbermereRestPresentationActor::StartChannelPresentation(
	AEmbermereCharacter* Character)
{
	EnsurePresentationComponents();
	if (PresentationSegments.Num() != SegmentCount || !Character)
	{
		ClearRestPresentation();
		return;
	}

	ActiveCharacter = Character;
	PresentationPhase = EEmbermereRestPresentationPhase::Channeling;
	PresentationAgeSeconds = 0.0f;
	PresentationRotationDegrees = 0.0f;
	PresentationRadius = ChannelRadius;
	PresentationColor = FLinearColor(0.2f, 0.78f, 1.0f, 1.0f);
	SetActorTickEnabled(true);
	UpdateSegmentPresentation(0.0f);
}

void AEmbermereRestPresentationActor::StartCompletionPresentation(
	AEmbermereCharacter* Character)
{
	EnsurePresentationComponents();
	if (PresentationSegments.Num() != SegmentCount || !Character)
	{
		ClearRestPresentation();
		return;
	}

	ActiveCharacter = Character;
	PresentationPhase = EEmbermereRestPresentationPhase::Completion;
	PresentationAgeSeconds = 0.0f;
	PresentationRadius = CompletionStartRadius;
	PresentationColor = FLinearColor(0.36f, 1.0f, 0.68f, 1.0f);
	SetActorTickEnabled(true);
	UpdateSegmentPresentation(0.0f);
}

void AEmbermereRestPresentationActor::UpdateSegmentPresentation(float DeltaSeconds)
{
	EnsurePresentationComponents();
	if (PresentationSegments.Num() != SegmentCount)
	{
		ClearRestPresentation();
		return;
	}

	PresentationRotationDegrees = FMath::Fmod(
		PresentationRotationDegrees + DeltaSeconds * RotationSpeedDegreesPerSecond,
		360.0f);

	float VisibilityEnvelope = 1.0f;
	float SegmentThickness = 7.0f;
	if (PresentationPhase == EEmbermereRestPresentationPhase::Channeling)
	{
		PresentationRadius = ChannelRadius + 2.5f * FMath::Sin(PresentationAgeSeconds * 4.0f);
		VisibilityEnvelope = 0.72f + 0.2f * FMath::Sin(PresentationAgeSeconds * 5.0f);
	}
	else
	{
		const float NormalizedAge = FMath::Clamp(
			PresentationAgeSeconds / CompletionLifetimeSeconds,
			0.0f,
			1.0f);
		const float ExpansionAlpha = FMath::InterpEaseOut(0.0f, 1.0f, NormalizedAge, 2.0f);
		PresentationRadius = FMath::Lerp(
			CompletionStartRadius,
			CompletionEndRadius,
			ExpansionAlpha);
		VisibilityEnvelope = FMath::Sin(NormalizedAge * PI);
		SegmentThickness = FMath::Lerp(7.0f, 10.0f, ExpansionAlpha);
	}

	FLinearColor AnimatedColor = PresentationColor;
	const float Brightness = 0.35f + 0.95f * VisibilityEnvelope;
	AnimatedColor.R *= Brightness;
	AnimatedColor.G *= Brightness;
	AnimatedColor.B *= Brightness;
	const float EmissiveStrength = 0.8f + 1.6f * VisibilityEnvelope;
	const float SegmentLength =
		2.0f * PresentationRadius * FMath::Tan(PI / static_cast<float>(SegmentCount)) * 0.68f;

	for (int32 SegmentIndex = 0; SegmentIndex < PresentationSegments.Num(); ++SegmentIndex)
	{
		UStaticMeshComponent* Segment = PresentationSegments[SegmentIndex];
		if (!Segment)
		{
			continue;
		}

		Segment->SetVisibility(true);
		Segment->SetHiddenInGame(false);
		if (!PresentationMaterials[SegmentIndex])
		{
			PresentationMaterials[SegmentIndex] = Segment->CreateDynamicMaterialInstance(0);
		}
		if (PresentationMaterials[SegmentIndex])
		{
			PresentationMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("Color"), AnimatedColor);
			PresentationMaterials[SegmentIndex]->SetVectorParameterValue(TEXT("BaseColor"), AnimatedColor);
			PresentationMaterials[SegmentIndex]->SetScalarParameterValue(
				TEXT("EmissiveStrength"),
				EmissiveStrength);
		}

		const float AngleDegrees =
			(360.0f * static_cast<float>(SegmentIndex)) /
				static_cast<float>(PresentationSegments.Num()) +
			PresentationRotationDegrees;
		const float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
		Segment->SetRelativeLocation(FVector(
			FMath::Cos(AngleRadians) * PresentationRadius,
			FMath::Sin(AngleRadians) * PresentationRadius,
			PresentationHeight));
		Segment->SetRelativeRotation(FRotator(0.0f, AngleDegrees + 90.0f, 0.0f));
		Segment->SetRelativeScale3D(FVector(
			SegmentLength / 100.0f,
			SegmentThickness / 100.0f,
			1.0f));
	}
}

void AEmbermereRestPresentationActor::HandleRestOutcome(const FEmbermereRestOutcome& Outcome)
{
#if WITH_DEV_AUTOMATION_TESTS
	++ObservedOutcomeCount;
#endif
	switch (Outcome.Result)
	{
	case EEmbermereRestResult::Started:
		StartChannelPresentation(Outcome.Character);
		break;
	case EEmbermereRestResult::Success:
		StartCompletionPresentation(Outcome.Character);
		break;
	case EEmbermereRestResult::AlreadyResting:
		break;
	case EEmbermereRestResult::Dead:
	case EEmbermereRestResult::InCombat:
	case EEmbermereRestResult::Interrupted:
		ClearRestPresentation();
		break;
	default:
		if (PresentationPhase == EEmbermereRestPresentationPhase::Channeling)
		{
			ClearRestPresentation();
		}
		break;
	}
}
