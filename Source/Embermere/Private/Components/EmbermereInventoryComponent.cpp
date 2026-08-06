#include "Components/EmbermereInventoryComponent.h"
#include "Engine/Engine.h"

namespace
{
	int32 GetInventoryCategorySortPriority(const EEmbermereItemCategory Category)
	{
		switch (Category)
		{
		case EEmbermereItemCategory::Weapon: return 0;
		case EEmbermereItemCategory::Armor: return 1;
		case EEmbermereItemCategory::Consumable: return 2;
		case EEmbermereItemCategory::Quest: return 3;
		case EEmbermereItemCategory::Misc: return 4;
		default: return 5;
		}
	}
}

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

bool UEmbermereInventoryComponent::SortStacksByCategoryAndName()
{
	if (Stacks.Num() <= 1)
	{
		return false;
	}

	const TArray<FEmbermereInventoryStack> OriginalOrder = Stacks;
	Stacks.StableSort([](const FEmbermereInventoryStack& Left, const FEmbermereInventoryStack& Right)
	{
		if (!Left.Item || !Right.Item)
		{
			return Left.Item != nullptr && Right.Item == nullptr;
		}

		const int32 LeftPriority = GetInventoryCategorySortPriority(Left.Item->Category);
		const int32 RightPriority = GetInventoryCategorySortPriority(Right.Item->Category);
		if (LeftPriority != RightPriority)
		{
			return LeftPriority < RightPriority;
		}

		return Left.Item->DisplayName.ToString().Compare(
			Right.Item->DisplayName.ToString(),
			ESearchCase::IgnoreCase) < 0;
	});

	bool bOrderChanged = false;
	for (int32 Index = 0; Index < Stacks.Num(); ++Index)
	{
		if (OriginalOrder[Index].Item != Stacks[Index].Item ||
			OriginalOrder[Index].Quantity != Stacks[Index].Quantity)
		{
			bOrderChanged = true;
			break;
		}
	}

	if (bOrderChanged)
	{
		OnInventoryChanged.Broadcast();
	}
	return bOrderChanged;
}

bool UEmbermereInventoryComponent::CanRestoreStacksForSaveGame(
	const TArray<FEmbermereInventoryStack>& NewStacks) const
{
	if (NewStacks.Num() > MaxSlots)
	{
		return false;
	}

	for (const FEmbermereInventoryStack& Stack : NewStacks)
	{
		if (!Stack.Item || Stack.Item->MaxStack <= 0 ||
			Stack.Quantity <= 0 || Stack.Quantity > Stack.Item->MaxStack)
		{
			return false;
		}
	}
	return true;
}

void UEmbermereInventoryComponent::RestoreStacksForSaveGame(
	const TArray<FEmbermereInventoryStack>& NewStacks)
{
	Stacks = NewStacks;
	OnInventoryChanged.Broadcast();
}
