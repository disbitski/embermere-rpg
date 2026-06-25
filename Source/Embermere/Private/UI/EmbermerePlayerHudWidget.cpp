#include "UI/EmbermerePlayerHudWidget.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"

void UEmbermerePlayerHudWidget::BindToCharacter(AEmbermereCharacter* Character)
{
	OwningEmbermereCharacter = Character;
	Stats = Character ? Character->Stats : nullptr;
	Combat = Character ? Character->Combat : nullptr;
	Hotbar = Character ? Character->Hotbar : nullptr;
	QuestLog = Character ? Character->QuestLog : nullptr;
	Inventory = Character ? Character->Inventory : nullptr;
}
