#include "Components/EmbermereRestServiceComponent.h"

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Data/EmbermereRestServiceData.h"
#include "EngineUtils.h"

UEmbermereRestServiceComponent::UEmbermereRestServiceComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UEmbermereRestServiceComponent::SetRestData(UEmbermereRestServiceData* NewRestData)
{
	ClearPendingRest();
	CooldownRemainingSeconds = 0.0f;
	RestData = NewRestData;
}

EEmbermereRestResult UEmbermereRestServiceComponent::CanBeginRest(
	const AEmbermereCharacter* Character) const
{
	if (bRestPending)
	{
		return EEmbermereRestResult::AlreadyResting;
	}

	if (CooldownRemainingSeconds > KINDA_SMALL_NUMBER)
	{
		return EEmbermereRestResult::Cooldown;
	}

	return EvaluateCharacterState(Character, true);
}

EEmbermereRestResult UEmbermereRestServiceComponent::TryBeginRest(
	AEmbermereCharacter* Character)
{
	const EEmbermereRestResult Preflight = CanBeginRest(Character);
	if (Preflight != EEmbermereRestResult::Started)
	{
		PublishOutcome(Preflight, Character);
		return Preflight;
	}

	PendingCharacter = Character;
	PendingStartLocation = Character->GetActorLocation();
	PendingElapsedSeconds = 0.0f;
	bRestPending = true;
	PublishOutcome(EEmbermereRestResult::Started, Character);
	return EEmbermereRestResult::Started;
}

FText UEmbermereRestServiceComponent::GetRestResultText(
	const FEmbermereRestOutcome& Outcome) const
{
	const FString ServiceName = RestData && !RestData->DisplayName.IsEmpty()
		? RestData->DisplayName.ToString()
		: TEXT("rest service");
	auto FormatAmount = [](float Amount)
	{
		const float RoundedAmount = FMath::RoundToFloat(Amount);
		return FMath::IsNearlyEqual(Amount, RoundedAmount, 0.01f)
			? FString::Printf(TEXT("%.0f"), Amount)
			: FString::Printf(TEXT("%.1f"), Amount);
	};

	switch (Outcome.Result)
	{
	case EEmbermereRestResult::Started:
		return RestData && !RestData->RestingText.IsEmpty()
			? RestData->RestingText
			: FText::FromString(TEXT("Resting. Remain still."));
	case EEmbermereRestResult::Success:
		if (Outcome.HealthRestored > 0.0f && Outcome.ManaRestored > 0.0f)
		{
			return FText::FromString(FString::Printf(
				TEXT("%s restored %s Health and %s Mana."),
				*ServiceName,
				*FormatAmount(Outcome.HealthRestored),
				*FormatAmount(Outcome.ManaRestored)));
		}
		if (Outcome.HealthRestored > 0.0f)
		{
			return FText::FromString(FString::Printf(
				TEXT("%s restored %s Health."),
				*ServiceName,
				*FormatAmount(Outcome.HealthRestored)));
		}
		return FText::FromString(FString::Printf(
			TEXT("%s restored %s Mana."),
			*ServiceName,
			*FormatAmount(Outcome.ManaRestored)));
	case EEmbermereRestResult::OutOfRange:
		return FText::FromString(FString::Printf(TEXT("Move closer to %s."), *ServiceName));
	case EEmbermereRestResult::Dead:
		return FText::FromString(TEXT("The fallen cannot rest here."));
	case EEmbermereRestResult::InCombat:
		return FText::FromString(TEXT("You cannot rest while an enemy is engaged."));
	case EEmbermereRestResult::ResourcesFull:
		return FText::FromString(TEXT("Health and mana are already full."));
	case EEmbermereRestResult::Cooldown:
		return FText::FromString(FString::Printf(
			TEXT("%s will settle in %d seconds."),
			*ServiceName,
			FMath::Max(1, FMath::CeilToInt(CooldownRemainingSeconds))));
	case EEmbermereRestResult::AlreadyResting:
		return FText::FromString(TEXT("You are already resting."));
	case EEmbermereRestResult::Interrupted:
		return FText::FromString(TEXT("Rest interrupted."));
	default:
		return FText::FromString(TEXT("That place cannot restore you right now."));
	}
}

bool UEmbermereRestServiceComponent::IsRestPending() const
{
	return bRestPending && PendingCharacter.IsValid();
}

float UEmbermereRestServiceComponent::GetCooldownRemainingSeconds() const
{
	return FMath::Max(0.0f, CooldownRemainingSeconds);
}

FEmbermereRestOutcome UEmbermereRestServiceComponent::GetLastOutcome() const
{
	return LastOutcome;
}

void UEmbermereRestServiceComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	AdvanceRest(DeltaTime);
}

void UEmbermereRestServiceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetTransientState();
	Super::EndPlay(EndPlayReason);
}

EEmbermereRestResult UEmbermereRestServiceComponent::EvaluateCharacterState(
	const AEmbermereCharacter* Character,
	bool bCheckResources) const
{
	if (!GetOwner() || !Character || !RestData || !RestData->HasValidDefinition() ||
		!Character->Stats || !Character->Stats->HasValidVitalState())
	{
		return EEmbermereRestResult::InvalidRequest;
	}

	if (Character->Stats->IsDead())
	{
		return EEmbermereRestResult::Dead;
	}

	if (IsCharacterInCombat(Character))
	{
		return EEmbermereRestResult::InCombat;
	}

	if (FVector::DistSquared(GetOwner()->GetActorLocation(), Character->GetActorLocation()) >
		FMath::Square(RestData->InteractionRadius))
	{
		return EEmbermereRestResult::OutOfRange;
	}

	if (bCheckResources && !Character->Stats->NeedsVitalRecovery(
		RestData->bRestoreHealth,
		RestData->bRestoreMana))
	{
		return EEmbermereRestResult::ResourcesFull;
	}

	return EEmbermereRestResult::Started;
}

