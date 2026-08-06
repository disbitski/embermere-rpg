#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Engine/Engine.h"
#include "UI/EmbermereGameplayMessageLibrary.h"

UEmbermereQuestLogComponent::UEmbermereQuestLogComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UEmbermereQuestLogComponent::AcceptQuest(UEmbermereQuestData* Quest)
{
	if (!Quest || ActiveQuest.Quest)
	{
		return false;
	}

	ActiveQuest.Quest = Quest;
	ActiveQuest.CurrentObjectiveCount = 0;
	ActiveQuest.bCompleted = false;
	OnQuestStateChanged.Broadcast(ActiveQuest);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Quest accepted: %s"), *Quest->Title.ToString())),
		FLinearColor(1.0f, 0.86f, 0.22f, 1.0f));
	return true;
}

bool UEmbermereQuestLogComponent::AddObjectiveProgress(FName ObjectiveId, int32 Amount)
{
	if (!ActiveQuest.Quest || ActiveQuest.bCompleted || Amount <= 0 || ActiveQuest.Quest->ObjectiveId != ObjectiveId)
	{
		return false;
	}

	ActiveQuest.CurrentObjectiveCount = FMath::Clamp(
		ActiveQuest.CurrentObjectiveCount + Amount,
		0,
		ActiveQuest.Quest->RequiredObjectiveCount);
	OnQuestStateChanged.Broadcast(ActiveQuest);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(
			TEXT("%s: %d/%d"),
			*ActiveQuest.Quest->Title.ToString(),
			ActiveQuest.CurrentObjectiveCount,
			ActiveQuest.Quest->RequiredObjectiveCount)),
		FLinearColor(0.46f, 0.95f, 1.0f, 1.0f));
	return true;
}

bool UEmbermereQuestLogComponent::TryCompleteActiveQuest()
{
	if (!ActiveQuest.Quest || ActiveQuest.bCompleted ||
		ActiveQuest.CurrentObjectiveCount < ActiveQuest.Quest->RequiredObjectiveCount)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	if (Owner)
	{
		if (UEmbermereStatsComponent* Stats = Owner->FindComponentByClass<UEmbermereStatsComponent>())
		{
			Stats->AddExperience(ActiveQuest.Quest->RewardExperience);
		}

		if (ActiveQuest.Quest->RewardCopper > 0)
		{
			if (UEmbermereWalletComponent* Wallet = Owner->FindComponentByClass<UEmbermereWalletComponent>())
			{
				if (Wallet->AddCopper(ActiveQuest.Quest->RewardCopper))
				{
					UEmbermereGameplayMessageLibrary::PostGameplayMessage(
						this,
						FText::FromString(FString::Printf(
							TEXT("Reward: %d copper"),
							ActiveQuest.Quest->RewardCopper)),
						FLinearColor(1.0f, 0.82f, 0.38f, 1.0f));
				}
			}
		}

		if (UEmbermereInventoryComponent* Inventory = Owner->FindComponentByClass<UEmbermereInventoryComponent>())
		{
			if (UEmbermereItemData* RewardItem = ActiveQuest.Quest->RewardItem.LoadSynchronous())
			{
				Inventory->AddItem(RewardItem, 1);
			}
		}
	}

	ActiveQuest.bCompleted = true;
	OnQuestStateChanged.Broadcast(ActiveQuest);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Quest complete: %s"), *ActiveQuest.Quest->Title.ToString())),
		FLinearColor(0.42f, 1.0f, 0.48f, 1.0f));
	return true;
}

bool UEmbermereQuestLogComponent::CanRestoreQuestStateForSaveGame(
	const FEmbermereQuestState& NewState) const
{
	if (!NewState.Quest)
	{
		return NewState.CurrentObjectiveCount == 0 && !NewState.bCompleted;
	}

	if (NewState.Quest->RequiredObjectiveCount <= 0 ||
		NewState.CurrentObjectiveCount < 0 ||
		NewState.CurrentObjectiveCount > NewState.Quest->RequiredObjectiveCount)
	{
		return false;
	}
	return !NewState.bCompleted ||
		NewState.CurrentObjectiveCount == NewState.Quest->RequiredObjectiveCount;
}

void UEmbermereQuestLogComponent::RestoreQuestStateForSaveGame(
	const FEmbermereQuestState& NewState)
{
	ActiveQuest = NewState;
	OnQuestStateChanged.Broadcast(ActiveQuest);
}
