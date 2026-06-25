#include "Components/EmbermereStatsComponent.h"

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
	CurrentHealth = MaxHealth;
	CurrentMana = MaxMana;
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

float UEmbermereStatsComponent::ApplyDamage(float DamageAmount)
{
	if (DamageAmount <= 0.0f || IsDead())
	{
		return 0.0f;
	}

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f && PreviousHealth > 0.0f)
	{
		OnDied.Broadcast();
	}

	return PreviousHealth - CurrentHealth;
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

void UEmbermereStatsComponent::RestoreMana(float ManaAmount)
{
	if (ManaAmount <= 0.0f)
	{
		return;
	}

	CurrentMana = FMath::Clamp(CurrentMana + ManaAmount, 0.0f, MaxMana);
	OnManaChanged.Broadcast(CurrentMana, MaxMana);
}

void UEmbermereStatsComponent::AddExperience(int32 ExperienceAmount)
{
	if (ExperienceAmount <= 0)
	{
		return;
	}

	CurrentExperience += ExperienceAmount;
	OnExperienceChanged.Broadcast(CurrentExperience);
}

bool UEmbermereStatsComponent::IsDead() const
{
	return CurrentHealth <= 0.0f;
}
