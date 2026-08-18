#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Engine/Engine.h"
#include "Interfaces/EmbermereTargetableDispatch.h"
#include "UI/EmbermereGameplayMessageLibrary.h"

namespace
{
void SetTargetedByPlayer(AActor* Target, bool bIsTargeted)
{
	EmbermereTargetableDispatch::SetTargetedByPlayer(Target, bIsTargeted);
}

FText GetCombatantName(AActor* Actor)
{
	const FText TargetableName = EmbermereTargetableDispatch::GetDisplayName(Actor);
	return TargetableName.IsEmpty() && Actor
		? FText::FromString(Actor->GetActorLabel())
		: TargetableName;
}

bool IsTargetAlive(AActor* Actor)
{
	return EmbermereTargetableDispatch::IsAlive(Actor);
}

bool ShouldGrantDefeatCredit(AActor* Actor)
{
	return EmbermereTargetableDispatch::ShouldGrantDefeatCredit(Actor);
}
}

UEmbermereCombatComponent::UEmbermereCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereCombatComponent::SetTarget(AActor* NewTarget)
{
	if (CurrentTarget == NewTarget)
	{
		return;
	}

	AActor* OldTarget = CurrentTarget;
	SetTargetedByPlayer(OldTarget, false);

	CurrentTarget = NewTarget;
	SetTargetedByPlayer(CurrentTarget.Get(), true);

	OnTargetChanged.Broadcast(CurrentTarget, OldTarget);
}

