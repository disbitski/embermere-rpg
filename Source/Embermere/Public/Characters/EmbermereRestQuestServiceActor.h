#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmbermereRestQuestServiceActor.generated.h"

class USceneComponent;
class UEmbermereInteractableComponent;
class UEmbermereRestQuestObjectiveRouterComponent;

UCLASS()
class EMBERMERE_API AEmbermereRestQuestServiceActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereRestQuestServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereRestQuestObjectiveRouterComponent> RestObjectiveRouter;
};
