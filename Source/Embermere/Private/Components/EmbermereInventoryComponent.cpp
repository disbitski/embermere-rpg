#include "Components/EmbermereInventoryComponent.h"
#include "Engine/Engine.h"

UEmbermereInventoryComponent::UEmbermereInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEmbermereInventoryComponent::CanAddItem(const UEmbermereItemData* Item, int32 Quantity) const
{
	if (!Item || Quantity <= 0 || Item->MaxStack <= 0)
	{
		return false;
	}

	int32 AvailableCapacity = FMath::Max(0, MaxSlots - Stacks.Num()) * Item->MaxStack;
	for (const FEmbermereInventoryStack& Stack : Stacks)
	{
		if (Stack.Item == Item)
		{
			AvailableCapacity += FMath::Max(0, Item->MaxStack - Stack.Quantity);
		}
	}
	return AvailableCapacity >= Quantity;
}

int32 UEmbermereInventoryComponent::GetItemQuantity(const UEmbermereItemData* Item) const
{
	int32 TotalQuantity = 0;
	for (const FEmbermereInventoryStack& Stack : Stacks)
	{
		if (Stack.Item == Item)
		{
			TotalQuantity += FMath::Max(0, Stack.Quantity);
		}
	}
	return TotalQuantity;
}

bool UEmbermereInventoryComponent::CanRemoveItem(const UEmbermereItemData* Item, int32 Quantity) const
{
	return Item && Quantity > 0 && GetItemQuantity(Item) >= Quantity;
}

bool UEmbermereInventoryComponent::AddItem(UEmbermereItemData* Item, int32 Quantity, bool bNotifyItemAdded)
{
	if (!CanAddItem(Item, Quantity))
	{
		return false;
	}

	const int32 RequestedQuantity = Quantity;

	for (FEmbermereInventoryStack& Stack : Stacks)
	{
		if (Stack.Item == Item && Stack.Quantity < Item->MaxStack)
		{
			const int32 AvailableSpace = Item->MaxStack - Stack.Quantity;
			const int32 Added = FMath::Min(AvailableSpace, Quantity);
			Stack.Quantity += Added;
			Quantity -= Added;
			if (Quantity <= 0)
			{
				OnInventoryChanged.Broadcast();
				if (bNotifyItemAdded)
				{
					OnItemAdded.Broadcast(Item, RequestedQuantity);
				}
				return true;
			}
		}
	}

	while (Quantity > 0 && Stacks.Num() < MaxSlots)
	{
		FEmbermereInventoryStack NewStack;
		NewStack.Item = Item;
		NewStack.Quantity = FMath::Min(Item->MaxStack, Quantity);
		Quantity -= NewStack.Quantity;
		Stacks.Add(NewStack);
	}

	const bool bAddedAll = Quantity <= 0;
	const int32 AddedQuantity = RequestedQuantity - Quantity;
	if (AddedQuantity > 0)
	{
		OnInventoryChanged.Broadcast();
		if (bNotifyItemAdded)
		{
			OnItemAdded.Broadcast(Item, AddedQuantity);
		}
	}
	return bAddedAll;
}

bool UEmbermereInventoryComponent::RemoveItem(UEmbermereItemData* Item, int32 Quantity)
{
	if (!CanRemoveItem(Item, Quantity))
	{
		return false;
	}

	for (int32 Index = Stacks.Num() - 1; Index >= 0 && Quantity > 0; --Index)
	{
		FEmbermereInventoryStack& Stack = Stacks[Index];
		if (Stack.Item != Item)
		{
			continue;
		}

		const int32 Removed = FMath::Min(Stack.Quantity, Quantity);
		Stack.Quantity -= Removed;
		Quantity -= Removed;

		if (Stack.Quantity <= 0)
		{
			Stacks.RemoveAt(Index);
		}
	}

	const bool bRemovedAll = Quantity <= 0;
	if (bRemovedAll)
	{
		OnInventoryChanged.Broadcast();
	}
	return bRemovedAll;
}
