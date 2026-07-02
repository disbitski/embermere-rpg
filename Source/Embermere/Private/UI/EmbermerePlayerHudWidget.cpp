#include "UI/EmbermerePlayerHudWidget.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Data/EmbermereItemData.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
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

	UBorder* MakePanel(UWidgetTree* WidgetTree, const FName Name, const FLinearColor Color)
	{
		UBorder* Border = WidgetTree ? WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name) : nullptr;
		if (!Border)
		{
			return nullptr;
		}

		Border->SetBrushColor(Color);
		Border->SetPadding(FMargin(12.0f, 10.0f));
		return Border;
	}

	UProgressBar* MakeBar(UWidgetTree* WidgetTree, const FName Name, const FLinearColor FillColor)
	{
		UProgressBar* Bar = WidgetTree ? WidgetTree->ConstructWidget<UProgressBar>(UProgressBar::StaticClass(), Name) : nullptr;
		if (!Bar)
		{
			return nullptr;
		}

		Bar->SetPercent(1.0f);
		Bar->SetFillColorAndOpacity(FillColor);
		return Bar;
	}

	USizeBox* MakeSizedWidget(UWidgetTree* WidgetTree, UWidget* Child, const FName Name, const float Width, const float Height)
	{
		USizeBox* SizeBox = WidgetTree ? WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), Name) : nullptr;
		if (!SizeBox || !Child)
		{
			return SizeBox;
		}

		SizeBox->SetWidthOverride(Width);
		SizeBox->SetHeightOverride(Height);
		SizeBox->AddChild(Child);
		return SizeBox;
	}

	UVerticalBox* MakePanelStack(UWidgetTree* WidgetTree, UBorder* Panel, const FName Name)
	{
		UVerticalBox* Stack = WidgetTree ? WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), Name) : nullptr;
		if (Panel && Stack)
		{
			Panel->SetContent(Stack);
		}
		return Stack;
	}

	void AddStackChild(UVerticalBox* Stack, UWidget* Child, const float BottomPadding = 4.0f)
	{
		if (!Stack || !Child)
		{
			return;
		}

		if (UVerticalBoxSlot* Slot = Stack->AddChildToVerticalBox(Child))
		{
			Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, BottomPadding));
		}
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
	BindComponentEvents();
	RefreshHudText();
}

TSharedRef<SWidget> UEmbermerePlayerHudWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
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

	const UWorld* World = GetWorld();
	const float Now = World ? World->GetTimeSeconds() : 0.0f;
	if (DialoguePanel && DialogueHideTimeSeconds > 0.0f && Now >= DialogueHideTimeSeconds)
	{
		DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);
		DialogueHideTimeSeconds = 0.0f;
	}
	if (LootPanel && LootHideTimeSeconds > 0.0f && Now >= LootHideTimeSeconds)
	{
		LootPanel->SetVisibility(ESlateVisibility::Collapsed);
		LootHideTimeSeconds = 0.0f;
	}
}

bool UEmbermerePlayerHudWidget::ToggleInventoryPanel()
{
	bInventoryPanelVisible = !bInventoryPanelVisible;
	UpdateInventoryPanelVisibility();
	return bInventoryPanelVisible;
}

