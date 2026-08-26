#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/EmbermereTargetable.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCharacter.generated.h"

class UCameraComponent;
class UMaterialInstanceDynamic;
class UStaticMeshComponent;
class USpringArmComponent;
class UEmbermereCombatComponent;
class UEmbermereEquipmentComponent;
class UEmbermereHotbarComponent;
class UEmbermereInventoryComponent;
class UEmbermereQuestLogComponent;
class UEmbermereRulesData;
class UEmbermereStatsComponent;
class UEmbermereTargetingComponent;
class UEmbermereWalletComponent;

UCLASS()
class EMBERMERE_API AEmbermereCharacter : public ACharacter, public IEmbermereTargetable
{
	GENERATED_BODY()

public:
	AEmbermereCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereStatsComponent> Stats;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereCombatComponent> Combat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereTargetingComponent> Targeting;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereHotbarComponent> Hotbar;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereInventoryComponent> Inventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereEquipmentComponent> Equipment;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereQuestLogComponent> QuestLog;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UEmbermereWalletComponent> Wallet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereRace Race = EEmbermereRace::Human;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereClass Class = EEmbermereClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText CharacterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Identity")
	bool bHasDeliberateCharacterChoice = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	TObjectPtr<UEmbermereRulesData> RulesData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseWalkSpeedCmPerSecond = 600.0f;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Identity")
	void ApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Identity")
	bool TryApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Identity")
	FText GetJourneyIdentitySummary() const;

	bool CanRestoreRaceAndClassForSaveGame(EEmbermereRace NewRace, EEmbermereClass NewClass) const;
	bool TryRestoreRaceAndClassForSaveGame(EEmbermereRace NewRace, EEmbermereClass NewClass);
	bool CanRestoreCharacterProgressionForSaveGame(
		EEmbermereRace NewRace,
		EEmbermereClass NewClass,
		int32 Experience,
		int32& OutLevel) const;
	bool TryRestoreCharacterProgressionForSaveGame(
		EEmbermereRace NewRace,
		EEmbermereClass NewClass,
		int32 Experience);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Movement")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Movement")
	void MoveRight(float Value);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Status Effects")
	void RefreshStatusEffectVfx();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	int32 GetStatusEffectVfxSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	int32 GetVisibleStatusEffectVfxSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	FLinearColor GetStatusEffectVfxColor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	bool IsStatusEffectVfxBeneficial() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	FString GetStatusEffectVfxMaterialPath() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	float GetStatusEffectVfxRadius() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Status Effects")
	float GetStatusEffectVfxRelativeHeight() const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Progression")
	void AdvanceLevelUpWorldVfx(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Progression")
	void ClearLevelUpWorldVfx();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	bool IsLevelUpWorldVfxVisible() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	int32 GetLevelUpWorldVfxSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	int32 GetVisibleLevelUpWorldVfxSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	bool AreLevelUpWorldVfxSegmentsNonColliding() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	FLinearColor GetLevelUpWorldVfxColor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	FString GetLevelUpWorldVfxMaterialPath() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	float GetLevelUpWorldVfxRadius() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	float GetLevelUpWorldVfxLifetimeSeconds() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Progression")
	int32 GetLevelUpWorldVfxLevelsGained() const;

	virtual bool IsAlive_Implementation() const override;
	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;
	virtual FVector GetCombatFeedbackAnchorLocation_Implementation() const override;
	virtual bool ShouldGrantDefeatCredit_Implementation() const override;
	virtual void SetTargetedByPlayer_Implementation(bool bIsTargeted) override;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void HandleTargetedByPlayer(bool bIsTargeted);
	void PrimeStarterHotbar();
	bool ValidateRaceAndClassLoadout(EEmbermereRace NewRace, EEmbermereClass NewClass) const;
	bool ApplyValidatedRaceAndClass(
		EEmbermereRace NewRace,
		EEmbermereClass NewClass,
		int32 Experience);
	void RefreshEquipmentStats();
	void UpdateStatusEffectVfx(float DeltaSeconds);
	float ResolveStatusEffectVfxRadius(bool bBeneficial) const;
	float ResolveStatusEffectVfxHeightOffset() const;
	void BindLevelUpWorldVfx();
	void UnbindLevelUpWorldVfx();
	void EnsureLevelUpWorldVfxComponents();
	FLinearColor ResolveLevelUpWorldVfxColor() const;

	UFUNCTION()
	void HandleLevelChangedForWorldVfx(int32 PreviousLevel, int32 CurrentLevel);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation|Status Effects")
	TArray<TObjectPtr<UStaticMeshComponent>> StatusEffectVfxSegments;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> StatusEffectVfxMaterials;

	float StatusEffectVfxRotationDegrees = 0.0f;
	float StatusEffectVfxRadius = 0.0f;
	float StatusEffectVfxRelativeHeight = 0.0f;
	FLinearColor StatusEffectVfxColor = FLinearColor::Transparent;
	bool bStatusEffectVfxVisible = false;
	bool bStatusEffectVfxBeneficial = false;

	UPROPERTY(VisibleAnywhere, Transient, Category = "Presentation|Progression")
	TArray<TObjectPtr<UStaticMeshComponent>> LevelUpWorldVfxSegments;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> LevelUpWorldVfxMaterials;

	float LevelUpWorldVfxAgeSeconds = 0.0f;
	float LevelUpWorldVfxRotationDegrees = 0.0f;
	float LevelUpWorldVfxRadius = 0.0f;
	FLinearColor LevelUpWorldVfxColor = FLinearColor::Transparent;
	FString LevelUpWorldVfxMaterialPath;
	int32 LevelUpWorldVfxLevelsGained = 0;
	bool bLevelUpWorldVfxVisible = false;

	UFUNCTION()
	void HandleEquipmentChanged();

#if WITH_DEV_AUTOMATION_TESTS
	friend class FEmbermereLevelUpWorldVfxPresentationTest;
#endif
};
