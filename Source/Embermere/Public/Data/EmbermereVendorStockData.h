#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmbermereVendorStockData.generated.h"

class UEmbermereItemData;

USTRUCT(BlueprintType)
struct FEmbermereVendorStockEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	TObjectPtr<UEmbermereItemData> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor", meta = (ClampMin = "1"))
	int32 UnitPriceCopper = 1;

	// -1 means the item has unlimited prototype stock.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor", meta = (ClampMin = "-1"))
	int32 InitialQuantity = -1;

	bool IsValid() const
	{
		return Item != nullptr && UnitPriceCopper > 0 && InitialQuantity != 0;
	}
};

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereVendorStockData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	FText VendorName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Vendor")
	TArray<FEmbermereVendorStockEntry> Entries;
};
