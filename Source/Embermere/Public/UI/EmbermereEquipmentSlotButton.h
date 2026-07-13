#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Types/EmbermereItemTypes.h"
#include "EmbermereEquipmentSlotButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FEmbermereEquipmentSlotClickedSignature,
	EEmbermereEquipmentSlot,
	EquipmentSlot);

UCLASS()
class EMBERMERE_API UEmbermereEquipmentSlotButton : public UButton
{
	GENERATED_BODY()

public:
	UEmbermereEquipmentSlotButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Equipment")
	FEmbermereEquipmentSlotClickedSignature OnEquipmentSlotClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Equipment")
	EEmbermereEquipmentSlot EquipmentSlot = EEmbermereEquipmentSlot::None;

	void SetEquipmentSlot(EEmbermereEquipmentSlot InEquipmentSlot);

private:
	UFUNCTION()
	void HandleClicked();
};
