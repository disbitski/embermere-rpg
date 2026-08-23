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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Progression")
	TArray<int32> ExperienceThresholds;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool IsClassAllowed(EEmbermereRace Race, EEmbermereClass Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetRaceDefinition(EEmbermereRace Race, FEmbermereRaceDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetClassDefinition(EEmbermereClass Class, FEmbermereClassDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool GetAbilityDefinition(FName AbilityId, FEmbermereAbilityDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules")
	bool IsCharacterIdentityValid(EEmbermereRace Race, EEmbermereClass Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rules|Progression")
	bool IsProgressionDefinitionValid() const;

	bool GetProgressionProfile(
		EEmbermereRace Race,
		EEmbermereClass Class,
		FEmbermereProgressionProfile& OutProfile) const;

	bool ResolveProgressionAttributes(
		EEmbermereRace Race,
		EEmbermereClass Class,
		int32 Experience,
		FEmbermereAttributeBlock& OutAttributes,
		int32& OutLevel) const;

	static bool IsProgressionProfileValid(const FEmbermereProgressionProfile& Profile);
	static bool ResolveProgression(
		const FEmbermereProgressionProfile& Profile,
		int32 Experience,
		FEmbermereAttributeBlock& OutAttributes,
		int32& OutLevel);

	static FName GetStableRaceId(EEmbermereRace Race);
	static FName GetStableClassId(EEmbermereClass Class);
	static bool TryResolveStableRaceId(FName RaceId, EEmbermereRace& OutRace);
	static bool TryResolveStableClassId(FName ClassId, EEmbermereClass& OutClass);
};
