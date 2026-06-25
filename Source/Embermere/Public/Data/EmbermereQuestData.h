#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Data/EmbermereItemData.h"
#include "EmbermereQuestData.generated.h"

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereQuestData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName QuestId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	FName ObjectiveId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 RequiredObjectiveCount = 3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rewards")
	int32 RewardExperience = 100;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rewards")
	TSoftObjectPtr<UEmbermereItemData> RewardItem;
};
