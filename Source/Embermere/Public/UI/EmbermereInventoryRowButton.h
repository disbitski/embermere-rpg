#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EmbermereInventoryRowButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereInventoryRowClickedSignature, int32, VisibleRowIndex);

UCLASS()
class EMBERMERE_API UEmbermereInventoryRowButton : public UButton
{
	GENERATED_BODY()

public:
	UEmbermereInventoryRowButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Inventory")
	FEmbermereInventoryRowClickedSignature OnInventoryRowClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Inventory")
	int32 VisibleRowIndex = INDEX_NONE;

	void SetVisibleRowIndex(int32 InVisibleRowIndex);

private:
	UFUNCTION()
	void HandleClicked();
};
