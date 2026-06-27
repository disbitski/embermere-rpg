#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"

UEmbermereInteractableComponent::UEmbermereInteractableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	DisplayName = FText::FromString(TEXT("Villager"));
}

void UEmbermereInteractableComponent::BeginPlay()
{
	Super::BeginPlay();
	CreateWorldMarker();
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

void UEmbermereInteractableComponent::CreateWorldMarker()
{
	if (!bShowWorldMarker || WorldMarkerText || !GetOwner())
	{
		return;
	}

	USceneComponent* AttachParent = GetOwner()->GetRootComponent();
	if (!AttachParent)
	{
		return;
	}

	WorldMarkerText = NewObject<UTextRenderComponent>(GetOwner(), TEXT("EmbermereInteractableMarker"));
	if (!WorldMarkerText)
	{
		return;
	}

	const bool bIsQuestGiver = QuestToOffer != nullptr;
	const FString MarkerString = bIsQuestGiver
		? FString::Printf(TEXT("!\n%s"), *DisplayName.ToString())
		: DisplayName.ToString();

	WorldMarkerText->SetupAttachment(AttachParent);
	WorldMarkerText->SetRelativeLocation(FVector(0.0f, 0.0f, MarkerHeight));
	WorldMarkerText->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	WorldMarkerText->SetText(FText::FromString(MarkerString));
	WorldMarkerText->SetTextRenderColor(bIsQuestGiver ? FColor(255, 220, 64) : FColor(180, 220, 255));
	WorldMarkerText->SetHorizontalAlignment(EHTA_Center);
	WorldMarkerText->SetVerticalAlignment(EVRTA_TextCenter);
	WorldMarkerText->SetWorldSize(bIsQuestGiver ? 48.0f : 32.0f);
	WorldMarkerText->SetCastShadow(false);
	WorldMarkerText->RegisterComponent();
}
