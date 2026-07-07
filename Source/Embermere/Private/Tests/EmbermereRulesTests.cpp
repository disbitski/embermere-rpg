#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Data/EmbermereRulesData.h"
#include "Misc/AutomationTest.h"
#include "UI/EmbermereEnemyNameplateWidget.h"
#include "UI/EmbermerePlayerHudWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereRaceClassRulesTest,
	"Embermere.Rules.RaceClassMatrix",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereRaceClassRulesTest::RunTest(const FString& Parameters)
{
	const UEmbermereRulesData* Rules = NewObject<UEmbermereRulesData>();
	TestNotNull(TEXT("Rules data can be created"), Rules);
	if (!Rules)
	{
		return false;
	}

	TestTrue(TEXT("Human can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Warrior));
	TestTrue(TEXT("Human can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Cleric));
	TestTrue(TEXT("Human can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Ranger));
	TestTrue(TEXT("Human can be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Human, EEmbermereClass::Wizard));

	TestTrue(TEXT("Dwarf can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
	TestTrue(TEXT("Dwarf can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Cleric));
	TestFalse(TEXT("Dwarf cannot be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Ranger));
	TestFalse(TEXT("Dwarf cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Dwarf, EEmbermereClass::Wizard));

	TestTrue(TEXT("Bullywug can be Warrior"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Warrior));
	TestTrue(TEXT("Bullywug can be Cleric"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Cleric));
	TestTrue(TEXT("Bullywug can be Ranger"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Ranger));
	TestFalse(TEXT("Bullywug cannot be Wizard"), Rules->IsClassAllowed(EEmbermereRace::Bullywug, EEmbermereClass::Wizard));

	FEmbermereClassDefinition WarriorDefinition;
	TestTrue(TEXT("Warrior class definition exists"), Rules->GetClassDefinition(EEmbermereClass::Warrior, WarriorDefinition));
	TestEqual(TEXT("Warrior starts with four abilities"), WarriorDefinition.StarterAbilityIds.Num(), 4);
	TestEqual(TEXT("Warrior first hotbar ability is Strike"), WarriorDefinition.StarterAbilityIds[0], FName("Strike"));

	FEmbermereAbilityDefinition StrikeDefinition;
	TestTrue(TEXT("Strike ability definition exists"), Rules->GetAbilityDefinition("Strike", StrikeDefinition));
	TestEqual(TEXT("Strike belongs to Warrior"), StrikeDefinition.OwningClass, EEmbermereClass::Warrior);
	TestEqual(TEXT("Strike targets enemies"), StrikeDefinition.TargetKind, EEmbermereAbilityTargetKind::Enemy);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatTargetSelectionPresentationTest,
	"Embermere.Combat.TargetSelectionPresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatTargetSelectionPresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	AEmbermereEnemyCharacter* FirstEnemy = NewObject<AEmbermereEnemyCharacter>();
	AEmbermereEnemyCharacter* SecondEnemy = NewObject<AEmbermereEnemyCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	TestNotNull(TEXT("First enemy can be created"), FirstEnemy);
	TestNotNull(TEXT("Second enemy can be created"), SecondEnemy);
	if (!Character || !FirstEnemy || !SecondEnemy)
	{
		return false;
	}

	TestNotNull(TEXT("Character has combat"), Character->Combat.Get());
	if (!Character->Combat)
	{
		return false;
	}

	TestFalse(TEXT("First enemy starts unselected"), FirstEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("Second enemy starts unselected"), SecondEnemy->IsSelectedByPlayer());
	TestTrue(TEXT("First enemy has a widget nameplate component"), FirstEnemy->HasNameplateWidget());
	TestTrue(
		TEXT("Enemy target presentation includes name and HP"),
		FirstEnemy->GetTargetPresentationText().ToString().Contains(TEXT("Marsh Prowler\nHP 100/100")));
	const FLinearColor HealthyPresentationColor = FirstEnemy->GetTargetPresentationColor();
	FirstEnemy->Stats->ApplyDamage(80.0f);
	const FLinearColor LowHealthPresentationColor = FirstEnemy->GetTargetPresentationColor();
	TestTrue(TEXT("Enemy target presentation color changes at low health"), LowHealthPresentationColor.R >= HealthyPresentationColor.R);
	TestTrue(TEXT("Enemy low-health presentation color has less green"), LowHealthPresentationColor.G < HealthyPresentationColor.G);

	Character->Combat->SetTarget(FirstEnemy);
	TestTrue(TEXT("First enemy becomes current target"), Character->Combat->CurrentTarget == FirstEnemy);
	TestTrue(TEXT("First enemy presentation is selected"), FirstEnemy->IsSelectedByPlayer());
	TestFalse(TEXT("Second enemy remains unselected"), SecondEnemy->IsSelectedByPlayer());

	Character->Combat->SetTarget(SecondEnemy);
	TestTrue(TEXT("Second enemy becomes current target"), Character->Combat->CurrentTarget == SecondEnemy);
	TestFalse(TEXT("First enemy presentation clears when target changes"), FirstEnemy->IsSelectedByPlayer());
	TestTrue(TEXT("Second enemy presentation is selected"), SecondEnemy->IsSelectedByPlayer());

	Character->Combat->SetTarget(nullptr);
	TestNull(TEXT("Current target clears when target is cleared"), Character->Combat->CurrentTarget.Get());
	TestFalse(TEXT("Second enemy presentation clears when target clears"), SecondEnemy->IsSelectedByPlayer());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereCombatDeadCasterRejectedTest,
	"Embermere.Combat.DeadCasterRejected",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereCombatDeadCasterRejectedTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	if (!Character || !Character->Combat || !Character->Stats)
	{
		return false;
	}

	FEmbermereAbilityDefinition RecoveryFocus;
	RecoveryFocus.AbilityId = "RecoveryFocus";
	RecoveryFocus.DisplayName = FText::FromString(TEXT("Recovery Focus"));
	RecoveryFocus.TargetKind = EEmbermereAbilityTargetKind::Self;
	RecoveryFocus.Power = 1.0f;
	RecoveryFocus.ManaCost = 5.0f;
	RecoveryFocus.Cooldown = 1.0f;

	Character->Stats->InitializeVitals();
	const float ManaBeforeDeadCast = Character->Stats->CurrentMana;
	Character->Stats->ApplyDamage(Character->Stats->MaxHealth);
	TestTrue(TEXT("Character is dead before rejected cast"), Character->Stats->IsDead());
	TestFalse(TEXT("Dead character cannot execute a self ability"), Character->Combat->ExecuteAbility(RecoveryFocus));
	TestEqual(TEXT("Dead character does not spend mana"), Character->Stats->CurrentMana, ManaBeforeDeadCast);

	Character->Stats->InitializeVitals();
	TestFalse(TEXT("Character is alive after recovery"), Character->Stats->IsDead());
	TestTrue(TEXT("Recovered character can execute a self ability"), Character->Combat->ExecuteAbility(RecoveryFocus));
	TestEqual(TEXT("Recovered character spends mana"), Character->Stats->CurrentMana, Character->Stats->MaxMana - RecoveryFocus.ManaCost);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyNameplateWidgetTest,
	"Embermere.UI.EnemyNameplateWidget",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyNameplateWidgetTest::RunTest(const FString& Parameters)
{
	UEmbermereEnemyNameplateWidget* NameplateWidget = NewObject<UEmbermereEnemyNameplateWidget>();
	TestNotNull(TEXT("Enemy nameplate widget can be created"), NameplateWidget);
	if (!NameplateWidget)
	{
		return false;
	}

	TestFalse(TEXT("Nameplate starts unselected"), NameplateWidget->IsNameplateSelected());
	NameplateWidget->SetNameplateState(
		FText::FromString(TEXT("Marsh Prowler")),
		45.0f,
		100.0f,
		FLinearColor(1.0f, 0.72f, 0.24f, 1.0f),
		true);
	TestTrue(TEXT("Nameplate reports selected after selected state"), NameplateWidget->IsNameplateSelected());

	NameplateWidget->SetNameplateState(
		FText::FromString(TEXT("Marsh Prowler")),
		0.0f,
		100.0f,
		FLinearColor(1.0f, 0.28f, 0.18f, 1.0f),
		false);
	TestFalse(TEXT("Nameplate reports unselected after hidden state"), NameplateWidget->IsNameplateSelected());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyLeashRulesTest,
	"Embermere.Enemy.LeashRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyLeashRulesTest::RunTest(const FString& Parameters)
{
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	TestNotNull(TEXT("Enemy can be created"), Enemy);
	if (!Enemy)
	{
		return false;
	}

	Enemy->LeashRadius = 1000.0f;
	Enemy->ReturnHomeRadius = 120.0f;

	TestFalse(TEXT("Enemy location inside leash stays valid"), Enemy->IsLocationOutsideLeashRadius(FVector(500.0f, 0.0f, 0.0f)));
	TestTrue(TEXT("Enemy location outside leash is detected"), Enemy->IsLocationOutsideLeashRadius(FVector(1200.0f, 0.0f, 0.0f)));
	TestFalse(TEXT("Enemy at home does not need return"), Enemy->ShouldReturnHomeFromLocation(FVector(80.0f, 0.0f, 0.0f)));
	TestTrue(TEXT("Enemy away from home should return"), Enemy->ShouldReturnHomeFromLocation(FVector(180.0f, 0.0f, 0.0f)));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryHudToggleTest,
	"Embermere.UI.InventoryToggle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryHudToggleTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	if (!HudWidget)
	{
		return false;
	}

	TestTrue(TEXT("Inventory panel starts visible"), HudWidget->IsInventoryPanelVisible());
	TestFalse(TEXT("First toggle hides inventory"), HudWidget->ToggleInventoryPanel());
	TestFalse(TEXT("Inventory panel reports hidden"), HudWidget->IsInventoryPanelVisible());
	TestTrue(TEXT("Second toggle shows inventory"), HudWidget->ToggleInventoryPanel());
	TestTrue(TEXT("Inventory panel reports visible"), HudWidget->IsInventoryPanelVisible());

	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	if (!Inventory)
	{
		return false;
	}

	UEmbermereItemData* FirstItem = NewObject<UEmbermereItemData>();
	FirstItem->ItemId = "RecruitPack";
	FirstItem->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	FirstItem->MaxStack = 5;

	UEmbermereItemData* SecondItem = NewObject<UEmbermereItemData>();
	SecondItem->ItemId = "MarshReed";
	SecondItem->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	SecondItem->MaxStack = 10;

	HudWidget->Inventory = Inventory;
	TestTrue(TEXT("First inventory item can be added"), Inventory->AddItem(FirstItem, 1));
	TestTrue(TEXT("Second inventory item can be added"), Inventory->AddItem(SecondItem, 1));
	TestEqual(TEXT("Inventory selection starts at the first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestTrue(TEXT("Inventory selection advances to the next stack"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection reports second stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);
	TestTrue(TEXT("Inventory selection wraps forward"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection wraps to first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestTrue(TEXT("Inventory selection wraps backward"), HudWidget->SelectNextInventoryItem(-1));
	TestEqual(TEXT("Inventory selection wraps to last stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereHudChatLogTest,
	"Embermere.UI.ChatLog",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereHudChatLogTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	if (!HudWidget)
	{
		return false;
	}

	TestEqual(TEXT("Chat log starts empty"), HudWidget->GetChatMessageCount(), 0);
	HudWidget->AddChatMessage(FText::FromString(TEXT("Marsh Prowler hits you for 4")), FLinearColor::Red);
	TestEqual(TEXT("Chat log stores a posted combat line"), HudWidget->GetChatMessageCount(), 1);

	for (int32 Index = 0; Index < 10; ++Index)
	{
		HudWidget->AddChatMessage(FText::FromString(FString::Printf(TEXT("Message %d"), Index)), FLinearColor::White);
	}
	TestEqual(TEXT("Chat log keeps the configured number of recent messages"), HudWidget->GetChatMessageCount(), HudWidget->GetChatMessageLimit());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereQuestRewardTest,
	"Embermere.Quests.CompletionRewards",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestRewardTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestNotNull(TEXT("Character can be created"), Character);
	if (!Character)
	{
		return false;
	}

	TestNotNull(TEXT("Character has stats"), Character->Stats.Get());
	TestNotNull(TEXT("Character has inventory"), Character->Inventory.Get());
	TestNotNull(TEXT("Character has quest log"), Character->QuestLog.Get());
	if (!Character->Stats || !Character->Inventory || !Character->QuestLog)
	{
		return false;
	}

	UEmbermereItemData* RewardItem = NewObject<UEmbermereItemData>();
	RewardItem->ItemId = "RecruitPack";
	RewardItem->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RewardItem->MaxStack = 5;

	UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
	Quest->QuestId = "FirstSignsAtTheRuin";
	Quest->Title = FText::FromString(TEXT("First Signs at the Ruin"));
	Quest->ObjectiveId = "StarterEnemyDefeated";
	Quest->RequiredObjectiveCount = 2;
	Quest->RewardExperience = 75;
	Quest->RewardItem = RewardItem;

	Character->Stats->InitializeVitals();

	TestTrue(TEXT("Quest can be accepted"), Character->QuestLog->AcceptQuest(Quest));
	TestTrue(TEXT("First objective progress is accepted"), Character->QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1));
	TestFalse(TEXT("Quest cannot complete early"), Character->QuestLog->TryCompleteActiveQuest());
	TestTrue(TEXT("Second objective progress is accepted"), Character->QuestLog->AddObjectiveProgress("StarterEnemyDefeated", 1));
	TestTrue(TEXT("Quest completes when objective count is met"), Character->QuestLog->TryCompleteActiveQuest());

	TestTrue(TEXT("Quest state is completed"), Character->QuestLog->ActiveQuest.bCompleted);
	TestEqual(TEXT("XP reward is granted"), Character->Stats->CurrentExperience, 75);
	TestEqual(TEXT("Reward item creates one inventory stack"), Character->Inventory->Stacks.Num(), 1);
	if (Character->Inventory->Stacks.Num() > 0)
	{
		TestTrue(TEXT("Reward stack contains reward item"), Character->Inventory->Stacks[0].Item == RewardItem);
		TestEqual(TEXT("Reward stack quantity is one"), Character->Inventory->Stacks[0].Quantity, 1);
	}

	TestFalse(TEXT("Completed quest cannot complete again"), Character->QuestLog->TryCompleteActiveQuest());

	return true;
}

#endif
