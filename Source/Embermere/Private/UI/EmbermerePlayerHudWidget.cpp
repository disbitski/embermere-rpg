#include "UI/EmbermerePlayerHudWidget.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereUiIconSet.h"
#include "UI/EmbermereEquipmentSlotButton.h"
#include "UI/EmbermereItemDragDropOperation.h"
#include "UI/EmbermereInventoryRowButton.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/ProgressBar.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Interfaces/EmbermereTargetable.h"
#include "InputCoreTypes.h"

namespace
{
	constexpr int32 ChatMessageLimit = 6;
	constexpr int32 InventoryVisibleRowCount = 8;
	constexpr float InventoryRowIconSize = 18.0f;
	constexpr float InventoryDetailIconSize = 42.0f;
	constexpr float EquipmentSlotIconSize = 18.0f;
	constexpr float LootPopupIconSize = 32.0f;
	constexpr float HotbarSlotIconSize = 32.0f;
	constexpr float PaperDollBackdropWidth = 128.0f;
	constexpr float PaperDollBackdropHeight = 160.0f;

	const TCHAR* GetEquipmentSlotLabel(EEmbermereEquipmentSlot Slot)
	{
		switch (Slot)
		{
		case EEmbermereEquipmentSlot::MainHand: return TEXT("Main Hand");
		case EEmbermereEquipmentSlot::OffHand: return TEXT("Off Hand");
		case EEmbermereEquipmentSlot::Head: return TEXT("Head");
		case EEmbermereEquipmentSlot::Chest: return TEXT("Chest");
		case EEmbermereEquipmentSlot::Hands: return TEXT("Hands");
		case EEmbermereEquipmentSlot::Legs: return TEXT("Legs");
		case EEmbermereEquipmentSlot::Feet: return TEXT("Feet");
		case EEmbermereEquipmentSlot::Back: return TEXT("Back");
		case EEmbermereEquipmentSlot::Neck: return TEXT("Neck");
		case EEmbermereEquipmentSlot::Ring: return TEXT("Ring");
		default: return TEXT("-");
		}
	}

	void AddStatDelta(TArray<FString>& Deltas, const TCHAR* Label, const float Value)
	{
		if (!FMath::IsNearlyZero(Value))
		{
			Deltas.Add(FString::Printf(TEXT("%s %+.0f"), Label, Value));
		}
	}

	FString GetStatDeltaSummary(
		const FEmbermereItemStatBonuses& Candidate,
		const FEmbermereItemStatBonuses& Equipped)
	{
		TArray<FString> Deltas;
		AddStatDelta(Deltas, TEXT("HP"), Candidate.MaxHealth - Equipped.MaxHealth);
		AddStatDelta(Deltas, TEXT("Mana"), Candidate.MaxMana - Equipped.MaxMana);
		AddStatDelta(Deltas, TEXT("Armor"), Candidate.Armor - Equipped.Armor);
		AddStatDelta(Deltas, TEXT("Power"), Candidate.Power - Equipped.Power);
		return Deltas.Num() > 0 ? FString::Join(Deltas, TEXT(", ")) : TEXT("No stat change");
	}

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

	void SetIconImage(UImage* Image, UTexture2D* Texture)
	{
		if (!Image)
		{
			return;
		}

		Image->SetBrushFromTexture(Texture, true);
		Image->SetVisibility(Texture ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}
}

UEmbermerePlayerHudWidget::UEmbermerePlayerHudWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UiIconSet = TSoftObjectPtr<UEmbermereUiIconSet>(FSoftObjectPath(
		TEXT("/Game/UI/Icons/DA_EmbermereUiIconSet.DA_EmbermereUiIconSet")));
}

UTexture2D* UEmbermerePlayerHudWidget::ResolveItemIconForUi(const UEmbermereItemData* Item) const
{
	const UEmbermereUiIconSet* IconSet = UiIconSet.IsNull() ? nullptr : UiIconSet.LoadSynchronous();
	return IconSet ? IconSet->ResolveItemIcon(Item) : nullptr;
}

UTexture2D* UEmbermerePlayerHudWidget::ResolveEquipmentSlotIconForUi(EEmbermereEquipmentSlot EquipmentSlot) const
{
	const UEmbermereUiIconSet* IconSet = UiIconSet.IsNull() ? nullptr : UiIconSet.LoadSynchronous();
	return IconSet ? IconSet->ResolveEquipmentSlotIcon(EquipmentSlot) : nullptr;
}

UTexture2D* UEmbermerePlayerHudWidget::ResolveAbilityIconForUi(const FEmbermereAbilityDefinition& Ability) const
{
	const UEmbermereUiIconSet* IconSet = UiIconSet.IsNull() ? nullptr : UiIconSet.LoadSynchronous();
	return IconSet ? IconSet->ResolveAbilityIcon(Ability) : nullptr;
}

UTexture2D* UEmbermerePlayerHudWidget::ResolvePaperDollBackdropForUi() const
{
	const UEmbermereUiIconSet* IconSet = UiIconSet.IsNull() ? nullptr : UiIconSet.LoadSynchronous();
	return IconSet ? IconSet->ResolvePaperDollBackdrop() : nullptr;
}

FVector2D UEmbermerePlayerHudWidget::GetInventoryRowIconDimensions() const
{
	return FVector2D(InventoryRowIconSize);
}

FVector2D UEmbermerePlayerHudWidget::GetInventoryDetailIconDimensions() const
{
	return FVector2D(InventoryDetailIconSize);
}

FVector2D UEmbermerePlayerHudWidget::GetEquipmentSlotIconDimensions() const
{
	return FVector2D(EquipmentSlotIconSize);
}

FVector2D UEmbermerePlayerHudWidget::GetLootPopupIconDimensions() const
{
	return FVector2D(LootPopupIconSize);
}

FVector2D UEmbermerePlayerHudWidget::GetHotbarSlotIconDimensions() const
{
	return FVector2D(HotbarSlotIconSize);
}

FVector2D UEmbermerePlayerHudWidget::GetPaperDollBackdropDimensions() const
{
	return FVector2D(PaperDollBackdropWidth, PaperDollBackdropHeight);
}

void UEmbermerePlayerHudWidget::BindToCharacter(AEmbermereCharacter* Character)
{
	OwningEmbermereCharacter = Character;
	Stats = Character ? Character->Stats : nullptr;
	Combat = Character ? Character->Combat : nullptr;
	Hotbar = Character ? Character->Hotbar : nullptr;
	QuestLog = Character ? Character->QuestLog : nullptr;
	Inventory = Character ? Character->Inventory : nullptr;
	Equipment = Character ? Character->Equipment : nullptr;
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

FReply UEmbermerePlayerHudWidget::NativeOnPreviewMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (!bInventoryPanelVisible || InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
	{
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	ClearPendingDrag();
	const FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
	const int32 StackIndex = FindInventoryStackAtScreenPosition(ScreenPosition);
	if (Inventory && Inventory->Stacks.IsValidIndex(StackIndex) && Inventory->Stacks[StackIndex].Item)
	{
		PendingDragItem = Inventory->Stacks[StackIndex].Item;
		PendingDragSource = EEmbermereItemDragSource::Inventory;
		PendingDragStackIndex = StackIndex;
		SelectInventoryItem(StackIndex);
	}
	else
	{
		const EEmbermereEquipmentSlot EquipmentSlot = FindEquipmentSlotAtScreenPosition(ScreenPosition);
		UEmbermereItemData* EquippedItem = Equipment ? Equipment->GetEquippedItem(EquipmentSlot) : nullptr;
		if (EquippedItem)
		{
			PendingDragItem = EquippedItem;
			PendingDragSource = EEmbermereItemDragSource::Equipment;
			PendingDragEquipmentSlot = EquipmentSlot;
		}
	}

	if (!PendingDragItem)
	{
		return Super::NativeOnPreviewMouseButtonDown(InGeometry, InMouseEvent);
	}

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UEmbermerePlayerHudWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && PendingDragItem)
	{
		UEmbermereItemData* ClickedItem = PendingDragItem;
		const EEmbermereItemDragSource ClickedSource = PendingDragSource;
		const EEmbermereEquipmentSlot ClickedSlot = PendingDragEquipmentSlot;
		const int32 ClickedStackIndex = PendingDragStackIndex;
		const FVector2D ScreenPosition = InMouseEvent.GetScreenSpacePosition();
		ClearPendingDrag();

		if (ClickedSource == EEmbermereItemDragSource::Inventory &&
			FindInventoryStackAtScreenPosition(ScreenPosition) == ClickedStackIndex)
		{
			SelectInventoryItem(ClickedStackIndex);
		}
		else if (ClickedSource == EEmbermereItemDragSource::Equipment &&
			FindEquipmentSlotAtScreenPosition(ScreenPosition) == ClickedSlot)
		{
			ReturnEquipmentItemToInventory(ClickedItem, ClickedSlot);
		}
		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UEmbermerePlayerHudWidget::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if (!PendingDragItem || PendingDragSource == EEmbermereItemDragSource::None)
	{
		Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
		return;
	}

	UEmbermereItemDragDropOperation* Operation = NewObject<UEmbermereItemDragDropOperation>(this);
	Operation->Item = PendingDragItem;
	Operation->Source = PendingDragSource;
	Operation->SourceEquipmentSlot = PendingDragEquipmentSlot;
	Operation->ResolvedIcon = ResolveItemIconForUi(PendingDragItem);
	Operation->Pivot = EDragPivot::MouseDown;

	Operation->DefaultDragVisual = Operation->CreateDragVisual();

	OutOperation = Operation;
	bInventoryDragInProgress = true;
	ClearPendingDrag();
}

bool UEmbermerePlayerHudWidget::NativeOnDragOver(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	const UEmbermereItemDragDropOperation* Operation = Cast<UEmbermereItemDragDropOperation>(InOperation);
	if (!Operation || !Operation->Item)
	{
		return Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);
	}

	ClearDropFeedback();
	const FVector2D ScreenPosition = InDragDropEvent.GetScreenSpacePosition();
	if (Operation->Source == EEmbermereItemDragSource::Inventory)
	{
		HighlightedDropSlot = FindEquipmentSlotAtScreenPosition(ScreenPosition);
		if (HighlightedDropSlot == EEmbermereEquipmentSlot::None)
		{
			return false;
		}
		bHighlightedDropSlotValid = CanDropInventoryItemOnEquipmentSlot(Operation->Item, HighlightedDropSlot);
		RefreshInventoryWindow();
		return true;
	}

	if (Operation->Source == EEmbermereItemDragSource::Equipment && IsInventoryListAtScreenPosition(ScreenPosition))
	{
		bInventoryListDropHighlighted = true;
		bInventoryListDropValid = CanReturnEquipmentItemToInventory(Operation->Item, Operation->SourceEquipmentSlot);
		RefreshInventoryWindow();
		return true;
	}

	return false;
}

bool UEmbermerePlayerHudWidget::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UEmbermereItemDragDropOperation* Operation = Cast<UEmbermereItemDragDropOperation>(InOperation);
	if (!Operation || !Operation->Item)
	{
		return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	}

	bInventoryDragInProgress = false;
	const FVector2D ScreenPosition = InDragDropEvent.GetScreenSpacePosition();
	bool bHandled = false;
	if (Operation->Source == EEmbermereItemDragSource::Inventory)
	{
		const EEmbermereEquipmentSlot TargetSlot = FindEquipmentSlotAtScreenPosition(ScreenPosition);
		if (TargetSlot != EEmbermereEquipmentSlot::None)
		{
			bHandled = EquipInventoryItemToSlot(Operation->Item, TargetSlot);
		}
	}
	else if (Operation->Source == EEmbermereItemDragSource::Equipment && IsInventoryListAtScreenPosition(ScreenPosition))
	{
		bHandled = ReturnEquipmentItemToInventory(Operation->Item, Operation->SourceEquipmentSlot);
	}

	ClearDropFeedback();
	RefreshInventoryWindow();
	return bHandled;
}

