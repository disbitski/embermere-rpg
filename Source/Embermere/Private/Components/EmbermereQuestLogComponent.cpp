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
	if (bMutationInProgress || EvaluateQuestAcceptance(Quest) != EEmbermereQuestAcceptanceResult::Success)
	{
		return false;
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	FEmbermereQuestState& NewState = QuestStates.AddDefaulted_GetRef();
	NewState.Quest = Quest;
	FocusedQuestId = Quest->QuestId;
	RefreshActiveQuestProjection();
	BroadcastQuestState(NewState);
	PublishLiveUpdate(NewState, EEmbermereQuestUpdateKind::Accepted, 0);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Quest accepted: %s"), *Quest->Title.ToString())),
		FLinearColor(1.0f, 0.86f, 0.22f, 1.0f));
	return true;
}

EEmbermereQuestAcceptanceResult UEmbermereQuestLogComponent::EvaluateQuestAcceptance(
	UEmbermereQuestData* Quest) const
{
	if (!IsQuestDataValid(Quest))
	{
		return EEmbermereQuestAcceptanceResult::InvalidQuest;
	}
	if (FindQuestStateIndex(Quest->QuestId) != INDEX_NONE)
	{
		return EEmbermereQuestAcceptanceResult::AlreadyTracked;
	}
	return QuestStates.Num() < MaxTrackedQuests
		? EEmbermereQuestAcceptanceResult::Success
		: EEmbermereQuestAcceptanceResult::LedgerFull;
}

FText UEmbermereQuestLogComponent::GetQuestAcceptanceResultText(
	EEmbermereQuestAcceptanceResult Result,
	UEmbermereQuestData* Quest) const
{
	switch (Result)
	{
	case EEmbermereQuestAcceptanceResult::InvalidQuest:
		return FText::FromString(TEXT("That quest is unavailable."));
	case EEmbermereQuestAcceptanceResult::AlreadyTracked:
		return FText::FromString(FString::Printf(
			TEXT("Quest already tracked: %s"),
			Quest ? *Quest->Title.ToString() : TEXT("Unknown Quest")));
	case EEmbermereQuestAcceptanceResult::OccupiedByOtherQuest:
		return FText::FromString(FString::Printf(
			TEXT("Finish your current quest before accepting %s."),
			Quest ? *Quest->Title.ToString() : TEXT("another quest")));
	case EEmbermereQuestAcceptanceResult::LedgerFull:
		return FText::FromString(TEXT("Your quest ledger is full."));
	case EEmbermereQuestAcceptanceResult::Success:
	default:
		return FText::GetEmpty();
	}
}

bool UEmbermereQuestLogComponent::GetQuestStateById(
	FName QuestId,
	FEmbermereQuestState& OutQuestState) const
{
	OutQuestState = FEmbermereQuestState();
	const int32 QuestIndex = FindQuestStateIndex(QuestId);
	if (!QuestStates.IsValidIndex(QuestIndex))
	{
		return false;
	}
	OutQuestState = QuestStates[QuestIndex];
	return true;
}

bool UEmbermereQuestLogComponent::IsQuestTracked(FName QuestId) const
{
	return FindQuestStateIndex(QuestId) != INDEX_NONE;
}

bool UEmbermereQuestLogComponent::FocusQuest(FName QuestId)
{
	if (FindQuestStateIndex(QuestId) == INDEX_NONE)
	{
		return false;
	}
	FocusedQuestId = QuestId;
	RefreshActiveQuestProjection();
	return true;
}

bool UEmbermereQuestLogComponent::AddObjectiveProgress(FName ObjectiveId, int32 Amount)
{
	if (ObjectiveId.IsNone() || Amount <= 0)
	{
		return false;
	}

	FName MatchingQuestId = NAME_None;
	for (const FEmbermereQuestState& QuestState : QuestStates)
	{
		if (QuestState.Quest && !QuestState.bCompleted &&
			QuestState.Quest->ObjectiveId == ObjectiveId)
		{
			if (!MatchingQuestId.IsNone())
			{
				return false;
			}
			MatchingQuestId = QuestState.Quest->QuestId;
		}
	}
	return !MatchingQuestId.IsNone() &&
		AddObjectiveProgressForQuest(MatchingQuestId, ObjectiveId, Amount);
}

