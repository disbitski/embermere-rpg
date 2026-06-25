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

	virtual bool IsHostileTo_Implementation(const AActor* Viewer) const override;
	virtual FText GetTargetDisplayName_Implementation() const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDeath();

	void Respawn();

private:
	FTransform SpawnTransform;
	FTimerHandle RespawnTimerHandle;
};