void UEmbermerePlayerHudWidget::NativeOnDragLeave(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	ClearDropFeedback();
	RefreshInventoryWindow();
}

void UEmbermerePlayerHudWidget::NativeOnDragCancelled(
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
	bInventoryDragInProgress = false;
	ClearPendingDrag();
	ClearDropFeedback();
	RefreshInventoryWindow();
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

bool UEmbermerePlayerHudWidget::SelectNextInventoryItem(int32 Direction)
{
	if (!Inventory || Inventory->Stacks.Num() <= 1 || Direction == 0)
	{
		return false;
	}

	const int32 StackCount = Inventory->Stacks.Num();
	const int32 Step = Direction > 0 ? 1 : -1;
	SelectedInventoryStackIndex = (SelectedInventoryStackIndex + Step + StackCount) % StackCount;
	RefreshHudText();
	return true;
}

bool UEmbermerePlayerHudWidget::SelectInventoryItem(int32 StackIndex)
{
	if (!Inventory || !Inventory->Stacks.IsValidIndex(StackIndex))
	{
		return false;
	}

	SelectedInventoryStackIndex = StackIndex;
	RefreshHudText();
	return true;
}

bool UEmbermerePlayerHudWidget::SortInventory()
{
	if (!Inventory || Inventory->Stacks.Num() <= 1 || PendingDragItem || bInventoryDragInProgress)
	{
		return false;
	}

	ClampSelectedInventoryStackIndex();
	const UEmbermereItemData* SelectedItem = Inventory->Stacks[SelectedInventoryStackIndex].Item;
	int32 SelectedItemOccurrence = 0;
	for (int32 Index = 0; Index < SelectedInventoryStackIndex; ++Index)
	{
		if (Inventory->Stacks[Index].Item == SelectedItem)
		{
			++SelectedItemOccurrence;
		}
	}

	if (!Inventory->SortStacksByCategoryAndName())
	{
		return false;
	}

	int32 MatchingOccurrence = 0;
	for (int32 Index = 0; Index < Inventory->Stacks.Num(); ++Index)
	{
		if (Inventory->Stacks[Index].Item != SelectedItem)
		{
			continue;
		}

		if (MatchingOccurrence == SelectedItemOccurrence)
		{
			SelectedInventoryStackIndex = Index;
			break;
		}
		++MatchingOccurrence;
	}

	ClampSelectedInventoryStackIndex();
	RefreshHudText();
	AddChatMessage(FText::FromString(TEXT("Inventory sorted")), FLinearColor(0.86f, 0.78f, 0.5f, 1.0f));
	return true;
}

int32 UEmbermerePlayerHudWidget::GetSelectedInventoryStackIndex() const
{
	return SelectedInventoryStackIndex;
}

FText UEmbermerePlayerHudWidget::GetInventoryDisplayText() const
{
	if (!Inventory || Inventory->Stacks.Num() <= 0)
	{
		const int32 MaxSlots = Inventory ? Inventory->MaxSlots : 24;
		return FText::FromString(FString::Printf(
			TEXT("Inventory\nSlots 0 / %d\nEmpty\nQuest rewards appear here.\nI Close"),
			MaxSlots));
	}

	const int32 StackCount = Inventory->Stacks.Num();
	const int32 ClampedSelectedIndex = FMath::Clamp(SelectedInventoryStackIndex, 0, StackCount - 1);
	const int32 MaxDisplayedStacks = FMath::Min(StackCount, 4);
	int32 FirstDisplayedIndex = FMath::Clamp(
		ClampedSelectedIndex - MaxDisplayedStacks + 1,
		0,
		FMath::Max(0, StackCount - MaxDisplayedStacks));
	const int32 LastDisplayedIndex = FMath::Min(StackCount, FirstDisplayedIndex + MaxDisplayedStacks);

	FString InventoryLine = FString::Printf(
		TEXT("Inventory\nSlots %d / %d\nInspecting %d/%d"),
		StackCount,
		Inventory->MaxSlots,
		ClampedSelectedIndex + 1,
		StackCount);

	if (StackCount > 1)
	{
		InventoryLine += TEXT("\nUse [ ] to inspect");
	}
	if (FirstDisplayedIndex > 0)
	{
		InventoryLine += TEXT("\n...");
	}
	for (int32 StackIndex = FirstDisplayedIndex; StackIndex < LastDisplayedIndex; ++StackIndex)
	{
		const FEmbermereInventoryStack& Stack = Inventory->Stacks[StackIndex];
		if (Stack.Item && Stack.Quantity > 0)
		{
			InventoryLine += FString::Printf(
				TEXT("\n%s %s x%d"),
				StackIndex == ClampedSelectedIndex ? TEXT(">") : TEXT(" "),
				*Stack.Item->DisplayName.ToString(),
				Stack.Quantity);
		}
	}
	if (LastDisplayedIndex < StackCount)
	{
		InventoryLine += FString::Printf(TEXT("\n+%d more"), StackCount - LastDisplayedIndex);
	}
	if (Inventory->Stacks.IsValidIndex(ClampedSelectedIndex))
	{
		const FEmbermereInventoryStack& SelectedStack = Inventory->Stacks[ClampedSelectedIndex];
		if (SelectedStack.Item)
		{
			const UEmbermereItemData* Item = SelectedStack.Item;
			InventoryLine += FString::Printf(
				TEXT("\n\n%s\nStack: %d / %d\n%s\nAction: %s"),
				*Item->DisplayName.ToString(),
				SelectedStack.Quantity,
				Item->MaxStack,
				*Item->GetCategoryDisplayName().ToString(),
				*GetSelectedInventoryActionLabel().ToString());
			if (Item->IsEquippable())
			{
				InventoryLine += FString::Printf(
					TEXT("\nSlot: %s\nRequired level: %d"),
					*Item->GetEquipmentSlotDisplayName().ToString(),
					FMath::Max(1, Item->RequiredLevel));
			}
			if (Item->IsEquippable() || Item->IsConsumable())
			{
				InventoryLine += FString::Printf(TEXT("\n%s"), *Item->GetEffectSummary().ToString());
			}
			const FText ComparisonText = BuildItemComparisonText(Item);
			if (!ComparisonText.IsEmpty())
			{
				InventoryLine += FString::Printf(TEXT("\n%s"), *ComparisonText.ToString());
			}
			if (!Item->Description.IsEmpty())
			{
				InventoryLine += FString::Printf(TEXT("\n%s"), *Item->Description.ToString());
			}
		}
	}

	return FText::FromString(InventoryLine);
}

FText UEmbermerePlayerHudWidget::GetSelectedInventoryActionLabel() const
{
	if (!Inventory || !Inventory->Stacks.IsValidIndex(SelectedInventoryStackIndex))
	{
		return FText::FromString(TEXT("Inspect"));
	}

	const UEmbermereItemData* Item = Inventory->Stacks[SelectedInventoryStackIndex].Item;
	if (!Item)
	{
		return FText::FromString(TEXT("Inspect"));
	}
	return Item->GetPrimaryActionLabel();
}

FText UEmbermerePlayerHudWidget::GetSelectedInventoryComparisonText() const
{
	if (!Inventory || !Inventory->Stacks.IsValidIndex(SelectedInventoryStackIndex))
	{
		return FText::GetEmpty();
	}

	return BuildItemComparisonText(Inventory->Stacks[SelectedInventoryStackIndex].Item);
}

FText UEmbermerePlayerHudWidget::GetSelectedInventoryTooltipText() const
{
	if (!Inventory || !Inventory->Stacks.IsValidIndex(SelectedInventoryStackIndex))
	{
		return FText::GetEmpty();
	}

	const FEmbermereInventoryStack& Stack = Inventory->Stacks[SelectedInventoryStackIndex];
	return BuildItemTooltipText(Stack.Item, Stack.Quantity);
}

FText UEmbermerePlayerHudWidget::BuildItemComparisonText(const UEmbermereItemData* Item) const
{
	if (!Item || !Item->IsEquippable())
	{
		return FText::GetEmpty();
	}

	const UEmbermereItemData* EquippedItem = Equipment ? Equipment->GetEquippedItem(Item->EquipmentSlot) : nullptr;
	if (EquippedItem == Item)
	{
		return FText::FromString(TEXT("Currently equipped"));
	}

	const FEmbermereItemStatBonuses EquippedBonuses = EquippedItem
		? EquippedItem->StatBonuses
		: FEmbermereItemStatBonuses();
	const FString ComparisonName = EquippedItem
		? EquippedItem->DisplayName.ToString()
		: FString::Printf(TEXT("empty %s"), GetEquipmentSlotLabel(Item->EquipmentSlot));
	return FText::FromString(FString::Printf(
		TEXT("Vs %s: %s"),
		*ComparisonName,
		*GetStatDeltaSummary(Item->StatBonuses, EquippedBonuses)));
}

FText UEmbermerePlayerHudWidget::BuildItemTooltipText(const UEmbermereItemData* Item, int32 Quantity) const
{
	if (!Item)
	{
		return FText::GetEmpty();
	}

	TArray<FString> Lines;
	Lines.Add(FString::Printf(TEXT("%s x%d"), *Item->DisplayName.ToString(), FMath::Max(1, Quantity)));
	if (Item->IsEquippable())
	{
		Lines.Add(FString::Printf(
			TEXT("%s | %s | Level %d"),
			*Item->GetCategoryDisplayName().ToString(),
			*Item->GetEquipmentSlotDisplayName().ToString(),
			FMath::Max(1, Item->RequiredLevel)));
	}
	else
	{
		Lines.Add(Item->GetCategoryDisplayName().ToString());
	}

	if (Item->IsEquippable() || Item->IsConsumable())
	{
		Lines.Add(Item->GetEffectSummary().ToString());
	}
	const FText ComparisonText = BuildItemComparisonText(Item);
	if (!ComparisonText.IsEmpty())
	{
		Lines.Add(ComparisonText.ToString());
	}
	if (!Item->Description.IsEmpty())
	{
		Lines.Add(TEXT(""));
		Lines.Add(Item->Description.ToString());
	}
	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

FText UEmbermerePlayerHudWidget::GetEquipmentDisplayText() const
{
	struct FSlotLabel
	{
		EEmbermereEquipmentSlot Slot;
		const TCHAR* Label;
	};
	static const FSlotLabel Slots[] = {
		{EEmbermereEquipmentSlot::Head, TEXT("Head")},
		{EEmbermereEquipmentSlot::Neck, TEXT("Neck")},
		{EEmbermereEquipmentSlot::Back, TEXT("Back")},
		{EEmbermereEquipmentSlot::Chest, TEXT("Chest")},
		{EEmbermereEquipmentSlot::MainHand, TEXT("Main Hand")},
		{EEmbermereEquipmentSlot::OffHand, TEXT("Off Hand")},
		{EEmbermereEquipmentSlot::Hands, TEXT("Hands")},
		{EEmbermereEquipmentSlot::Legs, TEXT("Legs")},
		{EEmbermereEquipmentSlot::Feet, TEXT("Feet")},
		{EEmbermereEquipmentSlot::Ring, TEXT("Ring")},
	};

	FString Result = TEXT("Equipment");
	for (const FSlotLabel& Slot : Slots)
	{
		const UEmbermereItemData* Item = Equipment ? Equipment->GetEquippedItem(Slot.Slot) : nullptr;
		Result += FString::Printf(TEXT("\n%-10s  %s"), Slot.Label, Item ? *Item->DisplayName.ToString() : TEXT("-"));
	}

	const FEmbermereItemStatBonuses Bonuses = Equipment ? Equipment->GetTotalStatBonuses() : FEmbermereItemStatBonuses();
	Result += FString::Printf(
		TEXT("\n\nBonuses\nHP %+.0f  Mana %+.0f\nArmor %+.0f  Power %+.0f\nClick an occupied slot to unequip."),
		Bonuses.MaxHealth,
		Bonuses.MaxMana,
		Bonuses.Armor,
		Bonuses.Power);
	return FText::FromString(Result);
}

bool UEmbermerePlayerHudWidget::ActivateSelectedInventoryItem()
{
	if (!Inventory || !Inventory->Stacks.IsValidIndex(SelectedInventoryStackIndex))
	{
		return false;
	}

	return ActivateInventoryItem(Inventory->Stacks[SelectedInventoryStackIndex].Item);
}

bool UEmbermerePlayerHudWidget::ActivateInventoryItem(UEmbermereItemData* Item)
{
	if (!Inventory || !Item || !Inventory->CanRemoveItem(Item, 1))
	{
		return false;
	}

	if (Item->IsConsumable())
	{
		if (!Stats)
		{
			return false;
		}

		const float HealthBefore = Stats->CurrentHealth;
		const float ManaBefore = Stats->CurrentMana;
		Stats->Heal(Item->ConsumableEffects.HealHealth);
		Stats->RestoreMana(Item->ConsumableEffects.RestoreMana);
		const bool bAppliedEffect = Stats->CurrentHealth > HealthBefore || Stats->CurrentMana > ManaBefore;
		if (!bAppliedEffect || !Inventory->RemoveItem(Item, 1))
		{
			return false;
		}

		AddChatMessage(
			FText::FromString(FString::Printf(TEXT("Used %s"), *Item->DisplayName.ToString())),
			FLinearColor(0.48f, 0.92f, 0.62f, 1.0f));
		ClampSelectedInventoryStackIndex();
		RefreshHudText();
		return true;
	}

	if (!Item->IsEquippable() || !Equipment)
	{
		return false;
	}
	if (Equipment->IsItemEquipped(Item))
	{
		return false;
	}

	const int32 CharacterLevel = Stats ? Stats->Level : 1;
	const bool bChanged = Equipment->EquipFromInventory(Item, CharacterLevel, Inventory);

	if (bChanged)
	{
		AddChatMessage(
			FText::FromString(FString::Printf(
				TEXT("Equipped %s"),
				*Item->DisplayName.ToString())),
			FLinearColor(1.0f, 0.82f, 0.3f, 1.0f));
		ClampSelectedInventoryStackIndex();
		RefreshHudText();
	}
	return bChanged;
}

bool UEmbermerePlayerHudWidget::EquipInventoryItemToSlot(
	UEmbermereItemData* Item,
	EEmbermereEquipmentSlot TargetSlot)
{
	if (!CanDropInventoryItemOnEquipmentSlot(Item, TargetSlot))
	{
		return false;
	}

	return ActivateInventoryItem(Item);
}

bool UEmbermerePlayerHudWidget::CanDropInventoryItemOnEquipmentSlot(
	const UEmbermereItemData* Item,
	EEmbermereEquipmentSlot TargetSlot) const
{
	if (!Item || !Inventory || !Equipment || !Item->IsEquippable() ||
		TargetSlot == EEmbermereEquipmentSlot::None || Item->EquipmentSlot != TargetSlot ||
		!Inventory->CanRemoveItem(Item, 1) || Equipment->IsItemEquipped(Item))
	{
		return false;
	}

	return Equipment->CanEquip(Item, Stats ? Stats->Level : 1);
}

bool UEmbermerePlayerHudWidget::ActivateEquipmentSlot(EEmbermereEquipmentSlot EquipmentSlot)
{
	if (!Inventory || !Equipment)
	{
		return false;
	}

	UEmbermereItemData* Item = Equipment->GetEquippedItem(EquipmentSlot);
	if (!Item)
	{
		return false;
	}
	if (!Equipment->UnequipToInventory(EquipmentSlot, Inventory))
	{
		AddChatMessage(
			FText::FromString(FString::Printf(
				TEXT("Cannot unequip %s: inventory is full."),
				*Item->DisplayName.ToString())),
			FLinearColor(1.0f, 0.42f, 0.25f, 1.0f));
		return false;
	}

	AddChatMessage(
		FText::FromString(FString::Printf(TEXT("Unequipped %s"), *Item->DisplayName.ToString())),
		FLinearColor(1.0f, 0.82f, 0.3f, 1.0f));
	ClampSelectedInventoryStackIndex();
	RefreshHudText();
	return true;
}

bool UEmbermerePlayerHudWidget::CanReturnEquipmentItemToInventory(
	const UEmbermereItemData* ExpectedItem,
	EEmbermereEquipmentSlot SourceSlot) const
{
	return ExpectedItem && Inventory && Equipment &&
		Equipment->GetEquippedItem(SourceSlot) == ExpectedItem &&
		Inventory->CanAddItem(ExpectedItem, 1);
}

bool UEmbermerePlayerHudWidget::ReturnEquipmentItemToInventory(
	UEmbermereItemData* ExpectedItem,
	EEmbermereEquipmentSlot SourceSlot)
{
	if (!ExpectedItem || !Equipment || Equipment->GetEquippedItem(SourceSlot) != ExpectedItem)
	{
		return false;
	}

	return ActivateEquipmentSlot(SourceSlot);
}

FText UEmbermerePlayerHudWidget::GetHotbarSlotDisplayText(int32 SlotIndex, float CooldownRemainingSeconds) const
{
	static const TCHAR* HotbarKeys[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("Alt+R"), TEXT("Alt+E"), TEXT("R"), TEXT("X"), TEXT("E"), TEXT("F") };
	const FString KeyText = SlotIndex >= 0 && SlotIndex < UE_ARRAY_COUNT(HotbarKeys) ? HotbarKeys[SlotIndex] : TEXT("?");
	FString AbilityText = TEXT("-");
	if (Hotbar && Hotbar->Slots.IsValidIndex(SlotIndex) && !Hotbar->Slots[SlotIndex].AbilityId.IsNone())
	{
		AbilityText = Hotbar->Slots[SlotIndex].DisplayName.ToString();
	}
	else if (SlotIndex == 9)
	{
		AbilityText = TEXT("Interact");
	}

	if (CooldownRemainingSeconds > 0.05f)
	{
		return FText::FromString(FString::Printf(
			TEXT("%s  %.1fs\n%s"),
			*KeyText,
			CooldownRemainingSeconds,
			*AbilityText));
	}

	return FText::FromString(FString::Printf(TEXT("%s\n%s"), *KeyText, *AbilityText));
}

FText UEmbermerePlayerHudWidget::GetHotbarSlotTooltipText(int32 SlotIndex) const
{
	if (!Hotbar || !Hotbar->Slots.IsValidIndex(SlotIndex) || Hotbar->Slots[SlotIndex].AbilityId.IsNone())
	{
		return SlotIndex == 9
			? FText::FromString(TEXT("Interact with a nearby character or object."))
			: FText::GetEmpty();
	}

	const FEmbermereAbilityDefinition& Ability = Hotbar->Slots[SlotIndex];
	const FString ManaText = Ability.ManaCost > 0.0f
		? FString::Printf(TEXT("%.0f Mana"), Ability.ManaCost)
		: TEXT("No mana");
	const FString RangeText = Ability.Range > 0.0f
		? FString::Printf(TEXT("%.1fm Range"), Ability.Range / 100.0f)
		: TEXT("Self");

	return FText::FromString(FString::Printf(
		TEXT("%s\n%s\nPower %.0f | %s | %s | %.1fs Cooldown"),
		*Ability.DisplayName.ToString(),
		*Ability.Description.ToString(),
		Ability.Power,
		*ManaText,
		*RangeText,
		Ability.Cooldown));
}

void UEmbermerePlayerHudWidget::AddChatMessage(const FText& Message, FLinearColor MessageColor)
{
	if (Message.IsEmpty())
	{
		return;
	}

	ChatMessages.Add(TPair<FText, FLinearColor>(Message, MessageColor));
	while (ChatMessages.Num() > ChatMessageLimit)
	{
		ChatMessages.RemoveAt(0);
	}
	RefreshChatMessages();
}

int32 UEmbermerePlayerHudWidget::GetChatMessageCount() const
{
	return ChatMessages.Num();
}

int32 UEmbermerePlayerHudWidget::GetChatMessageLimit() const
{
	return ChatMessageLimit;
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

	InventoryPanel = MakePanel(WidgetTree, TEXT("InventoryPanel"), FLinearColor(0.025f, 0.028f, 0.024f, 0.94f));
	UVerticalBox* InventoryStack = MakePanelStack(WidgetTree, InventoryPanel, TEXT("InventoryStack"));
	InventoryText = MakeHudText(WidgetTree, TEXT("InventoryTitleText"), FLinearColor(1.0f, 0.82f, 0.38f, 1.0f), 18.0f);
	InventoryCapacityText = MakeHudText(WidgetTree, TEXT("InventoryCapacityText"), FLinearColor(0.68f, 0.72f, 0.66f, 1.0f), 12.0f);
	InventorySortButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("InventorySortButton"));
	InventorySortText = MakeHudText(WidgetTree, TEXT("InventorySortText"), FLinearColor(0.98f, 0.86f, 0.52f, 1.0f), 12.0f);
	InventoryFooterText = MakeHudText(WidgetTree, TEXT("InventoryFooterText"), FLinearColor(0.72f, 0.75f, 0.68f, 1.0f), 11.0f);
	if (InventoryText)
	{
		InventoryText->SetText(FText::FromString(TEXT("Inventory")));
	}
	if (InventoryCapacityText)
	{
		InventoryCapacityText->SetJustification(ETextJustify::Right);
	}
	if (InventorySortButton && InventorySortText)
	{
		InventorySortButton->SetBackgroundColor(FLinearColor(0.22f, 0.16f, 0.055f, 0.92f));
		InventorySortButton->SetToolTipText(FText::FromString(TEXT("Sort by category, then name")));
		InventorySortButton->OnClicked.AddUniqueDynamic(this, &UEmbermerePlayerHudWidget::HandleInventorySortClicked);
		InventorySortText->SetText(FText::FromString(TEXT("Sort")));
		InventorySortText->SetJustification(ETextJustify::Center);
		InventorySortButton->AddChild(InventorySortText);
	}
	if (InventoryFooterText)
	{
		InventoryFooterText->SetText(FText::FromString(TEXT("Drag gear or click item   |   [ / ] Inspect   |   I Close")));
		InventoryFooterText->SetJustification(ETextJustify::Center);
	}

	UHorizontalBox* InventoryHeader = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("InventoryHeader"));
	if (InventoryHeader)
	{
		USizeBox* TitleSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("InventoryTitleSize"));
		USizeBox* CapacitySize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("InventoryCapacitySize"));
		USizeBox* SortSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("InventorySortSize"));
		if (TitleSize && InventoryText)
		{
			TitleSize->SetWidthOverride(300.0f);
			TitleSize->AddChild(InventoryText);
			InventoryHeader->AddChildToHorizontalBox(TitleSize);
		}
		if (CapacitySize && InventoryCapacityText)
		{
			CapacitySize->SetWidthOverride(170.0f);
			CapacitySize->AddChild(InventoryCapacityText);
			InventoryHeader->AddChildToHorizontalBox(CapacitySize);
		}
		if (SortSize && InventorySortButton)
		{
			SortSize->SetWidthOverride(72.0f);
			SortSize->SetHeightOverride(24.0f);
			SortSize->AddChild(InventorySortButton);
			InventoryHeader->AddChildToHorizontalBox(SortSize);
		}
	}
	AddStackChild(InventoryStack, InventoryHeader, 8.0f);

	UHorizontalBox* InventoryBody = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("InventoryBody"));
	UVerticalBox* InventoryListStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InventoryListStack"));
	UVerticalBox* InventoryDetailStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InventoryDetailStack"));
	UVerticalBox* InventoryEquipmentStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("InventoryEquipmentStack"));
	InventoryListDropPanel = MakePanel(WidgetTree, TEXT("InventoryListDropPanel"), FLinearColor(0.035f, 0.04f, 0.032f, 0.55f));
	if (InventoryListDropPanel)
	{
		InventoryListDropPanel->SetPadding(FMargin(0.0f));
		InventoryListDropPanel->SetContent(InventoryListStack);
	}
	InventoryRowTexts.Reset();
	InventoryRowIcons.Reset();
	InventoryRowButtons.Reset();
	for (int32 RowIndex = 0; RowIndex < InventoryVisibleRowCount; ++RowIndex)
	{
		UEmbermereInventoryRowButton* RowButton = WidgetTree->ConstructWidget<UEmbermereInventoryRowButton>(
			UEmbermereInventoryRowButton::StaticClass(),
			*FString::Printf(TEXT("InventoryRowButton_%d"), RowIndex));
		UHorizontalBox* RowContent = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			*FString::Printf(TEXT("InventoryRowContent_%d"), RowIndex));
		UImage* RowIcon = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(),
			*FString::Printf(TEXT("InventoryRowIcon_%d"), RowIndex));
		UTextBlock* RowText = MakeHudText(
			WidgetTree,
			*FString::Printf(TEXT("InventoryRowText_%d"), RowIndex),
			FLinearColor(0.82f, 0.84f, 0.78f, 1.0f),
			13.0f);
		if (!RowButton || !RowContent || !RowIcon || !RowText)
		{
			continue;
		}

		RowButton->SetVisibleRowIndex(RowIndex);
		RowButton->OnInventoryRowClicked.AddUniqueDynamic(this, &UEmbermerePlayerHudWidget::HandleInventoryRowClicked);
		RowButton->SetBackgroundColor(FLinearColor(0.08f, 0.085f, 0.075f, 0.72f));
		RowText->SetAutoWrapText(false);
		RowText->SetClipping(EWidgetClipping::ClipToBounds);
		RowText->SetMargin(FMargin(7.0f, 2.0f));
		RowIcon->SetVisibility(ESlateVisibility::Collapsed);
		USizeBox* RowIconSize = MakeSizedWidget(
			WidgetTree,
			RowIcon,
			*FString::Printf(TEXT("InventoryRowIconSize_%d"), RowIndex),
			InventoryRowIconSize,
			InventoryRowIconSize);
		if (UHorizontalBoxSlot* IconSlot = RowContent->AddChildToHorizontalBox(RowIconSize))
		{
			IconSlot->SetPadding(FMargin(4.0f, 2.0f, 2.0f, 2.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		if (UHorizontalBoxSlot* TextSlot = RowContent->AddChildToHorizontalBox(RowText))
		{
			TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			TextSlot->SetVerticalAlignment(VAlign_Center);
		}
		RowButton->AddChild(RowContent);
		InventoryRowButtons.Add(RowButton);
		InventoryRowIcons.Add(RowIcon);
		InventoryRowTexts.Add(RowText);
		AddStackChild(
			InventoryListStack,
			MakeSizedWidget(
				WidgetTree,
				RowButton,
				*FString::Printf(TEXT("InventoryRowSize_%d"), RowIndex),
				205.0f,
				23.0f),
			3.0f);
	}

	InventoryDetailNameText = MakeHudText(WidgetTree, TEXT("InventoryDetailNameText"), FLinearColor(1.0f, 0.84f, 0.44f, 1.0f), 16.0f);
	InventoryDetailIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("InventoryDetailIcon"));
	InventoryDetailMetaText = MakeHudText(WidgetTree, TEXT("InventoryDetailMetaText"), FLinearColor(0.7f, 0.82f, 0.72f, 1.0f), 12.0f);
	InventoryDetailDescriptionText = MakeHudText(WidgetTree, TEXT("InventoryDetailDescriptionText"), FLinearColor(0.84f, 0.83f, 0.76f, 1.0f), 13.0f);
	InventoryActionButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("InventoryActionButton"));
	InventoryActionText = MakeHudText(WidgetTree, TEXT("InventoryActionText"), FLinearColor(0.98f, 0.88f, 0.58f, 1.0f), 13.0f);
	InventoryEquipmentText = MakeHudText(WidgetTree, TEXT("InventoryEquipmentText"), FLinearColor(0.78f, 0.82f, 0.72f, 1.0f), 10.0f);
	if (InventoryDetailDescriptionText)
	{
		InventoryDetailDescriptionText->SetAutoWrapText(true);
	}
	UHorizontalBox* InventoryDetailHeader = WidgetTree->ConstructWidget<UHorizontalBox>(
		UHorizontalBox::StaticClass(), TEXT("InventoryDetailHeader"));
	if (InventoryDetailHeader && InventoryDetailIcon && InventoryDetailNameText)
	{
		InventoryDetailIcon->SetVisibility(ESlateVisibility::Collapsed);
		USizeBox* DetailIconSize = MakeSizedWidget(
			WidgetTree,
			InventoryDetailIcon,
			TEXT("InventoryDetailIconSize"),
			InventoryDetailIconSize,
			InventoryDetailIconSize);
		if (UHorizontalBoxSlot* IconSlot = InventoryDetailHeader->AddChildToHorizontalBox(DetailIconSize))
		{
			IconSlot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		if (UHorizontalBoxSlot* NameSlot = InventoryDetailHeader->AddChildToHorizontalBox(InventoryDetailNameText))
		{
			NameSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			NameSlot->SetVerticalAlignment(VAlign_Center);
		}
		AddStackChild(InventoryDetailStack, InventoryDetailHeader, 6.0f);
	}
	else
	{
		AddStackChild(InventoryDetailStack, InventoryDetailNameText, 8.0f);
	}
	AddStackChild(InventoryDetailStack, InventoryDetailMetaText, 12.0f);
	AddStackChild(InventoryDetailStack, InventoryDetailDescriptionText, 10.0f);
	if (InventoryActionButton && InventoryActionText)
	{
		InventoryActionButton->SetBackgroundColor(FLinearColor(0.28f, 0.19f, 0.055f, 0.92f));
		InventoryActionButton->OnClicked.AddUniqueDynamic(this, &UEmbermerePlayerHudWidget::HandleInventoryActionClicked);
		InventoryActionText->SetJustification(ETextJustify::Center);
		InventoryActionButton->AddChild(InventoryActionText);
		AddStackChild(
			InventoryDetailStack,
			MakeSizedWidget(WidgetTree, InventoryActionButton, TEXT("InventoryActionSize"), 150.0f, 28.0f),
			0.0f);
	}
	UTextBlock* EquipmentTitle = MakeHudText(WidgetTree, TEXT("InventoryEquipmentTitle"), FLinearColor(1.0f, 0.82f, 0.38f, 1.0f), 15.0f);
	UGridPanel* EquipmentGrid = WidgetTree->ConstructWidget<UGridPanel>(UGridPanel::StaticClass(), TEXT("InventoryEquipmentGrid"));
	UOverlay* EquipmentPresentation = WidgetTree->ConstructWidget<UOverlay>(
		UOverlay::StaticClass(),
		TEXT("InventoryEquipmentPresentation"));
	InventoryPaperDollBackdrop = WidgetTree->ConstructWidget<UImage>(
		UImage::StaticClass(),
		TEXT("InventoryPaperDollBackdrop"));
	if (InventoryPaperDollBackdrop)
	{
		SetIconImage(InventoryPaperDollBackdrop, ResolvePaperDollBackdropForUi());
		InventoryPaperDollBackdrop->SetRenderOpacity(0.82f);
	}
	if (EquipmentTitle)
	{
		EquipmentTitle->SetText(FText::FromString(TEXT("Equipment")));
		EquipmentTitle->SetJustification(ETextJustify::Center);
	}
	AddStackChild(InventoryEquipmentStack, EquipmentTitle, 5.0f);
	InventoryEquipmentSlotButtons.Reset();
	InventoryEquipmentSlotTexts.Reset();
	InventoryEquipmentSlotIcons.Reset();
	struct FEquipmentSlotLayout
	{
		EEmbermereEquipmentSlot Slot;
		int32 Row;
		int32 Column;
	};
	static const FEquipmentSlotLayout EquipmentSlotLayouts[] = {
		{EEmbermereEquipmentSlot::Head, 0, 1},
		{EEmbermereEquipmentSlot::Back, 0, 2},
		{EEmbermereEquipmentSlot::MainHand, 1, 0},
		{EEmbermereEquipmentSlot::Neck, 1, 1},
		{EEmbermereEquipmentSlot::OffHand, 1, 2},
		{EEmbermereEquipmentSlot::Hands, 2, 0},
		{EEmbermereEquipmentSlot::Chest, 2, 1},
		{EEmbermereEquipmentSlot::Ring, 2, 2},
		{EEmbermereEquipmentSlot::Legs, 3, 1},
		{EEmbermereEquipmentSlot::Feet, 4, 1},
	};
	for (const FEquipmentSlotLayout& Layout : EquipmentSlotLayouts)
	{
		UEmbermereEquipmentSlotButton* SlotButton = WidgetTree->ConstructWidget<UEmbermereEquipmentSlotButton>(
			UEmbermereEquipmentSlotButton::StaticClass(),
			*FString::Printf(TEXT("EquipmentSlotButton_%d"), static_cast<int32>(Layout.Slot)));
		UHorizontalBox* SlotContent = WidgetTree->ConstructWidget<UHorizontalBox>(
			UHorizontalBox::StaticClass(),
			*FString::Printf(TEXT("EquipmentSlotContent_%d"), static_cast<int32>(Layout.Slot)));
		UImage* SlotIcon = WidgetTree->ConstructWidget<UImage>(
			UImage::StaticClass(),
			*FString::Printf(TEXT("EquipmentSlotIcon_%d"), static_cast<int32>(Layout.Slot)));
		UTextBlock* SlotText = MakeHudText(
			WidgetTree,
			*FString::Printf(TEXT("EquipmentSlotText_%d"), static_cast<int32>(Layout.Slot)),
			FLinearColor(0.66f, 0.68f, 0.62f, 1.0f),
			7.0f);
		if (!SlotButton || !SlotContent || !SlotIcon || !SlotText || !EquipmentGrid)
		{
			continue;
		}

		SlotButton->SetEquipmentSlot(Layout.Slot);
		SlotButton->OnEquipmentSlotClicked.AddUniqueDynamic(this, &UEmbermerePlayerHudWidget::HandleEquipmentSlotClicked);
		SlotButton->SetBackgroundColor(FLinearColor(0.07f, 0.075f, 0.065f, 0.9f));
		SlotText->SetText(FText::FromString(FString::Printf(TEXT("%s\n-"), GetEquipmentSlotLabel(Layout.Slot))));
		SlotText->SetJustification(ETextJustify::Center);
		SlotText->SetAutoWrapText(false);
		SlotText->SetClipping(EWidgetClipping::ClipToBounds);
		USizeBox* SlotIconSize = MakeSizedWidget(
			WidgetTree,
			SlotIcon,
			*FString::Printf(TEXT("EquipmentSlotIconSize_%d"), static_cast<int32>(Layout.Slot)),
			EquipmentSlotIconSize,
			EquipmentSlotIconSize);
		if (UHorizontalBoxSlot* IconSlot = SlotContent->AddChildToHorizontalBox(SlotIconSize))
		{
			IconSlot->SetPadding(FMargin(2.0f, 0.0f, 2.0f, 0.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		if (UHorizontalBoxSlot* TextSlot = SlotContent->AddChildToHorizontalBox(SlotText))
		{
			TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			TextSlot->SetVerticalAlignment(VAlign_Center);
		}
		SlotButton->AddChild(SlotContent);
		InventoryEquipmentSlotButtons.Add(SlotButton);
		InventoryEquipmentSlotTexts.Add(SlotText);
		InventoryEquipmentSlotIcons.Add(SlotIcon);

		USizeBox* SlotSize = MakeSizedWidget(
			WidgetTree,
			SlotButton,
			*FString::Printf(TEXT("EquipmentSlotSize_%d"), static_cast<int32>(Layout.Slot)),
			62.0f,
			31.0f);
		if (UGridSlot* GridSlot = EquipmentGrid->AddChildToGrid(SlotSize, Layout.Row, Layout.Column))
		{
			GridSlot->SetPadding(FMargin(2.0f, 1.0f));
		}
	}
	if (EquipmentPresentation && EquipmentGrid)
	{
		USizeBox* PaperDollSize = MakeSizedWidget(
			WidgetTree,
			InventoryPaperDollBackdrop,
			TEXT("InventoryPaperDollBackdropSize"),
			PaperDollBackdropWidth,
			PaperDollBackdropHeight);
		if (UOverlaySlot* BackdropSlot = EquipmentPresentation->AddChildToOverlay(PaperDollSize))
		{
			BackdropSlot->SetHorizontalAlignment(HAlign_Center);
			BackdropSlot->SetVerticalAlignment(VAlign_Center);
		}
		if (UOverlaySlot* GridOverlaySlot = EquipmentPresentation->AddChildToOverlay(EquipmentGrid))
		{
			GridOverlaySlot->SetHorizontalAlignment(HAlign_Center);
			GridOverlaySlot->SetVerticalAlignment(VAlign_Center);
		}
		AddStackChild(InventoryEquipmentStack, EquipmentPresentation, 5.0f);
	}
	else
	{
		AddStackChild(InventoryEquipmentStack, EquipmentGrid, 5.0f);
	}
	if (InventoryEquipmentText)
	{
		InventoryEquipmentText->SetAutoWrapText(true);
		AddStackChild(InventoryEquipmentStack, InventoryEquipmentText, 0.0f);
	}

	if (InventoryBody)
	{
		USizeBox* ListSize = MakeSizedWidget(
			WidgetTree,
			InventoryListDropPanel ? static_cast<UWidget*>(InventoryListDropPanel) : static_cast<UWidget*>(InventoryListStack),
			TEXT("InventoryListSize"),
			205.0f,
			238.0f);
		USizeBox* DetailSize = MakeSizedWidget(WidgetTree, InventoryDetailStack, TEXT("InventoryDetailSize"), 225.0f, 238.0f);
		USizeBox* EquipmentSize = MakeSizedWidget(WidgetTree, InventoryEquipmentStack, TEXT("InventoryEquipmentSize"), 210.0f, 238.0f);
		if (UHorizontalBoxSlot* ListSlot = InventoryBody->AddChildToHorizontalBox(ListSize))
		{
			ListSlot->SetPadding(FMargin(0.0f, 0.0f, 14.0f, 0.0f));
		}
		if (UHorizontalBoxSlot* DetailSlot = InventoryBody->AddChildToHorizontalBox(DetailSize))
		{
			DetailSlot->SetPadding(FMargin(0.0f, 0.0f, 14.0f, 0.0f));
		}
		InventoryBody->AddChildToHorizontalBox(EquipmentSize);
	}
	AddStackChild(InventoryStack, InventoryBody, 8.0f);
	AddStackChild(InventoryStack, InventoryFooterText, 0.0f);
	if (InventoryPanel)
	{
		InventoryPanel->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}
	UpdateInventoryPanelVisibility();
	RefreshInventoryWindow();

	ChatPanel = MakePanel(WidgetTree, TEXT("ChatPanel"), FLinearColor(0.015f, 0.018f, 0.022f, 0.76f));
	ChatMessageStack = MakePanelStack(WidgetTree, ChatPanel, TEXT("ChatMessageStack"));
	if (ChatPanel)
	{
		ChatPanel->SetPadding(FMargin(10.0f, 8.0f));
		ChatPanel->SetClipping(EWidgetClipping::ClipToBoundsAlways);
		ChatPanel->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (ChatMessageStack)
	{
		ChatMessageStack->SetClipping(EWidgetClipping::ClipToBoundsAlways);
	}
	ChatMessageTexts.Reset();
	for (int32 MessageIndex = 0; MessageIndex < ChatMessageLimit; ++MessageIndex)
	{
		UTextBlock* MessageText = MakeHudText(
			WidgetTree,
			*FString::Printf(TEXT("ChatMessageText_%d"), MessageIndex),
			FLinearColor(0.88f, 0.9f, 0.86f, 1.0f),
			13.0f);
		if (!MessageText)
		{
			continue;
		}

		MessageText->SetAutoWrapText(false);
		MessageText->SetClipping(EWidgetClipping::ClipToBounds);
		MessageText->SetVisibility(ESlateVisibility::Collapsed);
		ChatMessageTexts.Add(MessageText);
		AddStackChild(ChatMessageStack, MessageText, 2.0f);
	}
	RefreshChatMessages();

	AddStackChild(LeftStack, StatusPanel, 10.0f);
	AddStackChild(LeftStack, TargetPanel, 10.0f);
	AddStackChild(LeftStack, QuestPanel, 0.0f);

	HotbarRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("HotbarRow"));
	HotbarSlotTexts.Reset();
	HotbarSlotPanels.Reset();
	HotbarSlotIcons.Reset();
	static const TCHAR* HotbarKeys[] = { TEXT("1"), TEXT("2"), TEXT("3"), TEXT("4"), TEXT("Alt+R"), TEXT("Alt+E"), TEXT("R"), TEXT("X"), TEXT("E"), TEXT("F") };
	for (int32 SlotIndex = 0; SlotIndex < UE_ARRAY_COUNT(HotbarKeys); ++SlotIndex)
	{
		USizeBox* SlotSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), *FString::Printf(TEXT("HotbarSlotSize_%d"), SlotIndex));
		UBorder* SlotPanel = MakePanel(WidgetTree, *FString::Printf(TEXT("HotbarSlotPanel_%d"), SlotIndex), FLinearColor(0.025f, 0.03f, 0.035f, 0.86f));
		UVerticalBox* SlotStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), *FString::Printf(TEXT("HotbarSlotStack_%d"), SlotIndex));
		UImage* SlotIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), *FString::Printf(TEXT("HotbarSlotIcon_%d"), SlotIndex));
		USizeBox* SlotIconSize = MakeSizedWidget(
			WidgetTree,
			SlotIcon,
			*FString::Printf(TEXT("HotbarSlotIconSize_%d"), SlotIndex),
			HotbarSlotIconSize,
			HotbarSlotIconSize);
		UTextBlock* SlotText = MakeHudText(WidgetTree, *FString::Printf(TEXT("HotbarSlotText_%d"), SlotIndex), FLinearColor(0.92f, 0.96f, 1.0f, 1.0f), 10.0f);
		if (!SlotSize || !SlotPanel || !SlotStack || !SlotIcon || !SlotIconSize || !SlotText)
		{
			continue;
		}

		SlotSize->SetWidthOverride(92.0f);
		SlotSize->SetHeightOverride(64.0f);
		SlotPanel->SetPadding(FMargin(4.0f, 2.0f));
		SlotIcon->SetVisibility(ESlateVisibility::Collapsed);
		SlotText->SetJustification(ETextJustify::Center);
		SlotText->SetAutoWrapText(false);
		SlotText->SetLineHeightPercentage(0.8f);
		SlotText->SetText(FText::FromString(FString::Printf(TEXT("%s\n-"), HotbarKeys[SlotIndex])));
		if (UVerticalBoxSlot* IconSlot = SlotStack->AddChildToVerticalBox(SlotIconSize))
		{
			IconSlot->SetHorizontalAlignment(HAlign_Center);
		}
		if (UVerticalBoxSlot* TextSlot = SlotStack->AddChildToVerticalBox(SlotText))
		{
			TextSlot->SetHorizontalAlignment(HAlign_Center);
		}
		SlotPanel->SetContent(SlotStack);
		SlotSize->AddChild(SlotPanel);
		HotbarSlotTexts.Add(SlotText);
		HotbarSlotPanels.Add(SlotPanel);
		HotbarSlotIcons.Add(SlotIcon);

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
	LootIcon = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("LootIcon"));
	LootTextBlock = MakeHudText(WidgetTree, TEXT("LootText"), FLinearColor(0.58f, 1.0f, 0.62f, 1.0f), 16.0f);
	UHorizontalBox* LootRow = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("LootRow"));
	if (LootPanel && LootIcon && LootTextBlock && LootRow)
	{
		LootIcon->SetVisibility(ESlateVisibility::Collapsed);
		USizeBox* LootIconSizeBox = MakeSizedWidget(
			WidgetTree,
			LootIcon,
			TEXT("LootIconSize"),
			LootPopupIconSize,
			LootPopupIconSize);
		if (UHorizontalBoxSlot* IconSlot = LootRow->AddChildToHorizontalBox(LootIconSizeBox))
		{
			IconSlot->SetPadding(FMargin(0.0f, 0.0f, 10.0f, 0.0f));
			IconSlot->SetVerticalAlignment(VAlign_Center);
		}
		LootTextBlock->SetAutoWrapText(true);
		if (UHorizontalBoxSlot* TextSlot = LootRow->AddChildToHorizontalBox(LootTextBlock))
		{
			TextSlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			TextSlot->SetVerticalAlignment(VAlign_Center);
		}
		LootPanel->SetContent(LootRow);
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
			InventorySlot->SetSize(FVector2D(700.0f, 330.0f));
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

	if (ChatPanel)
	{
		if (UCanvasPanelSlot* ChatSlot = RootCanvas->AddChildToCanvas(ChatPanel))
		{
			ChatSlot->SetAnchors(FAnchors(0.0f, 1.0f, 0.0f, 1.0f));
			ChatSlot->SetAlignment(FVector2D(0.0f, 1.0f));
			ChatSlot->SetPosition(FVector2D(24.0f, -112.0f));
			ChatSlot->SetSize(FVector2D(520.0f, 156.0f));
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

	ClampSelectedInventoryStackIndex();
	RefreshInventoryWindow();

	for (int32 SlotIndex = 0; SlotIndex < HotbarSlotTexts.Num(); ++SlotIndex)
	{
		UTextBlock* SlotText = HotbarSlotTexts[SlotIndex];
		if (!SlotText)
		{
			continue;
		}

		const float CooldownRemaining = Hotbar ? Hotbar->GetSlotCooldownRemaining(SlotIndex) : 0.0f;
		SlotText->SetText(GetHotbarSlotDisplayText(SlotIndex, CooldownRemaining));
		SlotText->SetColorAndOpacity(FSlateColor(
			CooldownRemaining > 0.05f
				? FLinearColor(0.58f, 0.62f, 0.68f, 1.0f)
				: FLinearColor(0.92f, 0.96f, 1.0f, 1.0f)));
		if (HotbarSlotPanels.IsValidIndex(SlotIndex) && HotbarSlotPanels[SlotIndex])
		{
			HotbarSlotPanels[SlotIndex]->SetToolTipText(GetHotbarSlotTooltipText(SlotIndex));
		}

		if (HotbarSlotIcons.IsValidIndex(SlotIndex))
		{
			UImage* SlotIcon = HotbarSlotIcons[SlotIndex];
			const bool bHasAbility = Hotbar && Hotbar->Slots.IsValidIndex(SlotIndex) &&
				!Hotbar->Slots[SlotIndex].AbilityId.IsNone();
			SetIconImage(
				SlotIcon,
				bHasAbility ? ResolveAbilityIconForUi(Hotbar->Slots[SlotIndex]) : nullptr);
			if (SlotIcon)
			{
				SlotIcon->SetColorAndOpacity(
					CooldownRemaining > 0.05f
						? FLinearColor(0.35f, 0.38f, 0.42f, 0.9f)
						: FLinearColor::White);
			}
		}
	}
}

void UEmbermerePlayerHudWidget::UpdateInventoryPanelVisibility()
{
	if (InventoryPanel)
	{
		InventoryPanel->SetVisibility(bInventoryPanelVisible ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}
}

void UEmbermerePlayerHudWidget::RefreshInventoryWindow()
{
	const int32 StackCount = Inventory ? Inventory->Stacks.Num() : 0;
	const int32 MaxSlots = Inventory ? Inventory->MaxSlots : 24;
	if (InventoryCapacityText)
	{
		InventoryCapacityText->SetText(FText::FromString(FString::Printf(TEXT("Slots %d / %d"), StackCount, MaxSlots)));
	}
	if (InventorySortButton)
	{
		InventorySortButton->SetIsEnabled(
			StackCount > 1 && !PendingDragItem && !bInventoryDragInProgress);
	}
	if (InventoryEquipmentText)
	{
		const FEmbermereItemStatBonuses Bonuses = Equipment ? Equipment->GetTotalStatBonuses() : FEmbermereItemStatBonuses();
		InventoryEquipmentText->SetText(FText::FromString(FString::Printf(
			TEXT("Bonuses  HP %+.0f  Mana %+.0f\nArmor %+.0f  Power %+.0f\nClick occupied slot to unequip"),
			Bonuses.MaxHealth,
			Bonuses.MaxMana,
			Bonuses.Armor,
			Bonuses.Power)));
	}
	if (InventoryListDropPanel)
	{
		InventoryListDropPanel->SetBrushColor(
			bInventoryListDropHighlighted
				? (bInventoryListDropValid
					? FLinearColor(0.34f, 0.24f, 0.055f, 0.95f)
					: FLinearColor(0.34f, 0.055f, 0.04f, 0.92f))
				: FLinearColor(0.035f, 0.04f, 0.032f, 0.55f));
	}
	if (InventoryFooterText)
	{
		if (HighlightedDropSlot != EEmbermereEquipmentSlot::None)
		{
			InventoryFooterText->SetText(FText::FromString(
				bHighlightedDropSlotValid
					? FString::Printf(TEXT("Equip in %s"), GetEquipmentSlotLabel(HighlightedDropSlot))
					: FString::Printf(TEXT("Cannot equip in %s"), GetEquipmentSlotLabel(HighlightedDropSlot))));
		}
		else if (bInventoryListDropHighlighted)
		{
			InventoryFooterText->SetText(FText::FromString(
				bInventoryListDropValid ? TEXT("Return to inventory") : TEXT("Inventory is full")));
		}
		else
		{
			InventoryFooterText->SetText(FText::FromString(TEXT("Drag gear or click item   |   [ / ] Inspect   |   I Close")));
		}
	}
	for (int32 Index = 0; Index < InventoryEquipmentSlotButtons.Num(); ++Index)
	{
		UEmbermereEquipmentSlotButton* SlotButton = InventoryEquipmentSlotButtons[Index];
		UTextBlock* SlotText = InventoryEquipmentSlotTexts.IsValidIndex(Index) ? InventoryEquipmentSlotTexts[Index] : nullptr;
		UImage* SlotIcon = InventoryEquipmentSlotIcons.IsValidIndex(Index) ? InventoryEquipmentSlotIcons[Index] : nullptr;
		if (!SlotButton || !SlotText)
		{
			continue;
		}

		const UEmbermereItemData* Item = Equipment ? Equipment->GetEquippedItem(SlotButton->EquipmentSlot) : nullptr;
		SetIconImage(
			SlotIcon,
			Item ? ResolveItemIconForUi(Item) : ResolveEquipmentSlotIconForUi(SlotButton->EquipmentSlot));
		SlotButton->SetIsEnabled(Item != nullptr);
		SlotButton->SetToolTipText(Item
			? FText::FromString(FString::Printf(
				TEXT("%s\n\nClick to unequip"),
				*BuildItemTooltipText(Item, 1).ToString()))
			: FText::FromString(FString::Printf(TEXT("Empty %s slot"), GetEquipmentSlotLabel(SlotButton->EquipmentSlot))));
		const bool bIsDropTarget = SlotButton->EquipmentSlot == HighlightedDropSlot;
		SlotButton->SetBackgroundColor(
			bIsDropTarget
				? (bHighlightedDropSlotValid
					? FLinearColor(0.46f, 0.31f, 0.055f, 0.98f)
					: FLinearColor(0.42f, 0.055f, 0.035f, 0.96f))
				: (Item
					? FLinearColor(0.3f, 0.2f, 0.055f, 0.96f)
					: FLinearColor(0.07f, 0.075f, 0.065f, 0.9f)));
		SlotText->SetColorAndOpacity(FSlateColor(
			Item
				? FLinearColor(1.0f, 0.8f, 0.3f, 1.0f)
				: FLinearColor(0.56f, 0.58f, 0.53f, 1.0f)));
		SlotText->SetText(Item
			? Item->DisplayName
			: FText::FromString(GetEquipmentSlotLabel(SlotButton->EquipmentSlot)));
	}

	for (int32 RowIndex = 0; RowIndex < InventoryRowTexts.Num(); ++RowIndex)
	{
		UEmbermereInventoryRowButton* RowButton = InventoryRowButtons.IsValidIndex(RowIndex)
			? InventoryRowButtons[RowIndex]
			: nullptr;
		if (RowButton)
		{
			RowButton->SetIsEnabled(false);
			RowButton->SetVisibility(ESlateVisibility::Collapsed);
			RowButton->SetBackgroundColor(FLinearColor(0.08f, 0.085f, 0.075f, 0.72f));
			RowButton->SetToolTipText(FText::GetEmpty());
		}

		UTextBlock* RowText = InventoryRowTexts[RowIndex];
		UImage* RowIcon = InventoryRowIcons.IsValidIndex(RowIndex) ? InventoryRowIcons[RowIndex] : nullptr;
		SetIconImage(RowIcon, nullptr);
		if (RowText)
		{
			RowText->SetText(FText::GetEmpty());
			RowText->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	if (!Inventory || StackCount <= 0)
	{
		SetIconImage(InventoryDetailIcon, nullptr);
		if (InventoryActionButton)
		{
			InventoryActionButton->SetVisibility(ESlateVisibility::Collapsed);
		}
		if (InventoryRowButtons.IsValidIndex(0) && InventoryRowButtons[0])
		{
			InventoryRowButtons[0]->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		if (InventoryRowTexts.IsValidIndex(0) && InventoryRowTexts[0])
		{
			InventoryRowTexts[0]->SetText(FText::FromString(TEXT("Empty")));
			InventoryRowTexts[0]->SetColorAndOpacity(FSlateColor(FLinearColor(0.62f, 0.64f, 0.6f, 1.0f)));
		}
		if (InventoryDetailNameText)
		{
			InventoryDetailNameText->SetText(FText::FromString(TEXT("No item selected")));
		}
		if (InventoryDetailMetaText)
		{
			InventoryDetailMetaText->SetText(FText::FromString(TEXT("Complete quests and defeat enemies")));
		}
		if (InventoryDetailDescriptionText)
		{
			InventoryDetailDescriptionText->SetText(FText::FromString(TEXT("Quest rewards and recovered items will appear in this window.")));
		}
		return;
	}

	const int32 SelectedIndex = FMath::Clamp(SelectedInventoryStackIndex, 0, StackCount - 1);
	FirstDisplayedInventoryStackIndex = FMath::Clamp(
		SelectedIndex - InventoryVisibleRowCount + 1,
		0,
		FMath::Max(0, StackCount - InventoryVisibleRowCount));
	for (int32 RowIndex = 0; RowIndex < InventoryRowTexts.Num(); ++RowIndex)
	{
		const int32 StackIndex = FirstDisplayedInventoryStackIndex + RowIndex;
		UEmbermereInventoryRowButton* RowButton = InventoryRowButtons.IsValidIndex(RowIndex)
			? InventoryRowButtons[RowIndex]
			: nullptr;
		UTextBlock* RowText = InventoryRowTexts[RowIndex];
		if (!RowButton || !RowText || !Inventory->Stacks.IsValidIndex(StackIndex))
		{
			continue;
		}

		const FEmbermereInventoryStack& Stack = Inventory->Stacks[StackIndex];
		if (!Stack.Item || Stack.Quantity <= 0)
		{
			continue;
		}

		const bool bSelected = StackIndex == SelectedIndex;
		UImage* RowIcon = InventoryRowIcons.IsValidIndex(RowIndex) ? InventoryRowIcons[RowIndex] : nullptr;
		SetIconImage(RowIcon, ResolveItemIconForUi(Stack.Item));
		RowButton->SetIsEnabled(true);
		RowButton->SetVisibility(ESlateVisibility::Visible);
		RowButton->SetToolTipText(BuildItemTooltipText(Stack.Item, Stack.Quantity));
		RowButton->SetBackgroundColor(
			bSelected
				? FLinearColor(0.28f, 0.19f, 0.055f, 0.92f)
				: FLinearColor(0.08f, 0.085f, 0.075f, 0.72f));
		RowText->SetText(FText::FromString(FString::Printf(
			TEXT("%s  x%d"),
			*Stack.Item->DisplayName.ToString(),
			Stack.Quantity)));
		RowText->SetColorAndOpacity(FSlateColor(
			bSelected
				? FLinearColor(1.0f, 0.8f, 0.3f, 1.0f)
				: FLinearColor(0.82f, 0.84f, 0.78f, 1.0f)));
	}

	const FEmbermereInventoryStack& SelectedStack = Inventory->Stacks[SelectedIndex];
	if (SelectedStack.Item)
	{
		SetIconImage(InventoryDetailIcon, ResolveItemIconForUi(SelectedStack.Item));
		if (InventoryDetailNameText)
		{
			InventoryDetailNameText->SetText(SelectedStack.Item->DisplayName);
		}
		if (InventoryDetailMetaText)
		{
			FString DetailMeta = FString::Printf(
				TEXT("Quantity %d / %d\n%s | %s"),
				SelectedStack.Quantity,
				SelectedStack.Item->MaxStack,
				*SelectedStack.Item->GetCategoryDisplayName().ToString(),
				*GetSelectedInventoryActionLabel().ToString());
			if (SelectedStack.Item->IsEquippable())
			{
				DetailMeta += FString::Printf(
					TEXT("\n%s | Level %d"),
					*SelectedStack.Item->GetEquipmentSlotDisplayName().ToString(),
					FMath::Max(1, SelectedStack.Item->RequiredLevel));
			}
			if (SelectedStack.Item->IsEquippable() || SelectedStack.Item->IsConsumable())
			{
				DetailMeta += FString::Printf(TEXT("\n%s"), *SelectedStack.Item->GetEffectSummary().ToString());
			}
			const FText ComparisonText = BuildItemComparisonText(SelectedStack.Item);
			if (!ComparisonText.IsEmpty())
			{
				DetailMeta += FString::Printf(TEXT("\n%s"), *ComparisonText.ToString());
			}
			DetailMeta += FString::Printf(TEXT("\nItem %d of %d"), SelectedIndex + 1, StackCount);
			InventoryDetailMetaText->SetText(FText::FromString(DetailMeta));
		}
		if (InventoryDetailDescriptionText)
		{
			InventoryDetailDescriptionText->SetText(
				SelectedStack.Item->Description.IsEmpty()
					? FText::FromString(TEXT("No description available."))
					: SelectedStack.Item->Description);
		}
		if (InventoryActionButton && InventoryActionText)
		{
			const bool bCanEquip = SelectedStack.Item->IsEquippable() && Equipment &&
				Equipment->CanEquip(SelectedStack.Item, Stats ? Stats->Level : 1);
			const bool bCanConsume = SelectedStack.Item->IsConsumable() && Stats &&
				((SelectedStack.Item->ConsumableEffects.HealHealth > 0.0f && Stats->CurrentHealth < Stats->MaxHealth) ||
				 (SelectedStack.Item->ConsumableEffects.RestoreMana > 0.0f && Stats->CurrentMana < Stats->MaxMana));
			const bool bSupportsAction = SelectedStack.Item->IsEquippable() || SelectedStack.Item->IsConsumable();
			InventoryActionButton->SetVisibility(bSupportsAction ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
			InventoryActionButton->SetIsEnabled(bCanEquip || bCanConsume);
			InventoryActionText->SetText(GetSelectedInventoryActionLabel());
		}
	}
}

void UEmbermerePlayerHudWidget::HandleInventoryRowClicked(int32 VisibleRowIndex)
{
	SelectInventoryItem(FirstDisplayedInventoryStackIndex + VisibleRowIndex);
}

void UEmbermerePlayerHudWidget::HandleInventoryActionClicked()
{
	ActivateSelectedInventoryItem();
}

void UEmbermerePlayerHudWidget::HandleInventorySortClicked()
{
	SortInventory();
}

void UEmbermerePlayerHudWidget::HandleEquipmentSlotClicked(EEmbermereEquipmentSlot EquipmentSlot)
{
	ActivateEquipmentSlot(EquipmentSlot);
}

int32 UEmbermerePlayerHudWidget::FindInventoryStackAtScreenPosition(const FVector2D& ScreenPosition) const
{
	for (int32 VisibleRowIndex = 0; VisibleRowIndex < InventoryRowButtons.Num(); ++VisibleRowIndex)
	{
		const UEmbermereInventoryRowButton* RowButton = InventoryRowButtons[VisibleRowIndex];
		if (!RowButton || !RowButton->IsVisible() ||
			!RowButton->GetCachedGeometry().IsUnderLocation(ScreenPosition))
		{
			continue;
		}

		const int32 StackIndex = FirstDisplayedInventoryStackIndex + VisibleRowIndex;
		return Inventory && Inventory->Stacks.IsValidIndex(StackIndex) ? StackIndex : INDEX_NONE;
	}

	return INDEX_NONE;
}

EEmbermereEquipmentSlot UEmbermerePlayerHudWidget::FindEquipmentSlotAtScreenPosition(
	const FVector2D& ScreenPosition) const
{
	for (const UEmbermereEquipmentSlotButton* SlotButton : InventoryEquipmentSlotButtons)
	{
		if (SlotButton && SlotButton->IsVisible() &&
			SlotButton->GetCachedGeometry().IsUnderLocation(ScreenPosition))
		{
			return SlotButton->EquipmentSlot;
		}
	}

	return EEmbermereEquipmentSlot::None;
}

bool UEmbermerePlayerHudWidget::IsInventoryListAtScreenPosition(const FVector2D& ScreenPosition) const
{
	return InventoryListDropPanel && InventoryListDropPanel->IsVisible() &&
		InventoryListDropPanel->GetCachedGeometry().IsUnderLocation(ScreenPosition);
}

void UEmbermerePlayerHudWidget::ClearPendingDrag()
{
	PendingDragItem = nullptr;
	PendingDragSource = EEmbermereItemDragSource::None;
	PendingDragEquipmentSlot = EEmbermereEquipmentSlot::None;
	PendingDragStackIndex = INDEX_NONE;
}

void UEmbermerePlayerHudWidget::ClearDropFeedback()
{
	HighlightedDropSlot = EEmbermereEquipmentSlot::None;
	bHighlightedDropSlotValid = false;
	bInventoryListDropHighlighted = false;
	bInventoryListDropValid = false;
}

void UEmbermerePlayerHudWidget::ClampSelectedInventoryStackIndex()
{
	if (!Inventory || Inventory->Stacks.Num() <= 0)
	{
		SelectedInventoryStackIndex = 0;
		return;
	}

	SelectedInventoryStackIndex = FMath::Clamp(SelectedInventoryStackIndex, 0, Inventory->Stacks.Num() - 1);
}

void UEmbermerePlayerHudWidget::RefreshChatMessages()
{
	if (ChatPanel)
	{
		ChatPanel->SetVisibility(ChatMessages.Num() > 0 ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	for (int32 MessageIndex = 0; MessageIndex < ChatMessageTexts.Num(); ++MessageIndex)
	{
		UTextBlock* MessageText = ChatMessageTexts[MessageIndex];
		if (!MessageText)
		{
			continue;
		}

		if (!ChatMessages.IsValidIndex(MessageIndex))
		{
			MessageText->SetText(FText::GetEmpty());
			MessageText->SetVisibility(ESlateVisibility::Collapsed);
			continue;
		}

		const TPair<FText, FLinearColor>& Message = ChatMessages[MessageIndex];
		MessageText->SetText(Message.Key);
		MessageText->SetColorAndOpacity(FSlateColor(Message.Value));
		MessageText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UEmbermerePlayerHudWidget::ShowLootPopup_Implementation(const FText& LootText)
{
	ShowLootPopupWithIcon(LootText, ResolveItemIconForUi(nullptr));
}

void UEmbermerePlayerHudWidget::ShowLootPopupWithIcon(const FText& LootText, UTexture2D* Icon)
{
	if (!LootPanel || !LootTextBlock)
	{
		return;
	}

	SetIconImage(LootIcon, Icon);
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

	ShowLootPopupWithIcon(
		FText::FromString(FString::Printf(TEXT("Received: %s x%d"), *Item->DisplayName.ToString(), Quantity)),
		ResolveItemIconForUi(Item));
	AddChatMessage(
		FText::FromString(FString::Printf(TEXT("Received: %s x%d"), *Item->DisplayName.ToString(), Quantity)),
		FLinearColor(0.58f, 1.0f, 0.62f, 1.0f));
}
