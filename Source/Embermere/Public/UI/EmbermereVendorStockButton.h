#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EmbermereVendorStockButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereVendorStockClickedSignature, int32, StockIndex);

UCLASS()
class EMBERMERE_API UEmbermereVendorStockButton : public UButton
{
	GENERATED_BODY()

public:
	UEmbermereVendorStockButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Vendor")
	FEmbermereVendorStockClickedSignature OnVendorStockClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Vendor")
	int32 StockIndex = INDEX_NONE;

	void SetStockIndex(int32 InStockIndex);

private:
	UFUNCTION()
	void HandleClicked();
};
