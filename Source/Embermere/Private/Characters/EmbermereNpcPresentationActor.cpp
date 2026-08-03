#include "Characters/EmbermereNpcPresentationActor.h"

#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"

namespace
{
	void ConfigurePresentationComponent(UPrimitiveComponent* Component)
	{
		if (!Component)
		{
			return;
		}

		Component->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Component->SetGenerateOverlapEvents(false);
		Component->CanCharacterStepUpOn = ECB_No;
		Component->SetCanEverAffectNavigation(false);
	}
}

AEmbermereNpcPresentationActor::AEmbermereNpcPresentationActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	StaticVisual = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticVisual"));
	StaticVisual->SetupAttachment(SceneRoot);
	ConfigurePresentationComponent(StaticVisual);

	SkeletalVisual = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalVisual"));
	SkeletalVisual->SetupAttachment(SceneRoot);
	ConfigurePresentationComponent(SkeletalVisual);
	SkeletalVisual->SetVisibility(false, true);
	SkeletalVisual->SetHiddenInGame(true, true);
}

void AEmbermereNpcPresentationActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshPresentation();
}

void AEmbermereNpcPresentationActor::RefreshPresentation()
{
	ConfigurePresentationComponent(StaticVisual);
	ConfigurePresentationComponent(SkeletalVisual);

	UStaticMesh* ResolvedStaticMesh = StaticVisualMesh.IsNull()
		? nullptr
		: StaticVisualMesh.LoadSynchronous();
	USkeletalMesh* ResolvedSkeletalMesh = SkeletalVisualMesh.IsNull()
		? nullptr
		: SkeletalVisualMesh.LoadSynchronous();

	const bool bUseSkeletalVisual = ResolvedSkeletalMesh &&
		(bPreferSkeletalVisual || !ResolvedStaticMesh);
	const bool bUseStaticVisual = ResolvedStaticMesh && !bUseSkeletalVisual;

	StaticVisual->SetRelativeTransform(VisualRelativeTransform);
	StaticVisual->SetStaticMesh(bUseStaticVisual ? ResolvedStaticMesh : nullptr);
	StaticVisual->SetVisibility(bUseStaticVisual, true);
	StaticVisual->SetHiddenInGame(!bUseStaticVisual, true);

	SkeletalVisual->SetRelativeTransform(VisualRelativeTransform);
	SkeletalVisual->SetSkeletalMeshAsset(bUseSkeletalVisual ? ResolvedSkeletalMesh : nullptr);
	SkeletalVisual->SetAnimInstanceClass(
		bUseSkeletalVisual && !AnimationClass.IsNull()
			? AnimationClass.LoadSynchronous()
			: nullptr);
	SkeletalVisual->SetVisibility(bUseSkeletalVisual, true);
	SkeletalVisual->SetHiddenInGame(!bUseSkeletalVisual, true);
}

EEmbermereNpcVisualMode AEmbermereNpcPresentationActor::GetResolvedVisualMode() const
{
	if (SkeletalVisual && SkeletalVisual->GetSkeletalMeshAsset() && SkeletalVisual->IsVisible())
	{
		return EEmbermereNpcVisualMode::SkeletalMesh;
	}

	if (StaticVisual && StaticVisual->GetStaticMesh() && StaticVisual->IsVisible())
	{
		return EEmbermereNpcVisualMode::StaticMesh;
	}

	return EEmbermereNpcVisualMode::None;
}

bool AEmbermereNpcPresentationActor::IsPresentationCollisionDisabled() const
{
	return StaticVisual && SkeletalVisual &&
		StaticVisual->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
		SkeletalVisual->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
		!StaticVisual->GetGenerateOverlapEvents() &&
		!SkeletalVisual->GetGenerateOverlapEvents();
}
