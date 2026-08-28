#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EmbermereRestServiceData.generated.h"

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereRestServiceData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	FName ServiceId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	FText PromptText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	FText RestingText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest", meta = (ClampMin = "1.0"))
	float InteractionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest", meta = (ClampMin = "0.1"))
	float ChannelDurationSeconds = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest", meta = (ClampMin = "0.0"))
	float CooldownSeconds = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest", meta = (ClampMin = "0.0"))
	float MovementInterruptDistance = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	bool bRestoreHealth = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	bool bRestoreMana = true;

	bool HasValidDefinition() const
	{
		return !ServiceId.IsNone() &&
			!DisplayName.IsEmpty() &&
			!PromptText.IsEmpty() &&
			!RestingText.IsEmpty() &&
			FMath::IsFinite(InteractionRadius) && InteractionRadius > 0.0f &&
			FMath::IsFinite(ChannelDurationSeconds) && ChannelDurationSeconds > 0.0f &&
			FMath::IsFinite(CooldownSeconds) && CooldownSeconds >= 0.0f &&
			FMath::IsFinite(MovementInterruptDistance) && MovementInterruptDistance >= 0.0f &&
			(bRestoreHealth || bRestoreMana);
	}
};
