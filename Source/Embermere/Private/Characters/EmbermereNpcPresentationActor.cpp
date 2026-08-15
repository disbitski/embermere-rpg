#include "Characters/EmbermereNpcPresentationActor.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimationAsset.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "UI/EmbermereNpcGreetingWidget.h"

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
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickInterval = 0.1f;

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

	ContextGreetingWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ContextGreetingWidget"));
	ContextGreetingWidget->SetupAttachment(SceneRoot);
	ContextGreetingWidget->SetRelativeLocation(ContextGreetingRelativeLocation);
	ContextGreetingWidget->SetWidgetClass(UEmbermereNpcGreetingWidget::StaticClass());
	ContextGreetingWidget->SetWidgetSpace(EWidgetSpace::Screen);
	ContextGreetingWidget->SetDrawSize(FVector2D(320.0f, 56.0f));
	ContextGreetingWidget->SetPivot(FVector2D(0.5f, 0.5f));
	ContextGreetingWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ContextGreetingWidget->SetGenerateOverlapEvents(false);
	ContextGreetingWidget->SetCanEverAffectNavigation(false);
	ContextGreetingWidget->SetVisibility(false);
	ContextGreetingWidget->SetHiddenInGame(true);
}

void AEmbermereNpcPresentationActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	RefreshPresentation();
	if (ContextGreetingWidget)
	{
		ContextGreetingWidget->SetRelativeLocation(ContextGreetingRelativeLocation);
		ContextGreetingWidget->SetDrawSize(FVector2D(320.0f, 56.0f));
		ContextGreetingWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ContextGreetingWidget->SetGenerateOverlapEvents(false);
		ContextGreetingWidget->SetCanEverAffectNavigation(false);
	}
}

void AEmbermereNpcPresentationActor::BeginPlay()
{
	Super::BeginPlay();
	SetActorTickEnabled(bEnableContextGreeting);
	EnsureQuestLogBinding();
	RefreshContextGreetingPresentation();
}

void AEmbermereNpcPresentationActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (BoundQuestLog)
	{
		BoundQuestLog->OnQuestStateChanged.RemoveDynamic(
			this,
			&AEmbermereNpcPresentationActor::HandleQuestStateChanged);
		BoundQuestLog = nullptr;
	}
	Super::EndPlay(EndPlayReason);
}

void AEmbermereNpcPresentationActor::Tick(const float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	EnsureQuestLogBinding();
	RefreshContextGreetingPresentation();
}

EEmbermereNpcGreetingState AEmbermereNpcPresentationActor::ResolveContextGreetingState(
	const UEmbermereQuestData* OfferedQuest,
	const FEmbermereQuestState& QuestState)
{
	if (!OfferedQuest)
	{
		return EEmbermereNpcGreetingState::Hidden;
	}

	if (!QuestState.Quest)
	{
		return EEmbermereNpcGreetingState::Available;
	}

	if (QuestState.Quest != OfferedQuest)
	{
		return EEmbermereNpcGreetingState::Hidden;
	}

	if (QuestState.bCompleted)
	{
		return EEmbermereNpcGreetingState::Completed;
	}

	if (QuestState.CurrentObjectiveCount >= OfferedQuest->RequiredObjectiveCount)
	{
		return EEmbermereNpcGreetingState::ReadyToTurnIn;
	}

	return EEmbermereNpcGreetingState::Active;
}

