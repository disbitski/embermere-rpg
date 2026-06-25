#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereHotbarComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEmbermereHotbarChangedSignature, int32, SlotIndex, const FEmbermereAbilityDefinition&, Ability);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereHotbarComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereHotbarComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hotbar")
	TArray<FEmbermereAbilityDefinition> Slots;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereHotbarChangedSignature OnHotbarChanged;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Hotbar")
	void SetAbilityInSlot(int32 SlotIndex, const FEmbermereAbilityDefinition& Ability);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Hotbar")
	bool ActivateSlot(int32 SlotIndex);
};
