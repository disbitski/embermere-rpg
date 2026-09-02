#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "EmbermereQuestLedgerRowButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FEmbermereQuestLedgerRowClickedSignature,
	int32,
	QuestIndex);

UCLASS()
class EMBERMERE_API UEmbermereQuestLedgerRowButton : public UButton
{
	GENERATED_BODY()

public:
	UEmbermereQuestLedgerRowButton(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Quest Ledger")
	FEmbermereQuestLedgerRowClickedSignature OnQuestLedgerRowClicked;

	UPROPERTY(BlueprintReadOnly, Category = "Embermere|Quest Ledger")
	int32 QuestIndex = INDEX_NONE;

	void SetQuestIndex(int32 InQuestIndex);

private:
	UFUNCTION()
	void HandleClicked();
};
