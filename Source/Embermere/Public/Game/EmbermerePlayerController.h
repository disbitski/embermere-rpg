#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Types/EmbermereTypes.h"
#include "EmbermerePlayerController.generated.h"

class AEmbermereCharacter;
class UEmbermereCharacterCreationWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float PlayerRespawnProtectionSeconds = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn")
	float OutOfBoundsRecoveryZ = -1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UEmbermerePlayerHudWidget> PlayerHudWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI|Character Creation")
	TSubclassOf<UEmbermereCharacterCreationWidget> CharacterCreationWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI|Character Creation")
	bool bShowCharacterCreationOnFirstPlay = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence")
	FString PrototypeSaveSlotName = TEXT("EmbermerePrototype");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Persistence", meta = (ClampMin = "0"))
	int32 PrototypeSaveUserIndex = 0;

	UFUNCTION(Exec, BlueprintCallable, Category = "Embermere|Persistence")
	void EmbermereSave();

	UFUNCTION(Exec, BlueprintCallable, Category = "Embermere|Persistence")
	void EmbermereLoad();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Persistence")
	EEmbermerePersistenceResult SavePrototypeProgress(FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Persistence")
	EEmbermerePersistenceResult LoadPrototypeProgress(FText& OutMessage);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Persistence")
	EEmbermerePersistenceResult InspectPrototypeSave(FText& OutSummary) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Persistence")
	bool DoesPrototypeSaveExist() const;

	void NotifyManualMoveForwardInput(float Value);
	void AddHudMessage(const FText& Message, FLinearColor MessageColor) const;
	bool TriggerOutOfBoundsRecoveryIfNeeded(AEmbermereCharacter* Character);
	void RefreshInteractiveInputMode();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	bool ShouldPresentCharacterCreation(const AEmbermereCharacter* InCharacter) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	bool IsCharacterCreationPanelVisible() const;

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
	void ToggleInventoryPanel();
	void ToggleSaveLoadPanel();
	void SelectPreviousInventoryItem();
	void SelectNextInventoryItem();
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
	FTransform ControlledSpawnTransform;
	FTimerHandle PlayerRespawnTimerHandle;
	TObjectPtr<UEmbermerePlayerHudWidget> PlayerHudWidget;
	TObjectPtr<UEmbermereCharacterCreationWidget> CharacterCreationWidget;

	AEmbermereCharacter* GetEmbermereCharacter() const;
	bool InteractWithNearestActor();
	void EnsurePlayerHud();
	void ShowCharacterCreationIfNeeded();
	void ShowTargetFeedback(AActor* TargetActor) const;
	void UpdateClassicMouseCameraMode();
	void UpdateInventoryInputMode(bool bInventoryVisible);

	UFUNCTION()
	void HandleCharacterChoiceConfirmed(EEmbermereRace ConfirmedRace, EEmbermereClass ConfirmedClass);
};
