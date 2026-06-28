#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EmbermerePlayerController.generated.h"

class AEmbermereCharacter;
class UEmbermerePlayerHudWidget;

UCLASS()
class EMBERMERE_API AEmbermerePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEmbermerePlayerController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float MouseTurnRate = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bInvertMouseY = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionRadius = 350.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool bAutorunEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float PlayerRespawnDelaySeconds = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEmbermerePlayerHudWidget> PlayerHudWidgetClass;

	void NotifyManualMoveForwardInput(float Value);

	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	UFUNCTION()
	void HandleControlledCharacterDied();

	UFUNCTION()
	void RespawnControlledCharacter();

	void OnLeftMousePressed();
	void OnLeftMouseReleased();
	void OnRightMousePressed();
	void OnRightMouseReleased();
	void ToggleAutorun();
	void ToggleInvertMouseY();
	void CycleTarget();
	void Turn(float Value);
	void LookUp(float Value);
	void ActivateHotbarSlot(int32 SlotIndex);
	void ActivateHotbar1();
	void ActivateHotbar2();
	void ActivateHotbar3();
	void ActivateHotbar4();
	void ActivateHotbar5();
	void ActivateHotbar6();
	void ActivateHotbar7();
	void ActivateHotbar8();
	void ActivateHotbar9();
	void ActivateHotbar10();

private:
	bool bLeftMouseDown = false;
	bool bRightMouseDown = false;
	bool bApplyingAutomaticForwardMovement = false;
	FTransform ControlledSpawnTransform;
	FTimerHandle PlayerRespawnTimerHandle;
	TObjectPtr<UEmbermerePlayerHudWidget> PlayerHudWidget;

	AEmbermereCharacter* GetEmbermereCharacter() const;
	bool InteractWithNearestActor();
	void EnsurePlayerHud();
	void ShowTargetFeedback(AActor* TargetActor) const;
	void UpdateClassicMouseCameraMode();
};
