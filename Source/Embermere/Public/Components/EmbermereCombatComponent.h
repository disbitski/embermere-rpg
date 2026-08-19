#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereTargetChangedSignature, AActor*, NewTarget, AActor*, OldTarget);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FEmbermereAbilityUsedSignature, FName, AbilityId, AActor*, Target, float, EffectAmount);

UENUM(BlueprintType)
enum class EEmbermereCombatResultKind : uint8
{
	Damage UMETA(DisplayName = "Damage"),
	Healing UMETA(DisplayName = "Healing"),
	ManaRestored UMETA(DisplayName = "Mana Restored"),
	BuffApplied UMETA(DisplayName = "Buff Applied"),
	ControlApplied UMETA(DisplayName = "Control Applied"),
	Miss UMETA(DisplayName = "Miss")
};

USTRUCT(BlueprintType)
struct FEmbermereCombatResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	FName AbilityId = NAME_None;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	TObjectPtr<AActor> Source = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	TObjectPtr<AActor> Target = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	EEmbermereCombatResultKind Kind = EEmbermereCombatResultKind::Damage;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	float AppliedAmount = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Combat Result")
	bool bTargetDefeated = false;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEmbermereCombatResultSignature, const FEmbermereCombatResult&);

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

	// Native observers receive facts only after the authoritative effect commits.
	FEmbermereCombatResultSignature OnCombatResult;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Combat")
	void SetTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Combat")
	bool ExecuteAbility(const FEmbermereAbilityDefinition& Ability);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Combat")
	bool IsTargetInRange(float Range) const;
};
