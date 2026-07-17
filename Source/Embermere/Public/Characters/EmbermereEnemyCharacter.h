#pragma once

#include "CoreMinimal.h"
#include "Characters/EmbermereCharacter.h"
#include "EmbermereEnemyCharacter.generated.h"

class UTextRenderComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class UEmbermereItemData;
class UWidgetComponent;

UCLASS()
class EMBERMERE_API AEmbermereEnemyCharacter : public AEmbermereCharacter
{
	GENERATED_BODY()

public:
	AEmbermereEnemyCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	FText EnemyName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float RespawnDelaySeconds = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot")
	TSoftObjectPtr<UEmbermereItemData> LootItem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot", meta = (ClampMin = "0"))
	int32 LootQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LootDropChance = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AggroRadius = 525.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackRange = 175.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackDamage = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackCooldownSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float MoveSpeedCmPerSecond = 165.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float LeashRadius = 1500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float ReturnHomeRadius = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float ReturnHomeSpeedCmPerSecond = 260.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float NameplateHeight = 225.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float TargetMarkerHeight = 292.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float TargetRingRadius = 96.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float TargetRingHeightOffset = -79.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float TargetRingThickness = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	FLinearColor TargetRingColor = FLinearColor(1.0f, 0.32f, 0.015f, 1.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting")
	float TargetRingRotationSpeedDegreesPerSecond = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Targeting", meta = (ClampMin = "0.0", ClampMax = "0.25"))
	float TargetRingPulseAmount = 0.08f;

	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	bool IsSelectedByPlayer() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	FText GetTargetPresentationText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	FLinearColor GetTargetPresentationColor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	bool HasNameplateWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	int32 GetTargetRingSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Targeting")
	FString GetTargetRingMaterialPath() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|AI")
	bool IsLocationOutsideLeashRadius(const FVector& Location) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|AI")
	bool ShouldReturnHomeFromLocation(const FVector& Location) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Loot")
	bool ShouldDropLoot(float RandomRoll) const;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Loot")
	bool GrantLootTo(AActor* Recipient);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void HandleTargetedByPlayer(bool bIsTargeted) override;

	UFUNCTION()
	void HandleDeath();

	UFUNCTION()
	void HandleHealthChanged(float CurrentHealth, float MaxHealth);

	void Respawn();

private:
	UPROPERTY(VisibleAnywhere, Category = "Enemy|Targeting")
	TObjectPtr<UTextRenderComponent> NameplateText;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Targeting")
	TObjectPtr<UTextRenderComponent> TargetMarkerText;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Targeting")
	TObjectPtr<UWidgetComponent> NameplateWidgetComponent;

	UPROPERTY(VisibleAnywhere, Category = "Enemy|Targeting")
	TArray<TObjectPtr<UStaticMeshComponent>> TargetRingSegments;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> TargetRingMaterials;

	FTransform SpawnTransform;
	FTimerHandle RespawnTimerHandle;
	TWeakObjectPtr<AActor> AggroTarget;
	double LastAttackTimeSeconds = -1000.0;
	bool bSelectedByPlayer = false;
	bool bReturningHome = false;

	void UpdatePrototypeAi(float DeltaSeconds);
	void UpdatePrototypeTargetPresentation();
	void UpdatePrototypeTargetRing(bool bIsVisible);
	AActor* FindAggroTarget() const;
	bool IsValidAggroTarget(const AActor* Candidate) const;
	bool ShouldLeashFromTarget(const AActor* Target) const;
	void DropAggroAndReturnHome();
	void UpdateReturnHome(float DeltaSeconds);
	void FaceTarget(const AActor* Target);
	void MoveTowardTarget(AActor* Target, float DeltaSeconds);
	void TryAttackTarget(AActor* Target);
};
