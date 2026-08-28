#include "Characters/EmbermereRestServiceActor.h"

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/SceneComponent.h"
#include "UI/EmbermereGameplayMessageLibrary.h"

AEmbermereRestServiceActor::AEmbermereRestServiceActor()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorEnableCollision(false);
	SetCanBeDamaged(false);

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Interactable = CreateDefaultSubobject<UEmbermereInteractableComponent>(TEXT("Interactable"));
	Interactable->DisplayName = FText::FromString(TEXT("Fenwatch Communal Well"));
	Interactable->DialogueText = FText::FromString(
		TEXT("Cool emberlit water gathers beneath the old stone."));
	Interactable->bShowWorldMarker = true;
	Interactable->MarkerHeight = 355.0f;

	RestService = CreateDefaultSubobject<UEmbermereRestServiceComponent>(TEXT("RestService"));
}

void AEmbermereRestServiceActor::BeginPlay()
{
	Super::BeginPlay();
	if (Interactable)
	{
		Interactable->OnInteracted.RemoveDynamic(this, &AEmbermereRestServiceActor::HandleInteracted);
		Interactable->OnInteracted.AddDynamic(this, &AEmbermereRestServiceActor::HandleInteracted);
	}
	if (RestService)
	{
		RestService->OnRestOutcome.RemoveDynamic(this, &AEmbermereRestServiceActor::HandleRestOutcome);
		RestService->OnRestOutcome.AddDynamic(this, &AEmbermereRestServiceActor::HandleRestOutcome);
	}
}

void AEmbermereRestServiceActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Interactable)
	{
		Interactable->OnInteracted.RemoveDynamic(this, &AEmbermereRestServiceActor::HandleInteracted);
	}
	if (RestService)
	{
		RestService->OnRestOutcome.RemoveDynamic(this, &AEmbermereRestServiceActor::HandleRestOutcome);
	}
	Super::EndPlay(EndPlayReason);
}

void AEmbermereRestServiceActor::HandleInteracted(
	AActor* Interactor,
	UEmbermereInteractableComponent* SourceInteractable)
{
	if (RestService)
	{
		RestService->TryBeginRest(Cast<AEmbermereCharacter>(Interactor));
	}
}

void AEmbermereRestServiceActor::HandleRestOutcome(FEmbermereRestOutcome Outcome)
{
	if (!RestService)
	{
		return;
	}

	FLinearColor MessageColor(0.88f, 0.82f, 0.62f, 1.0f);
	if (Outcome.Result == EEmbermereRestResult::Started)
	{
		MessageColor = FLinearColor(0.48f, 0.9f, 1.0f, 1.0f);
	}
	else if (Outcome.Result == EEmbermereRestResult::Success)
	{
		MessageColor = FLinearColor(0.45f, 1.0f, 0.7f, 1.0f);
	}
	else if (Outcome.Result == EEmbermereRestResult::InvalidRequest ||
		Outcome.Result == EEmbermereRestResult::Dead ||
		Outcome.Result == EEmbermereRestResult::InCombat ||
		Outcome.Result == EEmbermereRestResult::Interrupted)
	{
		MessageColor = FLinearColor(1.0f, 0.48f, 0.38f, 1.0f);
	}

	UEmbermereGameplayMessageLibrary::PostGameplayMessage(
		this,
		RestService->GetRestResultText(Outcome),
		MessageColor);
}
