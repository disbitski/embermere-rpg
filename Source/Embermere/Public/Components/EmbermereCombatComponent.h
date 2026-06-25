#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereTargetChangedSignature, AActor*, NewTarget, AActor*, OldTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEmbermereAbilityUsedSignature, FName, AbilityId, AActor*, Target, float, EffectAmount);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereCombatComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereTargetChangedSignature OnTargetChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereAbilityUsedSignature OnAbilityUsed;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Combat")
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Combat")
	bool ExecuteAbility(const FEmbermereAbilityDefinition& Ability);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Combat")
	bool IsTargetInRange(float Range) const;
};
