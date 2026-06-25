#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Engine/Engine.h"

UEmbermereInteractableComponent::UEmbermereInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DisplayName = FText::FromString(TEXT("Villager"));
}

void UEmbermereInteractableComponent::Interact(AActor* Interactor)
{
	if (GEngine && !DialogueText.IsEmpty())
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.0f,
			FColor::White,
			FString::Printf(TEXT("%s: %s"), *DisplayName.ToString(), *DialogueText.ToString()));
	}

	if (Interactor && QuestToOffer)
	{
		if (UEmbermereQuestLogComponent* QuestLog = Interactor->FindComponentByClass<UEmbermereQuestLogComponent>())
		{
			if (!QuestLog->AcceptQuest(QuestToOffer))
			{
				QuestLog->TryCompleteActiveQuest();
			}
		}
	}

	OnInteracted.Broadcast(Interactor, this);
}
