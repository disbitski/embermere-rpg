#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/Actor.h"
#include "UI/EmbermereGameplayMessageLibrary.h"

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
	if (!DialogueText.IsEmpty())
	{
		UEmbermereGameplayMessageLibrary::PostGameplayMessage(
			this,
			FText::FromString(FString::Printf(TEXT("%s: %s"), *DisplayName.ToString(), *DialogueText.ToString())),
			FLinearColor(0.92f, 0.92f, 0.88f, 1.0f));
	}

	if (Interactor && QuestToOffer)
	{
		if (UEmbermereQuestLogComponent* QuestLog = Interactor->FindComponentByClass<UEmbermereQuestLogComponent>())
		{
			const EEmbermereQuestAcceptanceResult Acceptance =
				QuestLog->EvaluateQuestAcceptance(QuestToOffer);
			switch (Acceptance)
			{
			case EEmbermereQuestAcceptanceResult::Success:
				QuestLog->AcceptQuest(QuestToOffer);
				break;
			case EEmbermereQuestAcceptanceResult::AlreadyTracked:
				QuestLog->TryCompleteQuest(QuestToOffer);
				break;
			case EEmbermereQuestAcceptanceResult::InvalidQuest:
			case EEmbermereQuestAcceptanceResult::OccupiedByOtherQuest:
				UEmbermereGameplayMessageLibrary::PostGameplayMessage(
					this,
					QuestLog->GetQuestAcceptanceResultText(Acceptance, QuestToOffer),
					FLinearColor(1.0f, 0.62f, 0.24f, 1.0f));
				break;
			default:
				break;
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
