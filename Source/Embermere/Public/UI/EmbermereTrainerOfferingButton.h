#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EmbermereTrainerOfferingButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereTrainerOfferingClickedSignature, int32, OfferingIndex);

UCLASS()
class EMBERMERE_API UEmbermereTrainerOfferingButton : public UButton
{
	GENERATED_BODY()

public:
	UEmbermereTrainerOfferingButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Trainer")
	FEmbermereTrainerOfferingClickedSignature OnTrainerOfferingClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Trainer")
	int32 OfferingIndex = INDEX_NONE;

	void SetOfferingIndex(int32 InOfferingIndex);

private:
	UFUNCTION()
	void HandleClicked();
};
