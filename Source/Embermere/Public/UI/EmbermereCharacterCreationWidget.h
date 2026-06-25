#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCharacterCreationWidget.generated.h"

class AEmbermereCharacter;
class UEmbermereRulesData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereCharacterChoiceSignature, EEmbermereRace, Race, EEmbermereClass, Class);

UCLASS(Abstract, Blueprintable)
class EMBERMERE_API UEmbermereCharacterCreationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rules")
	TObjectPtr<UEmbermereRulesData> RulesData;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	EEmbermereRace SelectedRace = EEmbermereRace::Human;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	EEmbermereClass SelectedClass = EEmbermereClass::Warrior;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereCharacterChoiceSignature OnCharacterChoiceConfirmed;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	bool SetRaceAndClass(EEmbermereRace Race, EEmbermereClass Class);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	void ConfirmChoice(AEmbermereCharacter* Character);
};
