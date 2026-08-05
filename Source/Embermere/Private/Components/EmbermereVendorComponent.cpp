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
	BuybackEntries.Reset();
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

EEmbermereVendorSellResult UEmbermereVendorComponent::CanSell(
	const UEmbermereItemData* Item,
	int32 Quantity,
	const UEmbermereInventoryComponent* Inventory,
	const UEmbermereWalletComponent* Wallet) const
{
	if (!Item || Quantity <= 0 || !Inventory || !Wallet)
	{
		return EEmbermereVendorSellResult::InvalidRequest;
	}

	if (Item->Category == EEmbermereItemCategory::Quest || Item->SellValueCopper <= 0)
	{
		return EEmbermereVendorSellResult::Unsellable;
	}

	if (!Inventory->CanRemoveItem(Item, Quantity))
	{
		return EEmbermereVendorSellResult::NotOwned;
	}

	const int64 TotalValue = static_cast<int64>(Item->SellValueCopper) * Quantity;
	if (TotalValue <= 0 || TotalValue > MAX_int32)
	{
		return EEmbermereVendorSellResult::InvalidRequest;
	}

	for (const FEmbermereVendorBuybackEntry& Entry : BuybackEntries)
	{
		if (Entry.Item == Item && Entry.UnitPriceCopper == Item->SellValueCopper &&
			Entry.Quantity > MAX_int32 - Quantity)
		{
			return EEmbermereVendorSellResult::InvalidRequest;
		}
	}

	return Wallet->CanAddCopper(static_cast<int32>(TotalValue))
		? EEmbermereVendorSellResult::Success
		: EEmbermereVendorSellResult::WalletFull;
}

EEmbermereVendorSellResult UEmbermereVendorComponent::TrySell(
	UEmbermereItemData* Item,
	int32 Quantity,
	UEmbermereInventoryComponent* Inventory,
	UEmbermereWalletComponent* Wallet)
{
	const EEmbermereVendorSellResult Preflight = CanSell(Item, Quantity, Inventory, Wallet);
	if (Preflight != EEmbermereVendorSellResult::Success)
	{
		return Preflight;
	}

	const int32 TotalValue = Item->SellValueCopper * Quantity;
	if (!Wallet->AddCopper(TotalValue))
	{
		return EEmbermereVendorSellResult::WalletFull;
	}

	if (!Inventory->RemoveItem(Item, Quantity))
	{
		Wallet->TrySpendCopper(TotalValue);
		return EEmbermereVendorSellResult::NotOwned;
	}

	RecordBuyback(Item, Quantity, Item->SellValueCopper);
	OnStockChanged.Broadcast();
	return EEmbermereVendorSellResult::Success;
}

FText UEmbermereVendorComponent::GetSellResultText(
	EEmbermereVendorSellResult Result,
	const UEmbermereItemData* Item,
	int32 Quantity) const
{
	const FString ItemName = Item ? Item->DisplayName.ToString() : TEXT("item");
	const int32 UnitValue = Item ? Item->SellValueCopper : 0;

	switch (Result)
	{
	case EEmbermereVendorSellResult::Success:
		return FText::FromString(FString::Printf(
			TEXT("Sold %s x%d for %d copper."),
			*ItemName,
			Quantity,
			UnitValue * Quantity));
	case EEmbermereVendorSellResult::NotOwned:
		return FText::FromString(TEXT("That item is no longer in your bag."));
	case EEmbermereVendorSellResult::Unsellable:
		return FText::FromString(FString::Printf(TEXT("%s cannot be sold."), *ItemName));
	case EEmbermereVendorSellResult::WalletFull:
		return FText::FromString(TEXT("Your purse cannot hold more copper."));
	default:
		return FText::FromString(TEXT("That sale is unavailable."));
	}
}

int32 UEmbermereVendorComponent::GetBuybackEntryCount() const
{
	return BuybackEntries.Num();
}

bool UEmbermereVendorComponent::GetBuybackEntry(
	int32 BuybackIndex,
	FEmbermereVendorBuybackEntry& OutEntry) const
{
	if (!BuybackEntries.IsValidIndex(BuybackIndex) || !BuybackEntries[BuybackIndex].IsValid())
	{
		return false;
	}

	OutEntry = BuybackEntries[BuybackIndex];
	return true;
}

