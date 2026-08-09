#include "UI/EmbermereTrainerOfferingButton.h"

UEmbermereTrainerOfferingButton::UEmbermereTrainerOfferingButton(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitIsFocusable(false);
	OnClicked.AddUniqueDynamic(this, &UEmbermereTrainerOfferingButton::HandleClicked);
}

void UEmbermereTrainerOfferingButton::SetOfferingIndex(int32 InOfferingIndex)
{
	OfferingIndex = InOfferingIndex;
}

void UEmbermereTrainerOfferingButton::HandleClicked()
{
	if (OfferingIndex != INDEX_NONE)
	{
		OnTrainerOfferingClicked.Broadcast(OfferingIndex);
	}
}
