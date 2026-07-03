#include "UI/EmbermereEnemyNameplateWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"

namespace
{
	UTextBlock* MakeNameplateText(UWidgetTree* WidgetTree, const FName Name, const FLinearColor Color, const float FontSize)
	{
		UTextBlock* TextBlock = WidgetTree ? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name) : nullptr;
		if (!TextBlock)
		{
			return nullptr;
		}

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetColorAndOpacity(FSlateColor(Color));
		TextBlock->SetJustification(ETextJustify::Center);
		TextBlock->SetShadowOffset(FVector2D(1.25f, 1.25f));
		TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
		return TextBlock;
	}

	void AddNameplateChild(UVerticalBox* Stack, UWidget* Child, const float BottomPadding)
	{
		if (!Stack || !Child)
		{
			return;
		}

		if (UVerticalBoxSlot* Slot = Stack->AddChildToVerticalBox(Child))
		{
			Slot->SetHorizontalAlignment(HAlign_Fill);
			Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, BottomPadding));
		}
	}
}

TSharedRef<SWidget> UEmbermereEnemyNameplateWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UEmbermereEnemyNameplateWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	UpdateVisibility();
}

void UEmbermereEnemyNameplateWidget::SetNameplateState(
	const FText& DisplayName,
	float CurrentHealth,
	float MaxHealth,
	const FLinearColor& AccentColor,
	bool bIsSelected)
{
	bSelected = bIsSelected;
	BuildDefaultLayout();

	const float SafeMaxHealth = FMath::Max(0.0f, MaxHealth);
	const float ClampedHealth = FMath::Clamp(CurrentHealth, 0.0f, SafeMaxHealth);
	const float HealthPercent = SafeMaxHealth > 0.0f ? ClampedHealth / SafeMaxHealth : 0.0f;

	if (RootPanel)
	{
		RootPanel->SetBrushColor(FLinearColor(0.015f, 0.014f, 0.012f, bIsSelected ? 0.86f : 0.0f));
	}
	if (SelectedMarkerText)
	{
		SelectedMarkerText->SetText(FText::FromString(TEXT("v")));
		SelectedMarkerText->SetColorAndOpacity(FSlateColor(AccentColor));
	}
	if (NameText)
	{
		NameText->SetText(DisplayName);
		NameText->SetColorAndOpacity(FSlateColor(AccentColor));
	}
	if (HealthText)
	{
		HealthText->SetText(FText::FromString(FString::Printf(TEXT("HP %.0f / %.0f"), ClampedHealth, SafeMaxHealth)));
	}
	if (HealthBar)
	{
		HealthBar->SetPercent(HealthPercent);
		HealthBar->SetFillColorAndOpacity(AccentColor);
	}

	UpdateVisibility();
}

bool UEmbermereEnemyNameplateWidget::IsNameplateSelected() const
{
	return bSelected;
}

void UEmbermereEnemyNameplateWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	USizeBox* RootSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("NameplateRootSize"));
	RootPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("NameplatePanel"));
	UVerticalBox* Stack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("NameplateStack"));
	if (!RootSize || !RootPanel || !Stack)
	{
		return;
	}

	WidgetTree->RootWidget = RootSize;
	RootSize->SetWidthOverride(196.0f);
	RootSize->SetHeightOverride(78.0f);
	RootSize->AddChild(RootPanel);

	RootPanel->SetPadding(FMargin(8.0f, 4.0f));
	RootPanel->SetBrushColor(FLinearColor(0.015f, 0.014f, 0.012f, 0.0f));
	RootPanel->SetContent(Stack);

	SelectedMarkerText = MakeNameplateText(WidgetTree, TEXT("SelectedMarkerText"), FLinearColor(1.0f, 0.75f, 0.24f, 1.0f), 16.0f);
	NameText = MakeNameplateText(WidgetTree, TEXT("EnemyNameText"), FLinearColor(1.0f, 0.82f, 0.46f, 1.0f), 14.0f);
	HealthText = MakeNameplateText(WidgetTree, TEXT("EnemyHealthText"), FLinearColor(0.9f, 0.88f, 0.78f, 1.0f), 11.0f);
	HealthBar = WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), TEXT("EnemyHealthBar"));

	AddNameplateChild(Stack, SelectedMarkerText, 0.0f);
	AddNameplateChild(Stack, NameText, 2.0f);
	AddNameplateChild(Stack, HealthText, 3.0f);
	if (HealthBar)
	{
		HealthBar->SetPercent(1.0f);
		HealthBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.68f, 0.18f, 1.0f));
		AddNameplateChild(Stack, HealthBar, 0.0f);
	}

	UpdateVisibility();
}

void UEmbermereEnemyNameplateWidget::UpdateVisibility()
{
	SetVisibility(bSelected ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}
