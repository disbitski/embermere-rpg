#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"

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
	return true;
}
