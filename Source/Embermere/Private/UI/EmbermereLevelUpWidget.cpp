#include "UI/EmbermereLevelUpWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

namespace
{
	constexpr float LevelUpPanelWidth = 360.0f;
	constexpr float LevelUpPanelHeight = 76.0f;
	constexpr float LevelUpLifetimeSeconds = 2.75f;
	constexpr float LevelUpFadeSeconds = 0.5f;
}

UEmbermereLevelUpWidget::UEmbermereLevelUpWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEmbermereLevelUpWidget::BindToStats(UEmbermereStatsComponent* InStats)
{
	if (BoundStats)
	{
		BoundStats->OnLevelChanged.RemoveDynamic(this, &UEmbermereLevelUpWidget::HandleLevelChanged);
	}
	BoundStats = InStats;
	ClearPresentation();
	if (BoundStats)
	{
		BoundStats->OnLevelChanged.AddUniqueDynamic(this, &UEmbermereLevelUpWidget::HandleLevelChanged);
	}
}

void UEmbermereLevelUpWidget::AdvancePresentation(float DeltaSeconds)
{
	if (!bPresentationVisible)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (World && World->IsPaused())
	{
		RefreshVisuals();
		return;
	}

	PresentationAgeSeconds += FMath::Max(0.0f, DeltaSeconds);
	if (PresentationAgeSeconds >= LevelUpLifetimeSeconds)
	{
		ClearPresentation();
		return;
	}
	RefreshVisuals();
}

void UEmbermereLevelUpWidget::ClearPresentation()
{
	bPresentationVisible = false;
	PresentationAgeSeconds = 0.0f;
	CachedTitle = FText::GetEmpty();
	CachedDetail = FText::GetEmpty();
	RefreshVisuals();
}

bool UEmbermereLevelUpWidget::IsLevelUpVisible() const
{
	return bPresentationVisible;
}

FText UEmbermereLevelUpWidget::GetLevelUpTitle() const
{
	return CachedTitle;
}

FText UEmbermereLevelUpWidget::GetLevelUpDetail() const
{
	return CachedDetail;
}

FVector2D UEmbermereLevelUpWidget::GetPanelDimensions() const
{
	return FVector2D(LevelUpPanelWidth, LevelUpPanelHeight);
}

float UEmbermereLevelUpWidget::GetPresentationLifetimeSeconds() const
{
	return LevelUpLifetimeSeconds;
}

TSharedRef<SWidget> UEmbermereLevelUpWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UEmbermereLevelUpWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	SynchronizeViewportBounds();
}

void UEmbermereLevelUpWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	SynchronizeViewportBounds();
	AdvancePresentation(InDeltaTime);
}

void UEmbermereLevelUpWidget::NativeDestruct()
{
	BindToStats(nullptr);
	Super::NativeDestruct();
}

void UEmbermereLevelUpWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("LevelUpRoot"));
	WidgetTree->RootWidget = RootCanvas;
	if (!RootCanvas)
	{
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
	LevelUpPanel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("LevelUpPanel"));
	UVerticalBox* Stack = WidgetTree->ConstructWidget<UVerticalBox>(
		UVerticalBox::StaticClass(),
		TEXT("LevelUpStack"));
	LevelUpTitleText = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass(),
		TEXT("LevelUpTitle"));
	LevelUpDetailText = WidgetTree->ConstructWidget<UTextBlock>(
		UTextBlock::StaticClass(),
		TEXT("LevelUpDetail"));
	if (!LevelUpPanel || !Stack || !LevelUpTitleText || !LevelUpDetailText)
	{
		return;
	}

	LevelUpPanel->SetBrushColor(FLinearColor(0.02f, 0.035f, 0.04f, 0.94f));
	LevelUpPanel->SetPadding(FMargin(14.0f, 7.0f));
	LevelUpPanel->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	LevelUpTitleText->SetJustification(ETextJustify::Center);
	LevelUpTitleText->SetAutoWrapText(false);
	LevelUpTitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.82f, 0.3f, 1.0f)));
	LevelUpTitleText->SetShadowOffset(FVector2D(1.5f, 1.5f));
	LevelUpTitleText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
	FSlateFontInfo TitleFont = LevelUpTitleText->GetFont();
	TitleFont.Size = 22;
	LevelUpTitleText->SetFont(TitleFont);
	LevelUpDetailText->SetJustification(ETextJustify::Center);
	LevelUpDetailText->SetAutoWrapText(false);
	LevelUpDetailText->SetColorAndOpacity(FSlateColor(FLinearColor(0.74f, 0.92f, 1.0f, 1.0f)));
	LevelUpDetailText->SetShadowOffset(FVector2D(1.0f, 1.0f));
	LevelUpDetailText->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
	FSlateFontInfo DetailFont = LevelUpDetailText->GetFont();
	DetailFont.Size = 13;
	LevelUpDetailText->SetFont(DetailFont);
	if (UVerticalBoxSlot* TitleSlot = Stack->AddChildToVerticalBox(LevelUpTitleText))
	{
		TitleSlot->SetHorizontalAlignment(HAlign_Fill);
		TitleSlot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, 2.0f));
	}
	if (UVerticalBoxSlot* DetailSlot = Stack->AddChildToVerticalBox(LevelUpDetailText))
	{
		DetailSlot->SetHorizontalAlignment(HAlign_Fill);
	}
	LevelUpPanel->SetContent(Stack);
	if (UCanvasPanelSlot* PanelSlot = RootCanvas->AddChildToCanvas(LevelUpPanel))
	{
		PanelSlot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
		PanelSlot->SetAlignment(FVector2D(0.5f, 0.0f));
		PanelSlot->SetPosition(FVector2D(0.0f, 92.0f));
		PanelSlot->SetSize(FVector2D(LevelUpPanelWidth, LevelUpPanelHeight));
	}
	RefreshVisuals();
}

