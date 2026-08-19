#include "UI/EmbermereCombatFeedbackWidget.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/EmbermereTargetableDispatch.h"

namespace
{
	constexpr int32 MaximumFeedbackEntries = 3;
	constexpr float FeedbackSlotWidth = 112.0f;
	constexpr float FeedbackSlotHeight = 32.0f;
	constexpr float FeedbackLifetimeSeconds = 1.25f;
	constexpr float FeedbackFadeSeconds = 0.3f;
	constexpr float FeedbackRisePixels = 18.0f;
	constexpr float FeedbackTargetClearance = 30.0f;
	constexpr float FeedbackHorizontalOffset = 224.0f;

	FText BuildFeedbackText(const FEmbermereCombatResult& Result)
	{
		if (Result.Kind == EEmbermereCombatResultKind::Miss)
		{
			return FText::FromString(TEXT("MISS"));
		}

		return FText::AsNumber(FMath::Max(1, FMath::RoundToInt(Result.AppliedAmount)));
	}

	FLinearColor GetFeedbackColor(const FEmbermereCombatResult& Result)
	{
		return Result.Kind == EEmbermereCombatResultKind::Miss
			? FLinearColor(0.68f, 0.88f, 1.0f, 1.0f)
			: FLinearColor(1.0f, 0.72f, 0.24f, 1.0f);
	}
}

UEmbermereCombatFeedbackWidget::UEmbermereCombatFeedbackWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEmbermereCombatFeedbackWidget::PresentCombatResult(const FEmbermereCombatResult& Result)
{
	const bool bDisplayableDamage =
		Result.Kind == EEmbermereCombatResultKind::Damage && Result.AppliedAmount > 0.0f;
	if ((!bDisplayableDamage && Result.Kind != EEmbermereCombatResultKind::Miss) || !IsValid(Result.Target))
	{
		return;
	}

	FFeedbackEntry NewEntry;
	NewEntry.Target = Result.Target;
	NewEntry.DisplayText = BuildFeedbackText(Result);
	NewEntry.Color = GetFeedbackColor(Result);
	ActiveEntries.Insert(MoveTemp(NewEntry), 0);
	if (ActiveEntries.Num() > MaximumFeedbackEntries)
	{
		ActiveEntries.SetNum(MaximumFeedbackEntries);
	}
	RefreshFeedbackVisuals();
}

void UEmbermereCombatFeedbackWidget::AdvancePresentation(float DeltaSeconds)
{
	const UWorld* World = GetWorld();
	if (World && World->IsPaused())
	{
		RefreshFeedbackVisuals();
		return;
	}

	for (FFeedbackEntry& Entry : ActiveEntries)
	{
		Entry.AgeSeconds += FMath::Max(0.0f, DeltaSeconds);
	}
	ActiveEntries.RemoveAll([](const FFeedbackEntry& Entry)
	{
		return !Entry.Target.IsValid() || Entry.AgeSeconds >= FeedbackLifetimeSeconds;
	});
	RefreshFeedbackVisuals();
}

void UEmbermereCombatFeedbackWidget::ClearForTarget(const AActor* Target)
{
	if (!Target)
	{
		return;
	}

	ActiveEntries.RemoveAll([Target](const FFeedbackEntry& Entry)
	{
		return Entry.Target.Get() == Target;
	});
	RefreshFeedbackVisuals();
}

void UEmbermereCombatFeedbackWidget::ClearAllFeedback()
{
	ActiveEntries.Reset();
	RefreshFeedbackVisuals();
}

int32 UEmbermereCombatFeedbackWidget::GetActiveFeedbackCount() const
{
	return ActiveEntries.Num();
}

FText UEmbermereCombatFeedbackWidget::GetFeedbackDisplayText(int32 EntryIndex) const
{
	return ActiveEntries.IsValidIndex(EntryIndex)
		? ActiveEntries[EntryIndex].DisplayText
		: FText::GetEmpty();
}

FVector2D UEmbermereCombatFeedbackWidget::GetFeedbackSlotDimensions() const
{
	return FVector2D(FeedbackSlotWidth, FeedbackSlotHeight);
}

int32 UEmbermereCombatFeedbackWidget::GetMaximumFeedbackEntries() const
{
	return MaximumFeedbackEntries;
}

float UEmbermereCombatFeedbackWidget::GetFeedbackLifetimeSeconds() const
{
	return FeedbackLifetimeSeconds;
}

TSharedRef<SWidget> UEmbermereCombatFeedbackWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UEmbermereCombatFeedbackWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	SynchronizeViewportBounds();
}

void UEmbermereCombatFeedbackWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	SynchronizeViewportBounds();
	AdvancePresentation(InDeltaTime);
}

void UEmbermereCombatFeedbackWidget::NativeDestruct()
{
	ClearAllFeedback();
	Super::NativeDestruct();
}

void UEmbermereCombatFeedbackWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(
		UCanvasPanel::StaticClass(),
		TEXT("CombatFeedbackRoot"));
	WidgetTree->RootWidget = RootCanvas;
	if (!RootCanvas)
	{
		return;
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
	BuildFeedbackPanels(WidgetTree, RootCanvas);
	RefreshFeedbackVisuals();
}

