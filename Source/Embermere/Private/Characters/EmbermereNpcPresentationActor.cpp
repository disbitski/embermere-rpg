#include "Characters/EmbermereNpcPresentationActor.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimationAsset.h"
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
	UClass* ResolvedAnimationClass = AnimationClass.IsNull()
		? nullptr
		: AnimationClass.LoadSynchronous();
	UAnimationAsset* ResolvedIdleAnimation = IdleAnimation.IsNull()
		? nullptr
		: IdleAnimation.LoadSynchronous();

	const bool bUseSkeletalVisual = ResolvedSkeletalMesh &&
		(bPreferSkeletalVisual || !ResolvedStaticMesh);
	const bool bUseStaticVisual = ResolvedStaticMesh && !bUseSkeletalVisual;

	StaticVisual->SetRelativeTransform(VisualRelativeTransform);
	StaticVisual->SetStaticMesh(bUseStaticVisual ? ResolvedStaticMesh : nullptr);
	StaticVisual->SetVisibility(bUseStaticVisual, true);
	StaticVisual->SetHiddenInGame(!bUseStaticVisual, true);

	SkeletalVisual->SetRelativeTransform(VisualRelativeTransform);
	SkeletalVisual->SetSkeletalMeshAsset(bUseSkeletalVisual ? ResolvedSkeletalMesh : nullptr);

	const bool bUseAnimationBlueprint = bUseSkeletalVisual && ResolvedAnimationClass;
	const bool bIdleAnimationCompatible = bUseSkeletalVisual &&
		ResolvedIdleAnimation &&
		ResolvedIdleAnimation->GetSkeleton() &&
		ResolvedSkeletalMesh->GetSkeleton() == ResolvedIdleAnimation->GetSkeleton();
	if (bUseAnimationBlueprint)
	{
		SkeletalVisual->OverrideAnimationData(nullptr, false, false, 0.0f, 1.0f);
		SkeletalVisual->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		SkeletalVisual->SetAnimInstanceClass(ResolvedAnimationClass);
	}
	else if (bIdleAnimationCompatible)
	{
		const float ResolvedPlayRate = FMath::Max(IdleAnimationPlayRate, 0.01f);
		SkeletalVisual->SetAnimInstanceClass(nullptr);
		SkeletalVisual->OverrideAnimationData(
			ResolvedIdleAnimation,
			bLoopIdleAnimation,
			true,
			0.0f,
			ResolvedPlayRate);

		// Clearing an Anim Blueprint class destroys the runtime instance without
		// changing SingleNode mode. Reinitialize registered components explicitly
		// so a live art swap consumes the same serialized data as construction.
		if (SkeletalVisual->IsRegistered())
		{
			SkeletalVisual->InitAnim(true);
		}
	}
	else
	{
		SkeletalVisual->SetAnimInstanceClass(nullptr);
		SkeletalVisual->OverrideAnimationData(nullptr, false, false, 0.0f, 1.0f);
	}
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

EEmbermereNpcAnimationMode AEmbermereNpcPresentationActor::GetResolvedAnimationMode() const
{
	if (GetResolvedVisualMode() != EEmbermereNpcVisualMode::SkeletalMesh || !SkeletalVisual)
	{
		return EEmbermereNpcAnimationMode::None;
	}

	if (SkeletalVisual->GetAnimationMode() == EAnimationMode::AnimationBlueprint && AnimationClass.Get())
	{
		return EEmbermereNpcAnimationMode::AnimationBlueprint;
	}

	if (SkeletalVisual->GetAnimationMode() == EAnimationMode::AnimationSingleNode &&
		SkeletalVisual->AnimationData.AnimToPlay)
	{
		return EEmbermereNpcAnimationMode::SingleNodeIdle;
	}

	return EEmbermereNpcAnimationMode::None;
}

bool AEmbermereNpcPresentationActor::IsPresentationCollisionDisabled() const
{
	return StaticVisual && SkeletalVisual &&
		StaticVisual->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
		SkeletalVisual->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
		!StaticVisual->GetGenerateOverlapEvents() &&
		!SkeletalVisual->GetGenerateOverlapEvents();
}
