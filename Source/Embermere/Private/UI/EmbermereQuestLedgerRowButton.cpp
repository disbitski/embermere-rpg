#include "UI/EmbermereQuestLedgerRowButton.h"

UEmbermereQuestLedgerRowButton::UEmbermereQuestLedgerRowButton(
	const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InitIsFocusable(false);
	OnClicked.AddUniqueDynamic(this, &UEmbermereQuestLedgerRowButton::HandleClicked);
}

void UEmbermereQuestLedgerRowButton::SetQuestIndex(int32 InQuestIndex)
{
	QuestIndex = InQuestIndex;
}

void UEmbermereQuestLedgerRowButton::HandleClicked()
{
	if (QuestIndex != INDEX_NONE)
	{
		OnQuestLedgerRowClicked.Broadcast(QuestIndex);
	}
}
