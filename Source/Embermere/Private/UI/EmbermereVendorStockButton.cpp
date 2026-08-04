#include "UI/EmbermereVendorStockButton.h"

UEmbermereVendorStockButton::UEmbermereVendorStockButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitIsFocusable(false);
	OnClicked.AddUniqueDynamic(this, &UEmbermereVendorStockButton::HandleClicked);
}

void UEmbermereVendorStockButton::SetStockIndex(int32 InStockIndex)
{
	StockIndex = InStockIndex;
}

void UEmbermereVendorStockButton::HandleClicked()
{
	if (StockIndex != INDEX_NONE)
	{
		OnVendorStockClicked.Broadcast(StockIndex);
	}
}
