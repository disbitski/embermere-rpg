#include "Data/EmbermereRulesData.h"

namespace EmbermereRules
{
	bool IsFiniteNonNegative(float Value)
	{
		return FMath::IsFinite(Value) && Value >= 0.0f;
	}

	bool IsStartingAttributesValid(const FEmbermereAttributeBlock& Attributes)
	{
		return FMath::IsFinite(Attributes.MaxHealth) && Attributes.MaxHealth >= 1.0f &&
			IsFiniteNonNegative(Attributes.MaxMana) &&
			IsFiniteNonNegative(Attributes.Strength) &&
			IsFiniteNonNegative(Attributes.Spirit) &&
			IsFiniteNonNegative(Attributes.Agility) &&
			IsFiniteNonNegative(Attributes.Intellect);
	}

	bool IsGrowthValid(const FEmbermereAttributeGrowth& Growth)
	{
		return IsFiniteNonNegative(Growth.MaxHealth) &&
			IsFiniteNonNegative(Growth.MaxMana) &&
			IsFiniteNonNegative(Growth.Strength) &&
			IsFiniteNonNegative(Growth.Spirit) &&
			IsFiniteNonNegative(Growth.Agility) &&
			IsFiniteNonNegative(Growth.Intellect);
	}

	FEmbermereAttributeGrowth AddGrowth(
		const FEmbermereAttributeGrowth& Left,
		const FEmbermereAttributeGrowth& Right)
	{
		FEmbermereAttributeGrowth Result;
		Result.MaxHealth = Left.MaxHealth + Right.MaxHealth;
		Result.MaxMana = Left.MaxMana + Right.MaxMana;
		Result.Strength = Left.Strength + Right.Strength;
		Result.Spirit = Left.Spirit + Right.Spirit;
		Result.Agility = Left.Agility + Right.Agility;
		Result.Intellect = Left.Intellect + Right.Intellect;
		return Result;
	}

	FEmbermereAttributeBlock MakeAttributes(
		float MaxHealth,
		float MaxMana,
		float Strength,
		float Spirit,
		float Agility,
		float Intellect)
	{
		FEmbermereAttributeBlock Attributes;
		Attributes.MaxHealth = MaxHealth;
		Attributes.MaxMana = MaxMana;
		Attributes.Strength = Strength;
		Attributes.Spirit = Spirit;
		Attributes.Agility = Agility;
		Attributes.Intellect = Intellect;
		return Attributes;
	}

	FEmbermereAttributeGrowth MakeGrowth(
		float MaxHealth,
		float MaxMana,
		float Strength,
		float Spirit,
		float Agility,
		float Intellect)
	{
		FEmbermereAttributeGrowth Growth;
		Growth.MaxHealth = MaxHealth;
		Growth.MaxMana = MaxMana;
		Growth.Strength = Strength;
		Growth.Spirit = Spirit;
		Growth.Agility = Agility;
		Growth.Intellect = Intellect;
		return Growth;
	}

	FEmbermereRaceDefinition MakeRace(
		EEmbermereRace Race,
		const TCHAR* Name,
		const TCHAR* Description,
		TArray<EEmbermereClass> AllowedClasses,
		const FEmbermereAttributeGrowth& LevelGrowth)
	{
		FEmbermereRaceDefinition Definition;
		Definition.Race = Race;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.AllowedClasses = MoveTemp(AllowedClasses);
		Definition.LevelGrowth = LevelGrowth;
		return Definition;
	}

	FEmbermereClassDefinition MakeClass(
		EEmbermereClass Class,
		const TCHAR* Name,
		const TCHAR* Description,
		TArray<FName> StarterAbilityIds,
		const FEmbermereAttributeBlock& StartingAttributes,
		const FEmbermereAttributeGrowth& LevelGrowth)
	{
		FEmbermereClassDefinition Definition;
		Definition.Class = Class;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.StarterAbilityIds = MoveTemp(StarterAbilityIds);
		Definition.StartingAttributes = StartingAttributes;
		Definition.LevelGrowth = LevelGrowth;
		return Definition;
	}

