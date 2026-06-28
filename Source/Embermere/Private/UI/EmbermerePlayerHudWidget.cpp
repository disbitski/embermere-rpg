#include "UI/EmbermerePlayerHudWidget.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Interfaces/EmbermereTargetable.h"

namespace
{
	UTextBlock* MakeHudText(UWidgetTree* WidgetTree, const FName Name, const FLinearColor Color, const float FontSize)
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
		TextBlock->SetShadowOffset(FVector2D(1.5f, 1.5f));
		TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.85f));
		return TextBlock;
	}
}

void UEmbermerePlayerHudWidget::BindToCharacter(AEmbermereCharacter* Character)
{
	OwningEmbermereCharacter = Character;
	Stats = Character ? Character->Stats : nullptr;
	Combat = Character ? Character->Combat : nullptr;
	Hotbar = Character ? Character->Hotbar : nullptr;
	QuestLog = Character ? Character->QuestLog : nullptr;
	Inventory = Character ? Character->Inventory : nullptr;
	RefreshHudText();
}

void UEmbermerePlayerHudWidget::NativeConstruct()
{
	Super::NativeConstruct();
	BuildDefaultLayout();
	RefreshHudText();
}

void UEmbermerePlayerHudWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	RefreshHudText();
}

void UEmbermerePlayerHudWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UCanvasPanel* RootCanvas = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("HudRootCanvas"));
	WidgetTree->RootWidget = RootCanvas;
	if (!RootCanvas)
	{
		return;
	}

	UVerticalBox* LeftStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("HudLeftStack"));
	UVerticalBox* BottomStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("HudBottomStack"));
	if (!LeftStack || !BottomStack)
	{
		return;
	}

	PlayerStatusText = MakeHudText(WidgetTree, TEXT("PlayerStatusText"), FLinearColor(0.95f, 0.95f, 0.9f, 1.0f), 18.0f);
	TargetText = MakeHudText(WidgetTree, TEXT("TargetText"), FLinearColor(1.0f, 0.82f, 0.32f, 1.0f), 18.0f);
	QuestText = MakeHudText(WidgetTree, TEXT("QuestText"), FLinearColor(0.72f, 0.9f, 1.0f, 1.0f), 16.0f);
	HotbarText = MakeHudText(WidgetTree, TEXT("HotbarText"), FLinearColor(0.9f, 0.95f, 1.0f, 1.0f), 16.0f);

	if (PlayerStatusText)
	{
		LeftStack->AddChildToVerticalBox(PlayerStatusText);
	}
	if (TargetText)
	{
		LeftStack->AddChildToVerticalBox(TargetText);
	}
	if (QuestText)
	{
		LeftStack->AddChildToVerticalBox(QuestText);
	}
	if (HotbarText)
	{
		BottomStack->AddChildToVerticalBox(HotbarText);
	}

	if (UCanvasPanelSlot* LeftSlot = RootCanvas->AddChildToCanvas(LeftStack))
	{
		LeftSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
		LeftSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		LeftSlot->SetPosition(FVector2D(24.0f, 24.0f));
		LeftSlot->SetAutoSize(true);
	}

	if (UCanvasPanelSlot* BottomSlot = RootCanvas->AddChildToCanvas(BottomStack))
	{
		BottomSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
		BottomSlot->SetAlignment(FVector2D(0.5f, 1.0f));
		BottomSlot->SetPosition(FVector2D(0.0f, -36.0f));
		BottomSlot->SetAutoSize(true);
	}
}

void UEmbermerePlayerHudWidget::RefreshHudText()
{
	if (PlayerStatusText)
	{
		if (Stats)
		{
			PlayerStatusText->SetText(FText::FromString(FString::Printf(
				TEXT("HP %.0f/%.0f   Mana %.0f/%.0f   XP %d"),
				Stats->CurrentHealth,
				Stats->MaxHealth,
				Stats->CurrentMana,
				Stats->MaxMana,
				Stats->CurrentExperience)));
		}
		else
		{
			PlayerStatusText->SetText(FText::FromString(TEXT("HP --/--   Mana --/--")));
		}
	}

	if (TargetText)
	{
		AActor* TargetActor = Combat ? Combat->CurrentTarget.Get() : nullptr;
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()))
		{
			const FText TargetName = IEmbermereTargetable::Execute_GetTargetDisplayName(TargetActor);
			if (const UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>())
			{
				TargetText->SetText(FText::FromString(FString::Printf(
					TEXT("Target: %s   %.0f/%.0f HP"),
					*TargetName.ToString(),
					TargetStats->CurrentHealth,
					TargetStats->MaxHealth)));
			}
			else
			{
				TargetText->SetText(FText::FromString(FString::Printf(TEXT("Target: %s"), *TargetName.ToString())));
			}
		}
		else
		{
			TargetText->SetText(FText::FromString(TEXT("Target: none")));
		}
	}

	if (QuestText)
	{
		if (QuestLog && QuestLog->ActiveQuest.Quest)
		{
			QuestText->SetText(FText::FromString(FString::Printf(
				TEXT("Quest: %s   %d/%d"),
				*QuestLog->ActiveQuest.Quest->Title.ToString(),
				QuestLog->ActiveQuest.CurrentObjectiveCount,
				QuestLog->ActiveQuest.Quest->RequiredObjectiveCount)));
		}
		else
		{
			QuestText->SetText(FText::FromString(TEXT("Quest: none")));
		}
	}

	if (HotbarText)
	{
		FString HotbarLine = TEXT("1 ");
		if (Hotbar && Hotbar->Slots.IsValidIndex(0) && !Hotbar->Slots[0].AbilityId.IsNone())
		{
			HotbarLine += Hotbar->Slots[0].DisplayName.ToString();
		}
		else
		{
			HotbarLine += TEXT("-");
		}

		HotbarLine += TEXT("    2 ");
		HotbarLine += Hotbar && Hotbar->Slots.IsValidIndex(1) && !Hotbar->Slots[1].AbilityId.IsNone()
			? Hotbar->Slots[1].DisplayName.ToString()
			: TEXT("-");

		HotbarLine += TEXT("    3 ");
		HotbarLine += Hotbar && Hotbar->Slots.IsValidIndex(2) && !Hotbar->Slots[2].AbilityId.IsNone()
			? Hotbar->Slots[2].DisplayName.ToString()
			: TEXT("-");

		HotbarLine += TEXT("    4 ");
		HotbarLine += Hotbar && Hotbar->Slots.IsValidIndex(3) && !Hotbar->Slots[3].AbilityId.IsNone()
			? Hotbar->Slots[3].DisplayName.ToString()
			: TEXT("-");

		HotbarLine += TEXT("    F Interact");
		HotbarText->SetText(FText::FromString(HotbarLine));
	}
}
