#include "Game/EmbermerePlayerController.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereTargetingComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEmbermerePlayerController::AEmbermerePlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableMouseOverEvents = false;
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

void AEmbermerePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (bAutorunEnabled || (bLeftMouseDown && bRightMouseDown))
		{
			Character->MoveForward(1.0f);
		}
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

void AEmbermerePlayerController::CycleTarget()
{
	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (Character->Targeting)
		{
			Character->Targeting->CycleTarget();
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
		AddPitchInput(Value * MouseTurnRate);
	}
}

void AEmbermerePlayerController::ActivateHotbarSlot(int32 SlotIndex)
{
	if (AEmbermereCharacter* Character = GetEmbermereCharacter())
	{
		if (Character->Hotbar)
		{
			Character->Hotbar->ActivateSlot(SlotIndex);
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
