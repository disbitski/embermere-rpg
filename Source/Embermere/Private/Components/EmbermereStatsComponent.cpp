#include "Components/EmbermereStatsComponent.h"
#include "Data/EmbermereRulesData.h"
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

void UEmbermereStatsComponent::ApplyStartingAttributes(const FEmbermereAttributeBlock& StartingAttributes)
{
	ApplyProgressionAttributes(StartingAttributes, Level, true, true);
}

bool UEmbermereStatsComponent::CanConfigureProgression(
	const FEmbermereProgressionProfile& Profile,
	int32 Experience,
	int32* OutLevel) const
{
	FEmbermereAttributeBlock ResolvedAttributes;
	int32 ResolvedLevel = 1;
	const bool bValid = UEmbermereRulesData::ResolveProgression(
		Profile,
		Experience,
		ResolvedAttributes,
		ResolvedLevel);
	if (OutLevel)
	{
		*OutLevel = bValid ? ResolvedLevel : 1;
	}
	return bValid;
}

bool UEmbermereStatsComponent::ConfigureProgression(
	const FEmbermereProgressionProfile& Profile,
	int32 Experience,
	bool bRestoreFullVitals)
{
	FEmbermereAttributeBlock ResolvedAttributes;
	int32 ResolvedLevel = 1;
	if (!UEmbermereRulesData::ResolveProgression(
		Profile,
		Experience,
		ResolvedAttributes,
		ResolvedLevel))
	{
		return false;
	}

	ProgressionProfile = Profile;
	bProgressionConfigured = true;
	CurrentExperience = Experience;
	ApplyProgressionAttributes(
		ResolvedAttributes,
		ResolvedLevel,
		bRestoreFullVitals,
		true);
	return true;
}

bool UEmbermereStatsComponent::IsProgressionConfigured() const
{
	return bProgressionConfigured;
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

bool UEmbermereStatsComponent::HasValidVitalState() const
{
	return FMath::IsFinite(MaxHealth) && MaxHealth > 0.0f &&
		FMath::IsFinite(CurrentHealth) && CurrentHealth >= 0.0f && CurrentHealth <= MaxHealth + KINDA_SMALL_NUMBER &&
		FMath::IsFinite(MaxMana) && MaxMana >= 0.0f &&
		FMath::IsFinite(CurrentMana) && CurrentMana >= 0.0f && CurrentMana <= MaxMana + KINDA_SMALL_NUMBER;
}

bool UEmbermereStatsComponent::NeedsVitalRecovery(
	bool bRestoreHealth,
	bool bRestoreMana) const
{
	if ((!bRestoreHealth && !bRestoreMana) || !HasValidVitalState() || IsDead())
	{
		return false;
	}

	return (bRestoreHealth && CurrentHealth < MaxHealth - KINDA_SMALL_NUMBER) ||
		(bRestoreMana && CurrentMana < MaxMana - KINDA_SMALL_NUMBER);
}

bool UEmbermereStatsComponent::TryRestoreVitalsToFull(
	bool bRestoreHealth,
	bool bRestoreMana,
	float& OutHealthRestored,
	float& OutManaRestored)
{
	OutHealthRestored = 0.0f;
	OutManaRestored = 0.0f;
	if (!NeedsVitalRecovery(bRestoreHealth, bRestoreMana))
	{
		return false;
	}

	OutHealthRestored = bRestoreHealth ? MaxHealth - CurrentHealth : 0.0f;
	OutManaRestored = bRestoreMana ? MaxMana - CurrentMana : 0.0f;
	if (OutHealthRestored > 0.0f)
	{
		CurrentHealth = MaxHealth;
	}
	if (OutManaRestored > 0.0f)
	{
		CurrentMana = MaxMana;
	}
	if (OutHealthRestored > 0.0f)
	{
		OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	}
	if (OutManaRestored > 0.0f)
	{
		OnManaChanged.Broadcast(CurrentMana, MaxMana);
	}
	return true;
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

	const int32 PreviousLevel = Level;
	const int32 NewExperience = CurrentExperience + ExperienceAmount;
	FEmbermereAttributeBlock ResolvedAttributes;
	int32 ResolvedLevel = Level;
	if (bProgressionConfigured &&
		!UEmbermereRulesData::ResolveProgression(
			ProgressionProfile,
			NewExperience,
			ResolvedAttributes,
			ResolvedLevel))
	{
		return false;
	}

	CurrentExperience = NewExperience;
	if (bProgressionConfigured && ResolvedLevel != PreviousLevel)
	{
		ApplyProgressionAttributes(
			ResolvedAttributes,
			ResolvedLevel,
			false,
			false);
	}
	OnExperienceChanged.Broadcast(CurrentExperience);
	if (ResolvedLevel > PreviousLevel)
	{
		OnLevelChanged.Broadcast(PreviousLevel, ResolvedLevel);
	}
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Gained %d XP (Total: %d)"), ExperienceAmount, CurrentExperience)),
		FLinearColor(1.0f, 0.86f, 0.22f, 1.0f));
	if (ResolvedLevel > PreviousLevel)
	{
		const FString LevelMessage = ResolvedLevel == PreviousLevel + 1
			? FString::Printf(TEXT("Level up! Reached Level %d."), ResolvedLevel)
			: FString::Printf(
				TEXT("Level up! Advanced from Level %d to Level %d."),
				PreviousLevel,
				ResolvedLevel);
		UEmbermereGameplayMessageLibrary::PostGameplayMessage(
			this,
			FText::FromString(LevelMessage),
			FLinearColor(0.35f, 0.9f, 1.0f, 1.0f));
	}
	return true;
}