FText AEmbermereNpcPresentationActor::ResolveContextGreetingText(
	const UEmbermereQuestData* OfferedQuest,
	const EEmbermereNpcGreetingState State)
{
	if (!OfferedQuest)
	{
		return FText::GetEmpty();
	}

	switch (State)
	{
	case EEmbermereNpcGreetingState::Available:
		return OfferedQuest->AvailableGreeting;
	case EEmbermereNpcGreetingState::Active:
		return OfferedQuest->ActiveGreeting;
	case EEmbermereNpcGreetingState::ReadyToTurnIn:
		return OfferedQuest->ReadyGreeting;
	case EEmbermereNpcGreetingState::Completed:
		return OfferedQuest->CompletedGreeting;
	default:
		return FText::GetEmpty();
	}
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

EEmbermereNpcGreetingState AEmbermereNpcPresentationActor::GetResolvedContextGreetingState() const
{
	const UEmbermereInteractableComponent* Authority = ContextAuthorityActor
		? ContextAuthorityActor->FindComponentByClass<UEmbermereInteractableComponent>()
		: nullptr;
	const FEmbermereQuestState EmptyQuestState;
	return ResolveContextGreetingState(
		Authority ? Authority->QuestToOffer.Get() : nullptr,
		BoundQuestLog ? BoundQuestLog->ActiveQuest : EmptyQuestState);
}

FText AEmbermereNpcPresentationActor::GetResolvedContextGreetingText() const
{
	const UEmbermereInteractableComponent* Authority = ContextAuthorityActor
		? ContextAuthorityActor->FindComponentByClass<UEmbermereInteractableComponent>()
		: nullptr;
	return ResolveContextGreetingText(
		Authority ? Authority->QuestToOffer.Get() : nullptr,
		GetResolvedContextGreetingState());
}

bool AEmbermereNpcPresentationActor::IsContextGreetingPresentationOnly() const
{
	return ContextGreetingWidget &&
		ContextGreetingWidget->GetCollisionEnabled() == ECollisionEnabled::NoCollision &&
		!ContextGreetingWidget->GetGenerateOverlapEvents() &&
		!ContextGreetingWidget->CanEverAffectNavigation();
}

void AEmbermereNpcPresentationActor::HandleQuestStateChanged(const FEmbermereQuestState& QuestState)
{
	RefreshContextGreetingPresentation();
}

void AEmbermereNpcPresentationActor::EnsureQuestLogBinding()
{
	if (BoundQuestLog || !bEnableContextGreeting || !GetWorld())
	{
		return;
	}

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	UEmbermereQuestLogComponent* QuestLog = PlayerPawn
		? PlayerPawn->FindComponentByClass<UEmbermereQuestLogComponent>()
		: nullptr;
	if (!QuestLog)
	{
		return;
	}

	BoundQuestLog = QuestLog;
	BoundQuestLog->OnQuestStateChanged.AddUniqueDynamic(
		this,
		&AEmbermereNpcPresentationActor::HandleQuestStateChanged);
}

void AEmbermereNpcPresentationActor::RefreshContextGreetingPresentation()
{
	if (!ContextGreetingWidget)
	{
		return;
	}

	ContextGreetingWidget->SetRelativeLocation(ContextGreetingRelativeLocation);
	ContextGreetingWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ContextGreetingWidget->SetGenerateOverlapEvents(false);
	ContextGreetingWidget->SetCanEverAffectNavigation(false);

	const EEmbermereNpcGreetingState State = GetResolvedContextGreetingState();
	const FText Text = GetResolvedContextGreetingText();
	const bool bShouldShow = bEnableContextGreeting &&
		BoundQuestLog &&
		State != EEmbermereNpcGreetingState::Hidden &&
		!Text.IsEmpty() &&
		IsLocalPlayerInGreetingRange();

	ContextGreetingWidget->SetVisibility(bShouldShow);
	ContextGreetingWidget->SetHiddenInGame(!bShouldShow);
	if (!bShouldShow)
	{
		return;
	}

	ContextGreetingWidget->InitWidget();
	if (UEmbermereNpcGreetingWidget* GreetingWidget = Cast<UEmbermereNpcGreetingWidget>(
		ContextGreetingWidget->GetUserWidgetObject()))
	{
		GreetingWidget->SetGreetingState(State, Text);
	}
}

bool AEmbermereNpcPresentationActor::IsLocalPlayerInGreetingRange() const
{
	if (!GetWorld())
	{
		return false;
	}

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	const APawn* PlayerPawn = PlayerController ? PlayerController->GetPawn() : nullptr;
	if (!PlayerPawn)
	{
		return false;
	}

	return FVector::DistSquared(PlayerPawn->GetActorLocation(), GetActorLocation()) <=
		FMath::Square(FMath::Max(ContextGreetingRadius, 0.0f));
}
