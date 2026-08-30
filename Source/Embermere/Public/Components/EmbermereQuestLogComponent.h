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

UENUM(BlueprintType)
enum class EEmbermereQuestAcceptanceResult : uint8
{
	Success,
	InvalidQuest,
	AlreadyTracked,
	OccupiedByOtherQuest
};

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereQuestLogComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereQuestLogComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Quest")
	FEmbermereQuestState ActiveQuest;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereQuestStateChangedSignature OnQuestStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool AcceptQuest(UEmbermereQuestData* Quest);

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	EEmbermereQuestAcceptanceResult EvaluateQuestAcceptance(UEmbermereQuestData* Quest) const;

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FText GetQuestAcceptanceResultText(
		EEmbermereQuestAcceptanceResult Result,
		UEmbermereQuestData* Quest) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool AddObjectiveProgress(FName ObjectiveId, int32 Amount = 1);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool TryCompleteActiveQuest();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Quest")
	bool TryCompleteQuest(UEmbermereQuestData* Quest);

	bool CanRestoreQuestStateForSaveGame(const FEmbermereQuestState& NewState) const;
	void RestoreQuestStateForSaveGame(const FEmbermereQuestState& NewState);
};
