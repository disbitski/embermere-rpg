#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Types/EmbermereItemTypes.h"
#include "EmbermereItemDragDropOperation.generated.h"

class UEmbermereItemData;
class UWidget;

UENUM(BlueprintType)
enum class EEmbermereItemDragSource : uint8
{
	None,
	Inventory,
	Equipment
};

UCLASS()
class EMBERMERE_API UEmbermereItemDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Inventory")
	TObjectPtr<UEmbermereItemData> Item;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Inventory")
	EEmbermereItemDragSource Source = EEmbermereItemDragSource::None;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Inventory")
	EEmbermereEquipmentSlot SourceEquipmentSlot = EEmbermereEquipmentSlot::None;

	FText GetVisualSigilText() const;
	FText GetVisualContextText() const;
	FLinearColor GetVisualAccentColor() const;
	UWidget* CreateDragVisual();
};
