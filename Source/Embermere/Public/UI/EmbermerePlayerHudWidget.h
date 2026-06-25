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

UCLASS(Abstract, Blueprintable)
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
};
