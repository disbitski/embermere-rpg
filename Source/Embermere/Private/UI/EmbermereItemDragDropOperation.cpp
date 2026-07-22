#include "UI/EmbermereItemDragDropOperation.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Data/EmbermereItemData.h"
#include "Engine/Texture2D.h"

namespace
{
	UTextBlock* MakeDragText(UObject* Owner, const FText& Text, const FLinearColor& Color, int32 FontSize)
	{
		UTextBlock* TextBlock = NewObject<UTextBlock>(Owner);
		if (!TextBlock)
		{
			return nullptr;
		}

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetText(Text);
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
		TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
		return TextBlock;
	}
}

FText UEmbermereItemDragDropOperation::GetVisualSigilText() const
{
	if (!Item)
	{
		return FText::FromString(TEXT("ITEM"));
	}

	switch (Item->Category)
	{
	case EEmbermereItemCategory::Armor: return FText::FromString(TEXT("ARM"));
	case EEmbermereItemCategory::Weapon: return FText::FromString(TEXT("WPN"));
	case EEmbermereItemCategory::Consumable: return FText::FromString(TEXT("USE"));
	case EEmbermereItemCategory::Quest: return FText::FromString(TEXT("QST"));
	default: return FText::FromString(TEXT("ITEM"));
	}
}

FText UEmbermereItemDragDropOperation::GetVisualContextText() const
{
	if (!Item)
	{
		return FText::GetEmpty();
	}

	if (Item->IsEquippable())
	{
		return FText::FromString(FString::Printf(
			TEXT("%s | Level %d"),
			*Item->GetEquipmentSlotDisplayName().ToString(),
			Item->RequiredLevel));
	}
	if (Item->IsConsumable())
	{
		return FText::FromString(FString::Printf(
			TEXT("%s | %s"),
			*Item->GetCategoryDisplayName().ToString(),
			*Item->GetEffectSummary().ToString()));
	}
	return Item->GetCategoryDisplayName();
}

FLinearColor UEmbermereItemDragDropOperation::GetVisualAccentColor() const
{
	if (!Item)
	{
		return FLinearColor(0.92f, 0.68f, 0.22f, 1.0f);
	}

	switch (Item->Category)
	{
	case EEmbermereItemCategory::Armor: return FLinearColor(0.95f, 0.68f, 0.20f, 1.0f);
	case EEmbermereItemCategory::Weapon: return FLinearColor(1.0f, 0.34f, 0.14f, 1.0f);
	case EEmbermereItemCategory::Consumable: return FLinearColor(0.36f, 0.78f, 0.42f, 1.0f);
	case EEmbermereItemCategory::Quest: return FLinearColor(0.34f, 0.68f, 1.0f, 1.0f);
	default: return FLinearColor(0.78f, 0.76f, 0.68f, 1.0f);
	}
}

UWidget* UEmbermereItemDragDropOperation::CreateDragVisual()
{
	if (!Item)
	{
		return nullptr;
	}

	const FLinearColor Accent = GetVisualAccentColor();
	USizeBox* RootSize = NewObject<USizeBox>(this);
	UBorder* OuterBorder = NewObject<UBorder>(RootSize);
	UBorder* InnerBorder = NewObject<UBorder>(OuterBorder);
	UHorizontalBox* Row = NewObject<UHorizontalBox>(InnerBorder);
	USizeBox* SigilSize = NewObject<USizeBox>(Row);
	UBorder* SigilBorder = NewObject<UBorder>(SigilSize);
	UImage* IconImage = ResolvedIcon ? NewObject<UImage>(SigilBorder) : nullptr;
	UVerticalBox* Details = NewObject<UVerticalBox>(Row);
	if (!RootSize || !OuterBorder || !InnerBorder || !Row || !SigilSize || !SigilBorder || !Details)
	{
		return nullptr;
	}

	RootSize->SetWidthOverride(236.0f);
	RootSize->SetHeightOverride(62.0f);
	RootSize->SetVisibility(ESlateVisibility::HitTestInvisible);
	RootSize->AddChild(OuterBorder);
	OuterBorder->SetBrushColor(Accent);
	OuterBorder->SetPadding(FMargin(2.0f));
	OuterBorder->SetContent(InnerBorder);
	InnerBorder->SetBrushColor(FLinearColor(0.025f, 0.035f, 0.027f, 0.97f));
	InnerBorder->SetPadding(FMargin(6.0f, 5.0f));
	InnerBorder->SetContent(Row);

	SigilSize->SetWidthOverride(44.0f);
	SigilSize->SetHeightOverride(44.0f);
	SigilSize->AddChild(SigilBorder);
	SigilBorder->SetBrushColor(FLinearColor(Accent.R * 0.2f, Accent.G * 0.2f, Accent.B * 0.2f, 1.0f));
	SigilBorder->SetPadding(FMargin(4.0f));
	if (IconImage)
	{
		IconImage->SetBrushFromTexture(ResolvedIcon, true);
		IconImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		SigilBorder->SetContent(IconImage);
	}
	else if (UTextBlock* SigilText = MakeDragText(SigilBorder, GetVisualSigilText(), Accent, 11))
	{
		SigilText->SetJustification(ETextJustify::Center);
		SigilBorder->SetContent(SigilText);
	}
	if (UHorizontalBoxSlot* SigilSlot = Row->AddChildToHorizontalBox(SigilSize))
	{
		SigilSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		SigilSlot->SetVerticalAlignment(VAlign_Center);
	}
	if (UHorizontalBoxSlot* DetailsSlot = Row->AddChildToHorizontalBox(Details))
	{
		DetailsSlot->SetVerticalAlignment(VAlign_Center);
		DetailsSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	if (UTextBlock* NameText = MakeDragText(Details, Item->DisplayName, FLinearColor(1.0f, 0.9f, 0.62f, 1.0f), 14))
	{
		if (UVerticalBoxSlot* NameSlot = Details->AddChildToVerticalBox(NameText))
		{
			NameSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 2.0f));
		}
	}
	if (UTextBlock* ContextText = MakeDragText(
		Details,
		GetVisualContextText(),
		FLinearColor(0.76f, 0.79f, 0.68f, 1.0f),
		10))
	{
		Details->AddChildToVerticalBox(ContextText);
	}

	return RootSize;
}
