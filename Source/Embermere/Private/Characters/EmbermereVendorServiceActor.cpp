#include "Characters/EmbermereVendorServiceActor.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/SceneComponent.h"

AEmbermereVendorServiceActor::AEmbermereVendorServiceActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Interactable = CreateDefaultSubobject<UEmbermereInteractableComponent>(TEXT("Interactable"));
	Interactable->DisplayName = FText::FromString(TEXT("Fenwatch Quartermaster"));
	Interactable->bShowWorldMarker = true;
	Interactable->MarkerHeight = 238.0f;

	Vendor = CreateDefaultSubobject<UEmbermereVendorComponent>(TEXT("Vendor"));
}
