#include "Characters/EmbermereTrainerServiceActor.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/SceneComponent.h"

AEmbermereTrainerServiceActor::AEmbermereTrainerServiceActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Interactable = CreateDefaultSubobject<UEmbermereInteractableComponent>(TEXT("Interactable"));
	Interactable->DisplayName = FText::FromString(TEXT("Fenwatch Armsmaster"));
	Interactable->bShowWorldMarker = true;
	Interactable->MarkerHeight = 238.0f;

	Trainer = CreateDefaultSubobject<UEmbermereTrainerComponent>(TEXT("Trainer"));
}