bool UEmbermerePlayerHudWidget::IsInventoryPanelVisible() const
{
	return bInventoryPanelVisible;
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

	UBorder* StatusPanel = MakePanel(WidgetTree, TEXT("StatusPanel"), FLinearColor(0.02f, 0.025f, 0.03f, 0.78f));
	UVerticalBox* StatusStack = MakePanelStack(WidgetTree, StatusPanel, TEXT("StatusStack"));
	PlayerStatusText = MakeHudText(WidgetTree, TEXT("PlayerStatusText"), FLinearColor(0.95f, 0.92f, 0.82f, 1.0f), 17.0f);
	HealthBar = MakeBar(WidgetTree, TEXT("HealthBar"), FLinearColor(0.72f, 0.08f, 0.06f, 1.0f));
	ManaBar = MakeBar(WidgetTree, TEXT("ManaBar"), FLinearColor(0.08f, 0.24f, 0.8f, 1.0f));
	AddStackChild(StatusStack, PlayerStatusText, 8.0f);
	AddStackChild(StatusStack, MakeSizedWidget(WidgetTree, HealthBar, TEXT("HealthBarSize"), 260.0f, 14.0f), 5.0f);
	AddStackChild(StatusStack, MakeSizedWidget(WidgetTree, ManaBar, TEXT("ManaBarSize"), 260.0f, 12.0f), 0.0f);

	TargetPanel = MakePanel(WidgetTree, TEXT("TargetPanel"), FLinearColor(0.075f, 0.04f, 0.025f, 0.78f));
	UVerticalBox* TargetStack = MakePanelStack(WidgetTree, TargetPanel, TEXT("TargetStack"));
	TargetText = MakeHudText(WidgetTree, TEXT("TargetText"), FLinearColor(1.0f, 0.78f, 0.28f, 1.0f), 17.0f);
	TargetRangeText = MakeHudText(WidgetTree, TEXT("TargetRangeText"), FLinearColor(0.82f, 0.86f, 0.92f, 1.0f), 13.0f);
	TargetHealthBar = MakeBar(WidgetTree, TEXT("TargetHealthBar"), FLinearColor(0.86f, 0.12f, 0.08f, 1.0f));
	AddStackChild(TargetStack, TargetText, 8.0f);
	AddStackChild(TargetStack, TargetRangeText, 6.0f);
	AddStackChild(TargetStack, MakeSizedWidget(WidgetTree, TargetHealthBar, TEXT("TargetHealthBarSize"), 260.0f, 12.0f), 0.0f);

	QuestPanel = MakePanel(WidgetTree, TEXT("QuestPanel"), FLinearColor(0.025f, 0.055f, 0.07f, 0.78f));
	UVerticalBox* QuestStack = MakePanelStack(WidgetTree, QuestPanel, TEXT("QuestStack"));
	QuestText = MakeHudText(WidgetTree, TEXT("QuestText"), FLinearColor(0.72f, 0.9f, 1.0f, 1.0f), 15.0f);
	AddStackChild(QuestStack, QuestText, 0.0f);

	InventoryPanel = MakePanel(WidgetTree, TEXT("InventoryPanel"), FLinearColor(0.035f, 0.035f, 0.025f, 0.78f));
	UVerticalBox* InventoryStack = MakePanelStack(WidgetTree, InventoryPanel, TEXT("InventoryStack"));
	InventoryText = MakeHudText(WidgetTree, TEXT("InventoryText"), FLinearColor(0.92f, 0.86f, 0.62f, 1.0f), 14.0f);
	if (InventoryText)
	{
		InventoryText->SetAutoWrapText(true);
	}
	AddStackChild(InventoryStack, InventoryText, 0.0f);
	UpdateInventoryPanelVisibility();

	AddStackChild(LeftStack, StatusPanel, 10.0f);
	AddStackChild(LeftStack, TargetPanel, 10.0f);
	AddStackChild(LeftStack, QuestPanel, 0.0f);

	HotbarRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HotbarRow"));
	HotbarSlotTexts.Reset();
	static const TCHAR* HotbarKeys[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("Alt+R"), TEXT("Alt+E"), TEXT("R"), TEXT("X"), TEXT("E"), TEXT("F") };
	for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(HotbarKeys); ++SlotIndex)
	{
		USizeBox* SlotSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), *FString::Printf(TEXT("HotbarSlotSize_%d"), SlotIndex));
		UBorder* SlotPanel = MakePanel(WidgetTree, *FString::Printf(TEXT("HotbarSlotPanel_%d"), SlotIndex), FLinearColor(0.025f, 0.03f, 0.035f, 0.86f));
		UTextBlock* SlotText = MakeHudText(WidgetTree, *FString::Printf(TEXT("HotbarSlotText_%d"), SlotIndex), FLinearColor(0.92f, 0.96f, 1.0f, 1.0f), 13.0f);
		if (!SlotSize || !SlotPanel || !SlotText)
		{
			continue;
		}

		SlotSize->SetWidthOverride(92.0f);
		SlotSize->SetHeightOverride(52.0f);
		SlotText->SetJustification(ETextJustify::Center);
		SlotText->SetAutoWrapText(true);
		SlotText->SetText(FText::FromString(FString::Printf(TEXT("%s\n-"), HotbarKeys[SlotIndex])));
		SlotPanel->SetContent(SlotText);
		SlotSize->AddChild(SlotPanel);
		HotbarSlotTexts.Add(SlotText);

		if (HotbarRow)
		{
			if (UHorizontalBoxSlot* Slot = HotbarRow->AddChildToHorizontalBox(SlotSize))
			{
				Slot->SetPadding(FMargin(3.0f, 0.0f));
			}
		}
	}
	AddStackChild(BottomStack, HotbarRow, 0.0f);

	DialoguePanel = MakePanel(WidgetTree, TEXT("DialoguePanel"), FLinearColor(0.025f, 0.02f, 0.018f, 0.88f));
	DialogueTextBlock = MakeHudText(WidgetTree, TEXT("DialogueText"), FLinearColor(1.0f, 0.94f, 0.76f, 1.0f), 16.0f);
	if (DialoguePanel && DialogueTextBlock)
	{
		DialogueTextBlock->SetAutoWrapText(true);
		DialoguePanel->SetContent(DialogueTextBlock);
		DialoguePanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	LootPanel = MakePanel(WidgetTree, TEXT("LootPanel"), FLinearColor(0.025f, 0.08f, 0.04f, 0.88f));
	LootTextBlock = MakeHudText(WidgetTree, TEXT("LootText"), FLinearColor(0.58f, 1.0f, 0.62f, 1.0f), 16.0f);
	if (LootPanel && LootTextBlock)
	{
		LootTextBlock->SetAutoWrapText(true);
		LootPanel->SetContent(LootTextBlock);
		LootPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UCanvasPanelSlot* LeftSlot = RootCanvas->AddChildToCanvas(LeftStack))
	{
		LeftSlot->SetAnchors(FAnchors(0.0f, 0.0f, 0.0f, 0.0f));
		LeftSlot->SetAlignment(FVector2D(0.0f, 0.0f));
		LeftSlot->SetPosition(FVector2D(24.0f, 24.0f));
		LeftSlot->SetAutoSize(true);
	}

	if (InventoryPanel)
	{
		if (UCanvasPanelSlot* InventorySlot = RootCanvas->AddChildToCanvas(InventoryPanel))
		{
			InventorySlot->SetAnchors(FAnchors(1.0f, 0.0f, 1.0f, 0.0f));
			InventorySlot->SetAlignment(FVector2D(1.0f, 0.0f));
			InventorySlot->SetPosition(FVector2D(-24.0f, 24.0f));
			InventorySlot->SetSize(FVector2D(340.0f, 224.0f));
		}
	}

	if (DialoguePanel)
	{
		if (UCanvasPanelSlot* DialogueSlot = RootCanvas->AddChildToCanvas(DialoguePanel))
		{
			DialogueSlot->SetAnchors(FAnchors(0.5f, 1.0f, 0.5f, 1.0f));
			DialogueSlot->SetAlignment(FVector2D(0.5f, 1.0f));
			DialogueSlot->SetPosition(FVector2D(0.0f, -108.0f));
			DialogueSlot->SetSize(FVector2D(620.0f, 72.0f));
		}
	}

	if (LootPanel)
	{
		if (UCanvasPanelSlot* LootSlot = RootCanvas->AddChildToCanvas(LootPanel))
		{
			LootSlot->SetAnchors(FAnchors(0.5f, 0.0f, 0.5f, 0.0f));
			LootSlot->SetAlignment(FVector2D(0.5f, 0.0f));
			LootSlot->SetPosition(FVector2D(0.0f, 24.0f));
			LootSlot->SetSize(FVector2D(420.0f, 52.0f));
		}
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
				TEXT("Level %d   XP %d\nHP %.0f/%.0f   Mana %.0f/%.0f"),
				Stats->Level,
				Stats->CurrentExperience,
				Stats->CurrentHealth,
				Stats->MaxHealth,
				Stats->CurrentMana,
				Stats->MaxMana)));
			if (HealthBar)
			{
				HealthBar->SetPercent(Stats->MaxHealth > 0.0f ? Stats->CurrentHealth / Stats->MaxHealth : 0.0f);
			}
			if (ManaBar)
			{
				ManaBar->SetPercent(Stats->MaxMana > 0.0f ? Stats->CurrentMana / Stats->MaxMana : 0.0f);
			}
		}
		else
		{
			PlayerStatusText->SetText(FText::FromString(TEXT("Level --   XP --\nHP --/--   Mana --/--")));
			if (HealthBar)
			{
				HealthBar->SetPercent(0.0f);
			}
			if (ManaBar)
			{
				ManaBar->SetPercent(0.0f);
			}
		}
	}

	if (TargetText)
	{
		AActor* TargetActor = Combat ? Combat->CurrentTarget.Get() : nullptr;
		if (TargetActor && TargetActor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()))
		{
			if (TargetPanel)
			{
				TargetPanel->SetVisibility(ESlateVisibility::Visible);
			}
			const FText TargetName = IEmbermereTargetable::Execute_GetTargetDisplayName(TargetActor);
			if (TargetRangeText)
			{
				FString RangeLine = TEXT("No hostile ability ready");
				if (OwningEmbermereCharacter && Hotbar)
				{
					for (const FEmbermereAbilityDefinition& Ability : Hotbar->Slots)
					{
						if (!Ability.AbilityId.IsNone() && Ability.TargetKind == EEmbermereAbilityTargetKind::Enemy)
						{
							const float Distance = FVector::Dist(OwningEmbermereCharacter->GetActorLocation(), TargetActor->GetActorLocation());
							RangeLine = FString::Printf(
								TEXT("%s: %s (%.0fm / %.0fm)"),
								*Ability.DisplayName.ToString(),
								Distance <= Ability.Range ? TEXT("in range") : TEXT("out of range"),
								Distance / 100.0f,
								Ability.Range / 100.0f);
							break;
						}
					}
				}
				TargetRangeText->SetText(FText::FromString(RangeLine));
			}
			if (const UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>())
			{
				TargetText->SetText(FText::FromString(FString::Printf(
					TEXT("Target: %s   %.0f/%.0f HP"),
					*TargetName.ToString(),
					TargetStats->CurrentHealth,
					TargetStats->MaxHealth)));
				if (TargetHealthBar)
				{
					TargetHealthBar->SetVisibility(ESlateVisibility::Visible);
					TargetHealthBar->SetPercent(TargetStats->MaxHealth > 0.0f ? TargetStats->CurrentHealth / TargetStats->MaxHealth : 0.0f);
				}
			}
			else
			{
				TargetText->SetText(FText::FromString(FString::Printf(TEXT("Target: %s"), *TargetName.ToString())));
				if (TargetHealthBar)
				{
					TargetHealthBar->SetVisibility(ESlateVisibility::Collapsed);
				}
			}
		}
		else
		{
			TargetText->SetText(FText::FromString(TEXT("Target: none")));
			if (TargetPanel)
			{
				TargetPanel->SetVisibility(ESlateVisibility::Collapsed);
			}
			if (TargetRangeText)
			{
				TargetRangeText->SetText(FText::GetEmpty());
			}
		}
	}

	if (QuestText)
	{
		if (QuestLog && QuestLog->ActiveQuest.Quest)
		{
			if (QuestPanel)
			{
				QuestPanel->SetVisibility(ESlateVisibility::Visible);
			}
			QuestText->SetText(FText::FromString(FString::Printf(
				TEXT("Quest\n%s\n%d/%d"),
				*QuestLog->ActiveQuest.Quest->Title.ToString(),
				QuestLog->ActiveQuest.CurrentObjectiveCount,
				QuestLog->ActiveQuest.Quest->RequiredObjectiveCount)));
		}
		else
		{
			QuestText->SetText(FText::FromString(TEXT("Quest: none")));
			if (QuestPanel)
			{
				QuestPanel->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

	if (InventoryText)
	{
		FString InventoryLine = TEXT("Inventory (I)\nEmpty");
		if (Inventory && Inventory->Stacks.Num() > 0)
		{
			InventoryLine = TEXT("Inventory (I)");
			const int32 MaxDisplayedStacks = FMath::Min(Inventory->Stacks.Num(), 4);
			const FEmbermereInventoryStack* FirstDisplayedStack = nullptr;
			for (int32 StackIndex = 0; StackIndex < MaxDisplayedStacks; ++StackIndex)
			{
				const FEmbermereInventoryStack& Stack = Inventory->Stacks[StackIndex];
				if (Stack.Item && Stack.Quantity > 0)
				{
					if (!FirstDisplayedStack)
					{
						FirstDisplayedStack = &Stack;
					}
					InventoryLine += FString::Printf(
						TEXT("\n%s x%d"),
						*Stack.Item->DisplayName.ToString(),
						Stack.Quantity);
				}
			}
			if (Inventory->Stacks.Num() > MaxDisplayedStacks)
			{
				InventoryLine += FString::Printf(TEXT("\n+%d more"), Inventory->Stacks.Num() - MaxDisplayedStacks);
			}
			if (FirstDisplayedStack && FirstDisplayedStack->Item)
			{
				const UEmbermereItemData* Item = FirstDisplayedStack->Item;
				InventoryLine += FString::Printf(
					TEXT("\n\nInspecting: %s\nStack: %d / %d"),
					*Item->DisplayName.ToString(),
					FirstDisplayedStack->Quantity,
					Item->MaxStack);
				if (!Item->Description.IsEmpty())
				{
					InventoryLine += FString::Printf(TEXT("\n%s"), *Item->Description.ToString());
				}
			}
		}
		InventoryText->SetText(FText::FromString(InventoryLine));
	}

	static const TCHAR* HotbarKeys[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("Alt+R"), TEXT("Alt+E"), TEXT("R"), TEXT("X"), TEXT("E"), TEXT("F") };
	for (int32 SlotIndex = 0; SlotIndex < HotbarSlotTexts.Num(); ++SlotIndex)
	{
		UTextBlock* SlotText = HotbarSlotTexts[SlotIndex];
		if (!SlotText)
		{
			continue;
		}

		const FString KeyText = SlotIndex < UE_ARRAY_COUNT(HotbarKeys) ? HotbarKeys[SlotIndex] : TEXT("?");
		FString AbilityText = TEXT("-");
		if (Hotbar && Hotbar->Slots.IsValidIndex(SlotIndex) && !Hotbar->Slots[SlotIndex].AbilityId.IsNone())
		{
			AbilityText = Hotbar->Slots[SlotIndex].DisplayName.ToString();
		}
		else if (SlotIndex == 9)
		{
			AbilityText = TEXT("Interact");
		}

		SlotText->SetText(FText::FromString(FString::Printf(TEXT("%s\n%s"), *KeyText, *AbilityText)));
	}
}

void UEmbermerePlayerHudWidget::UpdateInventoryPanelVisibility()
{
	if (InventoryPanel)
	{
		InventoryPanel->SetVisibility(bInventoryPanelVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UEmbermerePlayerHudWidget::ShowLootPopup_Implementation(const FText& LootText)
{
	if (!LootPanel || !LootTextBlock)
	{
		return;
	}

	LootTextBlock->SetText(LootText);
	LootPanel->SetVisibility(ESlateVisibility::Visible);
	if (const UWorld* World = GetWorld())
	{
		LootHideTimeSeconds = World->GetTimeSeconds() + 4.0f;
	}
}

void UEmbermerePlayerHudWidget::ShowDialogue_Implementation(const FText& SpeakerName, const FText& DialogueText)
{
	if (!DialoguePanel || !DialogueTextBlock)
	{
		return;
	}

	DialogueTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%s\n%s"), *SpeakerName.ToString(), *DialogueText.ToString())));
	DialoguePanel->SetVisibility(ESlateVisibility::Visible);
	if (const UWorld* World = GetWorld())
	{
		DialogueHideTimeSeconds = World->GetTimeSeconds() + 7.0f;
	}
}

void UEmbermerePlayerHudWidget::BindComponentEvents()
{
	if (Inventory)
	{
		Inventory->OnItemAdded.AddUniqueDynamic(this, &UEmbermerePlayerHudWidget::HandleItemAdded);
	}
}

void UEmbermerePlayerHudWidget::HandleItemAdded(UEmbermereItemData* Item, int32 Quantity)
{
	if (!Item || Quantity <= 0)
	{
		return;
	}

	ShowLootPopup(FText::FromString(FString::Printf(TEXT("Received: %s x%d"), *Item->DisplayName.ToString(), Quantity)));
}
