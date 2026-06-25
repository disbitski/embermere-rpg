#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Engine/Engine.h"

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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			4.0f,
			FColor::Yellow,
			FString::Printf(TEXT("Quest accepted: %s"), *Quest->Title.ToString()));
	}
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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Cyan,
			FString::Printf(
				TEXT("%s: %d/%d"),
				*ActiveQuest.Quest->Title.ToString(),
				ActiveQuest.CurrentObjectiveCount,
				ActiveQuest.Quest->RequiredObjectiveCount));
	}
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
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::Green,
			FString::Printf(TEXT("Quest complete: %s"), *ActiveQuest.Quest->Title.ToString()));
	}
	return true;
}