bool UEmbermereQuestLogComponent::AddObjectiveProgressForQuest(
	FName QuestId,
	FName ObjectiveId,
	int32 Amount)
{
	const int32 QuestIndex = FindQuestStateIndex(QuestId);
	if (bMutationInProgress || !QuestStates.IsValidIndex(QuestIndex) || ObjectiveId.IsNone() || Amount <= 0)
	{
		return false;
	}

	FEmbermereQuestState& QuestState = QuestStates[QuestIndex];
	if (!QuestState.Quest || QuestState.bCompleted ||
		QuestState.Quest->ObjectiveId != ObjectiveId ||
		QuestState.CurrentObjectiveCount >= QuestState.Quest->RequiredObjectiveCount)
	{
		return false;
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	const int32 PreviousCount = QuestState.CurrentObjectiveCount;
	const int32 Remaining = QuestState.Quest->RequiredObjectiveCount - PreviousCount;
	QuestState.CurrentObjectiveCount += FMath::Min(Amount, Remaining);
	FocusedQuestId = QuestId;
	RefreshActiveQuestProjection();
	BroadcastQuestState(QuestState);
	PublishLiveUpdate(QuestState,
		QuestState.CurrentObjectiveCount == QuestState.Quest->RequiredObjectiveCount
			? EEmbermereQuestUpdateKind::Ready : EEmbermereQuestUpdateKind::Progress,
		PreviousCount);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(
			TEXT("%s: %d/%d"),
			*QuestState.Quest->Title.ToString(),
			QuestState.CurrentObjectiveCount,
			QuestState.Quest->RequiredObjectiveCount)),
		FLinearColor(0.46f, 0.95f, 1.0f, 1.0f));
	return true;
}

bool UEmbermereQuestLogComponent::TryCompleteActiveQuest()
{
	return !FocusedQuestId.IsNone() && TryCompleteQuestById(FocusedQuestId);
}

bool UEmbermereQuestLogComponent::TryCompleteQuestById(FName QuestId)
{
	if (bMutationInProgress)
	{
		return false;
	}
	const int32 QuestIndex = FindQuestStateIndex(QuestId);
	if (!QuestStates.IsValidIndex(QuestIndex))
	{
		return false;
	}
	FEmbermereQuestState& QuestState = QuestStates[QuestIndex];
	UEmbermereQuestData* Quest = QuestState.Quest;
	if (!Quest || QuestState.bCompleted ||
		QuestState.CurrentObjectiveCount < Quest->RequiredObjectiveCount)
	{
		return false;
	}

	AActor* Owner = GetOwner();
	UEmbermereStatsComponent* Stats = Owner
		? Owner->FindComponentByClass<UEmbermereStatsComponent>()
		: nullptr;
	UEmbermereWalletComponent* Wallet = Owner
		? Owner->FindComponentByClass<UEmbermereWalletComponent>()
		: nullptr;
	UEmbermereInventoryComponent* Inventory = Owner
		? Owner->FindComponentByClass<UEmbermereInventoryComponent>()
		: nullptr;
	UEmbermereItemData* RewardItem = Quest->RewardItem.IsNull()
		? nullptr
		: Quest->RewardItem.LoadSynchronous();
	if ((Quest->RewardExperience > 0 && (!Stats || !Stats->CanAddExperience(Quest->RewardExperience))) ||
		(Quest->RewardCopper > 0 && (!Wallet || !Wallet->CanAddCopper(Quest->RewardCopper))) ||
		(!Quest->RewardItem.IsNull() && (!RewardItem || !Inventory || !Inventory->CanAddItem(RewardItem, 1))))
	{
		return false;
	}

	// Mark completion before reward delegates fire so a reentrant turn-in cannot
	// grant the same reward twice. All fallible reward operations were preflighted.
	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	QuestState.bCompleted = true;
	if (RewardItem)
	{
		Inventory->AddItem(RewardItem, 1);
	}
	if (Quest->RewardCopper > 0)
	{
		Wallet->AddCopper(Quest->RewardCopper);
		UEmbermereGameplayMessageLibrary::PostGameplayMessage(
			this,
			FText::FromString(FString::Printf(
				TEXT("Reward: %d copper"),
				Quest->RewardCopper)),
			FLinearColor(1.0f, 0.82f, 0.38f, 1.0f));
	}
	if (Quest->RewardExperience > 0)
	{
		Stats->TryAddExperience(Quest->RewardExperience);
	}

	FocusedQuestId = QuestId;
	RefreshActiveQuestProjection();
	BroadcastQuestState(QuestState);
	PublishLiveUpdate(QuestState, EEmbermereQuestUpdateKind::Completed, QuestState.CurrentObjectiveCount);
	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		FText::FromString(FString::Printf(TEXT("Quest complete: %s"), *Quest->Title.ToString())),
		FLinearColor(0.42f, 1.0f, 0.48f, 1.0f));
	return true;
}

bool UEmbermereQuestLogComponent::TryCompleteQuest(UEmbermereQuestData* Quest)
{
	if (!Quest)
	{
		return false;
	}
	const int32 QuestIndex = FindQuestStateIndex(Quest->QuestId);
	return QuestStates.IsValidIndex(QuestIndex) &&
		QuestStates[QuestIndex].Quest == Quest &&
		TryCompleteQuestById(Quest->QuestId);
}

