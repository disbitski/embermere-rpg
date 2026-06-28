#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmbermerePlayerHudWidget.generated.h"

class AActor;
class AEmbermereCharacter;
class UEmbermereCombatComponent;
class UEmbermereHotbarComponent;
class UEmbermereInventoryComponent;
class UEmbermereQuestLogComponent;
class UEmbermereStatsComponent;
class UTextBlock;

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

	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD")
	void BindToCharacter(AEmbermereCharacter* Character);

	UFUNCTION(BlueprintImplementableEvent, Category = "Embermere|HUD")
	void OnTargetChanged(AActor* NewTarget);

	UFUNCTION(BlueprintImplementableEvent, Category = "Embermere|HUD")
	void ShowLootPopup(const FText& LootText);

	UFUNCTION(BlueprintImplementableEvent, Category = "Embermere|HUD")
	void ShowDialogue(const FText& SpeakerName, const FText& DialogueText);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> PlayerStatusText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> TargetText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> QuestText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> HotbarText;

	void BuildDefaultLayout();
	void RefreshHudText();
};
