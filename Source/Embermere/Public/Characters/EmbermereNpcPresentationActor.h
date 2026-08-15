#pragma once

#include "CoreMinimal.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "GameFramework/Actor.h"
#include "UI/EmbermereNpcGreetingWidget.h"
#include "EmbermereNpcPresentationActor.generated.h"

class UAnimInstance;
class UAnimationAsset;
class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class UStaticMesh;
class UStaticMeshComponent;
class UEmbermereInteractableComponent;
class UEmbermereQuestData;
class UEmbermereQuestLogComponent;
class UWidgetComponent;

UENUM(BlueprintType)
enum class EEmbermereNpcVisualMode : uint8
{
	None,
	StaticMesh,
	SkeletalMesh
};

UENUM(BlueprintType)
enum class EEmbermereNpcAnimationMode : uint8
{
	None,
	AnimationBlueprint,
	SingleNodeIdle
};

/**
 * Presentation-only NPC wrapper. Quest, service, dialogue, and interaction
 * behavior belongs on a gameplay actor or subclass, never on the art asset.
 */
UCLASS(Blueprintable)
class EMBERMERE_API AEmbermereNpcPresentationActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereNpcPresentationActor();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaSeconds) override;

	static EEmbermereNpcGreetingState ResolveContextGreetingState(
		const UEmbermereQuestData* OfferedQuest,
		const FEmbermereQuestState& QuestState);
	static FText ResolveContextGreetingText(
		const UEmbermereQuestData* OfferedQuest,
		EEmbermereNpcGreetingState State);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UStaticMeshComponent> StaticVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USkeletalMeshComponent> SkeletalVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation|Context")
	TObjectPtr<UWidgetComponent> ContextGreetingWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UStaticMesh> StaticVisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<USkeletalMesh> SkeletalVisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftClassPtr<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UAnimationAsset> IdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bLoopIdleAnimation = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation", meta = (ClampMin = "0.01"))
	float IdleAnimationPlayRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bPreferSkeletalVisual = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	FTransform VisualRelativeTransform = FTransform::Identity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation|Context")
	bool bEnableContextGreeting = false;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Presentation|Context")
	TObjectPtr<AActor> ContextAuthorityActor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation|Context", meta = (ClampMin = "0.0"))
	float ContextGreetingRadius = 420.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation|Context")
	FVector ContextGreetingRelativeLocation = FVector(0.0f, 0.0f, 235.0f);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Embermere|NPC|Presentation")
	void RefreshPresentation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	EEmbermereNpcVisualMode GetResolvedVisualMode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	EEmbermereNpcAnimationMode GetResolvedAnimationMode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	bool IsPresentationCollisionDisabled() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	EEmbermereNpcGreetingState GetResolvedContextGreetingState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	FText GetResolvedContextGreetingText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	bool IsContextGreetingPresentationOnly() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UEmbermereQuestLogComponent> BoundQuestLog;

	UFUNCTION()
	void HandleQuestStateChanged(const FEmbermereQuestState& QuestState);

	void EnsureQuestLogBinding();
	void RefreshContextGreetingPresentation();
	bool IsLocalPlayerInGreetingRange() const;
};
