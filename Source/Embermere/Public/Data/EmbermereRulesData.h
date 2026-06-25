#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereRulesData.generated.h"

UCLASS(BlueprintType)
class EMBERMERE_API UEmbermereRulesData : public UDataAsset
{
	GENERATED_BODY()

public:
	UEmbermereRulesData();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Races")
	TArray<FEmbermereRaceDefinition> Races;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Classes")
	TArray<FEmbermereClassDefinition> Classes;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities")
	TArray<FEmbermereAbilityDefinition> Abilities;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool IsClassAllowed(EEmbermereRace Race, EEmbermereClass Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetRaceDefinition(EEmbermereRace Race, FEmbermereRaceDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetClassDefinition(EEmbermereClass Class, FEmbermereClassDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetAbilityDefinition(FName AbilityId, FEmbermereAbilityDefinition& OutDefinition) const;
};
