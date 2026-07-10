#include "UI/EmbermereInventoryRowButton.h"

UEmbermereInventoryRowButton::UEmbermereInventoryRowButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitIsFocusable(false);
	OnClicked.AddUniqueDynamic(this, &UEmbermereInventoryRowButton::HandleClicked);
}

void UEmbermereInventoryRowButton::SetVisibleRowIndex(int32 InVisibleRowIndex)
{
	VisibleRowIndex = InVisibleRowIndex;
}

void UEmbermereInventoryRowButton::HandleClicked()
{
	if (VisibleRowIndex != INDEX_NONE)
	{
		OnInventoryRowClicked.Broadcast(VisibleRowIndex);
	}
}
