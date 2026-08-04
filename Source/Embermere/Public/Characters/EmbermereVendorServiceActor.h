#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EmbermereVendorServiceActor.generated.h"

class USceneComponent;
class UEmbermereInteractableComponent;
class UEmbermereVendorComponent;

UCLASS()
class EMBERMERE_API AEmbermereVendorServiceActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereVendorServiceActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereInteractableComponent> Interactable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereVendorComponent> Vendor;
};
