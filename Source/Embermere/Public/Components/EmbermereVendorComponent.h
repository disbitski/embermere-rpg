#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereVendorStockData.h"
#include "EmbermereVendorComponent.generated.h"

class UEmbermereInventoryComponent;
class UEmbermereWalletComponent;

UENUM(BlueprintType)
enum class EEmbermereVendorPurchaseResult : uint8
{
	Success,
	InvalidRequest,
	OutOfStock,
	InsufficientFunds,
	InventoryFull
};

UENUM(BlueprintType)
enum class EEmbermereVendorSellResult : uint8
{
	Success,
	InvalidRequest,
	NotOwned,
	Unsellable,
	WalletFull
};

UENUM(BlueprintType)
enum class EEmbermereVendorBuybackResult : uint8
{
	Success,
	InvalidRequest,
	OutOfStock,
	InsufficientFunds,
	InventoryFull
};

USTRUCT(BlueprintType)
struct FEmbermereVendorBuybackEntry
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vendor")
	TObjectPtr<UEmbermereItemData> Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vendor")
	int32 Quantity = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vendor")
	int32 UnitPriceCopper = 0;

	bool IsValid() const
	{
		return Item && Quantity > 0 && UnitPriceCopper > 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereVendorStockChangedSignature);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereVendorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereVendorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	TObjectPtr<UEmbermereVendorStockData> StockData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor|Persistence")
	FName PersistenceId = NAME_None;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Vendor")
	TArray<int32> RemainingQuantities;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Vendor")
	TArray<FEmbermereVendorBuybackEntry> BuybackEntries;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor", meta = (ClampMin = "1", ClampMax = "16"))
	int32 MaxBuybackEntries = 8;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereVendorStockChangedSignature OnStockChanged;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Vendor")
	void SetStockData(UEmbermereVendorStockData* NewStockData);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Vendor")
	void ResetStock();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	int32 GetStockEntryCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	bool GetStockEntry(int32 StockIndex, FEmbermereVendorStockEntry& OutEntry) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	int32 GetRemainingQuantity(int32 StockIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	EEmbermereVendorPurchaseResult CanPurchase(
		int32 StockIndex,
		int32 Quantity,
		const UEmbermereInventoryComponent* Inventory,
		const UEmbermereWalletComponent* Wallet) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Vendor")
	EEmbermereVendorPurchaseResult TryPurchase(
		int32 StockIndex,
		int32 Quantity,
		UEmbermereInventoryComponent* Inventory,
		UEmbermereWalletComponent* Wallet);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	FText GetPurchaseResultText(EEmbermereVendorPurchaseResult Result, int32 StockIndex, int32 Quantity = 1) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	EEmbermereVendorSellResult CanSell(
		const UEmbermereItemData* Item,
		int32 Quantity,
		const UEmbermereInventoryComponent* Inventory,
		const UEmbermereWalletComponent* Wallet) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Vendor")
	EEmbermereVendorSellResult TrySell(
		UEmbermereItemData* Item,
		int32 Quantity,
		UEmbermereInventoryComponent* Inventory,
		UEmbermereWalletComponent* Wallet);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	FText GetSellResultText(
		EEmbermereVendorSellResult Result,
		const UEmbermereItemData* Item,
		int32 Quantity = 1) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	int32 GetBuybackEntryCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	bool GetBuybackEntry(int32 BuybackIndex, FEmbermereVendorBuybackEntry& OutEntry) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	EEmbermereVendorBuybackResult CanBuyback(
		int32 BuybackIndex,
		int32 Quantity,
		const UEmbermereInventoryComponent* Inventory,
		const UEmbermereWalletComponent* Wallet) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Vendor")
	EEmbermereVendorBuybackResult TryBuyback(
		int32 BuybackIndex,
		int32 Quantity,
		UEmbermereInventoryComponent* Inventory,
		UEmbermereWalletComponent* Wallet);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Vendor")
	FText GetBuybackResultText(
		EEmbermereVendorBuybackResult Result,
		const UEmbermereItemData* Item,
		int32 UnitPriceCopper,
		int32 Quantity = 1) const;

	bool CanRestoreStockForSaveGame(const TArray<int32>& NewRemainingQuantities) const;
	void RestoreStockForSaveGame(const TArray<int32>& NewRemainingQuantities);

protected:
	virtual void BeginPlay() override;

private:
	void RecordBuyback(UEmbermereItemData* Item, int32 Quantity, int32 UnitPriceCopper);
};
