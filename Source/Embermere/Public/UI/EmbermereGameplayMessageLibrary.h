#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmbermereGameplayMessageLibrary.generated.h"

UCLASS()
class EMBERMERE_API UEmbermereGameplayMessageLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Embermere|HUD", meta = (WorldContext = "WorldContextObject"))
	static void PostGameplayMessage(const UObject* WorldContextObject, const FText& Message, FLinearColor MessageColor);
};
