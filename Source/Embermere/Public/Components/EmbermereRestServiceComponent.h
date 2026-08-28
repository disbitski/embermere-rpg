#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EmbermereRestServiceComponent.generated.h"

class AEmbermereCharacter;
class UEmbermereRestServiceData;

UENUM(BlueprintType)
enum class EEmbermereRestResult : uint8
{
	Started,
	Success,
	InvalidRequest,
	OutOfRange,
	Dead,
	InCombat,
	ResourcesFull,
	Cooldown,
	AlreadyResting,
	Interrupted
};

USTRUCT(BlueprintType)
struct FEmbermereRestOutcome
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Rest")
	EEmbermereRestResult Result = EEmbermereRestResult::InvalidRequest;

	UPROPERTY(BlueprintReadOnly, Category = "Rest")
	TObjectPtr<AEmbermereCharacter> Character = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Rest")
	float HealthRestored = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Rest")
	float ManaRestored = 0.0f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FEmbermereRestOutcomeSignature,
	FEmbermereRestOutcome,
	Outcome);

UCLASS(ClassGroup = (Embermere), meta = (BlueprintSpawnableComponent))
class EMBERMERE_API UEmbermereRestServiceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEmbermereRestServiceComponent();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rest")
	TObjectPtr<UEmbermereRestServiceData> RestData;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereRestOutcomeSignature OnRestOutcome;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Rest")
	void SetRestData(UEmbermereRestServiceData* NewRestData);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rest")
	EEmbermereRestResult CanBeginRest(const AEmbermereCharacter* Character) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Rest")
	EEmbermereRestResult TryBeginRest(AEmbermereCharacter* Character);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rest")
	FText GetRestResultText(const FEmbermereRestOutcome& Outcome) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rest")
	bool IsRestPending() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rest")
	float GetCooldownRemainingSeconds() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Rest")
	FEmbermereRestOutcome GetLastOutcome() const;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	TWeakObjectPtr<AEmbermereCharacter> PendingCharacter;
	FVector PendingStartLocation = FVector::ZeroVector;
	float PendingElapsedSeconds = 0.0f;
	float CooldownRemainingSeconds = 0.0f;
	bool bRestPending = false;
	FEmbermereRestOutcome LastOutcome;

	EEmbermereRestResult EvaluateCharacterState(
		const AEmbermereCharacter* Character,
		bool bCheckResources) const;
	bool IsCharacterInCombat(const AEmbermereCharacter* Character) const;
	void AdvanceRest(float DeltaSeconds);
	void ResolvePendingRest();
	void ClearPendingRest();
	void ResetTransientState();
	void PublishOutcome(
		EEmbermereRestResult Result,
		AEmbermereCharacter* Character,
		float HealthRestored = 0.0f,
		float ManaRestored = 0.0f);

#if WITH_DEV_AUTOMATION_TESTS
	friend class FEmbermereRestRecoveryTransactionsTest;
	friend class FEmbermereRestInterruptionAndCombatTest;
#endif
};
