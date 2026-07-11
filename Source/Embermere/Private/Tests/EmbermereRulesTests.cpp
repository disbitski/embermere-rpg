#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Characters/EmbermereEnemyCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereHotbarComponent.h"
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
	TestEqual(TEXT("Enemy uses a smooth 24-segment target ring"), FirstEnemy->GetTargetRingSegmentCount(), 24);
	TestTrue(
		TEXT("Enemy target ring uses the Embermere emissive material"),
		FirstEnemy->GetTargetRingMaterialPath().Contains(TEXT("M_EmbermereTargetRing")));
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
	FEmbermereStatsDamageImmunityTest,
	"Embermere.Stats.DamageImmunity",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereStatsDamageImmunityTest::RunTest(const FString& Parameters)
{
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Stats component can be created"), Stats);
	if (!Stats)
	{
		return false;
	}

	Stats->InitializeVitals();
	Stats->GrantDamageImmunity(3.0f);
	TestTrue(TEXT("Damage immunity is active after grant"), Stats->IsDamageImmune());
	TestEqual(TEXT("Damage immunity prevents damage"), Stats->ApplyDamage(25.0f), 0.0f);
	TestEqual(TEXT("Health stays full during immunity"), Stats->CurrentHealth, Stats->MaxHealth);

	Stats->ClearDamageImmunity();
	TestFalse(TEXT("Damage immunity clears"), Stats->IsDamageImmune());
	TestEqual(TEXT("Damage applies after immunity clears"), Stats->ApplyDamage(25.0f), 25.0f);
	TestEqual(TEXT("Health changes after immunity clears"), Stats->CurrentHealth, Stats->MaxHealth - 25.0f);

	Stats->GrantDamageImmunity(3.0f);
	TestTrue(TEXT("Damage immunity can be granted again"), Stats->IsDamageImmune());
	Stats->InitializeVitals();
	TestFalse(TEXT("InitializeVitals clears old immunity"), Stats->IsDamageImmune());

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
	const FString EmptyInventoryText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory empty state is explicit"), EmptyInventoryText.Contains(TEXT("Empty")));
	TestTrue(TEXT("Inventory empty state points rewards into the panel"), EmptyInventoryText.Contains(TEXT("Quest rewards appear here")));
	TestTrue(TEXT("Inventory empty state reports slot capacity"), EmptyInventoryText.Contains(TEXT("Slots 0 / 24")));
	TestTrue(TEXT("Inventory empty state includes close control"), EmptyInventoryText.Contains(TEXT("I Close")));

	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* InventoryOwnerStats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created for inventory actions"), Equipment);
	TestNotNull(TEXT("Stats component can be created for inventory actions"), InventoryOwnerStats);
	if (!Inventory || !Equipment || !InventoryOwnerStats)
	{
		return false;
	}

	UEmbermereItemData* FirstItem = NewObject<UEmbermereItemData>();
	FirstItem->ItemId = "RecruitPack";
	FirstItem->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	FirstItem->Description = FText::FromString(TEXT("A small bundle for new Embermere adventurers."));
	FirstItem->MaxStack = 5;
	FirstItem->Category = EEmbermereItemCategory::Armor;
	FirstItem->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	FirstItem->RequiredLevel = 2;

	UEmbermereItemData* SecondItem = NewObject<UEmbermereItemData>();
	SecondItem->ItemId = "MarshReed";
	SecondItem->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	SecondItem->Description = FText::FromString(TEXT("A damp reed gathered near the prowler marsh."));
	SecondItem->MaxStack = 10;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = InventoryOwnerStats;
	TestTrue(TEXT("First inventory item can be added"), Inventory->AddItem(FirstItem, 1));
	TestTrue(TEXT("Second inventory item can be added"), Inventory->AddItem(SecondItem, 2));
	TestEqual(TEXT("Inventory selection starts at the first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	FString FirstItemDisplayText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory display reports occupied slot capacity"), FirstItemDisplayText.Contains(TEXT("Slots 2 / 24")));
	TestTrue(TEXT("Inventory display shows first stack position"), FirstItemDisplayText.Contains(TEXT("Inspecting 1/2")));
	TestTrue(TEXT("Inventory display marks selected first stack"), FirstItemDisplayText.Contains(TEXT("> Recruit Pack x1")));
	TestTrue(TEXT("Inventory display shows first item stack detail"), FirstItemDisplayText.Contains(TEXT("Stack: 1 / 5")));
	TestTrue(TEXT("Inventory display shows first item description"), FirstItemDisplayText.Contains(TEXT("new Embermere adventurers")));
	TestTrue(TEXT("Inventory display shows item category"), FirstItemDisplayText.Contains(TEXT("Armor")));
	TestTrue(TEXT("Inventory display shows primary action"), FirstItemDisplayText.Contains(TEXT("Action: Equip")));
	TestTrue(TEXT("Inventory display shows equipment slot"), FirstItemDisplayText.Contains(TEXT("Slot: Back")));
	TestTrue(TEXT("Inventory display shows required level"), FirstItemDisplayText.Contains(TEXT("Required level: 2")));
	TestTrue(TEXT("Inventory selection advances to the next stack"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection reports second stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);
	FString SecondItemDisplayText = HudWidget->GetInventoryDisplayText().ToString();
	TestTrue(TEXT("Inventory display shows second stack position"), SecondItemDisplayText.Contains(TEXT("Inspecting 2/2")));
	TestTrue(TEXT("Inventory display marks selected second stack"), SecondItemDisplayText.Contains(TEXT("> Marsh Reed x2")));
	TestTrue(TEXT("Inventory display shows second item stack detail"), SecondItemDisplayText.Contains(TEXT("Stack: 2 / 10")));
	TestTrue(TEXT("Inventory display shows bracket cycling hint for multiple stacks"), SecondItemDisplayText.Contains(TEXT("Use [ ] to inspect")));
	TestTrue(TEXT("Inventory selection wraps forward"), HudWidget->SelectNextInventoryItem(1));
	TestEqual(TEXT("Inventory selection wraps to first stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestTrue(TEXT("Inventory selection wraps backward"), HudWidget->SelectNextInventoryItem(-1));
	TestEqual(TEXT("Inventory selection wraps to last stack"), HudWidget->GetSelectedInventoryStackIndex(), 1);
	TestTrue(TEXT("Direct inventory selection supports mouse-driven rows"), HudWidget->SelectInventoryItem(0));
	TestEqual(TEXT("Direct inventory selection updates the inspected stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestFalse(TEXT("Direct inventory selection rejects an invalid row"), HudWidget->SelectInventoryItem(Inventory->Stacks.Num()));
	TestEqual(TEXT("Invalid direct selection preserves the inspected stack"), HudWidget->GetSelectedInventoryStackIndex(), 0);
	TestFalse(TEXT("Equip action respects item level requirement"), HudWidget->ActivateSelectedInventoryItem());
	InventoryOwnerStats->Level = 2;
	TestTrue(TEXT("Eligible selected item can be equipped"), HudWidget->ActivateSelectedInventoryItem());
	TestTrue(TEXT("Inventory action equips selected item"), Equipment->IsItemEquipped(FirstItem));
	TestEqual(TEXT("Equipped item action changes to Unequip"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Unequip")));
	TestTrue(TEXT("Selected equipped item can be unequipped"), HudWidget->ActivateSelectedInventoryItem());
	TestFalse(TEXT("Inventory action clears equipped state"), Equipment->IsItemEquipped(FirstItem));
	TestEqual(TEXT("Unequipped item action returns to Equip"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Equip")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEquipmentSlotRulesTest,
	"Embermere.Equipment.SlotRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentSlotRulesTest::RunTest(const FString& Parameters)
{
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereItemData* RecruitBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshReed = NewObject<UEmbermereItemData>();
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	if (!Equipment || !RecruitBlade || !MarshBlade || !MarshReed)
	{
		return false;
	}

	RecruitBlade->DisplayName = FText::FromString(TEXT("Recruit Blade"));
	RecruitBlade->Category = EEmbermereItemCategory::Weapon;
	RecruitBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	RecruitBlade->RequiredLevel = 2;
	RecruitBlade->StatBonuses.Power = 3.0f;

	MarshBlade->DisplayName = FText::FromString(TEXT("Marsh Blade"));
	MarshBlade->Category = EEmbermereItemCategory::Weapon;
	MarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;

	MarshReed->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	MarshReed->Category = EEmbermereItemCategory::Misc;

	TestTrue(TEXT("Weapon data reports equippable"), RecruitBlade->IsEquippable());
	TestEqual(TEXT("Weapon primary action is Equip"), RecruitBlade->GetPrimaryActionLabel().ToString(), FString(TEXT("Equip")));
	TestFalse(TEXT("Level requirement rejects low-level character"), Equipment->CanEquip(RecruitBlade, 1));
	TestTrue(TEXT("Level requirement accepts eligible character"), Equipment->CanEquip(RecruitBlade, 2));
	TestFalse(TEXT("Low-level equip attempt fails"), Equipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Eligible equip attempt succeeds"), Equipment->EquipItem(RecruitBlade, 2));
	TestTrue(TEXT("Main-hand slot stores equipped weapon"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestTrue(TEXT("Equipped item query finds weapon"), Equipment->IsItemEquipped(RecruitBlade));

	TestTrue(TEXT("Equipping a replacement weapon succeeds"), Equipment->EquipItem(MarshBlade, 2));
	TestTrue(TEXT("Replacement occupies the same slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestFalse(TEXT("Replaced weapon is no longer equipped"), Equipment->IsItemEquipped(RecruitBlade));
	TestFalse(TEXT("Miscellaneous item cannot be equipped"), Equipment->EquipItem(MarshReed, 2));
	TestTrue(TEXT("Unequip returns the removed weapon"), Equipment->UnequipItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestNull(TEXT("Main-hand slot clears after unequip"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand));
	TestNull(TEXT("Unequipping an empty slot is harmless"), Equipment->UnequipItem(EEmbermereEquipmentSlot::MainHand));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereHotbarCooldownDisplayTest,
	"Embermere.UI.HotbarCooldownDisplay",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereHotbarCooldownDisplayTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereHotbarComponent* Hotbar = NewObject<UEmbermereHotbarComponent>();
	TestNotNull(TEXT("HUD widget can be created for hotbar display"), HudWidget);
	TestNotNull(TEXT("Hotbar component can be created for display"), Hotbar);
	if (!HudWidget || !Hotbar)
	{
		return false;
	}

	FEmbermereAbilityDefinition Strike;
	Strike.AbilityId = "Strike";
	Strike.DisplayName = FText::FromString(TEXT("Strike"));
	Hotbar->SetAbilityInSlot(0, Strike);
	HudWidget->Hotbar = Hotbar;

	const FString ReadyText = HudWidget->GetHotbarSlotDisplayText(0, 0.0f).ToString();
	TestTrue(TEXT("Ready hotbar text includes key and ability"), ReadyText.Contains(TEXT("1\nStrike")));
	TestEqual(TEXT("Ready hotbar text has no countdown line"), ReadyText, FString(TEXT("1\nStrike")));

	const FString CoolingText = HudWidget->GetHotbarSlotDisplayText(0, 1.26f).ToString();
	TestTrue(TEXT("Cooling hotbar text keeps ability label"), CoolingText.Contains(TEXT("Strike")));
	TestTrue(TEXT("Cooling hotbar text rounds countdown for display"), CoolingText.Contains(TEXT("1.3s")));
	TestTrue(TEXT("Interact slot keeps its command label"), HudWidget->GetHotbarSlotDisplayText(9, 0.0f).ToString().Contains(TEXT("F\nInteract")));

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
