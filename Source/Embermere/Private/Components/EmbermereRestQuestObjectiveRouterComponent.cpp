#include "Components/EmbermereRestQuestObjectiveRouterComponent.h"

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereRestServiceActor.h"
#include "Components/EmbermereQuestLogComponent.h"

UEmbermereRestQuestObjectiveRouterComponent::UEmbermereRestQuestObjectiveRouterComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEmbermereRestQuestObjectiveRouterComponent::SetObservedRestService(
	AEmbermereRestServiceActor* NewRestService)
{
	if (ObservedRestService == NewRestService && RestOutcomeHandle.IsValid())
	{
		return;
	}

	UnbindFromObservedRestService();
	ObservedRestService = NewRestService;
	BindToObservedRestService();
}

bool UEmbermereRestQuestObjectiveRouterComponent::IsBoundToObservedRestService() const
{
	return RestOutcomeHandle.IsValid() && ObservedRestService &&
		ObservedRestService->RestService;
}

void UEmbermereRestQuestObjectiveRouterComponent::BeginPlay()
{
	Super::BeginPlay();
	BindToObservedRestService();
}

void UEmbermereRestQuestObjectiveRouterComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromObservedRestService();
	Super::EndPlay(EndPlayReason);
}

void UEmbermereRestQuestObjectiveRouterComponent::BindToObservedRestService()
{
	if (RestOutcomeHandle.IsValid() || !ObservedRestService ||
		!ObservedRestService->RestService)
	{
		return;
	}

	RestOutcomeHandle = ObservedRestService->RestService->OnRestOutcomeNative.AddUObject(
		this,
		&UEmbermereRestQuestObjectiveRouterComponent::HandleRestOutcome);
}

void UEmbermereRestQuestObjectiveRouterComponent::UnbindFromObservedRestService()
{
	if (ObservedRestService && ObservedRestService->RestService &&
		RestOutcomeHandle.IsValid())
	{
		ObservedRestService->RestService->OnRestOutcomeNative.Remove(RestOutcomeHandle);
	}
	RestOutcomeHandle.Reset();
}

void UEmbermereRestQuestObjectiveRouterComponent::HandleRestOutcome(
	const FEmbermereRestOutcome& Outcome)
{
	if (Outcome.Result != EEmbermereRestResult::Success || !Outcome.Character ||
		QuestId.IsNone() || ObjectiveId.IsNone())
	{
		return;
	}

	if (UEmbermereQuestLogComponent* QuestLog = Outcome.Character->QuestLog)
	{
		QuestLog->AddObjectiveProgressForQuest(QuestId, ObjectiveId, 1);
	}
}
