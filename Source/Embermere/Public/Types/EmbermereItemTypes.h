#pragma once

#include "CoreMinimal.h"
#include "EmbermereItemTypes.generated.h"

UENUM(BlueprintType)
enum class EEmbermereItemCategory : uint8
{
	Misc,
	Consumable,
	Weapon,
	Armor,
	Quest
};

UENUM(BlueprintType)
enum class EEmbermereEquipmentSlot : uint8
{
	None,
	MainHand,
	OffHand,
	Head,
	Chest,
	Hands,
	Legs,
	Feet,
	Back,
	Neck,
	Ring
};

USTRUCT(BlueprintType)
struct FEmbermereItemStatBonuses
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float MaxMana = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Armor = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	float Power = 0.0f;
};

USTRUCT(BlueprintType)
struct FEmbermereConsumableEffects
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
	float HealHealth = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Consumable")
	float RestoreMana = 0.0f;
};
