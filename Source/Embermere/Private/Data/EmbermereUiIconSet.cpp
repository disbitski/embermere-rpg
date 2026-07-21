#include "Data/EmbermereUiIconSet.h"

#include "Data/EmbermereItemData.h"
#include "Engine/Texture2D.h"

namespace
{
	UTexture2D* ResolveTexture(const TSoftObjectPtr<UTexture2D>& TextureReference)
	{
		return TextureReference.IsNull() ? nullptr : TextureReference.LoadSynchronous();
	}
}

UTexture2D* UEmbermereUiIconSet::ResolveItemIcon(const UEmbermereItemData* Item) const
{
	if (Item)
	{
		if (UTexture2D* ItemIcon = ResolveTexture(Item->Icon))
		{
			return ItemIcon;
		}

		if (const TSoftObjectPtr<UTexture2D>* CategoryIcon = CategoryFallbackIcons.Find(Item->Category))
		{
			if (UTexture2D* ResolvedCategoryIcon = ResolveTexture(*CategoryIcon))
			{
				return ResolvedCategoryIcon;
			}
		}
	}

	return ResolveTexture(MissingItemIcon);
}

UTexture2D* UEmbermereUiIconSet::ResolveEquipmentSlotIcon(EEmbermereEquipmentSlot Slot) const
{
	if (const TSoftObjectPtr<UTexture2D>* SlotIcon = EquipmentSlotIcons.Find(Slot))
	{
		if (UTexture2D* ResolvedSlotIcon = ResolveTexture(*SlotIcon))
		{
			return ResolvedSlotIcon;
		}
	}

	return ResolveTexture(MissingSlotIcon);
}
