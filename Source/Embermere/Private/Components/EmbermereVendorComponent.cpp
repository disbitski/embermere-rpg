#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Data/EmbermereItemData.h"

UEmbermereVendorComponent::UEmbermereVendorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereVendorComponent::BeginPlay()
{
	Super::BeginPlay();
	ResetStock();
}

void UEmbermereVendorComponent::SetStockData(UEmbermereVendorStockData* NewStockData)
{
	StockData = NewStockData;
	ResetStock();
}

void UEmbermereVendorComponent::ResetStock()
{
	RemainingQuantities.Reset();
	if (StockData)
	{
		RemainingQuantities.Reserve(StockData->Entries.Num());
		for (const FEmbermereVendorStockEntry& Entry : StockData->Entries)
		{
			RemainingQuantities.Add(Entry.InitialQuantity);
		}
	}
	OnStockChanged.Broadcast();
}

int32 UEmbermereVendorComponent::GetStockEntryCount() const
{
	return StockData ? StockData->Entries.Num() : 0;
}

bool UEmbermereVendorComponent::GetStockEntry(int32 StockIndex, FEmbermereVendorStockEntry& OutEntry) const
{
	if (!StockData || !StockData->Entries.IsValidIndex(StockIndex))
	{
		return false;
	}

	OutEntry = StockData->Entries[StockIndex];
	return true;
}

int32 UEmbermereVendorComponent::GetRemainingQuantity(int32 StockIndex) const
{
	return RemainingQuantities.IsValidIndex(StockIndex) ? RemainingQuantities[StockIndex] : 0;
}

EEmbermereVendorPurchaseResult UEmbermereVendorComponent::CanPurchase(
	int32 StockIndex,
	int32 Quantity,
	const UEmbermereInventoryComponent* Inventory,
	const UEmbermereWalletComponent* Wallet) const
{
	FEmbermereVendorStockEntry Entry;
	if (Quantity <= 0 || !Inventory || !Wallet || !GetStockEntry(StockIndex, Entry) || !Entry.IsValid())
	{
		return EEmbermereVendorPurchaseResult::InvalidRequest;
	}

	const int32 Remaining = GetRemainingQuantity(StockIndex);
	if (Remaining >= 0 && Remaining < Quantity)
	{
		return EEmbermereVendorPurchaseResult::OutOfStock;
	}

	const int64 TotalPrice = static_cast<int64>(Entry.UnitPriceCopper) * Quantity;
	if (TotalPrice <= 0 || TotalPrice > MAX_int32 || !Wallet->CanAfford(static_cast<int32>(TotalPrice)))
	{
		return EEmbermereVendorPurchaseResult::InsufficientFunds;
	}

	if (!Inventory->CanAddItem(Entry.Item, Quantity))
	{
		return EEmbermereVendorPurchaseResult::InventoryFull;
	}

	return EEmbermereVendorPurchaseResult::Success;
}

EEmbermereVendorPurchaseResult UEmbermereVendorComponent::TryPurchase(
	int32 StockIndex,
	int32 Quantity,
	UEmbermereInventoryComponent* Inventory,
	UEmbermereWalletComponent* Wallet)
{
	const EEmbermereVendorPurchaseResult Preflight = CanPurchase(StockIndex, Quantity, Inventory, Wallet);
	if (Preflight != EEmbermereVendorPurchaseResult::Success)
	{
		return Preflight;
	}

	FEmbermereVendorStockEntry Entry;
	GetStockEntry(StockIndex, Entry);
	const int32 TotalPrice = Entry.UnitPriceCopper * Quantity;
	if (!Wallet->TrySpendCopper(TotalPrice))
	{
		return EEmbermereVendorPurchaseResult::InsufficientFunds;
	}

	if (!Inventory->AddItem(Entry.Item, Quantity, false))
	{
		Wallet->AddCopper(TotalPrice);
		return EEmbermereVendorPurchaseResult::InventoryFull;
	}

	if (RemainingQuantities.IsValidIndex(StockIndex) && RemainingQuantities[StockIndex] >= 0)
	{
		RemainingQuantities[StockIndex] -= Quantity;
	}
	OnStockChanged.Broadcast();
	return EEmbermereVendorPurchaseResult::Success;
}

FText UEmbermereVendorComponent::GetPurchaseResultText(
	EEmbermereVendorPurchaseResult Result,
	int32 StockIndex,
	int32 Quantity) const
{
	FEmbermereVendorStockEntry Entry;
	const FString ItemName = GetStockEntry(StockIndex, Entry) && Entry.Item
		? Entry.Item->DisplayName.ToString()
		: TEXT("item");

	switch (Result)
	{
	case EEmbermereVendorPurchaseResult::Success:
		return FText::FromString(FString::Printf(
			TEXT("Purchased %s x%d for %d copper."),
			*ItemName,
			Quantity,
			Entry.UnitPriceCopper * Quantity));
	case EEmbermereVendorPurchaseResult::OutOfStock:
		return FText::FromString(FString::Printf(TEXT("%s is out of stock."), *ItemName));
	case EEmbermereVendorPurchaseResult::InsufficientFunds:
		return FText::FromString(TEXT("You do not have enough copper."));
	case EEmbermereVendorPurchaseResult::InventoryFull:
		return FText::FromString(TEXT("Your inventory cannot hold that purchase."));
	default:
		return FText::FromString(TEXT("That purchase is unavailable."));
	}
}
