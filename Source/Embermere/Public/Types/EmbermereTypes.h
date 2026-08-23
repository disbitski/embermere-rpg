#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EmbermereTypes.generated.h"

class USkeletalMesh;
class UTexture2D;

UENUM(BlueprintType)
enum class EEmbermereRace : uint8
{
	Human UMETA(DisplayName = "Human"),
	Elf UMETA(DisplayName = "Elf"),
	Dwarf UMETA(DisplayName = "Dwarf"),
	Gnome UMETA(DisplayName = "Gnome"),
	DarkElf UMETA(DisplayName = "Dark Elf"),
	Lizardman UMETA(DisplayName = "Lizardman"),
	Ogre UMETA(DisplayName = "Ogre"),
	Bullywug UMETA(DisplayName = "Bullywug")
};

UENUM(BlueprintType)
enum class EEmbermereClass : uint8
{
	Warrior UMETA(DisplayName = "Warrior"),
	Cleric UMETA(DisplayName = "Cleric"),
	Ranger UMETA(DisplayName = "Ranger"),
	Wizard UMETA(DisplayName = "Wizard")
};

UENUM(BlueprintType)
enum class EEmbermereAbilityTargetKind : uint8
{
	Enemy UMETA(DisplayName = "Enemy"),
	Ally UMETA(DisplayName = "Ally"),
	Self UMETA(DisplayName = "Self")
};

UENUM(BlueprintType)
enum class EEmbermereAbilityEffectType : uint8
{
	Damage UMETA(DisplayName = "Damage"),
	Heal UMETA(DisplayName = "Heal"),
	RestoreMana UMETA(DisplayName = "Restore Mana"),
	AttackPowerBuff UMETA(DisplayName = "Attack Power Buff"),
	ArmorBuff UMETA(DisplayName = "Armor Buff")
};

USTRUCT(BlueprintType)
struct FEmbermereAttributeBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxMana = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Strength = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Spirit = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Agility = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Intellect = 10.0f;
};

USTRUCT(BlueprintType)
struct FEmbermereAttributeGrowth
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float MaxHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float MaxMana = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Strength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Spirit = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Agility = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	float Intellect = 0.0f;
};

USTRUCT(BlueprintType)
struct FEmbermereProgressionProfile
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FEmbermereAttributeBlock StartingAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FEmbermereAttributeGrowth LevelGrowth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	TArray<int32> ExperienceThresholds;
};

USTRUCT(BlueprintType)
struct FEmbermereRaceDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	EEmbermereRace Race = EEmbermereRace::Human;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	FEmbermereAttributeBlock AttributeModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FEmbermereAttributeGrowth LevelGrowth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Race")
	TArray<EEmbermereClass> AllowedClasses;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TSoftObjectPtr<UTexture2D> Portrait;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TSoftObjectPtr<USkeletalMesh> DefaultMesh;
};

USTRUCT(BlueprintType)
struct FEmbermereClassDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	EEmbermereClass Class = EEmbermereClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	FEmbermereAttributeBlock StartingAttributes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression")
	FEmbermereAttributeGrowth LevelGrowth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Class")
	TArray<FName> StarterAbilityIds;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TSoftObjectPtr<UTexture2D> Icon;
};

USTRUCT(BlueprintType)
struct FEmbermereAbilityDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FName AbilityId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	EEmbermereClass OwningClass = EEmbermereClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	EEmbermereAbilityTargetKind TargetKind = EEmbermereAbilityTargetKind::Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	EEmbermereAbilityEffectType EffectType = EEmbermereAbilityEffectType::Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Power = 20.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float ManaCost = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Range = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float Cooldown = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ClampMin = "0.0"))
	float Duration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MovementSpeedMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Assets")
	TSoftObjectPtr<UTexture2D> Icon;
};