EEmbermereVendorBuybackResult UEmbermereVendorComponent::CanBuyback(
	int32 BuybackIndex,
	int32 Quantity,
	const UEmbermereInventoryComponent* Inventory,
	const UEmbermereWalletComponent* Wallet) const
{
	FEmbermereVendorBuybackEntry Entry;
	if (Quantity <= 0 || !Inventory || !Wallet || !GetBuybackEntry(BuybackIndex, Entry))
	{
		return EEmbermereVendorBuybackResult::InvalidRequest;
	}

	if (Entry.Quantity < Quantity)
	{
		return EEmbermereVendorBuybackResult::OutOfStock;
	}

	const int64 TotalPrice = static_cast<int64>(Entry.UnitPriceCopper) * Quantity;
	if (TotalPrice <= 0 || TotalPrice > MAX_int32 || !Wallet->CanAfford(static_cast<int32>(TotalPrice)))
	{
		return EEmbermereVendorBuybackResult::InsufficientFunds;
	}

	return Inventory->CanAddItem(Entry.Item, Quantity)
		? EEmbermereVendorBuybackResult::Success
		: EEmbermereVendorBuybackResult::InventoryFull;
}

EEmbermereVendorBuybackResult UEmbermereVendorComponent::TryBuyback(
	int32 BuybackIndex,
	int32 Quantity,
	UEmbermereInventoryComponent* Inventory,
	UEmbermereWalletComponent* Wallet)
{
	const EEmbermereVendorBuybackResult Preflight = CanBuyback(BuybackIndex, Quantity, Inventory, Wallet);
	if (Preflight != EEmbermereVendorBuybackResult::Success)
	{
		return Preflight;
	}

	FEmbermereVendorBuybackEntry Entry;
	GetBuybackEntry(BuybackIndex, Entry);
	const int32 TotalPrice = Entry.UnitPriceCopper * Quantity;
	if (!Wallet->TrySpendCopper(TotalPrice))
	{
		return EEmbermereVendorBuybackResult::InsufficientFunds;
	}

	if (!Inventory->AddItem(Entry.Item, Quantity, false))
	{
		Wallet->AddCopper(TotalPrice);
		return EEmbermereVendorBuybackResult::InventoryFull;
	}

	BuybackEntries[BuybackIndex].Quantity -= Quantity;
	if (BuybackEntries[BuybackIndex].Quantity <= 0)
	{
		BuybackEntries.RemoveAt(BuybackIndex);
	}
	OnStockChanged.Broadcast();
	return EEmbermereVendorBuybackResult::Success;
}

FText UEmbermereVendorComponent::GetBuybackResultText(
	EEmbermereVendorBuybackResult Result,
	const UEmbermereItemData* Item,
	int32 UnitPriceCopper,
	int32 Quantity) const
{
	const FString ItemName = Item ? Item->DisplayName.ToString() : TEXT("item");

	switch (Result)
	{
	case EEmbermereVendorBuybackResult::Success:
		return FText::FromString(FString::Printf(
			TEXT("Bought back %s x%d for %d copper."),
			*ItemName,
			Quantity,
			UnitPriceCopper * Quantity));
	case EEmbermereVendorBuybackResult::OutOfStock:
		return FText::FromString(TEXT("That buyback item is no longer available."));
	case EEmbermereVendorBuybackResult::InsufficientFunds:
		return FText::FromString(TEXT("You do not have enough copper to buy that item back."));
	case EEmbermereVendorBuybackResult::InventoryFull:
		return FText::FromString(TEXT("Your inventory cannot hold that buyback."));
	default:
		return FText::FromString(TEXT("That buyback is unavailable."));
	}
}

void UEmbermereVendorComponent::RecordBuyback(
	UEmbermereItemData* Item,
	int32 Quantity,
	int32 UnitPriceCopper)
{
	for (int32 Index = 0; Index < BuybackEntries.Num(); ++Index)
	{
		FEmbermereVendorBuybackEntry& Existing = BuybackEntries[Index];
		if (Existing.Item != Item || Existing.UnitPriceCopper != UnitPriceCopper)
		{
			continue;
		}

		Existing.Quantity += Quantity;
		const FEmbermereVendorBuybackEntry Updated = Existing;
		BuybackEntries.RemoveAt(Index);
		BuybackEntries.Insert(Updated, 0);
		return;
	}

	FEmbermereVendorBuybackEntry Entry;
	Entry.Item = Item;
	Entry.Quantity = Quantity;
	Entry.UnitPriceCopper = UnitPriceCopper;
	BuybackEntries.Insert(Entry, 0);
	BuybackEntries.SetNum(FMath::Min(BuybackEntries.Num(), FMath::Max(1, MaxBuybackEntries)));
}
