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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereVendorStockChangedSignature);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereVendorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereVendorComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	TObjectPtr<UEmbermereVendorStockData> StockData;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Transient, Category = "Vendor")
	TArray<int32> RemainingQuantities;

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

protected:
	virtual void BeginPlay() override;
};
