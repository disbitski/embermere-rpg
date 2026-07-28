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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereRace Race = EEmbermereRace::Human;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereClass Class = EEmbermereClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	TObjectPtr<UEmbermereRulesData> RulesData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float BaseWalkSpeedCmPerSecond = 600.0f;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Identity")
	void ApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass);

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

	virtual bool IsAlive_Implementation() const override;
	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;
	virtual void SetTargetedByPlayer_Implementation(bool bIsTargeted) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void HandleTargetedByPlayer(bool bIsTargeted);
	void PrimeStarterHotbar();
	void RefreshEquipmentStats();
	void UpdateStatusEffectVfx(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Presentation|Status Effects")
	TArray<TObjectPtr<UStaticMeshComponent>> StatusEffectVfxSegments;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> StatusEffectVfxMaterials;

	float StatusEffectVfxRotationDegrees = 0.0f;
	FLinearColor StatusEffectVfxColor = FLinearColor::Transparent;
	bool bStatusEffectVfxVisible = false;
	bool bStatusEffectVfxBeneficial = false;

	UFUNCTION()
	void HandleEquipmentChanged();
};
