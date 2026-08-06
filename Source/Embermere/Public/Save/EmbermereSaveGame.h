#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Types/EmbermereItemTypes.h"
#include "EmbermereSaveGame.generated.h"

namespace EmbermereSaveGameVersion
{
	inline constexpr int32 Current = 1;
}

USTRUCT(BlueprintType)
struct FEmbermereSavedInventoryStack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FName ItemId = NAME_None;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FSoftObjectPath ItemAsset;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	int32 Quantity = 0;
};

USTRUCT(BlueprintType)
struct FEmbermereSavedEquipmentItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	EEmbermereEquipmentSlot Slot = EEmbermereEquipmentSlot::None;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FName ItemId = NAME_None;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FSoftObjectPath ItemAsset;
};

USTRUCT(BlueprintType)
struct FEmbermereSavedQuestState
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	bool bHasActiveQuest = false;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FName QuestId = NAME_None;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FSoftObjectPath QuestAsset;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	int32 CurrentObjectiveCount = 0;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	bool bCompleted = false;
};

USTRUCT(BlueprintType)
struct FEmbermereSavedVendorStock
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FName VendorId = NAME_None;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FSoftObjectPath StockAsset;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	TArray<int32> RemainingQuantities;
};

UCLASS()
class EMBERMERE_API UEmbermereSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	int32 FormatVersion = EmbermereSaveGameVersion::Current;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	int32 Copper = 0;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	int32 CurrentExperience = 0;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	TArray<FEmbermereSavedInventoryStack> InventoryStacks;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	TArray<FEmbermereSavedEquipmentItem> EquippedItems;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	FEmbermereSavedQuestState QuestState;

	UPROPERTY(BlueprintReadOnly, SaveGame, Category = "Save Game")
	TArray<FEmbermereSavedVendorStock> VendorStocks;
};
