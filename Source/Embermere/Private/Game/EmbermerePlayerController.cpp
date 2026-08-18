#include "Game/EmbermerePlayerController.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInteractableComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereTargetingComponent.h"
#include "Components/EmbermereTrainerComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputCoreTypes.h"
#include "Interfaces/EmbermereTargetableDispatch.h"
#include "Kismet/GameplayStatics.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "TimerManager.h"
#include "UI/EmbermerePlayerHudWidget.h"

AEmbermerePlayerController::AEmbermerePlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
}

void AEmbermerePlayerController::BeginPlay()
{
	Super::BeginPlay();
	EnsurePlayerHud();
}

void AEmbermerePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("Turn", this, &AEmbermerePlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &AEmbermerePlayerController::LookUp);

	InputComponent->BindAction("LeftMouse", IE_Pressed, this, &AEmbermerePlayerController::OnLeftMousePressed);
	InputComponent->BindAction("LeftMouse", IE_Released, this, &AEmbermerePlayerController::OnLeftMouseReleased);
	InputComponent->BindAction("RightMouse", IE_Pressed, this, &AEmbermerePlayerController::OnRightMousePressed);
	InputComponent->BindAction("RightMouse", IE_Released, this, &AEmbermerePlayerController::OnRightMouseReleased);
	InputComponent->BindAction("Autorun", IE_Pressed, this, &AEmbermerePlayerController::ToggleAutorun);
	InputComponent->BindAction("CycleTarget", IE_Pressed, this, &AEmbermerePlayerController::CycleTarget);

	InputComponent->BindAction("Hotbar1", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar1);
	InputComponent->BindAction("Hotbar2", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar2);
	InputComponent->BindAction("Hotbar3", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar3);
	InputComponent->BindAction("Hotbar4", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar4);
	InputComponent->BindAction("Hotbar5", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar5);
	InputComponent->BindAction("Hotbar6", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar6);
	InputComponent->BindAction("Hotbar7", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar7);
	InputComponent->BindAction("Hotbar8", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar8);
	InputComponent->BindAction("Hotbar9", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar9);
	InputComponent->BindAction("Hotbar10", IE_Pressed, this, &AEmbermerePlayerController::ActivateHotbar10);
}

void AEmbermerePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (AEmbermereCharacter* Character = Cast<AEmbermereCharacter>(InPawn))
	{
		ControlledSpawnTransform = Character->GetActorTransform();
		if (Character->Stats)
		{
			Character->Stats->OnDied.AddUniqueDynamic(this, &AEmbermerePlayerController::HandleControlledCharacterDied);
		}

		EnsurePlayerHud();
		if (PlayerHudWidget)
		{
			PlayerHudWidget->BindToCharacter(Character);
		}
	}
}

void AEmbermerePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	const bool bControlDown = IsInputKeyDown(EKeys::LeftControl) || IsInputKeyDown(EKeys::RightControl);
	if (bControlDown && WasInputKeyJustPressed(EKeys::M))
	{
		ToggleInvertMouseY();
	}
	else if (WasInputKeyJustPressed(EKeys::M))
	{
		ToggleSaveLoadPanel();
	}
	if (WasInputKeyJustPressed(EKeys::I))
	{
		ToggleInventoryPanel();
	}
	if (WasInputKeyJustPressed(EKeys::LeftBracket))
	{
		SelectPreviousInventoryItem();
	}
	if (WasInputKeyJustPressed(EKeys::RightBracket))
	{
		SelectNextInventoryItem();
	}

	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (TriggerOutOfBoundsRecoveryIfNeeded(Character))
		{
			return;
		}

		if (bAutorunEnabled || (bLeftMouseDown && bRightMouseDown))
		{
			const FRotator YawRotation(0.0f, GetControlRotation().Yaw, 0.0f);
			Character->AddMovementInput(FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X), 1.0f);
		}
	}
}

