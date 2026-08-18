#pragma once

#include "CoreMinimal.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "EmbermerePracticeTargetActor.generated.h"

UCLASS()
class EMBERMERE_API AEmbermerePracticeTargetActor : public AEmbermereEnemyCharacter
{
	GENERATED_BODY()

public:
	AEmbermerePracticeTargetActor();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Practice Target")
	void ResetPracticeTarget();

protected:
	virtual void BeginPlay() override;

private:
	void FreezeAtAuthoredTransform();
};
