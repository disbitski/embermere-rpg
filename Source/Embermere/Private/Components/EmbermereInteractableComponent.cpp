#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"

UEmbermereInteractableComponent::UEmbermereInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DisplayName = FText::FromString(TEXT("Villager"));
}

void UEmbermereInteractableComponent::Interact(AActor* Interactor)
{
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
