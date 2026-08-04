#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmbermereWalletComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereWalletChangedSignature, int32, NewCopperBalance);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereWalletComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereWalletComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency", meta = (ClampMin = "0"))
	int32 StartingCopper = 40;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Currency")
	int32 Copper = 40;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereWalletChangedSignature OnWalletChanged;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Currency")
	void ResetToStartingCopper();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Currency")
	bool CanAfford(int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Currency")
	bool TrySpendCopper(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Currency")
	bool AddCopper(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Currency")
	void SetCopperForPrototype(int32 NewBalance);

protected:
	virtual void BeginPlay() override;
};
