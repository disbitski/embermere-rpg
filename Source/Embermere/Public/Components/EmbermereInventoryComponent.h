#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereItemData.h"
#include "EmbermereInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FEmbermereInventoryStack
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UEmbermereItemData> Item;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEmbermereInventoryChangedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereItemAddedSignature, UEmbermereItemData*, Item, int32, Quantity);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereInventoryComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	int32 MaxSlots = 24;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FEmbermereInventoryStack> Stacks;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereInventoryChangedSignature OnInventoryChanged;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereItemAddedSignature OnItemAdded;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Inventory")
	bool AddItem(UEmbermereItemData* Item, int32 Quantity = 1);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Inventory")
	bool RemoveItem(UEmbermereItemData* Item, int32 Quantity = 1);
};
