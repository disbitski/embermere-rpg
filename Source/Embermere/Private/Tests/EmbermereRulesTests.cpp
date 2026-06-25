#if WITH_DEV_AUTOMATION_TESTS

#include "Data/EmbermereRulesData.h"
#include "Misc/AutomationTest.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRaceClassRulesTest,
	"Embermere.Rules.RaceClassMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRaceClassRulesTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	TestNotNull(TEXT("Rules data can be created"), Rules);
	if (!Rules)
	{
		return false;
	}

	TestTrue(TEXT("Human can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestTrue(TEXT("Human can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Cleric));
	TestTrue(TEXT("Human can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Ranger));
	TestTrue(TEXT("Human can be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Wizard));

	TestTrue(TEXT("Dwarf can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
	TestTrue(TEXT("Dwarf can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Cleric));
	TestFalse(TEXT("Dwarf cannot be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Ranger));
	TestFalse(TEXT("Dwarf cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Wizard));

	TestTrue(TEXT("Bullywug can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Warrior));
	TestTrue(TEXT("Bullywug can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Cleric));
	TestTrue(TEXT("Bullywug can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Ranger));
	TestFalse(TEXT("Bullywug cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Wizard));

	FEmbermereClassDefinition WarriorDefinition;
	TestTrue(TEXT("Warrior class definition exists"), Rules->GetClassDefinition(EEmbermereClass::Warrior, WarriorDefinition));
	TestEqual(TEXT("Warrior starts with four abilities"), WarriorDefinition.StarterAbilityIds.Num(), 4);
	TestEqual(TEXT("Warrior first hotbar ability is Strike"), WarriorDefinition.StarterAbilityIds[0], FName("Strike"));

	FEmbermereAbilityDefinition StrikeDefinition;
	TestTrue(TEXT("Strike ability definition exists"), Rules->GetAbilityDefinition("Strike", StrikeDefinition));
	TestEqual(TEXT("Strike belongs to Warrior"), StrikeDefinition.OwningClass, EEmbermereClass::Warrior);
	TestEqual(TEXT("Strike targets enemies"), StrikeDefinition.TargetKind, EEmbermereAbilityTargetKind::Enemy);

	return true;
}

#endif
