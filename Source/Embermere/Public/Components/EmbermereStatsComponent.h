#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmbermereStatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereManaChangedSignature, float, CurrentMana, float, MaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereXpChangedSignature, int32, CurrentExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereDiedSignature);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereStatsComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxMana = 50.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentMana = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	int32 CurrentExperience = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float AttackPower = 10.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereManaChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereXpChangedSignature OnExperienceChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereDiedSignature OnDied;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void InitializeVitals();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	float ApplyDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	float Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	bool SpendMana(float ManaCost);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void RestoreMana(float ManaAmount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void AddExperience(int32 ExperienceAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats")
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;
};