void UEmbermereLevelUpWidget::SynchronizeViewportBounds()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!PlayerController)
	{
		return;
	}

	int32 ViewportWidth = 0;
	int32 ViewportHeight = 0;
	PlayerController->GetViewportSize(ViewportWidth, ViewportHeight);
	if (ViewportWidth <= 0 || ViewportHeight <= 0)
	{
		return;
	}

	const float ViewportScale = FMath::Max(
		UWidgetLayoutLibrary::GetViewportScale(PlayerController),
		KINDA_SMALL_NUMBER);
	const FVector2D LogicalViewportSize(
		static_cast<float>(ViewportWidth) / ViewportScale,
		static_cast<float>(ViewportHeight) / ViewportScale);
	if (LogicalViewportSize.Equals(CachedViewportSize, 0.5f))
	{
		return;
	}

	CachedViewportSize = LogicalViewportSize;
	SetAlignmentInViewport(FVector2D::ZeroVector);
	SetPositionInViewport(FVector2D::ZeroVector, false);
	SetDesiredSizeInViewport(LogicalViewportSize);
}

void UEmbermereLevelUpWidget::RefreshVisuals()
{
	if (!LevelUpPanel)
	{
		return;
	}
	if (!bPresentationVisible)
	{
		LevelUpPanel->SetVisibility(ESlateVisibility::Hidden);
		LevelUpPanel->SetRenderOpacity(0.0f);
		return;
	}

	if (LevelUpTitleText)
	{
		LevelUpTitleText->SetText(CachedTitle);
	}
	if (LevelUpDetailText)
	{
		LevelUpDetailText->SetText(CachedDetail);
	}
	const float RemainingSeconds = LevelUpLifetimeSeconds - PresentationAgeSeconds;
	const float Opacity = RemainingSeconds < LevelUpFadeSeconds
		? FMath::Clamp(RemainingSeconds / LevelUpFadeSeconds, 0.0f, 1.0f)
		: 1.0f;
	LevelUpPanel->SetRenderOpacity(Opacity);
	LevelUpPanel->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UEmbermereLevelUpWidget::HandleLevelChanged(int32 PreviousLevel, int32 CurrentLevel)
{
	if (!BoundStats || CurrentLevel <= PreviousLevel)
	{
		return;
	}

	FEmbermereProgressionPresentation Progression;
	const bool bHasProgression = BoundStats->GetProgressionPresentation(Progression);
	CachedTitle = FText::FromString(FString::Printf(TEXT("LEVEL %d"), CurrentLevel));
	if (bHasProgression && Progression.bAtLevelCap)
	{
		CachedDetail = FText::FromString(FString::Printf(
			TEXT("%d XP  |  LEVEL CAP"),
			Progression.CurrentExperience));
	}
	else if (CurrentLevel > PreviousLevel + 1)
	{
		CachedDetail = FText::FromString(FString::Printf(
			TEXT("Advanced %d levels  |  %d XP"),
			CurrentLevel - PreviousLevel,
			BoundStats->CurrentExperience));
	}
	else if (bHasProgression)
	{
		CachedDetail = FText::FromString(FString::Printf(
			TEXT("%d XP  |  Next: %d XP"),
			Progression.CurrentExperience,
			Progression.NextLevelThreshold));
	}
	else
	{
		CachedDetail = FText::FromString(FString::Printf(
			TEXT("%d XP"),
			BoundStats->CurrentExperience));
	}
	PresentationAgeSeconds = 0.0f;
	bPresentationVisible = true;
	RefreshVisuals();
}
