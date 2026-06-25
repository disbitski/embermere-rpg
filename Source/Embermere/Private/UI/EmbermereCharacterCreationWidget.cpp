#include "UI/EmbermereCharacterCreationWidget.h"
#include "Characters/EmbermereCharacter.h"
#include "Data/EmbermereRulesData.h"

bool UEmbermereCharacterCreationWidget::SetRaceAndClass(EEmbermereRace Race, EEmbermereClass Class)
{
	UEmbermereRulesData* EffectiveRules = RulesData ? RulesData : NewObject<UEmbermereRulesData>(this);
	if (!EffectiveRules || !EffectiveRules->IsClassAllowed(Race, Class))
	{
		return false;
	}

	SelectedRace = Race;
	SelectedClass = Class;
	return true;
}

void UEmbermereCharacterCreationWidget::ConfirmChoice(AEmbermereCharacter* Character)
{
	if (Character)
	{
		Character->ApplyRaceAndClass(SelectedRace, SelectedClass);
	}

	OnCharacterChoiceConfirmed.Broadcast(SelectedRace, SelectedClass);
}
