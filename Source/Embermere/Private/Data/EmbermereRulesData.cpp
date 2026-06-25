#include "Data/EmbermereRulesData.h"

namespace EmbermereRules
{
	FEmbermereRaceDefinition MakeRace(EEmbermereRace Race, const TCHAR* Name, const TCHAR* Description, TArray<EEmbermereClass> AllowedClasses)
	{
		FEmbermereRaceDefinition Definition;
		Definition.Race = Race;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.AllowedClasses = MoveTemp(AllowedClasses);
		return Definition;
	}

	FEmbermereClassDefinition MakeClass(EEmbermereClass Class, const TCHAR* Name, const TCHAR* Description, TArray<FName> StarterAbilityIds)
	{
		FEmbermereClassDefinition Definition;
		Definition.Class = Class;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.StarterAbilityIds = MoveTemp(StarterAbilityIds);
		return Definition;
	}

	FEmbermereAbilityDefinition MakeAbility(FName AbilityId, EEmbermereClass Class, const TCHAR* Name, const TCHAR* Description, EEmbermereAbilityTargetKind TargetKind, float Power, float ManaCost, float Range, float Cooldown)
	{
		FEmbermereAbilityDefinition Definition;
		Definition.AbilityId = AbilityId;
		Definition.OwningClass = Class;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.TargetKind = TargetKind;
		Definition.Power = Power;
		Definition.ManaCost = ManaCost;
		Definition.Range = Range;
		Definition.Cooldown = Cooldown;
		return Definition;
	}
}

