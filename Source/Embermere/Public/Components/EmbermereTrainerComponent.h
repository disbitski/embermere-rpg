#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereTrainerOfferingsData.h"
#include "EmbermereTrainerComponent.generated.h"

class UEmbermereStatsComponent;
class UEmbermereWalletComponent;

UENUM(BlueprintType)
enum class EEmbermereTrainingResult : uint8
{
	Success,
	InvalidRequest,
	LevelTooLow,
	InsufficientFunds,
	ProgressionCap,
	AlreadyCompleted
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereTrainingCompletedSignature);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereTrainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereTrainerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trainer")
	TObjectPtr<UEmbermereTrainerOfferingsData> OfferingsData;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereTrainingCompletedSignature OnTrainingCompleted;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Trainer")
	void SetOfferingsData(UEmbermereTrainerOfferingsData* NewOfferingsData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Trainer")
	int32 GetOfferingCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Trainer")
	bool GetOffering(int32 OfferingIndex, FEmbermereTrainerOffering& OutOffering) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Trainer")
	EEmbermereTrainingResult CanTrain(
		int32 OfferingIndex,
		const UEmbermereStatsComponent* Stats,
		const UEmbermereWalletComponent* Wallet) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Trainer")
	EEmbermereTrainingResult TryTrain(
		int32 OfferingIndex,
		UEmbermereStatsComponent* Stats,
		UEmbermereWalletComponent* Wallet);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Trainer")
	FText GetTrainingResultText(EEmbermereTrainingResult Result, int32 OfferingIndex) const;

private:
	TSet<FName> CompletedNonRepeatableOfferingIds;
};
