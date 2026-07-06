#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereCombatComponent.h"

UEmbermereHotbarComponent::UEmbermereHotbarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.SetNum(10);
	SlotReadyTimeSeconds.SetNum(10);
}

void UEmbermereHotbarComponent::SetAbilityInSlot(int32 SlotIndex, const FEmbermereAbilityDefinition& Ability)
{
	if (!Slots.IsValidIndex(SlotIndex))
	{
		return;
	}

	Slots[SlotIndex] = Ability;
	OnHotbarChanged.Broadcast(SlotIndex, Ability);
}

bool UEmbermereHotbarComponent::ActivateSlot(int32 SlotIndex)
{
	if (!Slots.IsValidIndex(SlotIndex) || Slots[SlotIndex].AbilityId.IsNone())
	{
		return false;
	}
	if (GetSlotCooldownRemaining(SlotIndex) > 0.0f)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return false;
	}

	if (UEmbermereCombatComponent* Combat = Owner->FindComponentByClass<UEmbermereCombatComponent>())
	{
		const bool bActivated = Combat->ExecuteAbility(Slots[SlotIndex]);
		if (bActivated)
		{
			if (!SlotReadyTimeSeconds.IsValidIndex(SlotIndex))
			{
				SlotReadyTimeSeconds.SetNum(Slots.Num());
			}
			if (const UWorld* World = GetWorld())
			{
				SlotReadyTimeSeconds[SlotIndex] = World->GetTimeSeconds() + FMath::Max(0.0f, Slots[SlotIndex].Cooldown);
			}
		}
		return bActivated;
	}

	return false;
}

float UEmbermereHotbarComponent::GetSlotCooldownRemaining(int32 SlotIndex) const
{
	if (!SlotReadyTimeSeconds.IsValidIndex(SlotIndex))
	{
		return 0.0f;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, static_cast<float>(SlotReadyTimeSeconds[SlotIndex] - World->GetTimeSeconds()));
}
