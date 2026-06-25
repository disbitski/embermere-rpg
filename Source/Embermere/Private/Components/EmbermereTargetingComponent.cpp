#include "Components/EmbermereTargetingComponent.h"
#include "Components/EmbermereCombatComponent.h"
#include "EngineUtils.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/EmbermereTargetable.h"

UEmbermereTargetingComponent::UEmbermereTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

AActor* UEmbermereTargetingComponent::CycleTarget()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	TArray<AActor*> Candidates;
	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector OwnerForward = Owner->GetActorForwardVector();
	const float MinDot = FMath::Cos(FMath::DegreesToRadians(ForwardConeDegrees * 0.5f));

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Candidate = *It;
		if (!Candidate || Candidate == Owner || !Candidate->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()))
		{
			continue;
		}

		if (!IEmbermereTargetable::Execute_IsAlive(Candidate) || !IEmbermereTargetable::Execute_IsHostileTo(Candidate, Owner))
		{
			continue;
		}

		const FVector ToCandidate = Candidate->GetActorLocation() - OwnerLocation;
		if (ToCandidate.SizeSquared() > FMath::Square(SearchRadius))
		{
			continue;
		}

		if (FVector::DotProduct(OwnerForward, ToCandidate.GetSafeNormal()) < MinDot)
		{
			continue;
		}

		Candidates.Add(Candidate);
	}

	UEmbermereCombatComponent* Combat = Owner->FindComponentByClass<UEmbermereCombatComponent>();
	AActor* NewTarget = FindNextTarget(Candidates, Combat ? Combat->CurrentTarget : nullptr);
	if (Combat)
	{
		Combat->SetTarget(NewTarget);
	}
	return NewTarget;
}

AActor* UEmbermereTargetingComponent::FindNextTarget(const TArray<AActor*>& Candidates, AActor* CurrentTarget) const
{
	if (Candidates.Num() == 0)
	{
		return nullptr;
	}

	TArray<AActor*> SortedCandidates = Candidates;
	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	SortedCandidates.Sort([OwnerLocation](const AActor& Left, const AActor& Right)
	{
		return FVector::DistSquared(OwnerLocation, Left.GetActorLocation()) < FVector::DistSquared(OwnerLocation, Right.GetActorLocation());
	});

	const int32 CurrentIndex = SortedCandidates.IndexOfByKey(CurrentTarget);
	return SortedCandidates[(CurrentIndex + 1) % SortedCandidates.Num()];
}
