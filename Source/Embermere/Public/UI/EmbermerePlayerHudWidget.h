#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EmbermereStatsComponent.h"
#include "Types/EmbermereItemTypes.h"
#include "Types/EmbermereTypes.h"
#include "UI/EmbermereItemDragDropOperation.h"
#include "EmbermerePlayerHudWidget.generated.h"

class AActor;
class AEmbermereCharacter;
class UEmbermereCombatComponent;
class UEmbermereCombatFeedbackWidget;
class UEmbermereEquipmentComponent;
class UEmbermereEquipmentSlotButton;
class UEmbermereHotbarComponent;
class UEmbermereItemData;
class UEmbermereUiIconSet;
class UEmbermereInventoryRowButton;
class UEmbermereInventoryComponent;
class UEmbermereLevelUpWidget;
class UEmbermereQuestLogComponent;
class UEmbermereQuestLedgerRowButton;
class UEmbermereTrainerComponent;
class UEmbermereTrainerOfferingButton;
class UEmbermereVendorComponent;
class UEmbermereVendorStockButton;
class UEmbermereWalletComponent;
class UBorder;
class UButton;
class UHorizontalBox;
class UImage;
class UProgressBar;
class UTextBlock;
class UTexture2D;
class UVerticalBox;
struct FEmbermereCombatResult;
struct FEmbermereQuestState;

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermerePlayerHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UEmbermerePlayerHudWidget(const FObjectInitializer& ObjectInitializer);

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

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereWalletComponent> Wallet;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereVendorComponent> ActiveVendor;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|HUD")
	TObjectPtr<UEmbermereTrainerComponent> ActiveTrainer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Embermere|HUD|Icons")
	TSoftObjectPtr<UEmbermereUiIconSet> UiIconSet;

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

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	bool ShowVendor(UEmbermereVendorComponent* Vendor);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	void CloseVendor();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Vendor")
	bool IsVendorPanelVisible() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Persistence")
	bool ToggleSaveLoadPanel();

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Persistence")
	void CloseSaveLoadPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Persistence")
	bool IsSaveLoadPanelVisible() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Persistence")
	FVector2D GetSaveLoadPanelDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Persistence")
	FVector2D GetChronicleButtonDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Persistence")
	FVector2D GetChronicleButtonViewportOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Persistence")
	bool IsChronicleButtonBottomRightAnchored() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Quest Ledger")
	bool ToggleQuestLedgerPanel();

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Quest Ledger")
	void CloseQuestLedgerPanel();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	bool IsQuestLedgerPanelVisible() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Quest Ledger")
	bool SelectQuestLedgerRecord(int32 QuestIndex);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Quest Ledger")
	bool SelectNextQuestLedgerRecord(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Quest Ledger")
	bool FocusSelectedQuest();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	int32 GetSelectedQuestLedgerIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	int32 GetQuestLedgerVisibleRecordCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FText GetQuestLedgerDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FText GetQuestLedgerRowDisplayText(int32 QuestIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FText GetQuestLedgerSelectedDetailDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FVector2D GetQuestLedgerPanelDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FVector2D GetQuestLedgerRowDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FVector2D GetQuestLedgerDetailDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FVector2D GetQuestLedgerButtonDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	FVector2D GetQuestLedgerButtonViewportOffset() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Quest Ledger")
	bool IsQuestLedgerButtonBottomRightAnchored() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	bool SelectVendorStockItem(int32 StockIndex);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	bool PurchaseSelectedVendorItem();

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	bool SellSelectedInventoryItem();

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Vendor")
	bool BuyBackMostRecentVendorItem();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Vendor")
	int32 GetSelectedVendorStockIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Vendor")
	FText GetVendorDisplayText() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Trainer")
	bool ShowTrainer(UEmbermereTrainerComponent* Trainer);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Trainer")
	void CloseTrainer();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Trainer")
	bool IsTrainerPanelVisible() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Trainer")
	bool SelectTrainerOffering(int32 OfferingIndex);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Trainer")
	bool SelectNextTrainerOffering(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD|Trainer")
	bool TrainSelectedOffering();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Trainer")
	int32 GetSelectedTrainerOfferingIndex() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Trainer")
	FText GetTrainerDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Trainer")
	FVector2D GetTrainerPanelDimensions() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool SelectNextInventoryItem(int32 Direction);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool SelectInventoryItem(int32 StackIndex);

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool SortInventory();

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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	bool CanDropInventoryItemOnEquipmentSlot(
		const UEmbermereItemData* Item,
		EEmbermereEquipmentSlot TargetSlot) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ActivateEquipmentSlot(EEmbermereEquipmentSlot EquipmentSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	bool CanReturnEquipmentItemToInventory(
		const UEmbermereItemData* ExpectedItem,
		EEmbermereEquipmentSlot SourceSlot) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	bool ReturnEquipmentItemToInventory(
		UEmbermereItemData* ExpectedItem,
		EEmbermereEquipmentSlot SourceSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetHotbarSlotDisplayText(int32 SlotIndex, float CooldownRemainingSeconds = 0.0f) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD")
	FText GetHotbarSlotTooltipText(int32 SlotIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	UTexture2D* ResolveItemIconForUi(const UEmbermereItemData* Item) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	UTexture2D* ResolveEquipmentSlotIconForUi(EEmbermereEquipmentSlot EquipmentSlot) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	UTexture2D* ResolveAbilityIconForUi(const FEmbermereAbilityDefinition& Ability) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	UTexture2D* ResolvePaperDollBackdropForUi() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetInventoryRowIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetInventoryDetailIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetEquipmentSlotIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetLootPopupIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetHotbarSlotIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Icons")
	FVector2D GetPaperDollBackdropDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	int32 GetPlayerStatusEffectCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	int32 GetTargetStatusEffectCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	FText GetPlayerStatusEffectDisplayText(int32 EffectIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	FText GetTargetStatusEffectDisplayText(int32 EffectIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	FVector2D GetStatusEffectIconDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Status Effects")
	FVector2D GetStatusEffectSlotDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	FText GetProgressionDisplayText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	float GetProgressionPercent() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	FVector2D GetProgressionBarDimensions() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDragOver(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PlayerStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ManaBar;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> ExperienceBar;

	UPROPERTY(Transient)
	TObjectPtr<UEmbermereLevelUpWidget> LevelUpOverlay;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> PlayerStatusEffectPanels;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> PlayerStatusEffectIcons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> PlayerStatusEffectTexts;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> TargetPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TargetText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TargetRangeText;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> TargetHealthBar;

	UPROPERTY(Transient)
	TObjectPtr<UEmbermereCombatFeedbackWidget> CombatFeedbackOverlay;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> TargetStatusEffectPanels;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> TargetStatusEffectIcons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> TargetStatusEffectTexts;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> QuestPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> QuestLedgerMenuButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerMenuText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> QuestLedgerPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerCountText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> QuestLedgerCloseButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerCloseText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereQuestLedgerRowButton>> QuestLedgerRowButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> QuestLedgerRowTexts;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerEmptyText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerDetailTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerDetailDescriptionText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerDetailStateText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerDetailObjectiveText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerDetailRewardText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> QuestLedgerFocusButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestLedgerFocusText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> InventoryPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryCapacityText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> InventorySortButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventorySortText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> InventoryRowTexts;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> InventoryRowIcons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereInventoryRowButton>> InventoryRowButtons;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> InventoryListDropPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryDetailNameText;

	UPROPERTY(Transient)
	TObjectPtr<UImage> InventoryDetailIcon;

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
	TObjectPtr<UImage> InventoryPaperDollBackdrop;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereEquipmentSlotButton>> InventoryEquipmentSlotButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> InventoryEquipmentSlotTexts;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> InventoryEquipmentSlotIcons;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> InventoryFooterText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> VendorPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorWalletText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> VendorCloseButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorCloseText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereVendorStockButton>> VendorRowButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> VendorRowIcons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> VendorRowTexts;

	UPROPERTY(Transient)
	TObjectPtr<UImage> VendorDetailIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorDetailNameText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorDetailMetaText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorDetailDescriptionText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> VendorBuyButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorBuyText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> VendorSellButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorSellText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> VendorBuybackButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorBuybackText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> VendorStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> TrainerPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerWalletText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> TrainerCloseButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerCloseText;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereTrainerOfferingButton>> TrainerRowButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> TrainerRowTexts;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerDetailNameText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerDetailMetaText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerDetailDescriptionText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> TrainerActionButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerActionText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TrainerStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> MenuButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> MenuText;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> SaveLoadPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadSlotText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadCurrentText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadSummaryText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> SaveLoadSaveButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadSaveText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> SaveLoadLoadButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadLoadText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> SaveLoadCancelButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadCancelText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> SaveLoadCloseButton;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SaveLoadCloseText;

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
	TArray<TObjectPtr<UBorder>> HotbarSlotPanels;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UImage>> HotbarSlotIcons;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> DialoguePanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DialogueTextBlock;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> LootPanel;

	UPROPERTY(Transient)
	TObjectPtr<UImage> LootIcon;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LootTextBlock;

	float DialogueHideTimeSeconds = 0.0f;
	float LootHideTimeSeconds = 0.0f;
	bool bInventoryPanelVisible = true;
	bool bVendorPanelVisible = false;
	bool bTrainerPanelVisible = false;
	bool bSaveLoadPanelVisible = false;
	bool bQuestLedgerPanelVisible = false;
	enum class ESaveLoadConfirmation : uint8
	{
		None,
		Save,
		Load
	};
	ESaveLoadConfirmation PendingSaveLoadConfirmation = ESaveLoadConfirmation::None;
	FText SaveLoadResultMessage;
	int32 SelectedInventoryStackIndex = 0;
	int32 SelectedVendorStockIndex = 0;
	int32 SelectedTrainerOfferingIndex = 0;
	int32 SelectedQuestLedgerIndex = 0;
	int32 FirstDisplayedInventoryStackIndex = 0;
	TArray<TPair<FText, FLinearColor>> ChatMessages;
	TObjectPtr<UEmbermereItemData> PendingDragItem;
	EEmbermereItemDragSource PendingDragSource = EEmbermereItemDragSource::None;
	EEmbermereEquipmentSlot PendingDragEquipmentSlot = EEmbermereEquipmentSlot::None;
	int32 PendingDragStackIndex = INDEX_NONE;
	EEmbermereEquipmentSlot HighlightedDropSlot = EEmbermereEquipmentSlot::None;
	bool bHighlightedDropSlotValid = false;
	bool bInventoryListDropHighlighted = false;
	bool bInventoryListDropValid = false;
	bool bInventoryDragInProgress = false;

	void BuildDefaultLayout();
	void RefreshHudText();
	void RefreshStatusEffectRow(
		const TArray<FEmbermereActiveStatusEffect>& Effects,
		const TArray<TObjectPtr<UBorder>>& Panels,
		const TArray<TObjectPtr<UImage>>& Icons,
		const TArray<TObjectPtr<UTextBlock>>& Texts);
	void BindComponentEvents();
	void UnbindComponentEvents();
	void UpdateInventoryPanelVisibility();
	void RefreshInventoryWindow();
	void UpdateVendorPanelVisibility();
	void RefreshVendorWindow();
	void UpdateTrainerPanelVisibility();
	void RefreshTrainerWindow();
	void UpdateSaveLoadPanelVisibility();
	void RefreshSaveLoadWindow();
	void UpdateQuestLedgerPanelVisibility();
	void RefreshQuestLedgerWindow();
	FString GetQuestLedgerStateLabel(const FEmbermereQuestState& QuestState) const;
	const FEmbermereQuestState* GetSelectedQuestLedgerState() const;
	FText GetQuestLedgerSelectedStateCopy() const;
	FText GetQuestLedgerSelectedObjectiveText() const;
	FText GetQuestLedgerSelectedRewardText() const;
	void RefreshChatMessages();
	void ShowLootPopupWithIcon(const FText& LootText, UTexture2D* Icon);
	void ClampSelectedInventoryStackIndex();
	FText BuildItemComparisonText(const UEmbermereItemData* Item) const;
	FText BuildItemTooltipText(const UEmbermereItemData* Item, int32 Quantity) const;
	int32 FindInventoryStackAtScreenPosition(const FVector2D& ScreenPosition) const;
	EEmbermereEquipmentSlot FindEquipmentSlotAtScreenPosition(const FVector2D& ScreenPosition) const;
	bool IsInventoryListAtScreenPosition(const FVector2D& ScreenPosition) const;
	void ClearPendingDrag();
	void ClearDropFeedback();
	UEmbermereItemData* GetSelectedInventoryItem() const;
	TArray<FEmbermereActiveStatusEffect> GetTargetStatusEffects() const;
	FText BuildStatusEffectDisplayText(const FEmbermereActiveStatusEffect& Effect) const;

	UFUNCTION()
	void HandleItemAdded(UEmbermereItemData* Item, int32 Quantity);

	void HandleCombatResult(const FEmbermereCombatResult& Result);

	UFUNCTION()
	void HandleCombatTargetChanged(AActor* NewTarget, AActor* OldTarget);

	UFUNCTION()
	void HandleInventoryRowClicked(int32 VisibleRowIndex);

	UFUNCTION()
	void HandleInventoryActionClicked();

	UFUNCTION()
	void HandleInventorySortClicked();

	UFUNCTION()
	void HandleVendorStockClicked(int32 StockIndex);

	UFUNCTION()
	void HandleVendorBuyClicked();

	UFUNCTION()
	void HandleVendorSellClicked();

	UFUNCTION()
	void HandleVendorBuybackClicked();

	UFUNCTION()
	void HandleVendorCloseClicked();

	UFUNCTION()
	void HandleTrainerOfferingClicked(int32 OfferingIndex);

	UFUNCTION()
	void HandleTrainerActionClicked();

	UFUNCTION()
	void HandleTrainerCloseClicked();

	UFUNCTION()
	void HandleMenuClicked();

	UFUNCTION()
	void HandleQuestLedgerMenuClicked();

	UFUNCTION()
	void HandleQuestLedgerRowClicked(int32 QuestIndex);

	UFUNCTION()
	void HandleQuestLedgerFocusClicked();

	UFUNCTION()
	void HandleQuestLedgerCloseClicked();

	UFUNCTION()
	void HandleSaveLoadSaveClicked();

	UFUNCTION()
	void HandleSaveLoadLoadClicked();

	UFUNCTION()
	void HandleSaveLoadCancelClicked();

	UFUNCTION()
	void HandleSaveLoadCloseClicked();

	UFUNCTION()
	void HandleEquipmentSlotClicked(EEmbermereEquipmentSlot EquipmentSlot);
};