bool UEmbermereCombatComponent::ExecuteAbility(const FEmbermereAbilityDefinition& Ability)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	UEmbermereStatsComponent* OwnerStats = Owner->FindComponentByClass<UEmbermereStatsComponent>();
	if (!OwnerStats)
	{
		return false;
	}
	if (OwnerStats->IsDead())
	{
		return false;
	}

	AActor* TargetActor = Ability.TargetKind == EEmbermereAbilityTargetKind::Self ? Owner : CurrentTarget.Get();
	if (!TargetActor)
	{
		return false;
	}

	if (Ability.TargetKind != EEmbermereAbilityTargetKind::Self)
	{
		if (!IsTargetInRange(Ability.Range))
		{
			return false;
		}

		if (!IsTargetAlive(TargetActor))
		{
			return false;
		}
	}

	if (!OwnerStats->SpendMana(Ability.ManaCost))
	{
		return false;
	}

	UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>();
	float EffectAmount = 0.0f;
	bool bAppliedPrimaryEffect = false;
	bool bAppliedMovementEffect = false;
	bool bTargetDiedAfterEffect = false;
	if (TargetStats)
	{
		switch (Ability.EffectType)
		{
		case EEmbermereAbilityEffectType::Damage:
			EffectAmount = TargetStats->ApplyDamage(Ability.Power + OwnerStats->GetEffectiveAttackPower());
			bAppliedPrimaryEffect = EffectAmount > 0.0f;
			if (TargetStats->IsDead())
			{
				bTargetDiedAfterEffect = true;
				if (ShouldGrantDefeatCredit(TargetActor))
				{
					if (UEmbermereQuestLogComponent* QuestLog = Owner->FindComponentByClass<UEmbermereQuestLogComponent>())
					{
						QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1);
					}
				}
			}
			break;
		case EEmbermereAbilityEffectType::Heal:
			EffectAmount = TargetStats->Heal(Ability.Power);
			bAppliedPrimaryEffect = EffectAmount > 0.0f;
			break;
		case EEmbermereAbilityEffectType::RestoreMana:
			EffectAmount = TargetStats->RestoreMana(Ability.Power);
			bAppliedPrimaryEffect = EffectAmount > 0.0f;
			break;
		case EEmbermereAbilityEffectType::AttackPowerBuff:
			bAppliedPrimaryEffect = TargetStats->GrantTemporaryAttackPower(Ability.Power, Ability.Duration);
			EffectAmount = bAppliedPrimaryEffect ? Ability.Power : 0.0f;
			break;
		case EEmbermereAbilityEffectType::ArmorBuff:
			bAppliedPrimaryEffect = TargetStats->GrantTemporaryArmor(Ability.Power, Ability.Duration);
			EffectAmount = bAppliedPrimaryEffect ? Ability.Power : 0.0f;
			break;
		default:
			break;
		}

		if (!FMath::IsNearlyEqual(Ability.MovementSpeedMultiplier, 1.0f))
		{
			bAppliedMovementEffect = TargetStats->GrantMovementSpeedMultiplier(
				Ability.MovementSpeedMultiplier,
				Ability.Duration);
		}

		if (bAppliedPrimaryEffect &&
			(Ability.EffectType == EEmbermereAbilityEffectType::AttackPowerBuff ||
			 Ability.EffectType == EEmbermereAbilityEffectType::ArmorBuff))
		{
			TargetStats->RegisterTimedStatusEffect(Ability, true);
		}
		if (bAppliedMovementEffect)
		{
			TargetStats->RegisterTimedStatusEffect(Ability, false);
		}
	}

	OnAbilityUsed.Broadcast(Ability.AbilityId, TargetActor, EffectAmount);
	const FText TargetName = GetCombatantName(TargetActor);
	if (bAppliedPrimaryEffect)
	{
		FString Message;
		FLinearColor MessageColor(1.0f, 0.58f, 0.16f, 1.0f);
		switch (Ability.EffectType)
		{
		case EEmbermereAbilityEffectType::Damage:
			Message = FString::Printf(
				TEXT("%s hit %s for %.0f"),
				*Ability.DisplayName.ToString(),
				*TargetName.ToString(),
				EffectAmount);
			break;
		case EEmbermereAbilityEffectType::Heal:
			Message = FString::Printf(TEXT("%s restored %.0f health"), *Ability.DisplayName.ToString(), EffectAmount);
			MessageColor = FLinearColor(0.36f, 0.95f, 0.46f, 1.0f);
			break;
		case EEmbermereAbilityEffectType::RestoreMana:
			Message = FString::Printf(TEXT("%s restored %.0f mana"), *Ability.DisplayName.ToString(), EffectAmount);
			MessageColor = FLinearColor(0.34f, 0.66f, 1.0f, 1.0f);
			break;
		case EEmbermereAbilityEffectType::AttackPowerBuff:
			Message = FString::Printf(
				TEXT("%s grants +%.0f Attack Power for %.0fs"),
				*Ability.DisplayName.ToString(),
				EffectAmount,
				Ability.Duration);
			MessageColor = FLinearColor(1.0f, 0.72f, 0.24f, 1.0f);
			break;
		case EEmbermereAbilityEffectType::ArmorBuff:
			Message = FString::Printf(
				TEXT("%s grants +%.0f Armor for %.0fs"),
				*Ability.DisplayName.ToString(),
				EffectAmount,
				Ability.Duration);
			MessageColor = FLinearColor(0.78f, 0.88f, 1.0f, 1.0f);
			break;
		default:
			break;
		}

		if (!Message.IsEmpty())
		{
			UEmbermereGameplayMessageLibrary::PostGameplayMessage(
				this,
				FText::FromString(Message),
				MessageColor);
		}
	}
	if (bAppliedMovementEffect)
	{
		const bool bRooted = Ability.MovementSpeedMultiplier <= KINDA_SMALL_NUMBER;
		const float SlowPercent = (1.0f - FMath::Clamp(Ability.MovementSpeedMultiplier, 0.0f, 1.0f)) * 100.0f;
		const FString ControlMessage = bRooted
			? FString::Printf(
				TEXT("%s rooted %s for %.0fs"),
				*Ability.DisplayName.ToString(),
				*TargetName.ToString(),
				Ability.Duration)
			: FString::Printf(
				TEXT("%s slowed %s by %.0f%% for %.0fs"),
				*Ability.DisplayName.ToString(),
				*TargetName.ToString(),
				SlowPercent,
				Ability.Duration);
		UEmbermereGameplayMessageLibrary::PostGameplayMessage(
			this,
			FText::FromString(ControlMessage),
			FLinearColor(0.42f, 0.82f, 1.0f, 1.0f));
	}
	if (bTargetDiedAfterEffect)
	{
		SetTarget(nullptr);
	}
	return bAppliedPrimaryEffect ||
		bAppliedMovementEffect ||
		Ability.TargetKind == EEmbermereAbilityTargetKind::Self;
}

bool UEmbermereCombatComponent::IsTargetInRange(float Range) const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !CurrentTarget)
	{
		return false;
	}

	if (Range <= 0.0f)
	{
		return true;
	}

	return FVector::DistSquared(Owner->GetActorLocation(), CurrentTarget->GetActorLocation()) <= FMath::Square(Range);
}
