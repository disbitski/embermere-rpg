#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmbermereTargetingComponent.generated.h"

class UEmbermereCombatComponent;

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereTargetingComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float SearchRadius = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Targeting")
	float ForwardConeDegrees = 120.0f;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Targeting")
	AActor* CycleTarget();

protected:
	AActor* FindNextTarget(const TArray<AActor*>& Candidates, AActor* CurrentTarget) const;
};
