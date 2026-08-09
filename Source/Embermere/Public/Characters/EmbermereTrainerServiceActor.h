#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmbermereTrainerServiceActor.generated.h"

class USceneComponent;
class UEmbermereInteractableComponent;
class UEmbermereTrainerComponent;

UCLASS()
class EMBERMERE_API AEmbermereTrainerServiceActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereTrainerServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereTrainerComponent> Trainer;
};
