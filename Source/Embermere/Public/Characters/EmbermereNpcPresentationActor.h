#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmbermereNpcPresentationActor.generated.h"

class UAnimInstance;
class USceneComponent;
class USkeletalMesh;
class USkeletalMeshComponent;
class UStaticMesh;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EEmbermereNpcVisualMode : uint8
{
	None,
	StaticMesh,
	SkeletalMesh
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<UStaticMeshComponent> StaticVisual;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation")
	TObjectPtr<USkeletalMeshComponent> SkeletalVisual;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<UStaticMesh> StaticVisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftObjectPtr<USkeletalMesh> SkeletalVisualMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	TSoftClassPtr<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	bool bPreferSkeletalVisual = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Presentation")
	FTransform VisualRelativeTransform = FTransform::Identity;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Embermere|NPC|Presentation")
	void RefreshPresentation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	EEmbermereNpcVisualMode GetResolvedVisualMode() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Presentation")
	bool IsPresentationCollisionDisabled() const;
};
