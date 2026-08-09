#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmbermereTrainerOfferingsData.generated.h"

UENUM(BlueprintType)
enum class EEmbermereTrainingEffectType : uint8
{
	Experience
};

USTRUCT(BlueprintType)
struct FEmbermereTrainerOffering
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	FName OfferingId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer", meta = (ClampMin = "1"))
	int32 CopperCost = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	EEmbermereTrainingEffectType EffectType = EEmbermereTrainingEffectType::Experience;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer", meta = (ClampMin = "1"))
	int32 ExperienceReward = 1;

	bool IsValid() const
	{
		return !OfferingId.IsNone() && !DisplayName.IsEmpty() && CopperCost > 0 && RequiredLevel > 0 &&
			EffectType == EEmbermereTrainingEffectType::Experience && ExperienceReward > 0;
	}
};

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereTrainerOfferingsData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	FText TrainerName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	TArray<FEmbermereTrainerOffering> Offerings;
};
