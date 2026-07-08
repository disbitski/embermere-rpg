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

	const float PreviousHealth = CurrentHealth;
	CurrentHealth = FMath::Clamp(CurrentHealth - DamageAmount, 0.0f, MaxHealth);
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	if (CurrentHealth <= 0.0f && PreviousHealth > 0.0f)
	{
		OnDied.Broadcast();
	}

	return PreviousHealth - CurrentHealth;
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
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Gained %d XP (Total: %d)"), ExperienceAmount, CurrentExperience)),
		FLinearColor(1.0f, 0.86f, 0.22f, 1.0f));
}

bool UEmbermereStatsComponent::IsDead() const
{
	return CurrentHealth <= 0.0f;
}
