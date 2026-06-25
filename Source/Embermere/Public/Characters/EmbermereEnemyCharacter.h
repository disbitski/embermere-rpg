#pragma once

#include "CoreMinimal.h"
#include "Characters/EmbermereCharacter.h"
#include "EmbermereEnemyCharacter.generated.h"

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AggroRadius = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackRange = 175.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackDamage = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float AttackCooldownSeconds = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Prototype AI")
	float MoveSpeedCmPerSecond = 165.0f;

	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION()
	void HandleDeath();

	void Respawn();

private:
	FTransform SpawnTransform;
	FTimerHandle RespawnTimerHandle;
	TWeakObjectPtr<AActor> AggroTarget;
	double LastAttackTimeSeconds = -1000.0;

	void UpdatePrototypeAi(float DeltaSeconds);
	AActor* FindAggroTarget() const;
	bool IsValidAggroTarget(const AActor* Candidate) const;
	void FaceTarget(const AActor* Target);
	void MoveTowardTarget(AActor* Target, float DeltaSeconds);
	void TryAttackTarget(AActor* Target);
};
