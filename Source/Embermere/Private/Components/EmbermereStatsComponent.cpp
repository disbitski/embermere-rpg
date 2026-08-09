#include "Components/EmbermereStatsComponent.h"
#include "Engine/Engine.h"
#include "UI/EmbermereGameplayMessageLibrary.h"

UEmbermereStatsComponent::UEmbermereStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereStatsComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeVitals();
}

void UEmbermereStatsComponent::InitializeVitals()
{
	ClearDamageImmunity();
	ClearTemporaryEffects();
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

float UEmbermereStatsComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || IsDead() || IsDamageImmune())
	{
		return 0.0f;
	}

	const float EffectiveDamage = DamageAmount * (100.0f / (100.0f + GetEffectiveArmor()));
	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - EffectiveDamage, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f && PreviousHealth > 0.0f)
	{
		OnDied.Broadcast();
	}

	return PreviousHealth - CurrentHealth;
}

void UEmbermereStatsComponent::ForceDeath()
{
	if (IsDead())
	{
		return;
	}

	ClearDamageImmunity();
	CurrentHealth = 0.0f;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnDied.Broadcast();
}

void UEmbermereStatsComponent::GrantDamageImmunity(float DurationSeconds)
{
	if (DurationSeconds <= 0.0f)
	{
		ClearDamageImmunity();
		return;
	}

	if (const UWorld* World = GetWorld())
	{
		DamageImmunityEndTimeSeconds = World->GetTimeSeconds() + DurationSeconds;
	}
	else
	{
		DamageImmunityEndTimeSeconds = DurationSeconds;
	}
}

void UEmbermereStatsComponent::ClearDamageImmunity()
{
	DamageImmunityEndTimeSeconds = -1.0f;
}

bool UEmbermereStatsComponent::IsDamageImmune() const
{
	if (DamageImmunityEndTimeSeconds <= 0.0f)
	{
		return false;
	}

	if (const UWorld* World = GetWorld())
	{
		return World->GetTimeSeconds() < DamageImmunityEndTimeSeconds;
	}

	return true;
}

float UEmbermereStatsComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || IsDead())
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	return CurrentHealth - PreviousHealth;
}

