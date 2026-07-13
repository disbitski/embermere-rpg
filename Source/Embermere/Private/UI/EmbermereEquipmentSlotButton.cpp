#include "UI/EmbermereEquipmentSlotButton.h"

UEmbermereEquipmentSlotButton::UEmbermereEquipmentSlotButton(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitIsFocusable(false);
	OnClicked.AddUniqueDynamic(this, &UEmbermereEquipmentSlotButton::HandleClicked);
}

void UEmbermereEquipmentSlotButton::SetEquipmentSlot(EEmbermereEquipmentSlot InEquipmentSlot)
{
	EquipmentSlot = InEquipmentSlot;
}

void UEmbermereEquipmentSlotButton::HandleClicked()
{
	if (EquipmentSlot != EEmbermereEquipmentSlot::None)
	{
		OnEquipmentSlotClicked.Broadcast(EquipmentSlot);
	}
}
