#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/EmbermereItemTypes.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereUiIconSet.generated.h"

class UEmbermereItemData;
class UTexture2D;

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereUiIconSet : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TMap<EEmbermereEquipmentSlot, TSoftObjectPtr<UTexture2D>> EquipmentSlotIcons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TMap<EEmbermereItemCategory, TSoftObjectPtr<UTexture2D>> CategoryFallbackIcons;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> MissingItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> MissingSlotIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icons")
	TSoftObjectPtr<UTexture2D> MissingAbilityIcon;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|UI|Icons")
	UTexture2D* ResolveItemIcon(const UEmbermereItemData* Item) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|UI|Icons")
	UTexture2D* ResolveEquipmentSlotIcon(EEmbermereEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|UI|Icons")
	UTexture2D* ResolveAbilityIcon(const FEmbermereAbilityDefinition& Ability) const;
};
