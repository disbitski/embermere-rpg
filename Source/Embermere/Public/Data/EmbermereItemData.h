#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/EmbermereItemTypes.h"
#include "EmbermereItemData.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	int32 MaxStack = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	EEmbermereItemCategory Category = EEmbermereItemCategory::Misc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	EEmbermereEquipmentSlot EquipmentSlot = EEmbermereEquipmentSlot::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	FEmbermereItemStatBonuses StatBonuses;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Assets")
	TSoftObjectPtr<UTexture2D> Icon;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Item")
	bool IsEquippable() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Item")
	FText GetCategoryDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Item")
	FText GetEquipmentSlotDisplayName() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Item")
	FText GetPrimaryActionLabel() const;
};