bool UEmbermereRestServiceComponent::IsCharacterInCombat(
	const AEmbermereCharacter* Character) const
{
	if (!Character)
	{
		return false;
	}

	if (Character->Combat)
	{
		if (const AEmbermereEnemyCharacter* SelectedEnemy =
			Cast<AEmbermereEnemyCharacter>(Character->Combat->CurrentTarget.Get()))
		{
			const bool bLivePrototypeEnemy = SelectedEnemy->bPrototypeAiEnabled &&
				SelectedEnemy->Stats && !SelectedEnemy->Stats->IsDead() && !SelectedEnemy->IsHidden();
			const bool bInsideEngagementRange = FVector::DistSquared(
				SelectedEnemy->GetActorLocation(),
				Character->GetActorLocation()) <= FMath::Square(SelectedEnemy->AggroRadius * 1.35f);
			if (bLivePrototypeEnemy &&
				(SelectedEnemy->IsActivelyEngagedWith(Character) || bInsideEngagementRange))
			{
				return true;
			}
		}
	}

	UWorld* World = Character->GetWorld();
	if (!World)
	{
		return false;
	}

	for (TActorIterator<AEmbermereEnemyCharacter> It(World); It; ++It)
	{
		if (It->IsActivelyEngagedWith(Character))
		{
			return true;
		}
	}
	return false;
}

void UEmbermereRestServiceComponent::AdvanceRest(float DeltaSeconds)
{
	if (!FMath::IsFinite(DeltaSeconds) || DeltaSeconds <= 0.0f)
	{
		return;
	}

	CooldownRemainingSeconds = FMath::Max(0.0f, CooldownRemainingSeconds - DeltaSeconds);
	if (!bRestPending)
	{
		return;
	}

	AEmbermereCharacter* Character = PendingCharacter.Get();
	if (!Character)
	{
		ClearPendingRest();
		PublishOutcome(EEmbermereRestResult::Interrupted, nullptr);
		return;
	}

	const EEmbermereRestResult CurrentState = EvaluateCharacterState(Character, false);
	if (CurrentState != EEmbermereRestResult::Started)
	{
		ClearPendingRest();
		PublishOutcome(CurrentState, Character);
		return;
	}

	if (FVector::DistSquared(Character->GetActorLocation(), PendingStartLocation) >
		FMath::Square(RestData->MovementInterruptDistance))
	{
		ClearPendingRest();
		PublishOutcome(EEmbermereRestResult::Interrupted, Character);
		return;
	}

	PendingElapsedSeconds += DeltaSeconds;
	if (PendingElapsedSeconds >= RestData->ChannelDurationSeconds)
	{
		ResolvePendingRest();
	}
}

void UEmbermereRestServiceComponent::ResolvePendingRest()
{
	AEmbermereCharacter* Character = PendingCharacter.Get();
	const EEmbermereRestResult CurrentState = EvaluateCharacterState(Character, false);
	if (CurrentState != EEmbermereRestResult::Started)
	{
		ClearPendingRest();
		PublishOutcome(CurrentState, Character);
		return;
	}

	if (FVector::DistSquared(Character->GetActorLocation(), PendingStartLocation) >
		FMath::Square(RestData->MovementInterruptDistance))
	{
		ClearPendingRest();
		PublishOutcome(EEmbermereRestResult::Interrupted, Character);
		return;
	}

	float HealthRestored = 0.0f;
	float ManaRestored = 0.0f;
	if (!Character->Stats->TryRestoreVitalsToFull(
		RestData->bRestoreHealth,
		RestData->bRestoreMana,
		HealthRestored,
		ManaRestored))
	{
		const EEmbermereRestResult Failure = Character->Stats->HasValidVitalState()
			? EEmbermereRestResult::ResourcesFull
			: EEmbermereRestResult::InvalidRequest;
		ClearPendingRest();
		PublishOutcome(Failure, Character);
		return;
	}

	const float NewCooldownSeconds = RestData->CooldownSeconds;
	ClearPendingRest();
	CooldownRemainingSeconds = NewCooldownSeconds;
	PublishOutcome(
		EEmbermereRestResult::Success,
		Character,
		HealthRestored,
		ManaRestored);
}

void UEmbermereRestServiceComponent::ClearPendingRest()
{
	PendingCharacter.Reset();
	PendingStartLocation = FVector::ZeroVector;
	PendingElapsedSeconds = 0.0f;
	bRestPending = false;
}

void UEmbermereRestServiceComponent::ResetTransientState()
{
	ClearPendingRest();
	CooldownRemainingSeconds = 0.0f;
}

void UEmbermereRestServiceComponent::PublishOutcome(
	EEmbermereRestResult Result,
	AEmbermereCharacter* Character,
	float HealthRestored,
	float ManaRestored)
{
	FEmbermereRestOutcome Outcome;
	Outcome.Result = Result;
	Outcome.Character = Character;
	Outcome.HealthRestored = HealthRestored;
	Outcome.ManaRestored = ManaRestored;
	LastOutcome = Outcome;
	OnRestOutcomeNative.Broadcast(Outcome);
	OnRestOutcome.Broadcast(Outcome);
}
