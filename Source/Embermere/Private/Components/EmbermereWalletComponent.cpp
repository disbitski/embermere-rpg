#include "Components/EmbermereWalletComponent.h"

UEmbermereWalletComponent::UEmbermereWalletComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereWalletComponent::BeginPlay()
{
	Super::BeginPlay();
	ResetToStartingCopper();
}

void UEmbermereWalletComponent::ResetToStartingCopper()
{
	Copper = FMath::Max(0, StartingCopper);
	OnWalletChanged.Broadcast(Copper);
}

bool UEmbermereWalletComponent::CanAfford(int32 Amount) const
{
	return Amount >= 0 && Copper >= Amount;
}

bool UEmbermereWalletComponent::CanAddCopper(int32 Amount) const
{
	return Amount > 0 && Copper <= MAX_int32 - Amount;
}

bool UEmbermereWalletComponent::TrySpendCopper(int32 Amount)
{
	if (Amount <= 0 || !CanAfford(Amount))
	{
		return false;
	}

	Copper -= Amount;
	OnWalletChanged.Broadcast(Copper);
	return true;
}

bool UEmbermereWalletComponent::AddCopper(int32 Amount)
{
	if (!CanAddCopper(Amount))
	{
		return false;
	}

	Copper += Amount;
	OnWalletChanged.Broadcast(Copper);
	return true;
}

void UEmbermereWalletComponent::SetCopperForPrototype(int32 NewBalance)
{
	Copper = FMath::Max(0, NewBalance);
	OnWalletChanged.Broadcast(Copper);
}
