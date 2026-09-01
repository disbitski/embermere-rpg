#include "Characters/EmbermereRestQuestServiceActor.h"

#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereRestQuestObjectiveRouterComponent.h"
#include "Components/SceneComponent.h"

AEmbermereRestQuestServiceActor::AEmbermereRestQuestServiceActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Interactable = CreateDefaultSubobject<UEmbermereInteractableComponent>(TEXT("Interactable"));
	Interactable->DisplayName = FText::FromString(TEXT("Fenwatch Notice Board"));
	Interactable->DialogueText = FText::FromString(
		TEXT("A fresh notice asks travelers to test the emberlit water at Fenwatch's communal well."));
	Interactable->bUseQuestStateDialogue = true;
	Interactable->bShowWorldMarker = true;
	Interactable->MarkerHeight = 305.0f;

	RestObjectiveRouter = CreateDefaultSubobject<UEmbermereRestQuestObjectiveRouterComponent>(
		TEXT("RestObjectiveRouter"));
}
