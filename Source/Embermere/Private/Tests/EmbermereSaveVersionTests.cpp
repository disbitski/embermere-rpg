#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereHotbarComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Misc/ScopeExit.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Save/EmbermereSaveGame.h"

namespace
{
	UEmbermereSaveGame* MakeVersionFixture(int32 Version)
	{
		UEmbermereSaveGame* Save = NewObject<UEmbermereSaveGame>();
		Save->FormatVersion = Version;
		Save->Copper = 40;
		Save->CurrentExperience = 175;
		if (Version != EmbermereSaveGameVersion::ProgressionOnly)
		{
			Save->RaceId = TEXT("Elf");
			Save->ClassId = TEXT("Wizard");
		}
		FEmbermereSavedInventoryStack Tonic;
		Tonic.ItemId = TEXT("MarshTonic");
		Tonic.ItemAsset = FSoftObjectPath(TEXT("/Game/Data/Items/DI_MarshTonic.DI_MarshTonic"));
		Tonic.Quantity = 1;
		Save->InventoryStacks.Add(Tonic);
		FEmbermereSavedEquipmentItem Pack;
		Pack.Slot = EEmbermereEquipmentSlot::Back;
		Pack.ItemId = TEXT("RecruitPack");
		Pack.ItemAsset = FSoftObjectPath(TEXT("/Game/Data/Items/DI_EmbermereRecruitPack.DI_EmbermereRecruitPack"));
		Save->EquippedItems.Add(Pack);
		FEmbermereSavedQuestRecord Quest;
		Quest.QuestId = TEXT("FirstSignsAtTheRuin");
		Quest.QuestAsset = FSoftObjectPath(TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
		Quest.ObjectiveId = TEXT("StarterEnemyDefeated");
		Quest.CurrentObjectiveCount = 3;
		Quest.bCompleted = true;
		if (Version == EmbermereSaveGameVersion::MultiQuestLedger)
		{
			Save->QuestStates.Add(Quest);
		}
		else
		{
			Save->QuestState.bHasActiveQuest = true;
			Save->QuestState.QuestId = Quest.QuestId;
			Save->QuestState.QuestAsset = Quest.QuestAsset;
			Save->QuestState.CurrentObjectiveCount = Quest.CurrentObjectiveCount;
			Save->QuestState.bCompleted = true;
		}
		return Save;
	}

	bool CaptureBytes(AEmbermereCharacter* Character, TArray<uint8>& Bytes)
	{
		UEmbermereSaveGame* Save = nullptr;
		FText Message;
		return UEmbermerePersistenceLibrary::CaptureGameState(Character, {}, Save, Message) ==
			EEmbermerePersistenceResult::Success && UGameplayStatics::SaveGameToMemory(Save, Bytes);
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereVersionStampRoundTripTest,
	"Embermere.Persistence.VersionStampRoundTrip",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereVersionStampRoundTripTest::RunTest(const FString& Parameters)
{
	UEmbermereSaveGame* Defaults = GetMutableDefault<UEmbermereSaveGame>();
	TestEqual(TEXT("Class default stays permanently unrecorded"), Defaults->FormatVersion, 0);
	TestFalse(TEXT("Unrecorded is never a supported schema"), EmbermereSaveGameVersion::IsSupported(0));
	for (const int32 Version : {1, 2, 3})
	{
		UEmbermereSaveGame* Save = MakeVersionFixture(Version);
		TArray<uint8> Bytes;
		TestTrue(TEXT("Explicit version serializes"), UGameplayStatics::SaveGameToMemory(Save, Bytes));
		UEmbermereSaveGame* Loaded = nullptr;
		{
			// Simulate a later executable with a different CDO, not a same-build round trip.
			TGuardValue<int32> LaterDefault(Defaults->FormatVersion, 99);
			Loaded = Cast<UEmbermereSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
		}
		if (!TestNotNull(TEXT("Archive reloads"), Loaded)) return false;
		TestEqual(TEXT("Recorded version survives a changed default"), Loaded->FormatVersion, Version);
		AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
		TestTrue(TEXT("Target starts with a different confirmed identity"),
			Target->TryApplyRaceAndClass(EEmbermereRace::Dwarf, EEmbermereClass::Warrior));
		FText Message;
		TestEqual(TEXT("Stamped legacy/current archive applies"),
			UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, Loaded, Message),
			EEmbermerePersistenceResult::Success);
		TestEqual(TEXT("Exact wallet restored"), Target->Wallet->Copper, 40);
		TestEqual(TEXT("Exact XP restored"), Target->Stats->CurrentExperience, 175);
		TestEqual(TEXT("Level is derived"), Target->Stats->Level, 2);
		TestEqual(TEXT("Version 1 fallback or recorded identity restored"), Target->Race,
			Version == 1 ? EEmbermereRace::Human : EEmbermereRace::Elf);
		TestEqual(TEXT("Correct starter hotbar"), Target->Hotbar->Slots[0].AbilityId,
			FName(Version == 1 ? TEXT("Strike") : TEXT("SparkBolt")));
		TestEqual(TEXT("Completed quest restored once"), Target->QuestLog->QuestStates.Num(), 1);
		if (Target->QuestLog->QuestStates.Num() != 1) return false;
		TestTrue(TEXT("Completed history retained"), Target->QuestLog->QuestStates[0].bCompleted);
		const float Health = Target->Stats->MaxHealth;
		const float Mana = Target->Stats->MaxMana;
		TArray<uint8> FirstRestore;
		TestTrue(TEXT("Restored owners capture"), CaptureBytes(Target, FirstRestore));
		TestEqual(TEXT("Second apply succeeds"),
			UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, Loaded, Message),
			EEmbermerePersistenceResult::Success);
		TArray<uint8> SecondRestore;
		TestTrue(TEXT("Repeated owners capture"), CaptureBytes(Target, SecondRestore));
		TestTrue(TEXT("Every durable record is idempotent"), FirstRestore == SecondRestore);
		TestEqual(TEXT("Equipment health does not stack"), Target->Stats->MaxHealth, Health);
		TestEqual(TEXT("Equipment mana does not stack"), Target->Stats->MaxMana, Mana);
		TestEqual(TEXT("Adapter does not rewrite source version"), Loaded->FormatVersion, Version);
	}
	TestEqual(TEXT("Test restores CDO"), Defaults->FormatVersion, 0);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereMissingVersionRollbackTest,
	"Embermere.Persistence.MissingVersionRollback",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereMissingVersionRollbackTest::RunTest(const FString& Parameters)
{
	UEmbermereSaveGame* Defaults = GetMutableDefault<UEmbermereSaveGame>();
	const FString Slot = TEXT("EmbermereVersionProbe_") + FGuid::NewGuid().ToString(EGuidFormats::Digits);
	ON_SCOPE_EXIT { UGameplayStatics::DeleteGameInSlot(Slot, 0); };
	AEmbermereCharacter* Target = NewObject<AEmbermereCharacter>();
	FText Message;
	TestEqual(TEXT("Seed all durable owners with valid native state"),
		UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, MakeVersionFixture(3), Message),
		EEmbermerePersistenceResult::Success);
	TArray<uint8> Before;
	TestTrue(TEXT("Baseline captures"), CaptureBytes(Target, Before));
	const float Health = Target->Stats->CurrentHealth;
	const float Mana = Target->Stats->CurrentMana;
	const FName Focus = Target->QuestLog->FocusedQuestId;
	for (const int32 HistoricalVersion : {1, 2, 3})
	{
		UEmbermereSaveGame* Historical = MakeVersionFixture(HistoricalVersion);
		Historical->Copper = 7;
		TArray<uint8> Bytes;
		{
			// Reproduce the old writer's default elision through Unreal's real archive.
			TGuardValue<int32> OldDefault(Defaults->FormatVersion, HistoricalVersion);
			TestTrue(TEXT("Historical archive writes"), UGameplayStatics::SaveGameToMemory(Historical, Bytes));
		}
		UEmbermereSaveGame* Loaded = Cast<UEmbermereSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
		if (!TestNotNull(TEXT("Unstamped archive remains structurally readable"), Loaded)) return false;
		TestEqual(TEXT("Omitted stamp never inherits Current"), Loaded->FormatVersion, 0);
		TestEqual(TEXT("Unknown version rejects before mutation"),
			UEmbermerePersistenceLibrary::ApplyGameState(Target, {}, Loaded, Message),
			EEmbermerePersistenceResult::UnsupportedVersion);
		TestEqual(TEXT("Honest missing-version copy"), Message.ToString(),
			FString(TEXT("Save version was not recorded. Recovery requires an explicit legacy version; this file is unchanged.")));
		TestTrue(TEXT("Only the unique test slot is written"), UGameplayStatics::SaveDataToSlot(Bytes, Slot, 0));
		FText Summary;
		TestEqual(TEXT("Chronicle inspection rejects the same unstamped file"),
			UEmbermerePersistenceLibrary::InspectSaveSlot(Slot, 0, Summary),
			EEmbermerePersistenceResult::UnsupportedVersion);
		TestEqual(TEXT("Inspection and load explain the same rejection"), Summary.ToString(), Message.ToString());
		TArray<uint8> AfterInspection;
		TestTrue(TEXT("Probe bytes remain readable"), UGameplayStatics::LoadDataFromSlot(AfterInspection, Slot, 0));
		TestTrue(TEXT("Inspection never repairs or rewrites bytes"), Bytes == AfterInspection);
		TArray<uint8> After;
		TestTrue(TEXT("Rejected owners capture"), CaptureBytes(Target, After));
		TestTrue(TEXT("Every durable owner rolls back exactly"), Before == After);
		TestEqual(TEXT("Health unchanged"), Target->Stats->CurrentHealth, Health);
		TestEqual(TEXT("Mana unchanged"), Target->Stats->CurrentMana, Mana);
		TestEqual(TEXT("Transient quest focus unchanged"), Target->QuestLog->FocusedQuestId, Focus);
		TestEqual(TEXT("Hotbar unchanged"), Target->Hotbar->Slots[0].AbilityId, FName(TEXT("SparkBolt")));
	}
	TestEqual(TEXT("Test restores permanent invalid default"), Defaults->FormatVersion, 0);
	return true;
}

#endif
