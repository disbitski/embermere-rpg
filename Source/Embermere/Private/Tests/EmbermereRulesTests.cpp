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
#include "Game/EmbermerePlayerController.h"
#include "Misc/AutomationTest.h"
#include "UI/EmbermereEnemyNameplateWidget.h"
#include "UI/EmbermereItemDragDropOperation.h"
#include "UI/EmbermerePlayerHudWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereAutorunCancellationTest,
	"Embermere.Input.AutorunCancellation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereAutorunCancellationTest::RunTest(const FString& Parameters)
{
	AEmbermerePlayerController* Controller = NewObject<AEmbermerePlayerController>();
	TestNotNull(TEXT("Player controller can be created"), Controller);
	if (!Controller)
	{
		return false;
	}

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(1.0f);
	TestFalse(TEXT("Forward input cancels autorun"), Controller->bAutorunEnabled);

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(-1.0f);
	TestFalse(TEXT("Backward input cancels autorun"), Controller->bAutorunEnabled);

	Controller->bAutorunEnabled = true;
	Controller->NotifyManualMoveForwardInput(0.0f);
	TestTrue(TEXT("Idle axis input leaves autorun enabled"), Controller->bAutorunEnabled);

	return true;
}

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

	TestEqual(TEXT("Starter enemy default aggro supports separated pulls"), Enemy->AggroRadius, 525.0f);
	TestEqual(TEXT("Starter enemy keeps first-pass attack damage"), Enemy->AttackDamage, 6.0f);
	TestEqual(TEXT("Starter enemy keeps first-pass attack cadence"), Enemy->AttackCooldownSeconds, 2.0f);

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
	FirstItem->StatBonuses.MaxHealth = 5.0f;
	FirstItem->StatBonuses.Armor = 1.0f;

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
	TestTrue(TEXT("Inventory display shows item stat summary"), FirstItemDisplayText.Contains(TEXT("+5 HP")) && FirstItemDisplayText.Contains(TEXT("+1 Armor")));
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
	TestEqual(TEXT("Equipped item leaves the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Remaining bag stack is preserved"), Inventory->Stacks[0].Item == SecondItem);
	const FString EquipmentText = HudWidget->GetEquipmentDisplayText().ToString();
	TestTrue(TEXT("Equipment panel shows Back-slot item"), EquipmentText.Contains(TEXT("Back")) && EquipmentText.Contains(TEXT("Recruit Pack")));
	TestTrue(TEXT("Equipment panel shows aggregate health bonus"), EquipmentText.Contains(TEXT("HP +5")));
	TestTrue(TEXT("Equipment panel shows aggregate armor bonus"), EquipmentText.Contains(TEXT("Armor +1")));
	TestTrue(TEXT("Occupied Back slot can be activated to unequip"), HudWidget->ActivateEquipmentSlot(EEmbermereEquipmentSlot::Back));
	TestFalse(TEXT("Equipment slot action clears equipped state"), Equipment->IsItemEquipped(FirstItem));
	TestEqual(TEXT("Unequipped item returns to the bag"), Inventory->Stacks.Num(), 2);
	TestTrue(TEXT("Returned item is appended to the bag"), Inventory->Stacks[1].Item == FirstItem);
	TestTrue(TEXT("Returned item can be selected"), HudWidget->SelectInventoryItem(1));
	TestEqual(TEXT("Unequipped item action returns to Equip"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Equip")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryItemComparisonTest,
	"Embermere.UI.ItemComparison",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryItemComparisonTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	if (!HudWidget || !Inventory || !Equipment)
	{
		return false;
	}

	UEmbermereItemData* WeatheredCloak = NewObject<UEmbermereItemData>();
	WeatheredCloak->ItemId = "WeatheredCloak";
	WeatheredCloak->DisplayName = FText::FromString(TEXT("Weathered Cloak"));
	WeatheredCloak->Category = EEmbermereItemCategory::Armor;
	WeatheredCloak->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	WeatheredCloak->StatBonuses.MaxHealth = 2.0f;
	WeatheredCloak->StatBonuses.Armor = 2.0f;

	UEmbermereItemData* FenwatchMantle = NewObject<UEmbermereItemData>();
	FenwatchMantle->ItemId = "FenwatchMantle";
	FenwatchMantle->DisplayName = FText::FromString(TEXT("Fenwatch Mantle"));
	FenwatchMantle->Description = FText::FromString(TEXT("A moss-green mantle issued to trusted Fenwatch scouts."));
	FenwatchMantle->Category = EEmbermereItemCategory::Armor;
	FenwatchMantle->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	FenwatchMantle->RequiredLevel = 2;
	FenwatchMantle->StatBonuses.MaxHealth = 5.0f;
	FenwatchMantle->StatBonuses.MaxMana = 1.0f;
	FenwatchMantle->StatBonuses.Armor = 1.0f;

	UEmbermereItemData* MarshTonic = NewObject<UEmbermereItemData>();
	MarshTonic->ItemId = "MarshTonic";
	MarshTonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	MarshTonic->Description = FText::FromString(TEXT("A sharp herbal draught."));
	MarshTonic->Category = EEmbermereItemCategory::Consumable;
	MarshTonic->MaxStack = 5;
	MarshTonic->ConsumableEffects.HealHealth = 25.0f;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	TestTrue(TEXT("Baseline cloak can be equipped"), Equipment->EquipItem(WeatheredCloak, 1));
	TestTrue(TEXT("Comparison candidate can be added"), Inventory->AddItem(FenwatchMantle, 1));
	TestTrue(TEXT("Consumable can be added"), Inventory->AddItem(MarshTonic, 2));

	const FString ComparisonText = HudWidget->GetSelectedInventoryComparisonText().ToString();
	TestTrue(TEXT("Comparison names the equipped item"), ComparisonText.Contains(TEXT("Vs Weathered Cloak")));
	TestTrue(TEXT("Comparison reports a health upgrade"), ComparisonText.Contains(TEXT("HP +3")));
	TestTrue(TEXT("Comparison reports a mana upgrade"), ComparisonText.Contains(TEXT("Mana +1")));
	TestTrue(TEXT("Comparison reports an armor downgrade"), ComparisonText.Contains(TEXT("Armor -1")));

	const FString TooltipText = HudWidget->GetSelectedInventoryTooltipText().ToString();
	TestTrue(TEXT("Tooltip names the inspected item"), TooltipText.Contains(TEXT("Fenwatch Mantle x1")));
	TestTrue(TEXT("Tooltip includes slot and level"), TooltipText.Contains(TEXT("Back | Level 2")));
	TestTrue(TEXT("Tooltip includes the item's effects"), TooltipText.Contains(TEXT("+5 HP")));
	TestTrue(TEXT("Tooltip includes equipment comparison"), TooltipText.Contains(TEXT("Vs Weathered Cloak")));
	TestTrue(TEXT("Tooltip includes item description"), TooltipText.Contains(TEXT("trusted Fenwatch scouts")));
	TestTrue(TEXT("Inventory detail text includes comparison"), HudWidget->GetInventoryDisplayText().ToString().Contains(TEXT("Armor -1")));

	TestNotNull(TEXT("Equipped item can be removed for empty-slot comparison"), Equipment->UnequipItem(EEmbermereEquipmentSlot::Back));
	const FString EmptySlotComparison = HudWidget->GetSelectedInventoryComparisonText().ToString();
	TestTrue(TEXT("Empty-slot comparison names the destination slot"), EmptySlotComparison.Contains(TEXT("Vs empty Back")));
	TestTrue(TEXT("Empty-slot comparison reports candidate armor"), EmptySlotComparison.Contains(TEXT("Armor +1")));

	TestTrue(TEXT("Consumable can be selected"), HudWidget->SelectInventoryItem(1));
	TestTrue(TEXT("Consumable has no equipment comparison"), HudWidget->GetSelectedInventoryComparisonText().IsEmpty());
	const FString ConsumableTooltip = HudWidget->GetSelectedInventoryTooltipText().ToString();
	TestTrue(TEXT("Consumable tooltip reports quantity"), ConsumableTooltip.Contains(TEXT("Marsh Tonic x2")));
	TestTrue(TEXT("Consumable tooltip reports its effect"), ConsumableTooltip.Contains(TEXT("Restores 25 HP")));
	TestFalse(TEXT("Consumable tooltip omits equipment comparison"), ConsumableTooltip.Contains(TEXT("Vs ")));

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryIdentityActionsTest,
	"Embermere.Inventory.IdentityActions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryIdentityActionsTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	TestNotNull(TEXT("Stats component can be created"), Stats);
	if (!HudWidget || !Inventory || !Equipment || !Stats)
	{
		return false;
	}

	UEmbermereItemData* RecruitPack = NewObject<UEmbermereItemData>();
	RecruitPack->ItemId = "RecruitPack";
	RecruitPack->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RecruitPack->Category = EEmbermereItemCategory::Armor;
	RecruitPack->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	RecruitPack->RequiredLevel = 1;
	RecruitPack->StatBonuses.MaxHealth = 5.0f;

	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = Stats;
	Stats->Level = 1;
	TestTrue(TEXT("Identity-action item can be added"), Inventory->AddItem(RecruitPack, 1));

	TestFalse(
		TEXT("Wrong-slot request is rejected"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Chest));
	TestEqual(TEXT("Wrong-slot rejection preserves bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestNull(TEXT("Wrong-slot rejection preserves empty Back slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back));

	TestTrue(
		TEXT("Matching-slot request equips through the identity action"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Successful identity action removes bag item"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Successful identity action equips requested item"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	TestFalse(
		TEXT("Stale item payload is rejected after item leaves bag"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Stale payload does not duplicate item into bag"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Stale payload leaves equipment intact"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	TestTrue(TEXT("A second copy of equipped gear can remain in the bag"), Inventory->AddItem(RecruitPack, 1));
	TestFalse(
		TEXT("Already-equipped item definition is a rejected no-op"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Rejected duplicate equip preserves bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestTrue(TEXT("Rejected duplicate equip preserves equipped item"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryDragDropTest,
	"Embermere.UI.InventoryDragDrop",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryDragDropTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereItemData* RecruitPack = NewObject<UEmbermereItemData>();
	TestNotNull(TEXT("HUD widget can be created"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created"), Inventory);
	TestNotNull(TEXT("Equipment component can be created"), Equipment);
	TestNotNull(TEXT("Stats component can be created"), Stats);
	TestNotNull(TEXT("Dragged item can be created"), RecruitPack);
	if (!HudWidget || !Inventory || !Equipment || !Stats || !RecruitPack)
	{
		return false;
	}

	RecruitPack->ItemId = "RecruitPack";
	RecruitPack->DisplayName = FText::FromString(TEXT("Recruit Pack"));
	RecruitPack->Category = EEmbermereItemCategory::Armor;
	RecruitPack->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	RecruitPack->RequiredLevel = 2;
	RecruitPack->MaxStack = 1;
	HudWidget->Inventory = Inventory;
	HudWidget->Equipment = Equipment;
	HudWidget->Stats = Stats;
	Stats->Level = 1;
	TestTrue(TEXT("Dragged gear can be added to the bag"), Inventory->AddItem(RecruitPack, 1));

	UEmbermereItemDragDropOperation* Operation = NewObject<UEmbermereItemDragDropOperation>();
	Operation->Item = RecruitPack;
	Operation->Source = EEmbermereItemDragSource::Inventory;
	TestTrue(TEXT("Typed drag payload preserves item identity"), Operation->Item == RecruitPack);
	TestEqual(TEXT("Typed drag payload records bag source"), Operation->Source, EEmbermereItemDragSource::Inventory);
	TestEqual(TEXT("Armor drag visual uses an armor sigil"), Operation->GetVisualSigilText().ToString(), FString(TEXT("ARM")));
	TestEqual(
		TEXT("Armor drag visual includes slot and level context"),
		Operation->GetVisualContextText().ToString(),
		FString(TEXT("Back | Level 2")));
	TestTrue(TEXT("Armor drag visual uses a warm fantasy accent"), Operation->GetVisualAccentColor().R > 0.9f);

	TestFalse(
		TEXT("Wrong equipment slot is not a valid drop target"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Chest));
	TestFalse(
		TEXT("Under-level character sees the matching slot as invalid"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	Stats->Level = 2;
	TestTrue(
		TEXT("Eligible matching slot is a valid drop target"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Valid drop equips through the atomic transaction"),
		HudWidget->EquipInventoryItemToSlot(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Equipped drag removes bag quantity"), Inventory->GetItemQuantity(RecruitPack), 0);
	TestTrue(TEXT("Equipped drag occupies Back slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);
	TestFalse(
		TEXT("Stale bag payload is rejected after the item moves"),
		HudWidget->CanDropInventoryItemOnEquipmentSlot(RecruitPack, EEmbermereEquipmentSlot::Back));

	UEmbermereItemData* StaleItem = NewObject<UEmbermereItemData>();
	StaleItem->Category = EEmbermereItemCategory::Armor;
	StaleItem->EquipmentSlot = EEmbermereEquipmentSlot::Back;
	TestFalse(
		TEXT("Mismatched equipped-item identity cannot return another item"),
		HudWidget->ReturnEquipmentItemToInventory(StaleItem, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Equipped item can return to an available bag"),
		HudWidget->CanReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Equipment-to-bag drop uses the atomic unequip transaction"),
		HudWidget->ReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestEqual(TEXT("Returned item restores bag quantity"), Inventory->GetItemQuantity(RecruitPack), 1);
	TestNull(TEXT("Returned item clears equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::Back));

	UEmbermerePlayerHudWidget* FullBagHud = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* FullBag = NewObject<UEmbermereInventoryComponent>();
	UEmbermereEquipmentComponent* FullBagEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereItemData* Filler = NewObject<UEmbermereItemData>();
	Filler->DisplayName = FText::FromString(TEXT("Filler"));
	Filler->MaxStack = 1;
	FullBag->MaxSlots = 1;
	FullBagHud->Inventory = FullBag;
	FullBagHud->Equipment = FullBagEquipment;
	TestTrue(TEXT("Full-bag fixture equips gear"), FullBagEquipment->EquipItem(RecruitPack, 2));
	TestTrue(TEXT("Full-bag fixture fills its only slot"), FullBag->AddItem(Filler, 1));
	TestFalse(
		TEXT("Full bag rejects equipment-to-bag drop preflight"),
		FullBagHud->CanReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestFalse(
		TEXT("Full bag rejects the actual equipment-to-bag drop"),
		FullBagHud->ReturnEquipmentItemToInventory(RecruitPack, EEmbermereEquipmentSlot::Back));
	TestTrue(
		TEXT("Rejected full-bag drop preserves equipped item"),
		FullBagEquipment->GetEquippedItem(EEmbermereEquipmentSlot::Back) == RecruitPack);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryStableSortingTest,
	"Embermere.Inventory.StableSorting",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryStableSortingTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	TestNotNull(TEXT("HUD widget can be created for inventory sorting"), HudWidget);
	TestNotNull(TEXT("Inventory component can be created for sorting"), Inventory);
	if (!HudWidget || !Inventory)
	{
		return false;
	}

	auto MakeItem = [](const TCHAR* ItemId, const TCHAR* DisplayName, EEmbermereItemCategory Category)
	{
		UEmbermereItemData* Item = NewObject<UEmbermereItemData>();
		Item->ItemId = FName(ItemId);
		Item->DisplayName = FText::FromString(DisplayName);
		Item->Category = Category;
		Item->MaxStack = 1;
		return Item;
	};

	UEmbermereItemData* AncientPebble = MakeItem(TEXT("AncientPebble"), TEXT("Ancient Pebble"), EEmbermereItemCategory::Misc);
	UEmbermereItemData* MarshTonic = MakeItem(TEXT("MarshTonic"), TEXT("Marsh Tonic"), EEmbermereItemCategory::Consumable);
	UEmbermereItemData* FenwatchSeal = MakeItem(TEXT("FenwatchSeal"), TEXT("Fenwatch Seal"), EEmbermereItemCategory::Quest);
	UEmbermereItemData* RecruitPack = MakeItem(TEXT("RecruitPack"), TEXT("Recruit Pack"), EEmbermereItemCategory::Armor);
	UEmbermereItemData* AshenBlade = MakeItem(TEXT("AshenBlade"), TEXT("Ashen Blade"), EEmbermereItemCategory::Weapon);
	UEmbermereItemData* BitterDraught = MakeItem(TEXT("BitterDraught"), TEXT("Bitter Draught"), EEmbermereItemCategory::Consumable);

	HudWidget->Inventory = Inventory;
	TestTrue(TEXT("Misc item enters the unsorted bag"), Inventory->AddItem(AncientPebble, 1));
	TestTrue(TEXT("Two identical consumable stacks enter the unsorted bag"), Inventory->AddItem(MarshTonic, 2));
	TestTrue(TEXT("Quest item enters the unsorted bag"), Inventory->AddItem(FenwatchSeal, 1));
	TestTrue(TEXT("Armor enters the unsorted bag"), Inventory->AddItem(RecruitPack, 1));
	TestTrue(TEXT("Weapon enters the unsorted bag"), Inventory->AddItem(AshenBlade, 1));
	TestTrue(TEXT("Alphabetically earlier consumable enters last"), Inventory->AddItem(BitterDraught, 1));
	TestEqual(TEXT("Sorting fixture has seven stacks"), Inventory->Stacks.Num(), 7);

	TestTrue(TEXT("Second duplicate stack can be selected"), HudWidget->SelectInventoryItem(2));
	TestTrue(TEXT("Mixed bag reports a changed order"), HudWidget->SortInventory());

	const TArray<UEmbermereItemData*> ExpectedOrder = {
		AshenBlade,
		RecruitPack,
		BitterDraught,
		MarshTonic,
		MarshTonic,
		FenwatchSeal,
		AncientPebble
	};
	for (int32 Index = 0; Index < ExpectedOrder.Num(); ++Index)
	{
		TestTrue(
			*FString::Printf(TEXT("Sorted stack %d has expected category/name order"), Index),
			Inventory->Stacks[Index].Item == ExpectedOrder[Index]);
	}

	TestEqual(
		TEXT("Selection follows the second identical stack by item occurrence"),
		HudWidget->GetSelectedInventoryStackIndex(),
		4);
	TestTrue(
		TEXT("Selected stack still points to the same item identity"),
		Inventory->Stacks[HudWidget->GetSelectedInventoryStackIndex()].Item == MarshTonic);
	TestTrue(
		TEXT("Stable sorting preserves the first identical stack before the selected duplicate"),
		Inventory->Stacks[3].Item == MarshTonic);

	TestFalse(TEXT("Sorting an already ordered bag is a no-op"), HudWidget->SortInventory());
	TestEqual(
		TEXT("No-op sorting preserves selected duplicate occurrence"),
		HudWidget->GetSelectedInventoryStackIndex(),
		4);

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
	TestEqual(TEXT("Equipment aggregates weapon power"), Equipment->GetTotalStatBonuses().Power, 3.0f);

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
	FEmbermereEquipmentInventoryTransactionsTest,
	"Embermere.Equipment.InventoryTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentInventoryTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereEquipmentComponent* Equipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* RecruitBlade = NewObject<UEmbermereItemData>();
	UEmbermereItemData* MarshBlade = NewObject<UEmbermereItemData>();
	if (!Equipment || !Inventory || !RecruitBlade || !MarshBlade)
	{
		AddError(TEXT("Could not create equipment transaction test objects"));
		return false;
	}

	RecruitBlade->DisplayName = FText::FromString(TEXT("Recruit Blade"));
	RecruitBlade->Category = EEmbermereItemCategory::Weapon;
	RecruitBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	MarshBlade->DisplayName = FText::FromString(TEXT("Marsh Blade"));
	MarshBlade->Category = EEmbermereItemCategory::Weapon;
	MarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;

	TestTrue(TEXT("Recruit weapon enters the bag"), Inventory->AddItem(RecruitBlade, 1));
	TestTrue(TEXT("Replacement weapon enters the bag"), Inventory->AddItem(MarshBlade, 1));
	TestTrue(TEXT("Equipping transfers the recruit weapon out of the bag"), Equipment->EquipFromInventory(RecruitBlade, 1, Inventory));
	TestTrue(TEXT("Recruit weapon occupies its equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Only the replacement remains in the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Replacement stack is preserved"), Inventory->Stacks[0].Item == MarshBlade);

	TestTrue(TEXT("Equipping a replacement succeeds transactionally"), Equipment->EquipFromInventory(MarshBlade, 1, Inventory));
	TestTrue(TEXT("Replacement occupies the slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == MarshBlade);
	TestEqual(TEXT("Replaced item returns to the bag"), Inventory->Stacks.Num(), 1);
	TestTrue(TEXT("Returned bag item is the recruit weapon"), Inventory->Stacks[0].Item == RecruitBlade);
	TestTrue(TEXT("Unequip transfers the replacement back to the bag"), Equipment->UnequipToInventory(EEmbermereEquipmentSlot::MainHand, Inventory));
	TestNull(TEXT("Unequip clears the equipment slot"), Equipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand));
	TestEqual(TEXT("Both items are in the bag after unequip"), Inventory->Stacks.Num(), 2);

	UEmbermereEquipmentComponent* FullBagEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* FullBagInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* Blocker = NewObject<UEmbermereItemData>();
	FullBagInventory->MaxSlots = 1;
	Blocker->DisplayName = FText::FromString(TEXT("Packed Supplies"));
	TestTrue(TEXT("Full-bag fixture equips a weapon directly"), FullBagEquipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Full-bag fixture fills its only slot"), FullBagInventory->AddItem(Blocker, 1));
	TestFalse(TEXT("Full bag rejects unequip"), FullBagEquipment->UnequipToInventory(EEmbermereEquipmentSlot::MainHand, FullBagInventory));
	TestTrue(TEXT("Rejected unequip leaves the weapon equipped"), FullBagEquipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Rejected unequip leaves the bag unchanged"), FullBagInventory->Stacks.Num(), 1);

	UEmbermereEquipmentComponent* RollbackEquipment = NewObject<UEmbermereEquipmentComponent>();
	UEmbermereInventoryComponent* RollbackInventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* StackableMarshBlade = NewObject<UEmbermereItemData>();
	RollbackInventory->MaxSlots = 1;
	StackableMarshBlade->DisplayName = FText::FromString(TEXT("Stackable Marsh Blade"));
	StackableMarshBlade->Category = EEmbermereItemCategory::Weapon;
	StackableMarshBlade->EquipmentSlot = EEmbermereEquipmentSlot::MainHand;
	StackableMarshBlade->MaxStack = 2;
	TestTrue(TEXT("Rollback fixture equips the original weapon"), RollbackEquipment->EquipItem(RecruitBlade, 1));
	TestTrue(TEXT("Rollback fixture fills its bag stack"), RollbackInventory->AddItem(StackableMarshBlade, 2));
	TestFalse(TEXT("Replacement fails when the old item cannot return to the bag"), RollbackEquipment->EquipFromInventory(StackableMarshBlade, 1, RollbackInventory));
	TestTrue(TEXT("Failed replacement preserves the original equipment"), RollbackEquipment->GetEquippedItem(EEmbermereEquipmentSlot::MainHand) == RecruitBlade);
	TestEqual(TEXT("Failed replacement restores the candidate stack"), RollbackInventory->Stacks[0].Quantity, 2);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEquipmentStatApplicationTest,
	"Embermere.Equipment.StatApplication",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEquipmentStatApplicationTest::RunTest(const FString& Parameters)
{
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	TestNotNull(TEXT("Stats component can be created for equipment bonuses"), Stats);
	if (!Stats)
	{
		return false;
	}

	Stats->InitializeVitals();
	FEmbermereItemStatBonuses Bonuses;
	Bonuses.MaxHealth = 20.0f;
	Bonuses.MaxMana = 10.0f;
	Bonuses.Armor = 25.0f;
	Bonuses.Power = 3.0f;
	Stats->ApplyEquipmentBonuses(Bonuses);

	TestEqual(TEXT("Equipment increases maximum health"), Stats->MaxHealth, 120.0f);
	TestEqual(TEXT("Equipment increases current health while preserving missing health"), Stats->CurrentHealth, 120.0f);
	TestEqual(TEXT("Equipment increases maximum mana"), Stats->MaxMana, 60.0f);
	TestEqual(TEXT("Equipment increases attack power"), Stats->AttackPower, 13.0f);
	TestEqual(TEXT("Equipment applies armor"), Stats->Armor, 25.0f);

	Stats->ApplyEquipmentBonuses(Bonuses);
	TestEqual(TEXT("Reapplying identical equipment bonuses is idempotent"), Stats->MaxHealth, 120.0f);
	TestTrue(TEXT("Armor mitigates incoming damage"), FMath::IsNearlyEqual(Stats->ApplyDamage(25.0f), 20.0f));

	Stats->ApplyEquipmentBonuses(FEmbermereItemStatBonuses());
	TestEqual(TEXT("Removing gear restores base maximum health"), Stats->MaxHealth, 100.0f);
	TestEqual(TEXT("Removing gear preserves missing health"), Stats->CurrentHealth, 80.0f);
	TestEqual(TEXT("Removing gear restores base attack power"), Stats->AttackPower, 10.0f);
	TestEqual(TEXT("Removing gear clears armor"), Stats->Armor, 0.0f);

	Stats->ApplyDamage(Stats->CurrentHealth);
	TestTrue(TEXT("Character is dead before equipment change"), Stats->IsDead());
	Stats->ApplyEquipmentBonuses(Bonuses);
	TestTrue(TEXT("Maximum-health gear does not resurrect a dead character"), Stats->IsDead());
	TestEqual(TEXT("Dead character remains at zero health after gear change"), Stats->CurrentHealth, 0.0f);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereEnemyLootRulesTest,
	"Embermere.Enemy.LootRules",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereEnemyLootRulesTest::RunTest(const FString& Parameters)
{
	AEmbermereEnemyCharacter* Enemy = NewObject<AEmbermereEnemyCharacter>();
	AEmbermereCharacter* Recipient = NewObject<AEmbermereCharacter>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	if (!Enemy || !Recipient || !Recipient->Inventory || !Tonic)
	{
		AddError(TEXT("Could not create enemy loot test objects"));
		return false;
	}

	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 5;
	Enemy->LootItem = Tonic;
	Enemy->LootQuantity = 2;
	Enemy->LootDropChance = 0.5f;

	TestTrue(TEXT("Loot roll inside chance succeeds"), Enemy->ShouldDropLoot(0.25f));
	TestFalse(TEXT("Loot roll outside chance fails"), Enemy->ShouldDropLoot(0.75f));
	Enemy->LootDropChance = 0.0f;
	TestFalse(TEXT("Zero drop chance never succeeds"), Enemy->ShouldDropLoot(0.0f));
	Enemy->LootDropChance = 1.0f;
	TestTrue(TEXT("Guaranteed drop chance accepts the highest runtime roll"), Enemy->ShouldDropLoot(0.999999f));
	TestTrue(TEXT("Loot can be granted to an inventory owner"), Enemy->GrantLootTo(Recipient));
	TestEqual(TEXT("Loot grant creates one stack"), Recipient->Inventory->Stacks.Num(), 1);
	TestEqual(TEXT("Loot grant uses configured quantity"), Recipient->Inventory->Stacks[0].Quantity, 2);
	TestTrue(TEXT("Loot grant stores configured item"), Recipient->Inventory->Stacks[0].Item == Tonic);

	Enemy->LootQuantity = 0;
	TestFalse(TEXT("Zero-quantity loot is rejected"), Enemy->GrantLootTo(Recipient));
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereConsumableUseTest,
	"Embermere.Inventory.ConsumableUse",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereConsumableUseTest::RunTest(const FString& Parameters)
{
	UEmbermerePlayerHudWidget* HudWidget = NewObject<UEmbermerePlayerHudWidget>();
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereStatsComponent* Stats = NewObject<UEmbermereStatsComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	if (!HudWidget || !Inventory || !Stats || !Tonic)
	{
		AddError(TEXT("Could not create consumable test objects"));
		return false;
	}

	Tonic->ItemId = "MarshTonic";
	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->Category = EEmbermereItemCategory::Consumable;
	Tonic->MaxStack = 5;
	Tonic->ConsumableEffects.HealHealth = 25.0f;
	Tonic->ConsumableEffects.RestoreMana = 10.0f;
	HudWidget->Inventory = Inventory;
	HudWidget->Stats = Stats;
	Stats->InitializeVitals();
	Stats->ApplyDamage(40.0f);
	Stats->SpendMana(20.0f);
	TestTrue(TEXT("Consumable stack can be added"), Inventory->AddItem(Tonic, 2));
	TestEqual(TEXT("Consumable primary action is Use"), HudWidget->GetSelectedInventoryActionLabel().ToString(), FString(TEXT("Use")));
	TestTrue(TEXT("Consumable summary reports recovery"), Tonic->GetEffectSummary().ToString().Contains(TEXT("Restores 25 HP")));

	TestTrue(TEXT("Consumable applies when health or mana is missing"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Consumable restores health"), Stats->CurrentHealth, 85.0f);
	TestEqual(TEXT("Consumable restores mana"), Stats->CurrentMana, 40.0f);
	TestEqual(TEXT("Successful use consumes one item"), Inventory->Stacks[0].Quantity, 1);

	TestTrue(TEXT("Second consumable can finish recovery"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Second consumable caps health"), Stats->CurrentHealth, Stats->MaxHealth);
	TestEqual(TEXT("Second consumable caps mana"), Stats->CurrentMana, Stats->MaxMana);
	TestEqual(TEXT("Empty consumable stack is removed"), Inventory->Stacks.Num(), 0);

	TestTrue(TEXT("Fresh consumable can be added at full resources"), Inventory->AddItem(Tonic, 1));
	TestFalse(TEXT("Consumable cannot be wasted at full resources"), HudWidget->ActivateSelectedInventoryItem());
	TestEqual(TEXT("Failed use preserves inventory"), Inventory->Stacks[0].Quantity, 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FEmbermereInventoryCapacityTransactionsTest,
	"Embermere.Inventory.CapacityTransactions",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereInventoryCapacityTransactionsTest::RunTest(const FString& Parameters)
{
	UEmbermereInventoryComponent* Inventory = NewObject<UEmbermereInventoryComponent>();
	UEmbermereItemData* Tonic = NewObject<UEmbermereItemData>();
	UEmbermereItemData* Reed = NewObject<UEmbermereItemData>();
	if (!Inventory || !Tonic || !Reed)
	{
		AddError(TEXT("Could not create inventory capacity transaction test objects"));
		return false;
	}

	Inventory->MaxSlots = 1;
	Tonic->DisplayName = FText::FromString(TEXT("Marsh Tonic"));
	Tonic->MaxStack = 2;
	Reed->DisplayName = FText::FromString(TEXT("Marsh Reed"));
	Reed->MaxStack = 10;

	TestTrue(TEXT("A stack that fits can be added"), Inventory->AddItem(Tonic, 2));
	TestEqual(TEXT("Quantity query sums the stored stack"), Inventory->GetItemQuantity(Tonic), 2);
	TestFalse(TEXT("Over-capacity add is rejected"), Inventory->AddItem(Tonic, 1));
	TestEqual(TEXT("Rejected stack growth leaves quantity unchanged"), Inventory->GetItemQuantity(Tonic), 2);
	TestFalse(TEXT("Different item cannot enter a full bag"), Inventory->AddItem(Reed, 1));
	TestEqual(TEXT("Rejected different item creates no partial stack"), Inventory->GetItemQuantity(Reed), 0);
	TestFalse(TEXT("Over-quantity removal is rejected"), Inventory->RemoveItem(Tonic, 3));
	TestEqual(TEXT("Rejected removal leaves quantity unchanged"), Inventory->GetItemQuantity(Tonic), 2);
	TestTrue(TEXT("Available quantity can be removed"), Inventory->RemoveItem(Tonic, 1));
	TestEqual(TEXT("Successful removal decrements exactly once"), Inventory->GetItemQuantity(Tonic), 1);

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
