#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/EmbermereItemTypes.h"
#include "EmbermerePlayerHudWidget.generated.h"

class AActor;
class AEmbermereCharacter;
class UEmbermereCombatComponent;
class UEmbermereEquipmentComponent;
class UEmbermereEquipmentSlotButton;
class UEmbermereHotbarComponent;
class UEmbermereItemData;
class UEmbermereInventoryRowButton;
class UEmbermereInventoryComponent;
class UEmbermereQuestLogComponent;
class UEmbermereStatsComponent;
class UBorder;
class UButton;
class UHorizontalBox;
class UProgressBar;
class UTextBlock;
class UVerticalBox;

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermerePlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<AEmbermereCharacter> OwningEmbermereCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereStatsComponent> Stats;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereCombatComponent> Combat;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereHotbarComponent> Hotbar;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereQuestLogComponent> QuestLog;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereInventoryComponent> Inventory;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereEquipmentComponent> Equipment;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	void BindToCharacter(AEmbermereCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "Embermere|HUD")
	void OnTargetChanged(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Embermere|HUD")
	void ShowLootPopup(const FText& LootText);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Embermere|HUD")
	void ShowDialogue(const FText& SpeakerName, const FText& DialogueText);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	void AddChatMessage(const FText& Message, FLinearColor MessageColor);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	int32 GetChatMessageCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	int32 GetChatMessageLimit() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ToggleInventoryPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	bool IsInventoryPanelVisible() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool SelectNextInventoryItem(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool SelectInventoryItem(int32 StackIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	int32 GetSelectedInventoryStackIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetInventoryDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetSelectedInventoryActionLabel() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetSelectedInventoryComparisonText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetSelectedInventoryTooltipText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetEquipmentDisplayText() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ActivateSelectedInventoryItem();

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ActivateInventoryItem(UEmbermereItemData* Item);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool EquipInventoryItemToSlot(UEmbermereItemData* Item, EEmbermereEquipmentSlot TargetSlot);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ActivateEquipmentSlot(EEmbermereEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetHotbarSlotDisplayText(int32 SlotIndex, float CooldownRemainingSeconds = 0.0f) const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PlayerStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ManaBar;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> TargetPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TargetText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TargetRangeText;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> TargetHealthBar;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> QuestPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> InventoryPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryCapacityText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> InventoryRowTexts;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereInventoryRowButton>> InventoryRowButtons;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryDetailNameText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryDetailMetaText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryDetailDescriptionText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> InventoryActionButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryActionText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryEquipmentText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereEquipmentSlotButton>> InventoryEquipmentSlotButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> InventoryEquipmentSlotTexts;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryFooterText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> ChatPanel;

	UPROPERTY(Transient)
	TObjectPtr<UVerticalBox> ChatMessageStack;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> ChatMessageTexts;

	UPROPERTY(Transient)
	TObjectPtr<UHorizontalBox> HotbarRow;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> HotbarSlotTexts;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> DialoguePanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DialogueTextBlock;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> LootPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LootTextBlock;

	float DialogueHideTimeSeconds = 0.0f;
	float LootHideTimeSeconds = 0.0f;
	bool bInventoryPanelVisible = true;
	int32 SelectedInventoryStackIndex = 0;
	int32 FirstDisplayedInventoryStackIndex = 0;
	TArray<TPair<FText, FLinearColor>> ChatMessages;

	void BuildDefaultLayout();
	void RefreshHudText();
	void BindComponentEvents();
	void UpdateInventoryPanelVisibility();
	void RefreshInventoryWindow();
	void RefreshChatMessages();
	void ClampSelectedInventoryStackIndex();
	FText BuildItemComparisonText(const UEmbermereItemData* Item) const;
	FText BuildItemTooltipText(const UEmbermereItemData* Item, int32 Quantity) const;

	UFUNCTION()
	void HandleItemAdded(UEmbermereItemData* Item, int32 Quantity);

	UFUNCTION()
	void HandleInventoryRowClicked(int32 VisibleRowIndex);

	UFUNCTION()
	void HandleInventoryActionClicked();

	UFUNCTION()
	void HandleEquipmentSlotClicked(EEmbermereEquipmentSlot EquipmentSlot);
};
