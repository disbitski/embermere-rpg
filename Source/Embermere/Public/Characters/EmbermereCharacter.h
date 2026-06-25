#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/EmbermereTargetable.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UEmbermereCombatComponent;
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
	TObjectPtr<UEmbermereQuestLogComponent> QuestLog;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereRace Race = EEmbermereRace::Human;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	EEmbermereClass Class = EEmbermereClass::Warrior;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rules")
	TObjectPtr<UEmbermereRulesData> RulesData;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Identity")
	void ApplyRaceAndClass(EEmbermereRace NewRace, EEmbermereClass NewClass);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Movement")
	void MoveForward(float Value);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Movement")
	void MoveRight(float Value);

	virtual bool IsAlive_Implementation() const override;
	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void PrimeStarterHotbar();
};