bool UEmbermereQuestLogComponent::CanRestoreQuestStateForSaveGame(
	const FEmbermereQuestState& NewState) const
{
	if (!NewState.Quest)
	{
		return NewState.CurrentObjectiveCount == 0 && !NewState.bCompleted;
	}
	return CanRestoreQuestStatesForSaveGame(
		TArray<FEmbermereQuestState>{NewState});
}

void UEmbermereQuestLogComponent::RestoreQuestStateForSaveGame(
	const FEmbermereQuestState& NewState)
{
	if (!CanRestoreQuestStateForSaveGame(NewState))
	{
		return;
	}
	RestoreQuestStatesForSaveGame(
		NewState.Quest ? TArray<FEmbermereQuestState>{NewState} : TArray<FEmbermereQuestState>());
}

bool UEmbermereQuestLogComponent::CanRestoreQuestStatesForSaveGame(
	const TArray<FEmbermereQuestState>& NewStates) const
{
	if (bMutationInProgress || NewStates.Num() > MaxTrackedQuests)
	{
		return false;
	}

	TSet<FName> SeenQuestIds;
	for (const FEmbermereQuestState& NewState : NewStates)
	{
		if (!IsQuestDataValid(NewState.Quest) ||
			SeenQuestIds.Contains(NewState.Quest->QuestId) ||
			NewState.CurrentObjectiveCount < 0 ||
			NewState.CurrentObjectiveCount > NewState.Quest->RequiredObjectiveCount ||
			(NewState.bCompleted &&
				NewState.CurrentObjectiveCount != NewState.Quest->RequiredObjectiveCount))
		{
			return false;
		}
		SeenQuestIds.Add(NewState.Quest->QuestId);
	}
	return true;
}

void UEmbermereQuestLogComponent::RestoreQuestStatesForSaveGame(
	const TArray<FEmbermereQuestState>& NewStates)
{
	if (!CanRestoreQuestStatesForSaveGame(NewStates))
	{
		return;
	}

	TGuardValue<bool> MutationGuard(bMutationInProgress, true);
	QuestStates = NewStates;
	FocusedQuestId = NAME_None;
	for (const FEmbermereQuestState& QuestState : QuestStates)
	{
		if (!QuestState.bCompleted)
		{
			FocusedQuestId = QuestState.Quest->QuestId;
			break;
		}
	}
	if (FocusedQuestId.IsNone() && !QuestStates.IsEmpty())
	{
		FocusedQuestId = QuestStates[0].Quest->QuestId;
	}
	RefreshActiveQuestProjection();
	OnPresentationReset.Broadcast();
	OnQuestStateChanged.Broadcast(ActiveQuest);
}

const TArray<FEmbermereQuestState>& UEmbermereQuestLogComponent::GetQuestStatesForSaveGame() const
{
	return QuestStates;
}

int32 UEmbermereQuestLogComponent::FindQuestStateIndex(FName QuestId) const
{
	if (QuestId.IsNone())
	{
		return INDEX_NONE;
	}
	return QuestStates.IndexOfByPredicate([QuestId](const FEmbermereQuestState& QuestState)
	{
		return QuestState.Quest && QuestState.Quest->QuestId == QuestId;
	});
}

bool UEmbermereQuestLogComponent::IsQuestDataValid(const UEmbermereQuestData* Quest) const
{
	return Quest && !Quest->QuestId.IsNone() && !Quest->ObjectiveId.IsNone() &&
		Quest->RequiredObjectiveCount > 0 && Quest->RewardExperience >= 0 &&
		Quest->RewardCopper >= 0;
}

void UEmbermereQuestLogComponent::RefreshActiveQuestProjection()
{
	ActiveQuest = FEmbermereQuestState();
	const int32 QuestIndex = FindQuestStateIndex(FocusedQuestId);
	if (QuestStates.IsValidIndex(QuestIndex))
	{
		ActiveQuest = QuestStates[QuestIndex];
	}
	else
	{
		FocusedQuestId = NAME_None;
	}
}

void UEmbermereQuestLogComponent::BroadcastQuestState(const FEmbermereQuestState& QuestState)
{
	const FEmbermereQuestState Snapshot = QuestState;
	OnQuestStateChanged.Broadcast(Snapshot);
}

void UEmbermereQuestLogComponent::PublishLiveUpdate(
	const FEmbermereQuestState& State, EEmbermereQuestUpdateKind Kind, int32 PreviousCount)
{
	FEmbermereQuestUpdate Update;
	Update.Sequence = ++LiveUpdateSequence;
	Update.QuestId = State.Quest->QuestId;
	Update.ObjectiveId = State.Quest->ObjectiveId;
	Update.Title = State.Quest->Title;
	Update.Instructions = State.Quest->ObjectiveInstructions;
	Update.PreviousCount = PreviousCount;
	Update.CurrentCount = State.CurrentObjectiveCount;
	Update.RequiredCount = State.Quest->RequiredObjectiveCount;
	Update.Kind = Kind;
	OnLiveQuestUpdate.Broadcast(Update);
}