	FEmbermereAbilityDefinition MakeAbility(
		FName AbilityId,
		EEmbermereClass Class,
		const TCHAR* Name,
		const TCHAR* Description,
		EEmbermereAbilityTargetKind TargetKind,
		float Power,
		float ManaCost,
		float Range,
		float Cooldown,
		EEmbermereAbilityEffectType EffectType = EEmbermereAbilityEffectType::Damage,
		float Duration = 0.0f,
		float MovementSpeedMultiplier = 1.0f)
	{
		FEmbermereAbilityDefinition Definition;
		const FString IconName = FString::Printf(TEXT("T_Icon_Ability_%s"), *AbilityId.ToString());
		Definition.AbilityId = AbilityId;
		Definition.OwningClass = Class;
		Definition.DisplayName = FText::FromString(Name);
		Definition.Description = FText::FromString(Description);
		Definition.TargetKind = TargetKind;
		Definition.EffectType = EffectType;
		Definition.Power = Power;
		Definition.ManaCost = ManaCost;
		Definition.Range = Range;
		Definition.Cooldown = Cooldown;
		Definition.Duration = Duration;
		Definition.MovementSpeedMultiplier = MovementSpeedMultiplier;
		Definition.Icon = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(FString::Printf(
			TEXT("/Game/UI/Icons/Abilities/%s.%s"),
			*IconName,
			*IconName)));
		return Definition;
	}
}

