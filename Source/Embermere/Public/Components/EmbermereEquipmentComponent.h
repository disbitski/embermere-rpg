#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereItemData.h"
#include "EmbermereEquipmentComponent.generated.h"

class UEmbermereInventoryComponent;

USTRUCT(BlueprintType)
struct FEmbermereEquippedItem
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	EEmbermereEquipmentSlot Slot = EEmbermereEquipmentSlot::None;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TObjectPtr<UEmbermereItemData> Item;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereEquipmentChangedSignature);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereEquipmentComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	TArray<FEmbermereEquippedItem> EquippedItems;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereEquipmentChangedSignature OnEquipmentChanged;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Equipment")
	bool CanEquip(const UEmbermereItemData* Item, int32 CharacterLevel) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Equipment")
	bool EquipItem(UEmbermereItemData* Item, int32 CharacterLevel);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Equipment")
	bool EquipFromInventory(UEmbermereItemData* Item, int32 CharacterLevel, UEmbermereInventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Equipment")
	UEmbermereItemData* UnequipItem(EEmbermereEquipmentSlot Slot);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Equipment")
	bool UnequipToInventory(EEmbermereEquipmentSlot Slot, UEmbermereInventoryComponent* Inventory);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Equipment")
	UEmbermereItemData* GetEquippedItem(EEmbermereEquipmentSlot Slot) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Equipment")
	bool IsItemEquipped(const UEmbermereItemData* Item) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Equipment")
	FEmbermereItemStatBonuses GetTotalStatBonuses() const;
};
