#include "Data/EmbermereItemData.h"

bool UEmbermereItemData::IsEquippable() const
{
	return EquipmentSlot != EEmbermereEquipmentSlot::None &&
		(Category == EEmbermereItemCategory::Weapon || Category == EEmbermereItemCategory::Armor);
}

bool UEmbermereItemData::IsConsumable() const
{
	return Category == EEmbermereItemCategory::Consumable &&
		(ConsumableEffects.HealHealth > 0.0f || ConsumableEffects.RestoreMana > 0.0f);
}

FText UEmbermereItemData::GetCategoryDisplayName() const
{
	switch (Category)
	{
	case EEmbermereItemCategory::Consumable:
		return FText::FromString(TEXT("Consumable"));
	case EEmbermereItemCategory::Weapon:
		return FText::FromString(TEXT("Weapon"));
	case EEmbermereItemCategory::Armor:
		return FText::FromString(TEXT("Armor"));
	case EEmbermereItemCategory::Quest:
		return FText::FromString(TEXT("Quest Item"));
	default:
		return FText::FromString(TEXT("Miscellaneous"));
	}
}

FText UEmbermereItemData::GetEquipmentSlotDisplayName() const
{
	switch (EquipmentSlot)
	{
	case EEmbermereEquipmentSlot::MainHand:
		return FText::FromString(TEXT("Main Hand"));
	case EEmbermereEquipmentSlot::OffHand:
		return FText::FromString(TEXT("Off Hand"));
	case EEmbermereEquipmentSlot::Head:
		return FText::FromString(TEXT("Head"));
	case EEmbermereEquipmentSlot::Chest:
		return FText::FromString(TEXT("Chest"));
	case EEmbermereEquipmentSlot::Hands:
		return FText::FromString(TEXT("Hands"));
	case EEmbermereEquipmentSlot::Legs:
		return FText::FromString(TEXT("Legs"));
	case EEmbermereEquipmentSlot::Feet:
		return FText::FromString(TEXT("Feet"));
	case EEmbermereEquipmentSlot::Back:
		return FText::FromString(TEXT("Back"));
	case EEmbermereEquipmentSlot::Neck:
		return FText::FromString(TEXT("Neck"));
	case EEmbermereEquipmentSlot::Ring:
		return FText::FromString(TEXT("Ring"));
	default:
		return FText::FromString(TEXT("None"));
	}
}

FText UEmbermereItemData::GetPrimaryActionLabel() const
{
	if (IsEquippable())
	{
		return FText::FromString(TEXT("Equip"));
	}
	if (Category == EEmbermereItemCategory::Consumable)
	{
		return FText::FromString(TEXT("Use"));
	}
	return FText::FromString(TEXT("Inspect"));
}

FText UEmbermereItemData::GetEffectSummary() const
{
	TArray<FString> Effects;
	if (IsEquippable())
	{
		if (!FMath::IsNearlyZero(StatBonuses.MaxHealth))
		{
			Effects.Add(FString::Printf(TEXT("%+.0f HP"), StatBonuses.MaxHealth));
		}
		if (!FMath::IsNearlyZero(StatBonuses.MaxMana))
		{
			Effects.Add(FString::Printf(TEXT("%+.0f Mana"), StatBonuses.MaxMana));
		}
		if (!FMath::IsNearlyZero(StatBonuses.Armor))
		{
			Effects.Add(FString::Printf(TEXT("%+.0f Armor"), StatBonuses.Armor));
		}
		if (!FMath::IsNearlyZero(StatBonuses.Power))
		{
			Effects.Add(FString::Printf(TEXT("%+.0f Power"), StatBonuses.Power));
		}
	}
	else if (IsConsumable())
	{
		if (ConsumableEffects.HealHealth > 0.0f)
		{
			Effects.Add(FString::Printf(TEXT("Restores %.0f HP"), ConsumableEffects.HealHealth));
		}
		if (ConsumableEffects.RestoreMana > 0.0f)
		{
			Effects.Add(FString::Printf(TEXT("Restores %.0f Mana"), ConsumableEffects.RestoreMana));
		}
	}

	return FText::FromString(Effects.Num() > 0 ? FString::Join(Effects, TEXT(", ")) : TEXT("No gameplay effects"));
}
