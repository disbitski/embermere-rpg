#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/EmbermereQuestData.h"
#include "EmbermereQuestLogComponent.generated.h"

USTRUCT(BlueprintType)
struct FEmbermereQuestState
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TObjectPtr<UEmbermereQuestData> Quest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	int32 CurrentObjectiveCount = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	bool bCompleted = false;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEmbermereQuestStateChangedSignature, const FEmbermereQuestState&, QuestState);

enum class EEmbermereQuestUpdateKind : uint8
{
	Accepted,
	Progress,
	Ready,
	Completed
};

// Value-only snapshot: observers never retain pointers into the mutable ledger.
struct FEmbermereQuestUpdate
{
	uint64 Sequence = 0;
	FName QuestId;
	FName ObjectiveId;
	FText Title;
	FText Instructions;
	int32 PreviousCount = 0;
	int32 CurrentCount = 0;
	int32 RequiredCount = 0;
	EEmbermereQuestUpdateKind Kind = EEmbermereQuestUpdateKind::Accepted;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FEmbermereQuestUpdateSignature, const FEmbermereQuestUpdate&);
DECLARE_MULTICAST_DELEGATE(FEmbermereQuestPresentationResetSignature);

UENUM(BlueprintType)
enum class EEmbermereQuestAcceptanceResult : uint8
{
	Success,
	InvalidQuest,
	AlreadyTracked,
	OccupiedByOtherQuest,
	LedgerFull
};

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereQuestLogComponent();
	static constexpr int32 MaxTrackedQuests = 8;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	TArray<FEmbermereQuestState> QuestStates;

	// Compatibility projection for existing Blueprint and HUD readers. The
	// keyed QuestStates ledger remains the only mutable quest authority.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	FEmbermereQuestState ActiveQuest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Transient, Category = "Quest")
	FName FocusedQuestId = NAME_None;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereQuestStateChangedSignature OnQuestStateChanged;

	FEmbermereQuestUpdateSignature OnLiveQuestUpdate;
	FEmbermereQuestPresentationResetSignature OnPresentationReset;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool AcceptQuest(UEmbermereQuestData* Quest);

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	EEmbermereQuestAcceptanceResult EvaluateQuestAcceptance(UEmbermereQuestData* Quest) const;

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FText GetQuestAcceptanceResultText(
		EEmbermereQuestAcceptanceResult Result,
		UEmbermereQuestData* Quest) const;

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	bool GetQuestStateById(FName QuestId, FEmbermereQuestState& OutQuestState) const;

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	bool IsQuestTracked(FName QuestId) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool FocusQuest(FName QuestId);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool AddObjectiveProgress(FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool AddObjectiveProgressForQuest(FName QuestId, FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool TryCompleteActiveQuest();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool TryCompleteQuest(UEmbermereQuestData* Quest);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool TryCompleteQuestById(FName QuestId);

	bool CanRestoreQuestStateForSaveGame(const FEmbermereQuestState& NewState) const;
	void RestoreQuestStateForSaveGame(const FEmbermereQuestState& NewState);
	bool CanRestoreQuestStatesForSaveGame(const TArray<FEmbermereQuestState>& NewStates) const;
	void RestoreQuestStatesForSaveGame(const TArray<FEmbermereQuestState>& NewStates);
	const TArray<FEmbermereQuestState>& GetQuestStatesForSaveGame() const;

private:
	bool bMutationInProgress = false;
	uint64 LiveUpdateSequence = 0;
	void PublishLiveUpdate(const FEmbermereQuestState& State,
		EEmbermereQuestUpdateKind Kind, int32 PreviousCount);
	int32 FindQuestStateIndex(FName QuestId) const;
	bool IsQuestDataValid(const UEmbermereQuestData* Quest) const;
	void RefreshActiveQuestProjection();
	void BroadcastQuestState(const FEmbermereQuestState& QuestState);
};
