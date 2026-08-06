#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereInventoryComponent.h"

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

bool UEmbermereEquipmentComponent::EquipFromInventory(
	UEmbermereItemData* Item,
	int32 CharacterLevel,
	UEmbermereInventoryComponent* Inventory)
{
	if (!Inventory || !CanEquip(Item, CharacterLevel))
	{
		return false;
	}

	UEmbermereItemData* ReplacedItem = GetEquippedItem(Item->EquipmentSlot);
	if (ReplacedItem == Item)
	{
		return true;
	}
	if (!Inventory->RemoveItem(Item, 1))
	{
		return false;
	}

	if (ReplacedItem && !Inventory->CanAddItem(ReplacedItem, 1))
	{
		Inventory->AddItem(Item, 1, false);
		return false;
	}
	if (ReplacedItem && !Inventory->AddItem(ReplacedItem, 1, false))
	{
		Inventory->AddItem(Item, 1, false);
		return false;
	}
	if (!EquipItem(Item, CharacterLevel))
	{
		if (ReplacedItem)
		{
			Inventory->RemoveItem(ReplacedItem, 1);
		}
		Inventory->AddItem(Item, 1, false);
		return false;
	}
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

bool UEmbermereEquipmentComponent::UnequipToInventory(
	EEmbermereEquipmentSlot Slot,
	UEmbermereInventoryComponent* Inventory)
{
	UEmbermereItemData* Item = GetEquippedItem(Slot);
	if (!Inventory || !Item || !Inventory->CanAddItem(Item, 1))
	{
		return false;
	}
	if (!Inventory->AddItem(Item, 1, false))
	{
		return false;
	}
	if (UnequipItem(Slot) != Item)
	{
		Inventory->RemoveItem(Item, 1);
		return false;
	}
	return true;
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

FEmbermereItemStatBonuses UEmbermereEquipmentComponent::GetTotalStatBonuses() const
{
	FEmbermereItemStatBonuses Total;
	for (const FEmbermereEquippedItem& EquippedItem : EquippedItems)
	{
		if (!EquippedItem.Item)
		{
			continue;
		}

		Total.MaxHealth += EquippedItem.Item->StatBonuses.MaxHealth;
		Total.MaxMana += EquippedItem.Item->StatBonuses.MaxMana;
		Total.Armor += EquippedItem.Item->StatBonuses.Armor;
		Total.Power += EquippedItem.Item->StatBonuses.Power;
	}
	return Total;
}

bool UEmbermereEquipmentComponent::CanRestoreEquippedItemsForSaveGame(
	const TArray<FEmbermereEquippedItem>& NewEquippedItems,
	int32 CharacterLevel) const
{
	TSet<EEmbermereEquipmentSlot> SeenSlots;
	for (const FEmbermereEquippedItem& EquippedItem : NewEquippedItems)
	{
		if (EquippedItem.Slot == EEmbermereEquipmentSlot::None ||
			!EquippedItem.Item ||
			EquippedItem.Item->EquipmentSlot != EquippedItem.Slot ||
			!CanEquip(EquippedItem.Item, CharacterLevel) ||
			SeenSlots.Contains(EquippedItem.Slot))
		{
			return false;
		}
		SeenSlots.Add(EquippedItem.Slot);
	}
	return true;
}

void UEmbermereEquipmentComponent::RestoreEquippedItemsForSaveGame(
	const TArray<FEmbermereEquippedItem>& NewEquippedItems)
{
	EquippedItems = NewEquippedItems;
	OnEquipmentChanged.Broadcast();
}
