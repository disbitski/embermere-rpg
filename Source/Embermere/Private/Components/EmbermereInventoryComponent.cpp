#include "Components/EmbermereInventoryComponent.h"

UEmbermereInventoryComponent::UEmbermereInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEmbermereInventoryComponent::AddItem(UEmbermereItemData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return false;
	}

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
	OnInventoryChanged.Broadcast();
	return bAddedAll;
}

bool UEmbermereInventoryComponent::RemoveItem(UEmbermereItemData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
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
