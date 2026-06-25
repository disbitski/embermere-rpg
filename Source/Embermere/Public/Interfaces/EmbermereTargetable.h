#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EmbermereTargetable.generated.h"

UINTERFACE(BlueprintType)
class EMBERMERE_API UEmbermereTargetable : public UInterface
{
	GENERATED_BODY()
};

class EMBERMERE_API IEmbermereTargetable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Embermere|Targeting")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Embermere|Targeting")
	bool IsHostileTo(const AActor* Viewer) const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Embermere|Targeting")
	FText GetTargetDisplayName() const;
};