UEmbermereRulesData::UEmbermereRulesData()
{
	using namespace EmbermereRules;
	ExperienceThresholds = {0, 100, 250, 450, 700};

	Races = {
		MakeRace(EEmbermereRace::Human, TEXT("Human"), TEXT("Adaptable folk from the roads and farms around Embermere."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger, EEmbermereClass::Wizard }, MakeGrowth(2.0f, 2.0f, 0.5f, 0.5f, 0.5f, 0.5f)),
		MakeRace(EEmbermereRace::Elf, TEXT("Elf"), TEXT("Graceful forest kin with old songs and sharp eyes."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger, EEmbermereClass::Wizard }, MakeGrowth(1.0f, 4.0f, 0.25f, 0.5f, 0.75f, 1.0f)),
		MakeRace(EEmbermereRace::Dwarf, TEXT("Dwarf"), TEXT("Stone-blooded defenders and temple keepers."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric }, MakeGrowth(5.0f, 1.0f, 1.0f, 0.5f, 0.25f, 0.25f)),
		MakeRace(EEmbermereRace::Gnome, TEXT("Gnome"), TEXT("Small, brilliant, and difficult to surprise."), { EEmbermereClass::Cleric, EEmbermereClass::Wizard }, MakeGrowth(1.0f, 5.0f, 0.25f, 0.75f, 0.5f, 1.0f)),
		MakeRace(EEmbermereRace::DarkElf, TEXT("Dark Elf"), TEXT("Moonless exiles with blade craft and forbidden lore."), { EEmbermereClass::Warrior, EEmbermereClass::Ranger, EEmbermereClass::Wizard }, MakeGrowth(2.0f, 4.0f, 0.5f, 0.5f, 0.75f, 0.75f)),
		MakeRace(EEmbermereRace::Lizardman, TEXT("Lizardman"), TEXT("Scale-clad hunters from the warm marsh edge."), { EEmbermereClass::Warrior, EEmbermereClass::Ranger }, MakeGrowth(4.0f, 1.0f, 0.75f, 0.25f, 0.75f, 0.25f)),
		MakeRace(EEmbermereRace::Ogre, TEXT("Ogre"), TEXT("Massive warriors whose shamans remember ancient oaths."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric }, MakeGrowth(6.0f, 0.0f, 1.25f, 0.25f, 0.25f, 0.0f)),
		MakeRace(EEmbermereRace::Bullywug, TEXT("Bullywug"), TEXT("Amphibian wanderers with swamp songs and stubborn courage."), { EEmbermereClass::Warrior, EEmbermereClass::Cleric, EEmbermereClass::Ranger }, MakeGrowth(3.0f, 2.0f, 0.5f, 0.75f, 0.5f, 0.25f))
	};

	Classes = {
		MakeClass(EEmbermereClass::Warrior, TEXT("Warrior"), TEXT("A durable front-line fighter built around threat and weapon pressure."), { "Strike", "Taunt", "ShieldSlam", "BattleShout" }, MakeAttributes(100.0f, 50.0f, 10.0f, 8.0f, 10.0f, 7.0f), MakeGrowth(8.0f, 1.0f, 1.5f, 0.5f, 0.75f, 0.25f)),
		MakeClass(EEmbermereClass::Cleric, TEXT("Cleric"), TEXT("A holy caster who heals allies and punishes the restless dead."), { "Smite", "LesserHeal", "Ward", "Judgment" }, MakeAttributes(95.0f, 80.0f, 8.0f, 14.0f, 8.0f, 12.0f), MakeGrowth(5.0f, 6.0f, 0.5f, 1.5f, 0.5f, 1.0f)),
		MakeClass(EEmbermereClass::Ranger, TEXT("Ranger"), TEXT("A wilderness fighter with bow pressure, snares, and steady melee backup."), { "QuickShot", "Snare", "TwinCut", "NaturesFocus" }, MakeAttributes(100.0f, 60.0f, 10.0f, 9.0f, 14.0f, 9.0f), MakeGrowth(6.0f, 3.0f, 1.0f, 0.5f, 1.5f, 0.5f)),
		MakeClass(EEmbermereClass::Wizard, TEXT("Wizard"), TEXT("A fragile spellcaster built around roots, mana, and burst damage."), { "SparkBolt", "FrostRoot", "ArcaneBurst", "Meditate" }, MakeAttributes(80.0f, 110.0f, 6.0f, 12.0f, 8.0f, 16.0f), MakeGrowth(3.0f, 8.0f, 0.25f, 1.0f, 0.5f, 1.75f))
	};

	Abilities = {
		MakeAbility("Strike", EEmbermereClass::Warrior, TEXT("Strike"), TEXT("A simple weapon attack."), EEmbermereAbilityTargetKind::Enemy, 18.0f, 0.0f, 225.0f, 1.5f),
		MakeAbility("Taunt", EEmbermereClass::Warrior, TEXT("Taunt"), TEXT("Forces the enemy's attention in future party play."), EEmbermereAbilityTargetKind::Enemy, 4.0f, 0.0f, 400.0f, 8.0f),
		MakeAbility("ShieldSlam", EEmbermereClass::Warrior, TEXT("Shield Slam"), TEXT("A heavier shield attack."), EEmbermereAbilityTargetKind::Enemy, 30.0f, 0.0f, 175.0f, 6.0f),
		MakeAbility("BattleShout", EEmbermereClass::Warrior, TEXT("Battle Shout"), TEXT("Raises Attack Power by 8 for 10 seconds."), EEmbermereAbilityTargetKind::Self, 8.0f, 0.0f, 0.0f, 12.0f, EEmbermereAbilityEffectType::AttackPowerBuff, 10.0f),

		MakeAbility("Smite", EEmbermereClass::Cleric, TEXT("Smite"), TEXT("A focused holy damage spell."), EEmbermereAbilityTargetKind::Enemy, 22.0f, 8.0f, 800.0f, 2.5f),
		MakeAbility("LesserHeal", EEmbermereClass::Cleric, TEXT("Lesser Heal"), TEXT("Restores a modest amount of health."), EEmbermereAbilityTargetKind::Self, 28.0f, 12.0f, 0.0f, 3.0f, EEmbermereAbilityEffectType::Heal),
		MakeAbility("Ward", EEmbermereClass::Cleric, TEXT("Ward"), TEXT("Raises Armor by 10 for 10 seconds."), EEmbermereAbilityTargetKind::Self, 10.0f, 10.0f, 0.0f, 12.0f, EEmbermereAbilityEffectType::ArmorBuff, 10.0f),
		MakeAbility("Judgment", EEmbermereClass::Cleric, TEXT("Judgment"), TEXT("A stronger holy strike."), EEmbermereAbilityTargetKind::Enemy, 34.0f, 18.0f, 650.0f, 8.0f),

		MakeAbility("QuickShot", EEmbermereClass::Ranger, TEXT("Quick Shot"), TEXT("A fast ranged attack."), EEmbermereAbilityTargetKind::Enemy, 18.0f, 0.0f, 900.0f, 1.8f),
		MakeAbility("Snare", EEmbermereClass::Ranger, TEXT("Snare"), TEXT("Deals light damage and slows the target by 50% for 6 seconds."), EEmbermereAbilityTargetKind::Enemy, 8.0f, 0.0f, 800.0f, 8.0f, EEmbermereAbilityEffectType::Damage, 6.0f, 0.5f),
		MakeAbility("TwinCut", EEmbermereClass::Ranger, TEXT("Twin Cut"), TEXT("A quick melee follow-up."), EEmbermereAbilityTargetKind::Enemy, 24.0f, 0.0f, 180.0f, 4.0f),
		MakeAbility("NaturesFocus", EEmbermereClass::Ranger, TEXT("Nature's Focus"), TEXT("Raises Attack Power by 8 for 10 seconds."), EEmbermereAbilityTargetKind::Self, 8.0f, 0.0f, 0.0f, 12.0f, EEmbermereAbilityEffectType::AttackPowerBuff, 10.0f),

		MakeAbility("SparkBolt", EEmbermereClass::Wizard, TEXT("Spark Bolt"), TEXT("A reliable starter bolt."), EEmbermereAbilityTargetKind::Enemy, 26.0f, 10.0f, 900.0f, 2.0f),
		MakeAbility("FrostRoot", EEmbermereClass::Wizard, TEXT("Frost Root"), TEXT("Deals light damage and roots the target for 4 seconds."), EEmbermereAbilityTargetKind::Enemy, 10.0f, 12.0f, 800.0f, 8.0f, EEmbermereAbilityEffectType::Damage, 4.0f, 0.0f),
		MakeAbility("ArcaneBurst", EEmbermereClass::Wizard, TEXT("Arcane Burst"), TEXT("A high-damage burst spell."), EEmbermereAbilityTargetKind::Enemy, 42.0f, 24.0f, 750.0f, 7.0f),
		MakeAbility("Meditate", EEmbermereClass::Wizard, TEXT("Meditate"), TEXT("Restores 18 mana."), EEmbermereAbilityTargetKind::Self, 18.0f, 0.0f, 0.0f, 15.0f, EEmbermereAbilityEffectType::RestoreMana)
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

bool UEmbermereRulesData::IsCharacterIdentityValid(
	EEmbermereRace Race,
	EEmbermereClass Class) const
{
	if (!IsClassAllowed(Race, Class))
	{
		return false;
	}

	FEmbermereClassDefinition ClassDefinition;
	if (!GetClassDefinition(Class, ClassDefinition) || ClassDefinition.StarterAbilityIds.Num() < 4)
	{
		return false;
	}
	for (int32 Index = 0; Index < 4; ++Index)
	{
		FEmbermereAbilityDefinition AbilityDefinition;
		if (!GetAbilityDefinition(ClassDefinition.StarterAbilityIds[Index], AbilityDefinition) ||
			AbilityDefinition.OwningClass != Class)
		{
			return false;
		}
	}
	return true;
}

bool UEmbermereRulesData::IsProgressionDefinitionValid() const
{
	if (ExperienceThresholds.Num() < 2 || ExperienceThresholds[0] != 0)
	{
		return false;
	}
	for (int32 Index = 1; Index < ExperienceThresholds.Num(); ++Index)
	{
		if (ExperienceThresholds[Index] <= ExperienceThresholds[Index - 1])
		{
			return false;
		}
	}
	for (const FEmbermereRaceDefinition& RaceDefinition : Races)
	{
		if (!EmbermereRules::IsGrowthValid(RaceDefinition.LevelGrowth))
		{
			return false;
		}
	}
	for (const FEmbermereClassDefinition& ClassDefinition : Classes)
	{
		if (!EmbermereRules::IsStartingAttributesValid(ClassDefinition.StartingAttributes) ||
			!EmbermereRules::IsGrowthValid(ClassDefinition.LevelGrowth))
		{
			return false;
		}
	}
	return true;
}

bool UEmbermereRulesData::GetProgressionProfile(
	EEmbermereRace Race,
	EEmbermereClass Class,
	FEmbermereProgressionProfile& OutProfile) const
{
	OutProfile = FEmbermereProgressionProfile();
	if (!IsCharacterIdentityValid(Race, Class) || !IsProgressionDefinitionValid())
	{
		return false;
	}

	FEmbermereRaceDefinition RaceDefinition;
	FEmbermereClassDefinition ClassDefinition;
	if (!GetRaceDefinition(Race, RaceDefinition) ||
		!GetClassDefinition(Class, ClassDefinition))
	{
		return false;
	}

	OutProfile.StartingAttributes = ClassDefinition.StartingAttributes;
	OutProfile.LevelGrowth = EmbermereRules::AddGrowth(
		RaceDefinition.LevelGrowth,
		ClassDefinition.LevelGrowth);
	OutProfile.ExperienceThresholds = ExperienceThresholds;
	return IsProgressionProfileValid(OutProfile);
}

bool UEmbermereRulesData::ResolveProgressionAttributes(
	EEmbermereRace Race,
	EEmbermereClass Class,
	int32 Experience,
	FEmbermereAttributeBlock& OutAttributes,
	int32& OutLevel) const
{
	FEmbermereProgressionProfile Profile;
	return GetProgressionProfile(Race, Class, Profile) &&
		ResolveProgression(Profile, Experience, OutAttributes, OutLevel);
}

bool UEmbermereRulesData::IsProgressionProfileValid(
	const FEmbermereProgressionProfile& Profile)
{
	if (!EmbermereRules::IsStartingAttributesValid(Profile.StartingAttributes) ||
		!EmbermereRules::IsGrowthValid(Profile.LevelGrowth) ||
		Profile.ExperienceThresholds.Num() < 2 ||
		Profile.ExperienceThresholds[0] != 0)
	{
		return false;
	}
	for (int32 Index = 1; Index < Profile.ExperienceThresholds.Num(); ++Index)
	{
		if (Profile.ExperienceThresholds[Index] <= Profile.ExperienceThresholds[Index - 1])
		{
			return false;
		}
	}
	return true;
}

bool UEmbermereRulesData::ResolveProgression(
	const FEmbermereProgressionProfile& Profile,
	int32 Experience,
	FEmbermereAttributeBlock& OutAttributes,
	int32& OutLevel)
{
	OutAttributes = FEmbermereAttributeBlock();
	OutLevel = 1;
	if (Experience < 0 || !IsProgressionProfileValid(Profile))
	{
		return false;
	}

	for (int32 Index = 1; Index < Profile.ExperienceThresholds.Num(); ++Index)
	{
		if (Experience < Profile.ExperienceThresholds[Index])
		{
			break;
		}
		OutLevel = Index + 1;
	}

	const float GrowthSteps = static_cast<float>(OutLevel - 1);
	OutAttributes.MaxHealth = Profile.StartingAttributes.MaxHealth +
		Profile.LevelGrowth.MaxHealth * GrowthSteps;
	OutAttributes.MaxMana = Profile.StartingAttributes.MaxMana +
		Profile.LevelGrowth.MaxMana * GrowthSteps;
	OutAttributes.Strength = Profile.StartingAttributes.Strength +
		Profile.LevelGrowth.Strength * GrowthSteps;
	OutAttributes.Spirit = Profile.StartingAttributes.Spirit +
		Profile.LevelGrowth.Spirit * GrowthSteps;
	OutAttributes.Agility = Profile.StartingAttributes.Agility +
		Profile.LevelGrowth.Agility * GrowthSteps;
	OutAttributes.Intellect = Profile.StartingAttributes.Intellect +
		Profile.LevelGrowth.Intellect * GrowthSteps;
	return EmbermereRules::IsStartingAttributesValid(OutAttributes);
}

FName UEmbermereRulesData::GetStableRaceId(EEmbermereRace Race)
{
	switch (Race)
	{
	case EEmbermereRace::Human: return TEXT("Human");
	case EEmbermereRace::Elf: return TEXT("Elf");
	case EEmbermereRace::Dwarf: return TEXT("Dwarf");
	case EEmbermereRace::Gnome: return TEXT("Gnome");
	case EEmbermereRace::DarkElf: return TEXT("DarkElf");
	case EEmbermereRace::Lizardman: return TEXT("Lizardman");
	case EEmbermereRace::Ogre: return TEXT("Ogre");
	case EEmbermereRace::Bullywug: return TEXT("Bullywug");
	default: return NAME_None;
	}
}

FName UEmbermereRulesData::GetStableClassId(EEmbermereClass Class)
{
	switch (Class)
	{
	case EEmbermereClass::Warrior: return TEXT("Warrior");
	case EEmbermereClass::Cleric: return TEXT("Cleric");
	case EEmbermereClass::Ranger: return TEXT("Ranger");
	case EEmbermereClass::Wizard: return TEXT("Wizard");
	default: return NAME_None;
	}
}

bool UEmbermereRulesData::TryResolveStableRaceId(FName RaceId, EEmbermereRace& OutRace)
{
	for (const EEmbermereRace Race : {
		EEmbermereRace::Human,
		EEmbermereRace::Elf,
		EEmbermereRace::Dwarf,
		EEmbermereRace::Gnome,
		EEmbermereRace::DarkElf,
		EEmbermereRace::Lizardman,
		EEmbermereRace::Ogre,
		EEmbermereRace::Bullywug})
	{
		if (GetStableRaceId(Race) == RaceId)
		{
			OutRace = Race;
			return true;
		}
	}
	return false;
}

bool UEmbermereRulesData::TryResolveStableClassId(FName ClassId, EEmbermereClass& OutClass)
{
	for (const EEmbermereClass Class : {
		EEmbermereClass::Warrior,
		EEmbermereClass::Cleric,
		EEmbermereClass::Ranger,
		EEmbermereClass::Wizard})
	{
		if (GetStableClassId(Class) == ClassId)
		{
			OutClass = Class;
			return true;
		}
	}
	return false;
}
