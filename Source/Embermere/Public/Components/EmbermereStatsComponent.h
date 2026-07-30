#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/EmbermereItemTypes.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereStatsComponent.generated.h"

class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereManaChangedSignature, float, CurrentMana, float, MaxMana);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereXpChangedSignature, int32, CurrentExperience);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereDiedSignature);

USTRUCT(BlueprintType)
struct FEmbermereActiveStatusEffect
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Status Effect")
	FEmbermereAbilityDefinition Ability;

	UPROPERTY(BlueprintReadOnly, Category = "Status Effect")
	float RemainingSeconds = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Status Effect")
	bool bBeneficial = true;
};

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float Armor = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	FEmbermereItemStatBonuses EquipmentBonuses;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Temporary Effects")
	float TemporaryAttackPowerBonus = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Temporary Effects")
	float TemporaryArmorBonus = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats|Temporary Effects")
	float ActiveMovementSpeedMultiplier = 1.0f;

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
	void ForceDeath();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void GrantDamageImmunity(float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void ClearDamageImmunity();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats")
	bool IsDamageImmune() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	float Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	bool SpendMana(float ManaCost);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	float RestoreMana(float ManaAmount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats|Temporary Effects")
	bool GrantTemporaryAttackPower(float BonusAmount, float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats|Temporary Effects")
	bool GrantTemporaryArmor(float BonusAmount, float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats|Temporary Effects")
	bool GrantMovementSpeedMultiplier(float SpeedMultiplier, float DurationSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats|Temporary Effects")
	bool RegisterTimedStatusEffect(const FEmbermereAbilityDefinition& Ability, bool bBeneficial);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats|Temporary Effects")
	TArray<FEmbermereActiveStatusEffect> GetActiveStatusEffects() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats|Temporary Effects")
	void ClearTemporaryEffects();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats|Temporary Effects")
	float GetEffectiveAttackPower() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats|Temporary Effects")
	float GetEffectiveArmor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats|Temporary Effects")
	float GetMovementSpeedMultiplier() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void AddExperience(int32 ExperienceAmount);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Stats")
	bool IsDead() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Stats")
	void ApplyEquipmentBonuses(const FEmbermereItemStatBonuses& NewBonuses);

protected:
	virtual void BeginPlay() override;

private:
	struct FTimedStatusEffectRecord
	{
		FEmbermereAbilityDefinition Ability;
		float EndTimeSeconds = -1.0f;
		bool bBeneficial = true;
	};

	float DamageImmunityEndTimeSeconds = -1.0f;
	float AttackPowerBuffEndTimeSeconds = -1.0f;
	float ArmorBuffEndTimeSeconds = -1.0f;
	float MovementSpeedEffectEndTimeSeconds = -1.0f;
	TArray<FTimedStatusEffectRecord> ActiveStatusEffects;

	bool IsTimedEffectActive(float EndTimeSeconds) const;
	float GetEffectRemainingSeconds(float EndTimeSeconds) const;
	float GetEffectEndTime(float DurationSeconds) const;
};