bool AEmbermerePlayerController::TriggerOutOfBoundsRecoveryIfNeeded(AEmbermereCharacter* Character)
{
	if (!Character ||
		!Character->Stats ||
		Character->Stats->IsDead() ||
		Character->GetActorLocation().Z > OutOfBoundsRecoveryZ)
	{
		return false;
	}

	bAutorunEnabled = false;
	Character->Stats->ForceDeath();
	return true;
}

void AEmbermerePlayerController::NotifyManualMoveForwardInput(float Value)
{
	if (bAutorunEnabled && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		bAutorunEnabled = false;
	}
}

void AEmbermerePlayerController::HandleControlledCharacterDied()
{
	bAutorunEnabled = false;
	SetIgnoreMoveInput(true);

	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->StopMovementImmediately();
			Movement->DisableMovement();
		}
	}

	AddHudMessage(FText::FromString(TEXT("You have fallen.")), FLinearColor(1.0f, 0.18f, 0.12f, 1.0f));

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			PlayerRespawnTimerHandle,
			this,
			&AEmbermerePlayerController::RespawnControlledCharacter,
			FMath::Max(0.1f, PlayerRespawnDelaySeconds),
			false);
	}
}

void AEmbermerePlayerController::RespawnControlledCharacter()
{
	AEmbermereCharacter* Character = GetEmbermereCharacter();
	if (!Character || !Character->Stats)
	{
		SetIgnoreMoveInput(false);
		return;
	}

	Character->SetActorTransform(ControlledSpawnTransform, false, nullptr, ETeleportType::TeleportPhysics);
	if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
		Movement->SetMovementMode(MOVE_Walking);
	}
	Character->Stats->InitializeVitals();
	Character->Stats->GrantDamageImmunity(PlayerRespawnProtectionSeconds);
	SetIgnoreMoveInput(false);

	if (PlayerRespawnProtectionSeconds > 0.0f)
	{
		AddHudMessage(
			FText::FromString(FString::Printf(TEXT("You recover at the village. Protected for %.0fs."), PlayerRespawnProtectionSeconds)),
			FLinearColor(0.42f, 1.0f, 0.48f, 1.0f));
	}
	else
	{
		AddHudMessage(FText::FromString(TEXT("You recover at the village.")), FLinearColor(0.42f, 1.0f, 0.48f, 1.0f));
	}
}

void AEmbermerePlayerController::OnLeftMousePressed()
{
	bLeftMouseDown = true;
	UpdateClassicMouseCameraMode();
}

void AEmbermerePlayerController::OnLeftMouseReleased()
{
	bLeftMouseDown = false;
	UpdateClassicMouseCameraMode();
}

void AEmbermerePlayerController::OnRightMousePressed()
{
	bRightMouseDown = true;
	UpdateClassicMouseCameraMode();
}

void AEmbermerePlayerController::OnRightMouseReleased()
{
	bRightMouseDown = false;
	UpdateClassicMouseCameraMode();
}

void AEmbermerePlayerController::ToggleAutorun()
{
	bAutorunEnabled = !bAutorunEnabled;
}

