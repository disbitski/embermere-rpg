#pragma once

#include "CoreMinimal.h"
#include "Interfaces/EmbermereTargetable.h"

namespace EmbermereTargetableDispatch
{
inline bool UsesBlueprintDispatch(const AActor* Actor)
{
	return Actor && Actor->GetClass()->HasAnyClassFlags(CLASS_CompiledFromBlueprint);
}

inline bool IsAlive(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}
	if (UsesBlueprintDispatch(Actor))
	{
		return IEmbermereTargetable::Execute_IsAlive(Actor);
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		return NativeTargetable->IsAlive_Implementation();
	}
	return Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()) &&
		IEmbermereTargetable::Execute_IsAlive(Actor);
}

inline bool IsHostileTo(AActor* Actor, const AActor* Viewer)
{
	if (!Actor)
	{
		return false;
	}
	if (UsesBlueprintDispatch(Actor))
	{
		return IEmbermereTargetable::Execute_IsHostileTo(Actor, Viewer);
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		return NativeTargetable->IsHostileTo_Implementation(Viewer);
	}
	return Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()) &&
		IEmbermereTargetable::Execute_IsHostileTo(Actor, Viewer);
}

inline FText GetDisplayName(AActor* Actor)
{
	if (!Actor)
	{
		return FText::GetEmpty();
	}
	if (UsesBlueprintDispatch(Actor))
	{
		return IEmbermereTargetable::Execute_GetTargetDisplayName(Actor);
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		return NativeTargetable->GetTargetDisplayName_Implementation();
	}
	return Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass())
		? IEmbermereTargetable::Execute_GetTargetDisplayName(Actor)
		: FText::GetEmpty();
}

inline FVector GetCombatFeedbackAnchorLocation(AActor* Actor)
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}
	if (UsesBlueprintDispatch(Actor))
	{
		const FVector BlueprintAnchor =
			IEmbermereTargetable::Execute_GetCombatFeedbackAnchorLocation(Actor);
		const FVector ActorLocation = Actor->GetActorLocation();
		const bool bUsableBlueprintAnchor =
			!BlueprintAnchor.ContainsNaN() &&
			FVector::DistSquared2D(BlueprintAnchor, ActorLocation) <= FMath::Square(5000.0f) &&
			BlueprintAnchor.Z >= ActorLocation.Z + 10.0f &&
			BlueprintAnchor.Z <= ActorLocation.Z + 2000.0f;
		if (bUsableBlueprintAnchor)
		{
			return BlueprintAnchor;
		}

		// Saved Blueprint classes can predate a newly added native interface event and
		// temporarily return the generated zero-value thunk until they are resaved.
		if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
		{
			return NativeTargetable->GetCombatFeedbackAnchorLocation_Implementation();
		}
		return ActorLocation + FVector(0.0f, 0.0f, 100.0f);
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		return NativeTargetable->GetCombatFeedbackAnchorLocation_Implementation();
	}
	return Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass())
		? IEmbermereTargetable::Execute_GetCombatFeedbackAnchorLocation(Actor)
		: Actor->GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
}

inline bool ShouldGrantDefeatCredit(AActor* Actor)
{
	if (!Actor)
	{
		return false;
	}
	if (UsesBlueprintDispatch(Actor))
	{
		return IEmbermereTargetable::Execute_ShouldGrantDefeatCredit(Actor);
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		return NativeTargetable->ShouldGrantDefeatCredit_Implementation();
	}
	return Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()) &&
		IEmbermereTargetable::Execute_ShouldGrantDefeatCredit(Actor);
}

inline void SetTargetedByPlayer(AActor* Actor, bool bIsTargeted)
{
	if (!Actor)
	{
		return;
	}
	if (UsesBlueprintDispatch(Actor))
	{
		IEmbermereTargetable::Execute_SetTargetedByPlayer(Actor, bIsTargeted);
		return;
	}
	if (IEmbermereTargetable* NativeTargetable = Cast<IEmbermereTargetable>(Actor))
	{
		NativeTargetable->SetTargetedByPlayer_Implementation(bIsTargeted);
		return;
	}
	if (Actor->GetClass()->ImplementsInterface(UEmbermereTargetable::StaticClass()))
	{
		IEmbermereTargetable::Execute_SetTargetedByPlayer(Actor, bIsTargeted);
	}
}
}
