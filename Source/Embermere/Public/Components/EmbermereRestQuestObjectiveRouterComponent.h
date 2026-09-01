#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/EmbermereRestServiceComponent.h"
#include "EmbermereRestQuestObjectiveRouterComponent.generated.h"

class AEmbermereRestServiceActor;

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereRestQuestObjectiveRouterComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereRestQuestObjectiveRouterComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Routing")
	TObjectPtr<AEmbermereRestServiceActor> ObservedRestService;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Routing")
	FName QuestId = TEXT("FenwatchStillWaters");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest Routing")
	FName ObjectiveId = TEXT("FenwatchRestCompleted");

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	void SetObservedRestService(AEmbermereRestServiceActor* NewRestService);

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	bool IsBoundToObservedRestService() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FDelegateHandle RestOutcomeHandle;

	void BindToObservedRestService();
	void UnbindFromObservedRestService();
	void HandleRestOutcome(const FEmbermereRestOutcome& Outcome);
};