bool UEmbermereStatsComponent::SpendMana(float ManaCost)
{
	if (ManaCost <= 0.0f)
	{
		return true;
	}

	if (CurrentMana < ManaCost)
	{
		return false;
	}

	CurrentMana = FMath::Clamp(CurrentMana - ManaCost, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	return true;
}

float UEmbermereStatsComponent::RestoreMana(float ManaAmount)
{
	if (ManaAmount <= 0.0f)
	{
		return 0.0f;
	}

	const float PreviousMana = CurrentMana;
	CurrentMana = FMath::Clamp(CurrentMana + ManaAmount, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
	return CurrentMana - PreviousMana;
}

bool UEmbermereStatsComponent::GrantTemporaryAttackPower(float BonusAmount, float DurationSeconds)
{
	if (BonusAmount <= 0.0f || DurationSeconds <= 0.0f)
	{
		return false;
	}

	if (!IsTimedEffectActive(AttackPowerBuffEndTimeSeconds))
	{
		TemporaryAttackPowerBonus = 0.0f;
	}
	TemporaryAttackPowerBonus = FMath::Max(TemporaryAttackPowerBonus, BonusAmount);
	AttackPowerBuffEndTimeSeconds = FMath::Max(AttackPowerBuffEndTimeSeconds, GetEffectEndTime(DurationSeconds));
	return true;
}

bool UEmbermereStatsComponent::GrantTemporaryArmor(float BonusAmount, float DurationSeconds)
{
	if (BonusAmount <= 0.0f || DurationSeconds <= 0.0f)
	{
		return false;
	}

	if (!IsTimedEffectActive(ArmorBuffEndTimeSeconds))
	{
		TemporaryArmorBonus = 0.0f;
	}
	TemporaryArmorBonus = FMath::Max(TemporaryArmorBonus, BonusAmount);
	ArmorBuffEndTimeSeconds = FMath::Max(ArmorBuffEndTimeSeconds, GetEffectEndTime(DurationSeconds));
	return true;
}

bool UEmbermereStatsComponent::GrantMovementSpeedMultiplier(float SpeedMultiplier, float DurationSeconds)
{
	if (SpeedMultiplier < 0.0f ||
		SpeedMultiplier > 1.0f ||
		FMath::IsNearlyEqual(SpeedMultiplier, 1.0f) ||
		DurationSeconds <= 0.0f)
	{
		return false;
	}

	if (!IsTimedEffectActive(MovementSpeedEffectEndTimeSeconds))
	{
		ActiveMovementSpeedMultiplier = 1.0f;
	}
	ActiveMovementSpeedMultiplier = FMath::Min(ActiveMovementSpeedMultiplier, SpeedMultiplier);
	MovementSpeedEffectEndTimeSeconds = FMath::Max(
		MovementSpeedEffectEndTimeSeconds,
		GetEffectEndTime(DurationSeconds));
	return true;
}

bool UEmbermereStatsComponent::RegisterTimedStatusEffect(
	const FEmbermereAbilityDefinition& Ability,
	bool bBeneficial)
{
	if (Ability.AbilityId.IsNone() || Ability.DisplayName.IsEmpty() || Ability.Duration <= 0.0f)
	{
		return false;
	}

	ActiveStatusEffects.RemoveAll([this](const FTimedStatusEffectRecord& Record)
	{
		return !IsTimedEffectActive(Record.EndTimeSeconds);
	});

	const float NewEndTimeSeconds = GetEffectEndTime(Ability.Duration);
	for (FTimedStatusEffectRecord& Record : ActiveStatusEffects)
	{
		if (Record.Ability.AbilityId != Ability.AbilityId)
		{
			continue;
		}

		Record.Ability = Ability;
		Record.EndTimeSeconds = FMath::Max(Record.EndTimeSeconds, NewEndTimeSeconds);
		Record.bBeneficial = bBeneficial;
		return true;
	}

	FTimedStatusEffectRecord& NewRecord = ActiveStatusEffects.AddDefaulted_GetRef();
	NewRecord.Ability = Ability;
	NewRecord.EndTimeSeconds = NewEndTimeSeconds;
	NewRecord.bBeneficial = bBeneficial;
	return true;
}

TArray<FEmbermereActiveStatusEffect> UEmbermereStatsComponent::GetActiveStatusEffects() const
{
	TArray<FEmbermereActiveStatusEffect> Results;
	for (const FTimedStatusEffectRecord& Record : ActiveStatusEffects)
	{
		const float RemainingSeconds = GetEffectRemainingSeconds(Record.EndTimeSeconds);
		if (RemainingSeconds <= 0.0f)
		{
			continue;
		}

		FEmbermereActiveStatusEffect& Effect = Results.AddDefaulted_GetRef();
		Effect.Ability = Record.Ability;
		Effect.RemainingSeconds = RemainingSeconds;
		Effect.bBeneficial = Record.bBeneficial;
	}
	return Results;
}

void UEmbermereStatsComponent::ClearTemporaryEffects()
{
	TemporaryAttackPowerBonus = 0.0f;
	TemporaryArmorBonus = 0.0f;
	ActiveMovementSpeedMultiplier = 1.0f;
	AttackPowerBuffEndTimeSeconds = -1.0f;
	ArmorBuffEndTimeSeconds = -1.0f;
	MovementSpeedEffectEndTimeSeconds = -1.0f;
	ActiveStatusEffects.Reset();
}

float UEmbermereStatsComponent::GetEffectiveAttackPower() const
{
	const float ActiveBonus = IsTimedEffectActive(AttackPowerBuffEndTimeSeconds)
		? TemporaryAttackPowerBonus
		: 0.0f;
	return FMath::Max(0.0f, AttackPower + ActiveBonus);
}

float UEmbermereStatsComponent::GetEffectiveArmor() const
{
	const float ActiveBonus = IsTimedEffectActive(ArmorBuffEndTimeSeconds)
		? TemporaryArmorBonus
		: 0.0f;
	return FMath::Max(0.0f, Armor + ActiveBonus);
}

float UEmbermereStatsComponent::GetMovementSpeedMultiplier() const
{
	return IsTimedEffectActive(MovementSpeedEffectEndTimeSeconds)
		? FMath::Max(0.0f, ActiveMovementSpeedMultiplier)
		: 1.0f;
}

void UEmbermereStatsComponent::AddExperience(int32 ExperienceAmount)
{
	TryAddExperience(ExperienceAmount);
}

bool UEmbermereStatsComponent::CanAddExperience(int32 ExperienceAmount) const
{
	return ExperienceAmount > 0 && CurrentExperience >= 0 && CurrentExperience <= MAX_int32 - ExperienceAmount;
}

bool UEmbermereStatsComponent::TryAddExperience(int32 ExperienceAmount)
{
	if (!CanAddExperience(ExperienceAmount))
	{
		return false;
	}

	CurrentExperience += ExperienceAmount;
	OnExperienceChanged.Broadcast(CurrentExperience);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Gained %d XP (Total: %d)"), ExperienceAmount, CurrentExperience)),
		FLinearColor(1.0f, 0.86f, 0.22f, 1.0f));
	return true;
}

bool UEmbermereStatsComponent::IsDead() const
{
	return CurrentHealth <= 0.0f;
}

void UEmbermereStatsComponent::ApplyEquipmentBonuses(const FEmbermereItemStatBonuses& NewBonuses)
{
	const bool bWasDead = IsDead();
	const float MissingHealth = FMath::Max(0.0f, MaxHealth - CurrentHealth);
	const float MissingMana = FMath::Max(0.0f, MaxMana - CurrentMana);

	MaxHealth = FMath::Max(1.0f, MaxHealth - EquipmentBonuses.MaxHealth + NewBonuses.MaxHealth);
	MaxMana = FMath::Max(0.0f, MaxMana - EquipmentBonuses.MaxMana + NewBonuses.MaxMana);
	AttackPower = FMath::Max(0.0f, AttackPower - EquipmentBonuses.Power + NewBonuses.Power);
	Armor = FMath::Max(0.0f, Armor - EquipmentBonuses.Armor + NewBonuses.Armor);
	EquipmentBonuses = NewBonuses;

	CurrentHealth = bWasDead ? 0.0f : FMath::Clamp(MaxHealth - MissingHealth, 0.0f, MaxHealth);
	CurrentMana = FMath::Clamp(MaxMana - MissingMana, 0.0f, MaxMana);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

void UEmbermereStatsComponent::RestoreExperienceForSaveGame(int32 NewExperience)
{
	CurrentExperience = FMath::Max(0, NewExperience);
	OnExperienceChanged.Broadcast(CurrentExperience);
}

bool UEmbermereStatsComponent::IsTimedEffectActive(float EndTimeSeconds) const
{
	return GetEffectRemainingSeconds(EndTimeSeconds) > 0.0f;
}

float UEmbermereStatsComponent::GetEffectRemainingSeconds(float EndTimeSeconds) const
{
	if (EndTimeSeconds <= 0.0f)
	{
		return 0.0f;
	}

	const float CurrentTimeSeconds = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
	return FMath::Max(0.0f, EndTimeSeconds - CurrentTimeSeconds);
}

float UEmbermereStatsComponent::GetEffectEndTime(float DurationSeconds) const
{
	if (const UWorld* World = GetWorld())
	{
		return World->GetTimeSeconds() + DurationSeconds;
	}

	return DurationSeconds;
}
