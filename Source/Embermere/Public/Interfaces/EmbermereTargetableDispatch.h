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