UEmbermereRulesData::UEmbermereRulesData()
{
	using namespace EmbermereRules;

	Races = {
		MakeRace(EEmbermereRace::Human, TEXT("Human"), TEXT("Adaptable folk from the roads and farms around Embermere."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger, EEmbermereClass::Wizard }),
		MakeRace(EEmbermereRace::Elf, TEXT("Elf"), TEXT("Graceful forest kin with old songs and sharp eyes."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger, EEmbermereClass::Wizard }),
		MakeRace(EEmbermereRace::Dwarf, TEXT("Dwarf"), TEXT("Stone-blooded defenders and temple keepers."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric }),
		MakeRace(EEmbermereRace::Gnome, TEXT("Gnome"), TEXT("Small, brilliant, and difficult to surprise."), { EEmbermereClass::Cleric, EEmbermereClass::Wizard }),
		MakeRace(EEmbermereRace::DarkElf, TEXT("Dark Elf"), TEXT("Moonless exiles with blade craft and forbidden lore."), { EEmbermereClass::Warrior, EEmbermereClass::Ranger, EEmbermereClass::Wizard }),
		MakeRace(EEmbermereRace::Lizardman, TEXT("Lizardman"), TEXT("Scale-clad hunters from the warm marsh edge."), { EEmbermereClass::Warrior, EEmbermereClass::Ranger }),
		MakeRace(EEmbermereRace::Ogre, TEXT("Ogre"), TEXT("Massive warriors whose shamans remember ancient oaths."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric }),
		MakeRace(EEmbermereRace::Bullywug, TEXT("Bullywug"), TEXT("Amphibian wanderers with swamp songs and stubborn courage."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger })
	};

	Classes = {
		MakeClass(EEmbermereClass::Warrior, TEXT("Warrior"), TEXT("A durable front-line fighter built around threat and weapon pressure."), { "Strike", "Taunt", "ShieldSlam", "BattleShout" }),
		MakeClass(EEmbermereClass::Cleric, TEXT("Cleric"), TEXT("A holy caster who heals allies and punishes the restless dead."), { "Smite", "LesserHeal", "Ward", "Judgment" }),
		MakeClass(EEmbermereClass::Ranger, TEXT("Ranger"), TEXT("A wilderness fighter with bow pressure, snares, and steady melee backup."), { "QuickShot", "Snare", "TwinCut", "NaturesFocus" }),
		MakeClass(EEmbermereClass::Wizard, TEXT("Wizard"), TEXT("A fragile spellcaster built around roots, mana, and burst damage."), { "SparkBolt", "FrostRoot", "ArcaneBurst", "Meditate" })
	};

	Abilities = {
		MakeAbility("Strike", EEmbermereClass::Warrior, TEXT("Strike"), TEXT("A simple weapon attack."), EEmbermereAbilityTargetKind::Enemy, 18.0f, 0.0f, 225.0f, 1.5f),
		MakeAbility("Taunt", EEmbermereClass::Warrior, TEXT("Taunt"), TEXT("Forces the enemy's attention in future party play."), EEmbermereAbilityTargetKind::Enemy, 4.0f, 0.0f, 400.0f, 8.0f),
		MakeAbility("ShieldSlam", EEmbermereClass::Warrior, TEXT("Shield Slam"), TEXT("A heavier shield attack."), EEmbermereAbilityTargetKind::Enemy, 30.0f, 0.0f, 175.0f, 6.0f),
		MakeAbility("BattleShout", EEmbermereClass::Warrior, TEXT("Battle Shout"), TEXT("A self-buff placeholder for later threat and damage tuning."), EEmbermereAbilityTargetKind::Self, 8.0f, 0.0f, 0.0f, 12.0f),

		MakeAbility("Smite", EEmbermereClass::Cleric, TEXT("Smite"), TEXT("A focused holy damage spell."), EEmbermereAbilityTargetKind::Enemy, 22.0f, 8.0f, 800.0f, 2.5f),
		MakeAbility("LesserHeal", EEmbermereClass::Cleric, TEXT("Lesser Heal"), TEXT("Restores a modest amount of health."), EEmbermereAbilityTargetKind::Self, 28.0f, 12.0f, 0.0f, 3.0f),
		MakeAbility("Ward", EEmbermereClass::Cleric, TEXT("Ward"), TEXT("A defensive blessing placeholder."), EEmbermereAbilityTargetKind::Self, 10.0f, 10.0f, 0.0f, 12.0f),
		MakeAbility("Judgment", EEmbermereClass::Cleric, TEXT("Judgment"), TEXT("A stronger holy strike."), EEmbermereAbilityTargetKind::Enemy, 34.0f, 18.0f, 650.0f, 8.0f),

		MakeAbility("QuickShot", EEmbermereClass::Ranger, TEXT("Quick Shot"), TEXT("A fast ranged attack."), EEmbermereAbilityTargetKind::Enemy, 18.0f, 0.0f, 900.0f, 1.8f),
		MakeAbility("Snare", EEmbermereClass::Ranger, TEXT("Snare"), TEXT("Slows the target in the full prototype; deals light damage now."), EEmbermereAbilityTargetKind::Enemy, 8.0f, 0.0f, 800.0f, 8.0f),
		MakeAbility("TwinCut", EEmbermereClass::Ranger, TEXT("Twin Cut"), TEXT("A quick melee follow-up."), EEmbermereAbilityTargetKind::Enemy, 24.0f, 0.0f, 180.0f, 4.0f),
		MakeAbility("NaturesFocus", EEmbermereClass::Ranger, TEXT("Nature's Focus"), TEXT("A self-buff placeholder for later ranged tuning."), EEmbermereAbilityTargetKind::Self, 8.0f, 0.0f, 0.0f, 12.0f),

		MakeAbility("SparkBolt", EEmbermereClass::Wizard, TEXT("Spark Bolt"), TEXT("A reliable starter bolt."), EEmbermereAbilityTargetKind::Enemy, 26.0f, 10.0f, 900.0f, 2.0f),
		MakeAbility("FrostRoot", EEmbermereClass::Wizard, TEXT("Frost Root"), TEXT("Roots the target in the full prototype; deals light damage now."), EEmbermereAbilityTargetKind::Enemy, 10.0f, 12.0f, 800.0f, 8.0f),
		MakeAbility("ArcaneBurst", EEmbermereClass::Wizard, TEXT("Arcane Burst"), TEXT("A high-damage burst spell."), EEmbermereAbilityTargetKind::Enemy, 42.0f, 24.0f, 750.0f, 7.0f),
		MakeAbility("Meditate", EEmbermereClass::Wizard, TEXT("Meditate"), TEXT("A mana recovery placeholder."), EEmbermereAbilityTargetKind::Self, 18.0f, 0.0f, 0.0f, 15.0f)
	};
}

bool UEmbermereRulesData::IsClassAllowed(EEmbermereRace Race, EEmbermereClass Class) const
{
	FEmbermereRaceDefinition RaceDefinition;
	return GetRaceDefinition(Race, RaceDefinition) && RaceDefinition.AllowedClasses.Contains(Class);
}

bool UEmbermereRulesData::GetRaceDefinition(EEmbermereRace Race, FEmbermereRaceDefinition& OutDefinition) const
{
	for (const FEmbermereRaceDefinition& Definition : Races)
	{
		if (Definition.Race == Race)
		{
			OutDefinition = Definition;
			return true;
		}
	}
	return false;
}

bool UEmbermereRulesData::GetClassDefinition(EEmbermereClass Class, FEmbermereClassDefinition& OutDefinition) const
{
	for (const FEmbermereClassDefinition& Definition : Classes)
	{
		if (Definition.Class == Class)
		{
			OutDefinition = Definition;
			return true;
		}
	}
	return false;
}

bool UEmbermereRulesData::GetAbilityDefinition(FName AbilityId, FEmbermereAbilityDefinition& OutDefinition) const
{
	for (const FEmbermereAbilityDefinition& Definition : Abilities)
	{
		if (Definition.AbilityId == AbilityId)
		{
			OutDefinition = Definition;
			return true;
		}
	}
	return false;
}
