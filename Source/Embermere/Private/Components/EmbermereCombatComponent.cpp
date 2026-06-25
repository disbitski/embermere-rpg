#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Engine/Engine.h"
#include "Interfaces/EmbermereTargetable.h"

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
	CurrentTarget = NewTarget;
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

		if (!TargetActor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()) ||
			!IEmbermereTargetable::Execute_IsAlive(TargetActor))
		{
			return false;
		}
	}

	if (!OwnerStats->SpendMana(Ability.ManaCost))
	{
		return false;
	}

	float EffectAmount = 0.0f;
	if (Ability.TargetKind == EEmbermereAbilityTargetKind::Self || Ability.TargetKind == EEmbermereAbilityTargetKind::Ally)
	{
		if (UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>())
		{
			EffectAmount = TargetStats->Heal(Ability.Power);
		}
	}
	else if (UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>())
	{
		EffectAmount = TargetStats->ApplyDamage(Ability.Power + OwnerStats->AttackPower);
		if (TargetStats->IsDead())
		{
			if (UEmbermereQuestLogComponent* QuestLog = Owner->FindComponentByClass<UEmbermereQuestLogComponent>())
			{
				QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1);
			}
		}
	}

	OnAbilityUsed.Broadcast(Ability.AbilityId, TargetActor, EffectAmount);
	if (GEngine && EffectAmount > 0.0f)
	{
		const FText TargetName = TargetActor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass())
			? IEmbermereTargetable::Execute_GetTargetDisplayName(TargetActor)
			: FText::FromString(TargetActor->GetActorLabel());
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.5f,
			FColor::Orange,
			FString::Printf(TEXT("%s hit %s for %.0f"), *Ability.DisplayName.ToString(), *TargetName.ToString(), EffectAmount));
	}
	return EffectAmount > 0.0f || Ability.TargetKind == EEmbermereAbilityTargetKind::Self;
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