void UEmbermereCombatFeedbackWidget::BuildFeedbackPanels(
	UWidgetTree* LayoutTree,
	UCanvasPanel* HostCanvas)
{
	if (!LayoutTree || !HostCanvas)
	{
		return;
	}

	FeedbackPanels.Reset();
	FeedbackTexts.Reset();
	for (int32 EntryIndex = 0; EntryIndex < MaximumFeedbackEntries; ++EntryIndex)
	{
		UBorder* Panel = LayoutTree->ConstructWidget<UBorder>(
			UBorder::StaticClass(),
			*FString::Printf(TEXT("CombatFeedbackPanel_%d"), EntryIndex));
		UTextBlock* Text = LayoutTree->ConstructWidget<UTextBlock>(
			UTextBlock::StaticClass(),
			*FString::Printf(TEXT("CombatFeedbackText_%d"), EntryIndex));
		if (!Panel || !Text)
		{
			continue;
		}

		Panel->SetBrushColor(FLinearColor(0.015f, 0.02f, 0.025f, 0.62f));
		Panel->SetPadding(FMargin(4.0f, 1.0f));
		Panel->SetVisibility(ESlateVisibility::Hidden);
		Text->SetJustification(ETextJustify::Center);
		Text->SetAutoWrapText(false);
		Text->SetClipping(EWidgetClipping::ClipToBounds);
		Text->SetShadowOffset(FVector2D(1.5f, 1.5f));
		Text->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.9f));
		FSlateFontInfo FontInfo = Text->GetFont();
		FontInfo.Size = 17;
		Text->SetFont(FontInfo);
		Panel->SetContent(Text);

		if (UCanvasPanelSlot* Slot = HostCanvas->AddChildToCanvas(Panel))
		{
			Slot->SetAnchors(FAnchors(0.0f));
			Slot->SetAlignment(FVector2D(0.5f, 1.0f));
			Slot->SetSize(FVector2D(FeedbackSlotWidth, FeedbackSlotHeight));
			Slot->SetZOrder(20 + EntryIndex);
		}
		FeedbackPanels.Add(Panel);
		FeedbackTexts.Add(Text);
	}
}

void UEmbermereCombatFeedbackWidget::SynchronizeViewportBounds()
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

void UEmbermereCombatFeedbackWidget::RefreshFeedbackVisuals()
{
	APlayerController* PlayerController = GetOwningPlayer();
	const float ViewportScale = PlayerController
		? FMath::Max(UWidgetLayoutLibrary::GetViewportScale(PlayerController), KINDA_SMALL_NUMBER)
		: 1.0f;
	for (int32 EntryIndex = 0; EntryIndex < FeedbackPanels.Num(); ++EntryIndex)
	{
		UBorder* Panel = FeedbackPanels[EntryIndex];
		UTextBlock* Text = FeedbackTexts.IsValidIndex(EntryIndex) ? FeedbackTexts[EntryIndex] : nullptr;
		if (!Panel || !Text || !ActiveEntries.IsValidIndex(EntryIndex))
		{
			if (Panel)
			{
				Panel->SetVisibility(ESlateVisibility::Hidden);
			}
			continue;
		}

		const FFeedbackEntry& Entry = ActiveEntries[EntryIndex];
		AActor* Target = Entry.Target.Get();
		FVector2D ScreenPosition;
		if (!PlayerController || !Target)
		{
			Panel->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}

		const FVector WorldPosition =
			EmbermereTargetableDispatch::GetCombatFeedbackAnchorLocation(Target) +
			FVector(0.0f, 0.0f, FeedbackTargetClearance);
		if (!PlayerController->ProjectWorldLocationToScreen(WorldPosition, ScreenPosition, true))
		{
			Panel->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}

		const float NormalizedAge = FMath::Clamp(Entry.AgeSeconds / FeedbackLifetimeSeconds, 0.0f, 1.0f);
		const float RemainingSeconds = FeedbackLifetimeSeconds - Entry.AgeSeconds;
		const float FadeOpacity = RemainingSeconds < FeedbackFadeSeconds
			? FMath::Clamp(RemainingSeconds / FeedbackFadeSeconds, 0.0f, 1.0f)
			: 1.0f;
		if (UCanvasPanelSlot* Slot = Cast<UCanvasPanelSlot>(Panel->Slot))
		{
			const FVector2D LocalScreenPosition = ScreenPosition / ViewportScale;
			const bool bFitsOnRight =
				LocalScreenPosition.X + FeedbackHorizontalOffset + (FeedbackSlotWidth * 0.5f) <=
				CachedViewportSize.X;
			const float HorizontalOffset = bFitsOnRight
				? FeedbackHorizontalOffset
				: -FeedbackHorizontalOffset;
			Slot->SetPosition(LocalScreenPosition + FVector2D(
				HorizontalOffset,
				-(FeedbackRisePixels * NormalizedAge) - (EntryIndex * FeedbackSlotHeight)));
		}
		Text->SetText(Entry.DisplayText);
		Text->SetColorAndOpacity(FSlateColor(Entry.Color));
		Panel->SetRenderOpacity(FadeOpacity);
		Panel->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
