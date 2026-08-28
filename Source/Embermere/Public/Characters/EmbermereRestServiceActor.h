#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/EmbermereRestServiceComponent.h"
#include "EmbermereRestServiceActor.generated.h"

class USceneComponent;
class UEmbermereInteractableComponent;

UCLASS()
class EMBERMERE_API AEmbermereRestServiceActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereRestServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereRestServiceComponent> RestService;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void HandleInteracted(AActor* Interactor, UEmbermereInteractableComponent* SourceInteractable);

	UFUNCTION()
	void HandleRestOutcome(FEmbermereRestOutcome Outcome);
};
