#include "UI/EmbermereQuestUpdateWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"

void UEmbermereQuestUpdateWidget::BindToQuestLog(
	UEmbermereQuestLogComponent* Log, UEmbermereStatsComponent* Stats)
{
	if (BoundLog)
	{
		BoundLog->OnLiveQuestUpdate.RemoveAll(this);
		BoundLog->OnPresentationReset.RemoveAll(this);
	}
	if (BoundStats)
	{
		BoundStats->OnDied.RemoveDynamic(this, &UEmbermereQuestUpdateWidget::HandleDeath);
	}
	BoundLog = Log;
	BoundStats = Stats;
	LastSequence = 0;
	ClearPresentation();
	if (BoundLog)
	{
		BoundLog->OnLiveQuestUpdate.AddUObject(this, &UEmbermereQuestUpdateWidget::PresentUpdate);
		BoundLog->OnPresentationReset.AddUObject(this, &UEmbermereQuestUpdateWidget::ClearPresentation);
	}
	if (BoundStats)
	{
		BoundStats->OnDied.AddUniqueDynamic(this, &UEmbermereQuestUpdateWidget::HandleDeath);
	}
}

void UEmbermereQuestUpdateWidget::PresentUpdate(const FEmbermereQuestUpdate& Update)
{
	if (!BoundLog || Update.Sequence <= LastSequence || Update.QuestId.IsNone() ||
		Update.ObjectiveId.IsNone() || Update.RequiredCount <= 0 ||
		Update.CurrentCount < 0 || Update.CurrentCount > Update.RequiredCount)
	{
		return;
	}
	LastSequence = Update.Sequence;
	if (bSuppressed || (BoundStats && BoundStats->CurrentHealth <= 0.0f))
	{
		return;
	}
	CurrentUpdate = Update;
	bVisible = true;
	AgeSeconds = 0.0f;
	RefreshVisuals();
}

void UEmbermereQuestUpdateWidget::AdvancePresentation(float DeltaSeconds)
{
	if (!bVisible || (GetWorld() && GetWorld()->IsPaused()))
	{
		return;
	}
	if (BoundStats && BoundStats->CurrentHealth <= 0.0f)
	{
		ClearPresentation();
		return;
	}
	AgeSeconds += FMath::IsFinite(DeltaSeconds) ? FMath::Max(0.0f, DeltaSeconds) : 0.0f;
	if (AgeSeconds >= LifetimeSeconds)
	{
		ClearPresentation();
	}
	else
	{
		RefreshVisuals();
	}
}

void UEmbermereQuestUpdateWidget::SetSuppressed(bool bSuppress)
{
	bSuppressed = bSuppress;
	if (bSuppress)
	{
		ClearPresentation();
	}
}

void UEmbermereQuestUpdateWidget::ClearPresentation()
{
	bVisible = false;
	AgeSeconds = 0.0f;
	CurrentUpdate = FEmbermereQuestUpdate();
	RefreshVisuals();
}

void UEmbermereQuestUpdateWidget::HandleDeath()
{
	ClearPresentation();
}

FText UEmbermereQuestUpdateWidget::GetUpdateHeading() const
{
	if (!bVisible) return FText::GetEmpty();
	switch (CurrentUpdate.Kind)
	{
	case EEmbermereQuestUpdateKind::Accepted: return FText::FromString(TEXT("QUEST ACCEPTED"));
	case EEmbermereQuestUpdateKind::Ready: return FText::FromString(TEXT("READY TO RETURN"));
	case EEmbermereQuestUpdateKind::Completed: return FText::FromString(TEXT("QUEST COMPLETED"));
	default: return FText::FromString(TEXT("QUEST PROGRESS"));
	}
}

FText UEmbermereQuestUpdateWidget::GetUpdateDetail() const
{
	if (!bVisible) return FText::GetEmpty();
	const FString Instructions = CurrentUpdate.Instructions.ToString().TrimStartAndEnd();
	return FText::FromString(FString::Printf(TEXT("%d / %d   %s"),
		CurrentUpdate.CurrentCount, CurrentUpdate.RequiredCount,
		Instructions.IsEmpty() ? TEXT("Objective details unavailable.") : *Instructions));
}