void AEmbermerePlayerController::ToggleInvertMouseY()
{
	bInvertMouseY = !bInvertMouseY;

	AddHudMessage(
		FText::FromString(bInvertMouseY ? TEXT("Mouse Y inverted") : TEXT("Mouse Y normal")),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
}

void AEmbermerePlayerController::EmbermereSave()
{
	FText ResultMessage;
	const EEmbermerePersistenceResult Result = SavePrototypeProgress(ResultMessage);
	AddHudMessage(
		ResultMessage,
		Result == EEmbermerePersistenceResult::Success
			? FLinearColor(0.42f, 1.0f, 0.48f, 1.0f)
			: FLinearColor(1.0f, 0.24f, 0.16f, 1.0f));
}

void AEmbermerePlayerController::EmbermereLoad()
{
	FText ResultMessage;
	const EEmbermerePersistenceResult Result = LoadPrototypeProgress(ResultMessage);
	AddHudMessage(
		ResultMessage,
		Result == EEmbermerePersistenceResult::Success
			? FLinearColor(0.42f, 1.0f, 0.48f, 1.0f)
			: FLinearColor(1.0f, 0.24f, 0.16f, 1.0f));
}

EEmbermerePersistenceResult AEmbermerePlayerController::SavePrototypeProgress(FText& OutMessage)
{
	return UEmbermerePersistenceLibrary::SaveWorldStateToSlot(
		GetEmbermereCharacter(),
		PrototypeSaveSlotName,
		PrototypeSaveUserIndex,
		OutMessage);
}

EEmbermerePersistenceResult AEmbermerePlayerController::LoadPrototypeProgress(FText& OutMessage)
{
	return UEmbermerePersistenceLibrary::LoadWorldStateFromSlot(
		GetEmbermereCharacter(),
		PrototypeSaveSlotName,
		PrototypeSaveUserIndex,
		OutMessage);
}

EEmbermerePersistenceResult AEmbermerePlayerController::InspectPrototypeSave(FText& OutSummary) const
{
	return UEmbermerePersistenceLibrary::InspectSaveSlot(
		PrototypeSaveSlotName,
		PrototypeSaveUserIndex,
		OutSummary);
}

bool AEmbermerePlayerController::DoesPrototypeSaveExist() const
{
	return !PrototypeSaveSlotName.IsEmpty() && PrototypeSaveUserIndex >= 0 &&
		UGameplayStatics::DoesSaveGameExist(PrototypeSaveSlotName, PrototypeSaveUserIndex);
}

void AEmbermerePlayerController::ToggleInventoryPanel()
{
	if (!PlayerHudWidget)
	{
		return;
	}

	const bool bNowVisible = PlayerHudWidget->ToggleInventoryPanel();
	RefreshInteractiveInputMode();
	AddHudMessage(
		FText::FromString(bNowVisible ? TEXT("Inventory shown") : TEXT("Inventory hidden")),
		FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
}

void AEmbermerePlayerController::ToggleSaveLoadPanel()
{
	if (!PlayerHudWidget)
	{
		return;
	}

	PlayerHudWidget->ToggleSaveLoadPanel();
	RefreshInteractiveInputMode();
}

void AEmbermerePlayerController::SelectPreviousInventoryItem()
{
	if (PlayerHudWidget && PlayerHudWidget->IsTrainerPanelVisible())
	{
		PlayerHudWidget->SelectNextTrainerOffering(-1);
		return;
	}

	if (PlayerHudWidget && PlayerHudWidget->SelectNextInventoryItem(-1))
	{
		AddHudMessage(FText::FromString(TEXT("Inventory inspect previous")), FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
	}
}

void AEmbermerePlayerController::SelectNextInventoryItem()
{
	if (PlayerHudWidget && PlayerHudWidget->IsTrainerPanelVisible())
	{
		PlayerHudWidget->SelectNextTrainerOffering(1);
		return;
	}

	if (PlayerHudWidget && PlayerHudWidget->SelectNextInventoryItem(1))
	{
		AddHudMessage(FText::FromString(TEXT("Inventory inspect next")), FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
	}
}

void AEmbermerePlayerController::CycleTarget()
{
	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (Character->Targeting)
		{
			ShowTargetFeedback(Character->Targeting->CycleTarget());
		}
	}
}

void AEmbermerePlayerController::Turn(float Value)
{
	if ((bLeftMouseDown || bRightMouseDown) && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddYawInput(Value * MouseTurnRate);
	}
}

void AEmbermerePlayerController::LookUp(float Value)
{
	if ((bLeftMouseDown || bRightMouseDown) && FMath::Abs(Value) > KINDA_SMALL_NUMBER)
	{
		AddPitchInput(Value * MouseTurnRate * (bInvertMouseY ? -1.0f : 1.0f));
	}
}

void AEmbermerePlayerController::ActivateHotbarSlot(int32 SlotIndex)
{
	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (Character->Hotbar)
		{
			if (Character->Hotbar->Slots.IsValidIndex(SlotIndex) && !Character->Hotbar->Slots[SlotIndex].AbilityId.IsNone())
			{
				const float CooldownRemaining = Character->Hotbar->GetSlotCooldownRemaining(SlotIndex);
				if (CooldownRemaining > 0.05f)
				{
					AddHudMessage(
						FText::FromString(FString::Printf(
							TEXT("%s ready in %.1fs"),
							*Character->Hotbar->Slots[SlotIndex].DisplayName.ToString(),
							CooldownRemaining)),
						FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
					return;
				}
			}

			const bool bActivatedAbility = Character->Hotbar->ActivateSlot(SlotIndex);
			if (!bActivatedAbility && SlotIndex == 9)
			{
				InteractWithNearestActor();
			}
			else if (!bActivatedAbility && Character->Hotbar->Slots.IsValidIndex(SlotIndex) && !Character->Hotbar->Slots[SlotIndex].AbilityId.IsNone())
			{
				AddHudMessage(
					FText::FromString(FString::Printf(TEXT("Unable to use %s"), *Character->Hotbar->Slots[SlotIndex].DisplayName.ToString())),
					FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
			}
		}
	}
}

void AEmbermerePlayerController::ActivateHotbar1() { ActivateHotbarSlot(0); }
void AEmbermerePlayerController::ActivateHotbar2() { ActivateHotbarSlot(1); }
void AEmbermerePlayerController::ActivateHotbar3() { ActivateHotbarSlot(2); }
void AEmbermerePlayerController::ActivateHotbar4() { ActivateHotbarSlot(3); }
void AEmbermerePlayerController::ActivateHotbar5() { ActivateHotbarSlot(4); }
void AEmbermerePlayerController::ActivateHotbar6() { ActivateHotbarSlot(5); }
void AEmbermerePlayerController::ActivateHotbar7() { ActivateHotbarSlot(6); }
void AEmbermerePlayerController::ActivateHotbar8() { ActivateHotbarSlot(7); }
void AEmbermerePlayerController::ActivateHotbar9() { ActivateHotbarSlot(8); }
void AEmbermerePlayerController::ActivateHotbar10() { ActivateHotbarSlot(9); }

AEmbermereCharacter* AEmbermerePlayerController::GetEmbermereCharacter() const
{
	return Cast<AEmbermereCharacter>(GetPawn());
}

bool AEmbermerePlayerController::InteractWithNearestActor()
{
	AEmbermereCharacter* Character = GetEmbermereCharacter();
	if (!Character)
	{
		return false;
	}

	UEmbermereInteractableComponent* BestInteractable = nullptr;
	float BestDistanceSquared = FMath::Square(InteractionRadius);
	const FVector CharacterLocation = Character->GetActorLocation();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Candidate = *It;
		if (!Candidate || Candidate == Character)
		{
			continue;
		}

		UEmbermereInteractableComponent* Interactable = Candidate->FindComponentByClass<UEmbermereInteractableComponent>();
		if (!Interactable)
		{
			continue;
		}

		const float DistanceSquared = FVector::DistSquared(CharacterLocation, Candidate->GetActorLocation());
		if (DistanceSquared <= BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestInteractable = Interactable;
		}
	}

	if (!BestInteractable)
	{
		return false;
	}

	if (PlayerHudWidget && !BestInteractable->DialogueText.IsEmpty())
	{
		PlayerHudWidget->ShowDialogue(BestInteractable->DisplayName, BestInteractable->DialogueText);
	}

	BestInteractable->Interact(Character);
	if (UEmbermereVendorComponent* Vendor = BestInteractable->GetOwner()->FindComponentByClass<UEmbermereVendorComponent>())
	{
		if (PlayerHudWidget)
		{
			PlayerHudWidget->ShowVendor(Vendor);
			RefreshInteractiveInputMode();
		}
	}
	else if (UEmbermereTrainerComponent* Trainer =
		BestInteractable->GetOwner()->FindComponentByClass<UEmbermereTrainerComponent>())
	{
		if (PlayerHudWidget)
		{
			PlayerHudWidget->ShowTrainer(Trainer);
			RefreshInteractiveInputMode();
		}
	}
	return true;
}

void AEmbermerePlayerController::EnsurePlayerHud()
{
	if (PlayerHudWidget)
	{
		return;
	}

	TSubclassOf<UEmbermerePlayerHudWidget> EffectiveHudClass = PlayerHudWidgetClass;
	if (!EffectiveHudClass)
	{
		EffectiveHudClass = UEmbermerePlayerHudWidget::StaticClass();
	}
	if (!EffectiveHudClass)
	{
		return;
	}

	PlayerHudWidget = CreateWidget<UEmbermerePlayerHudWidget>(this, EffectiveHudClass);
	if (!PlayerHudWidget)
	{
		return;
	}

	PlayerHudWidget->AddToViewport();
	PlayerHudWidget->BindToCharacter(GetEmbermereCharacter());
	RefreshInteractiveInputMode();
}

void AEmbermerePlayerController::RefreshInteractiveInputMode()
{
	const bool bInteractiveUiVisible = PlayerHudWidget &&
		(PlayerHudWidget->IsInventoryPanelVisible() || PlayerHudWidget->IsVendorPanelVisible() ||
			PlayerHudWidget->IsTrainerPanelVisible() || PlayerHudWidget->IsSaveLoadPanelVisible());
	UpdateInventoryInputMode(bInteractiveUiVisible);
}

void AEmbermerePlayerController::AddHudMessage(const FText& Message, FLinearColor MessageColor) const
{
	if (PlayerHudWidget)
	{
		PlayerHudWidget->AddChatMessage(Message, MessageColor);
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, MessageColor.ToFColor(true), Message.ToString());
	}
}

void AEmbermerePlayerController::ShowTargetFeedback(AActor* TargetActor) const
{

	if (!TargetActor)
	{
		AddHudMessage(FText::FromString(TEXT("No hostile target")), FLinearColor(0.86f, 0.88f, 0.9f, 1.0f));
		return;
	}

	const AEmbermereCharacter* Character = GetEmbermereCharacter();
	const FText TargetableName = EmbermereTargetableDispatch::GetDisplayName(TargetActor);
	const FText TargetName = TargetableName.IsEmpty()
		? FText::FromString(TargetActor->GetActorLabel())
		: TargetableName;

	FString HealthText = TEXT("");
	if (const UEmbermereStatsComponent* TargetStats = TargetActor->FindComponentByClass<UEmbermereStatsComponent>())
	{
		HealthText = FString::Printf(TEXT(" %.0f/%.0f HP"), TargetStats->CurrentHealth, TargetStats->MaxHealth);
	}

	FString RangeText = TEXT("");
	if (Character && Character->Hotbar)
	{
		for (const FEmbermereAbilityDefinition& Ability : Character->Hotbar->Slots)
		{
			if (!Ability.AbilityId.IsNone() && Ability.TargetKind == EEmbermereAbilityTargetKind::Enemy)
			{
				const bool bInRange = FVector::DistSquared(Character->GetActorLocation(), TargetActor->GetActorLocation()) <=
					FMath::Square(Ability.Range);
				RangeText = FString::Printf(
					TEXT(" - %s for %s"),
					bInRange ? TEXT("in range") : TEXT("out of range"),
					*Ability.DisplayName.ToString());
				break;
			}
		}
	}

	AddHudMessage(
		FText::FromString(FString::Printf(TEXT("Target: %s%s%s"), *TargetName.ToString(), *HealthText, *RangeText)),
		FLinearColor(0.46f, 0.95f, 1.0f, 1.0f));
}

void AEmbermerePlayerController::UpdateClassicMouseCameraMode()
{
	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		Character->bUseControllerRotationYaw = bRightMouseDown;
		if (UCharacterMovementComponent* Movement = Character->GetCharacterMovement())
		{
			Movement->bOrientRotationToMovement = !bRightMouseDown;
		}
	}
}

void AEmbermerePlayerController::UpdateInventoryInputMode(bool bInventoryVisible)
{
	bShowMouseCursor = bInventoryVisible;
	bEnableClickEvents = bInventoryVisible;
	bEnableMouseOverEvents = bInventoryVisible;

	if (bInventoryVisible && PlayerHudWidget)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PlayerHudWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		SetInputMode(InputMode);
		return;
	}

	bLeftMouseDown = false;
	bRightMouseDown = false;
	UpdateClassicMouseCameraMode();
	SetInputMode(FInputModeGameOnly());
}
