#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereQuestData.h"
#include "EmbermereInteractableComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereInteractedSignature, AActor*, Interactor, UEmbermereInteractableComponent*, Interactable);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereInteractableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereInteractableComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (MultiLine = true))
	FText DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UEmbermereQuestData> QuestToOffer;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	bool bShowWorldMarker = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Marker")
	float MarkerHeight = 185.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereInteractedSignature OnInteracted;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Interaction")
	void Interact(AActor* Interactor);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<class UTextRenderComponent> WorldMarkerText;

	void CreateWorldMarker();
};
