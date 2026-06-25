#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereCombatComponent.h"

UEmbermereHotbarComponent::UEmbermereHotbarComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.SetNum(10);
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

	if (UEmbermereCombatComponent* Combat = GetOwner()->FindComponentByClass<UEmbermereCombatComponent>())
	{
		return Combat->ExecuteAbility(Slots[SlotIndex]);
	}

	return false;
}