bool UEmbermereStatsComponent::GetProgressionPresentation(
	FEmbermereProgressionPresentation& OutPresentation) const
{
	OutPresentation = FEmbermereProgressionPresentation();
	if (!bProgressionConfigured ||
		!UEmbermereRulesData::IsProgressionProfileValid(ProgressionProfile) ||
		CurrentExperience < 0)
	{
		return false;
	}

	FEmbermereAttributeBlock ResolvedAttributes;
	int32 ResolvedLevel = 0;
	if (!UEmbermereRulesData::ResolveProgression(
			ProgressionProfile,
			CurrentExperience,
			ResolvedAttributes,
			ResolvedLevel) ||
		ResolvedLevel != Level)
	{
		return false;
	}

	const int32 ThresholdIndex = ResolvedLevel - 1;
	if (!ProgressionProfile.ExperienceThresholds.IsValidIndex(ThresholdIndex))
	{
		return false;
	}

	OutPresentation.CurrentLevel = ResolvedLevel;
	OutPresentation.CurrentExperience = CurrentExperience;
	OutPresentation.CurrentLevelThreshold = ProgressionProfile.ExperienceThresholds[ThresholdIndex];
	OutPresentation.bAtLevelCap =
		ThresholdIndex == ProgressionProfile.ExperienceThresholds.Num() - 1;
	OutPresentation.NextLevelThreshold = OutPresentation.bAtLevelCap
		? OutPresentation.CurrentLevelThreshold
		: ProgressionProfile.ExperienceThresholds[ThresholdIndex + 1];
	OutPresentation.ExperienceIntoLevel = FMath::Max(
		0,
		CurrentExperience - OutPresentation.CurrentLevelThreshold);
	OutPresentation.ExperienceRequiredForLevel = OutPresentation.bAtLevelCap
		? 0
		: OutPresentation.NextLevelThreshold - OutPresentation.CurrentLevelThreshold;
	OutPresentation.NormalizedProgress = OutPresentation.bAtLevelCap
		? 1.0f
		: FMath::Clamp(
			static_cast<float>(OutPresentation.ExperienceIntoLevel) /
				static_cast<float>(OutPresentation.ExperienceRequiredForLevel),
			0.0f,
			1.0f);
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
	if (bProgressionConfigured)
	{
		FEmbermereAttributeBlock ResolvedAttributes;
		int32 ResolvedLevel = Level;
		if (UEmbermereRulesData::ResolveProgression(
			ProgressionProfile,
			CurrentExperience,
			ResolvedAttributes,
			ResolvedLevel))
		{
			ApplyProgressionAttributes(
				ResolvedAttributes,
				ResolvedLevel,
				false,
				false);
		}
	}
}

void UEmbermereStatsComponent::ApplyProgressionAttributes(
	const FEmbermereAttributeBlock& Attributes,
	int32 NewLevel,
	bool bRestoreFullVitals,
	bool bClearTransientEffects)
{
	const bool bWasDead = IsDead();
	const float MissingHealth = FMath::Max(0.0f, MaxHealth - CurrentHealth);
	const float MissingMana = FMath::Max(0.0f, MaxMana - CurrentMana);
	if (bClearTransientEffects)
	{
		ClearDamageImmunity();
		ClearTemporaryEffects();
	}

	Level = FMath::Max(1, NewLevel);
	Strength = FMath::Max(0.0f, Attributes.Strength);
	Spirit = FMath::Max(0.0f, Attributes.Spirit);
	Agility = FMath::Max(0.0f, Attributes.Agility);
	Intellect = FMath::Max(0.0f, Attributes.Intellect);
	MaxHealth = FMath::Max(1.0f, Attributes.MaxHealth + EquipmentBonuses.MaxHealth);
	MaxMana = FMath::Max(0.0f, Attributes.MaxMana + EquipmentBonuses.MaxMana);
	AttackPower = FMath::Max(0.0f, Strength + EquipmentBonuses.Power);
	Armor = FMath::Max(0.0f, EquipmentBonuses.Armor);
	if (bRestoreFullVitals)
	{
		CurrentHealth = MaxHealth;
		CurrentMana = MaxMana;
	}
	else
	{
		CurrentHealth = bWasDead ? 0.0f : FMath::Clamp(MaxHealth - MissingHealth, 0.0f, MaxHealth);
		CurrentMana = FMath::Clamp(MaxMana - MissingMana, 0.0f, MaxMana);
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
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
