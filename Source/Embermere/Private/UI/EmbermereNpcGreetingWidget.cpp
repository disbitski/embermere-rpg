#include "UI/EmbermereNpcGreetingWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"

namespace
{
	constexpr float GreetingPanelWidth = 320.0f;
	constexpr float GreetingPanelHeight = 56.0f;

	UTextBlock* MakeGreetingText(
		UWidgetTree* WidgetTree,
		const FName Name,
		const float FontSize,
		const ETextJustify::Type Justification)
	{
		UTextBlock* TextBlock = WidgetTree
			? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name)
			: nullptr;
		if (!TextBlock)
		{
			return nullptr;
		}

		FSlateFontInfo FontInfo = TextBlock->GetFont();
		FontInfo.Size = FontSize;
		TextBlock->SetFont(FontInfo);
		TextBlock->SetJustification(Justification);
		TextBlock->SetShadowOffset(FVector2D(1.0f, 1.0f));
		TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.92f));
		return TextBlock;
	}
}

TSharedRef<SWidget> UEmbermereNpcGreetingWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UEmbermereNpcGreetingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	UpdatePresentation();
}

void UEmbermereNpcGreetingWidget::SetGreetingState(
	const EEmbermereNpcGreetingState NewState,
	const FText& NewGreetingText)
{
	GreetingState = NewState;
	GreetingText = NewGreetingText;
	BuildDefaultLayout();
	UpdatePresentation();
}

EEmbermereNpcGreetingState UEmbermereNpcGreetingWidget::GetGreetingState() const
{
	return GreetingState;
}

FText UEmbermereNpcGreetingWidget::GetGreetingText() const
{
	return GreetingText;
}

FVector2D UEmbermereNpcGreetingWidget::GetPanelDimensions() const
{
	return FVector2D(GreetingPanelWidth, GreetingPanelHeight);
}

void UEmbermereNpcGreetingWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	USizeBox* RootSize = WidgetTree->ConstructWidget<USizeBox>(
		USizeBox::StaticClass(),
		TEXT("NpcGreetingRootSize"));
	RootPanel = WidgetTree->ConstructWidget<UBorder>(
		UBorder::StaticClass(),
		TEXT("NpcGreetingPanel"));
	UHorizontalBox* Row = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(),
		TEXT("NpcGreetingRow"));
	if (!RootSize || !RootPanel || !Row)
	{
		return;
	}

	WidgetTree->RootWidget = RootSize;
	RootSize->SetWidthOverride(GreetingPanelWidth);
	RootSize->SetHeightOverride(GreetingPanelHeight);
	RootSize->AddChild(RootPanel);

	RootPanel->SetPadding(FMargin(10.0f, 7.0f));
	RootPanel->SetContent(Row);

	StateMarkerText = MakeGreetingText(WidgetTree, TEXT("NpcGreetingStateMarker"), 18.0f, ETextJustify::Center);
	GreetingTextBlock = MakeGreetingText(WidgetTree, TEXT("NpcGreetingText"), 13.0f, ETextJustify::Left);
	if (StateMarkerText)
	{
		if (UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(StateMarkerText))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			Slot->SetVerticalAlignment(VAlign_Center);
			Slot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));
		}
	}
	if (GreetingTextBlock)
	{
		GreetingTextBlock->SetAutoWrapText(false);
		if (UHorizontalBoxSlot* Slot = Row->AddChildToHorizontalBox(GreetingTextBlock))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			Slot->SetVerticalAlignment(VAlign_Center);
		}
	}

	UpdatePresentation();
}

void UEmbermereNpcGreetingWidget::UpdatePresentation()
{
	FLinearColor AccentColor(0.78f, 0.78f, 0.72f, 1.0f);
	FString StateMarker;
	switch (GreetingState)
	{
	case EEmbermereNpcGreetingState::Available:
		AccentColor = FLinearColor(1.0f, 0.78f, 0.22f, 1.0f);
		StateMarker = TEXT("!");
		break;
	case EEmbermereNpcGreetingState::Active:
		AccentColor = FLinearColor(0.52f, 0.88f, 1.0f, 1.0f);
		StateMarker = TEXT("...");
		break;
	case EEmbermereNpcGreetingState::ReadyToTurnIn:
		AccentColor = FLinearColor(0.62f, 1.0f, 0.48f, 1.0f);
		StateMarker = TEXT("?");
		break;
	case EEmbermereNpcGreetingState::Completed:
		AccentColor = FLinearColor(0.62f, 0.82f, 0.72f, 1.0f);
		StateMarker = TEXT("+");
		break;
	default:
		break;
	}

	if (RootPanel)
	{
		RootPanel->SetBrushColor(FLinearColor(
			AccentColor.R * 0.08f,
			AccentColor.G * 0.08f,
			AccentColor.B * 0.08f,
			0.9f));
	}
	if (StateMarkerText)
	{
		StateMarkerText->SetText(FText::FromString(StateMarker));
		StateMarkerText->SetColorAndOpacity(FSlateColor(AccentColor));
	}
	if (GreetingTextBlock)
	{
		GreetingTextBlock->SetText(GreetingText);
		GreetingTextBlock->SetColorAndOpacity(FSlateColor(FLinearColor(0.94f, 0.92f, 0.84f, 1.0f)));
	}

	const bool bShouldShow = GreetingState != EEmbermereNpcGreetingState::Hidden &&
		!GreetingText.IsEmpty();
	SetVisibility(bShouldShow ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
}
