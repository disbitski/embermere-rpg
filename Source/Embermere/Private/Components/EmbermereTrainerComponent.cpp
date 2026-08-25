#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereWalletComponent.h"

UEmbermereTrainerComponent::UEmbermereTrainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereTrainerComponent::SetOfferingsData(UEmbermereTrainerOfferingsData* NewOfferingsData)
{
	if (OfferingsData != NewOfferingsData)
	{
		CompletedNonRepeatableOfferingIds.Reset();
	}
	OfferingsData = NewOfferingsData;
}

int32 UEmbermereTrainerComponent::GetOfferingCount() const
{
	return OfferingsData ? OfferingsData->Offerings.Num() : 0;
}

bool UEmbermereTrainerComponent::GetOffering(
	int32 OfferingIndex,
	FEmbermereTrainerOffering& OutOffering) const
{
	if (!OfferingsData || !OfferingsData->Offerings.IsValidIndex(OfferingIndex))
	{
		return false;
	}

	OutOffering = OfferingsData->Offerings[OfferingIndex];
	return true;
}

EEmbermereTrainingResult UEmbermereTrainerComponent::CanTrain(
	int32 OfferingIndex,
	const UEmbermereStatsComponent* Stats,
	const UEmbermereWalletComponent* Wallet) const
{
	FEmbermereTrainerOffering Offering;
	if (!Stats || !Wallet || !OfferingsData || !OfferingsData->HasValidOfferings() ||
		!GetOffering(OfferingIndex, Offering))
	{
		return EEmbermereTrainingResult::InvalidRequest;
	}

	if (!Offering.bRepeatable && CompletedNonRepeatableOfferingIds.Contains(Offering.OfferingId))
	{
		return EEmbermereTrainingResult::AlreadyCompleted;
	}

	if (Stats->Level < Offering.RequiredLevel)
	{
		return EEmbermereTrainingResult::LevelTooLow;
	}

	if (!Wallet->CanAfford(Offering.CopperCost))
	{
		return EEmbermereTrainingResult::InsufficientFunds;
	}

	if (!Stats->CanAddExperience(Offering.ExperienceReward))
	{
		return EEmbermereTrainingResult::ProgressionCap;
	}

	return EEmbermereTrainingResult::Success;
}

EEmbermereTrainingResult UEmbermereTrainerComponent::TryTrain(
	int32 OfferingIndex,
	UEmbermereStatsComponent* Stats,
	UEmbermereWalletComponent* Wallet)
{
	const EEmbermereTrainingResult Preflight = CanTrain(OfferingIndex, Stats, Wallet);
	if (Preflight != EEmbermereTrainingResult::Success)
	{
		return Preflight;
	}

	FEmbermereTrainerOffering Offering;
	GetOffering(OfferingIndex, Offering);
	if (!Wallet->TrySpendCopper(Offering.CopperCost))
	{
		return EEmbermereTrainingResult::InsufficientFunds;
	}

	if (!Stats->TryAddExperience(Offering.ExperienceReward))
	{
		Wallet->AddCopper(Offering.CopperCost);
		return EEmbermereTrainingResult::ProgressionCap;
	}

	if (!Offering.bRepeatable)
	{
		CompletedNonRepeatableOfferingIds.Add(Offering.OfferingId);
	}

	OnTrainingCompleted.Broadcast();
	return EEmbermereTrainingResult::Success;
}

FText UEmbermereTrainerComponent::GetTrainingResultText(
	EEmbermereTrainingResult Result,
	int32 OfferingIndex) const
{
	FEmbermereTrainerOffering Offering;
	const FString OfferingName = GetOffering(OfferingIndex, Offering)
		? Offering.DisplayName.ToString()
		: TEXT("training");

	switch (Result)
	{
	case EEmbermereTrainingResult::Success:
		return FText::FromString(FString::Printf(
			TEXT("Completed %s for %d copper and gained %d XP."),
			*OfferingName,
			Offering.CopperCost,
			Offering.ExperienceReward));
	case EEmbermereTrainingResult::LevelTooLow:
		return FText::FromString(FString::Printf(
			TEXT("%s requires level %d."),
			*OfferingName,
			Offering.RequiredLevel));
	case EEmbermereTrainingResult::InsufficientFunds:
		return FText::FromString(TEXT("You do not have enough copper for that training."));
	case EEmbermereTrainingResult::ProgressionCap:
		return FText::FromString(TEXT("That training cannot be applied to your current progression."));
	case EEmbermereTrainingResult::AlreadyCompleted:
		return FText::FromString(FString::Printf(
			TEXT("%s has already been completed."),
			*OfferingName));
	default:
		return FText::FromString(TEXT("That training is unavailable."));
	}
}
