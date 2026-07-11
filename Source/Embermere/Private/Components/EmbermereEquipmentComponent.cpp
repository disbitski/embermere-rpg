#include "Components/EmbermereEquipmentComponent.h"

UEmbermereEquipmentComponent::UEmbermereEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEmbermereEquipmentComponent::CanEquip(const UEmbermereItemData* Item, int32 CharacterLevel) const
{
	return Item && Item->IsEquippable() && CharacterLevel >= FMath::Max(1, Item->RequiredLevel);
}

bool UEmbermereEquipmentComponent::EquipItem(UEmbermereItemData* Item, int32 CharacterLevel)
{
	if (!CanEquip(Item, CharacterLevel))
	{
		return false;
	}

	for (FEmbermereEquippedItem& EquippedItem : EquippedItems)
	{
		if (EquippedItem.Slot == Item->EquipmentSlot)
		{
			if (EquippedItem.Item == Item)
			{
				return true;
			}
			EquippedItem.Item = Item;
			OnEquipmentChanged.Broadcast();
			return true;
		}
	}

	FEmbermereEquippedItem EquippedItem;
	EquippedItem.Slot = Item->EquipmentSlot;
	EquippedItem.Item = Item;
	EquippedItems.Add(EquippedItem);
	OnEquipmentChanged.Broadcast();
	return true;
}

UEmbermereItemData* UEmbermereEquipmentComponent::UnequipItem(EEmbermereEquipmentSlot Slot)
{
	for (int32 Index = 0; Index < EquippedItems.Num(); ++Index)
	{
		if (EquippedItems[Index].Slot != Slot)
		{
			continue;
		}

		UEmbermereItemData* RemovedItem = EquippedItems[Index].Item;
		EquippedItems.RemoveAt(Index);
		OnEquipmentChanged.Broadcast();
		return RemovedItem;
	}

	return nullptr;
}

UEmbermereItemData* UEmbermereEquipmentComponent::GetEquippedItem(EEmbermereEquipmentSlot Slot) const
{
	for (const FEmbermereEquippedItem& EquippedItem : EquippedItems)
	{
		if (EquippedItem.Slot == Slot)
		{
			return EquippedItem.Item;
		}
	}
	return nullptr;
}

bool UEmbermereEquipmentComponent::IsItemEquipped(const UEmbermereItemData* Item) const
{
	return Item && GetEquippedItem(Item->EquipmentSlot) == Item;
}