TSharedRef<SWidget> UEmbermereQuestUpdateWidget::RebuildWidget()
{
	BuildLayout();
	return Super::RebuildWidget();
}

void UEmbermereQuestUpdateWidget::NativeTick(const FGeometry& Geometry, float DeltaSeconds)
{
	Super::NativeTick(Geometry, DeltaSeconds);
	AdvancePresentation(DeltaSeconds);
}

void UEmbermereQuestUpdateWidget::NativeDestruct()
{
	BindToQuestLog(nullptr, nullptr);
	Super::NativeDestruct();
}

void UEmbermereQuestUpdateWidget::BuildLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget) return;
	SetVisibility(ESlateVisibility::HitTestInvisible);
	UCanvasPanel* Root = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("QuestUpdateRoot"));
	WidgetTree->RootWidget = Root;
	Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("QuestUpdatePanel"));
	Panel->SetPadding(FMargin(0.0f));
	Panel->SetBrushColor(FLinearColor(0.02f, 0.035f, 0.03f, 0.94f));
	Panel->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	UCanvasPanelSlot* PanelSlot = Root->AddChildToCanvas(Panel);
	PanelSlot->SetAnchors(FAnchors(1.0f, 0.0f));
	PanelSlot->SetAlignment(FVector2D(1.0f, 0.0f));
	PanelSlot->SetPosition(FVector2D(-24.0f, 84.0f));
	PanelSlot->SetSize(GetPanelDimensions());
	UCanvasPanel* Content = WidgetTree->ConstructWidget<UCanvasPanel>();
	Panel->SetContent(Content);
	auto AddText = [this, Content](const TCHAR* Name, int32 FontSize, float Y, float Height)
	{
		UTextBlock* Text = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		FSlateFontInfo Font = Text->GetFont();
		Font.Size = FontSize;
		Text->SetFont(Font);
		Text->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.93f, 0.86f)));
		Text->SetClipping(EWidgetClipping::ClipToBoundsAlways);
		UCanvasPanelSlot* Slot = Content->AddChildToCanvas(Text);
		Slot->SetPosition(FVector2D(74.0f, Y));
		Slot->SetSize(FVector2D(332.0f, Height));
		return Text;
	};
	Heading = AddText(TEXT("QuestUpdateHeading"), 11, 10.0f, 17.0f);
	Title = AddText(TEXT("QuestUpdateTitle"), 17, 29.0f, 25.0f);
	Detail = AddText(TEXT("QuestUpdateDetail"), 12, 59.0f, 40.0f);
	Detail->SetAutoWrapText(true);
	UImage* Emblem = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("QuestUpdateEmblem"));
	UTexture2D* Texture = LoadObject<UTexture2D>(nullptr,
		TEXT("/Game/UI/Icons/T_FenwatchJournalEmblem.T_FenwatchJournalEmblem"));
	if (Texture)
	{
		Emblem->SetBrushFromTexture(Texture);
	}
	else
	{
		Emblem->SetVisibility(ESlateVisibility::Hidden);
	}
	UCanvasPanelSlot* EmblemSlot = Content->AddChildToCanvas(Emblem);
	EmblemSlot->SetPosition(FVector2D(12.0f, 27.0f));
	EmblemSlot->SetSize(FVector2D(48.0f, 48.0f));
	RefreshVisuals();
}

void UEmbermereQuestUpdateWidget::RefreshVisuals()
{
	if (!Panel) return;
	Panel->SetVisibility(bVisible ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	Panel->SetRenderOpacity(bVisible ? FMath::Clamp((LifetimeSeconds - AgeSeconds) / 0.4f, 0.0f, 1.0f) : 0.0f);
	Heading->SetText(GetUpdateHeading());
	Heading->SetColorAndOpacity(FSlateColor(CurrentUpdate.Kind == EEmbermereQuestUpdateKind::Completed
		? FLinearColor(0.5f, 1.0f, 0.65f) : FLinearColor(1.0f, 0.8f, 0.34f)));
	Title->SetText(GetUpdateTitle());
	Detail->SetText(GetUpdateDetail());
}
