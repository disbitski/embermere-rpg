#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/TextBlock.h"
#include "Misc/AutomationTest.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Save/EmbermereSaveGame.h"
#include "UI/EmbermereQuestUpdateWidget.h"

namespace
{
	UEmbermereQuestData* MakeUpdateQuest()
	{
		UEmbermereQuestData* Quest = NewObject<UEmbermereQuestData>();
		Quest->QuestId = TEXT("UpdateProbe");
		Quest->ObjectiveId = TEXT("ProbeProgress");
		Quest->Title = FText::FromString(TEXT("A Fenwatch Journey"));
		Quest->ObjectiveInstructions = FText::FromString(TEXT("Complete three steps."));
		Quest->RequiredObjectiveCount = 3;
		Quest->RewardExperience = 25;
		Quest->RewardCopper = 10;
		return Quest;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereQuestUpdateContractTest,
	"Embermere.Quests.LiveUpdateContract",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestUpdateContractTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereQuestData* Quest = MakeUpdateQuest();
	UEmbermereQuestLogComponent* Log = Character->QuestLog;
	const int32 StartingCopper = Character->Wallet->Copper;
	TArray<FEmbermereQuestUpdate> Events;
	Log->OnLiveQuestUpdate.AddLambda([&, this](const FEmbermereQuestUpdate& Update)
	{
		FEmbermereQuestState State;
		TestTrue(TEXT("Event follows committed keyed state"), Log->GetQuestStateById(Update.QuestId, State));
		TestEqual(TEXT("Payload equals already committed count"), Update.CurrentCount, State.CurrentObjectiveCount);
		TestEqual(TEXT("Stable objective identity copied"), Update.ObjectiveId, Quest->ObjectiveId);
		TestFalse(TEXT("Reentrant progress is rejected"), Log->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId));
		TestFalse(TEXT("Reentrant restore preflight is rejected"), Log->CanRestoreQuestStatesForSaveGame({}));
		if (Update.Kind == EEmbermereQuestUpdateKind::Completed)
		{
			TestTrue(TEXT("Completion published after terminal state"), State.bCompleted);
			TestEqual(TEXT("Copper committed before completion event"), Character->Wallet->Copper, StartingCopper + 10);
			TestEqual(TEXT("XP committed before completion event"), Character->Stats->CurrentExperience, 25);
			TestFalse(TEXT("Reentrant completion rejected"), Log->TryCompleteQuest(Quest));
		}
		Events.Add(Update);
	});
	TestTrue(TEXT("Accept commits"), Log->AcceptQuest(Quest));
	TestEqual(TEXT("One accepted event"), Events.Num(), 1);
	TestEqual(TEXT("Accepted kind"), Events.Last().Kind, EEmbermereQuestUpdateKind::Accepted);
	TestFalse(TEXT("Duplicate accept rejected"), Log->AcceptQuest(Quest));
	TestFalse(TEXT("Wrong ID rejected"), Log->AddObjectiveProgressForQuest(TEXT("Wrong"), Quest->ObjectiveId));
	TestFalse(TEXT("Wrong objective rejected"), Log->AddObjectiveProgressForQuest(Quest->QuestId, TEXT("Wrong")));
	TestFalse(TEXT("Zero progress rejected"), Log->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId, 0));
	Log->FocusQuest(Quest->QuestId);
	TestEqual(TEXT("Rejections and focus emit nothing"), Events.Num(), 1);
	TestTrue(TEXT("Progress commits"), Log->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId, 1));
	TestEqual(TEXT("Progress kind"), Events.Last().Kind, EEmbermereQuestUpdateKind::Progress);
	TestEqual(TEXT("Previous count copied"), Events.Last().PreviousCount, 0);
	TestTrue(TEXT("Large progress saturates without overflow"), Log->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId, MAX_int32));
	TestEqual(TEXT("Exactly one cap event"), Events.Num(), 3);
	TestEqual(TEXT("Ready kind, not duplicate progress"), Events.Last().Kind, EEmbermereQuestUpdateKind::Ready);
	TestEqual(TEXT("Ready count exact"), Events.Last().CurrentCount, 3);
	TestEqual(TEXT("Ready previous count exact"), Events.Last().PreviousCount, 1);
	TestFalse(TEXT("Capped progress rejected"), Log->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId));
	Character->Wallet->SetCopperForPrototype(MAX_int32);
	TestFalse(TEXT("Reward preflight rejects overflow"), Log->TryCompleteQuest(Quest));
	TestEqual(TEXT("Rejected reward emits nothing"), Events.Num(), 3);
	Character->Wallet->SetCopperForPrototype(StartingCopper);
	TestTrue(TEXT("Turn-in commits"), Log->TryCompleteQuest(Quest));
	TestEqual(TEXT("Exactly four events"), Events.Num(), 4);
	TestEqual(TEXT("Strict session sequence"), Events.Last().Sequence, uint64(4));
	Quest->Title = FText::FromString(TEXT("Changed after event"));
	TestEqual(TEXT("Old event owns immutable display copy"), Events[0].Title.ToString(), FString(TEXT("A Fenwatch Journey")));
	const TArray<FEmbermereQuestState> Saved = Log->QuestStates;
	int32 ResetCount = 0;
	Log->OnPresentationReset.AddLambda([&ResetCount]() { ++ResetCount; });
	Log->RestoreQuestStatesForSaveGame(Saved);
	Log->RestoreQuestStatesForSaveGame(Saved);
	TestEqual(TEXT("Two restores explicitly clear observers"), ResetCount, 2);
	TestEqual(TEXT("Restore is live-event silent"), Events.Num(), 4);
	TestEqual(TEXT("Restore does not replay copper"), Character->Wallet->Copper, StartingCopper + 10);
	TestEqual(TEXT("Restore does not replay XP"), Character->Stats->CurrentExperience, 25);
	Log->OnLiveQuestUpdate.Clear();
	Log->OnPresentationReset.Clear();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereQuestUpdatePresentationTest,
	"Embermere.UI.QuestUpdatePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestUpdatePresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereQuestData* Quest = MakeUpdateQuest();
	UEmbermereQuestUpdateWidget* Widget = NewObject<UEmbermereQuestUpdateWidget>();
	Widget->BindToQuestLog(Character->QuestLog, Character->Stats);
	TestFalse(TEXT("Fresh observer hidden"), Widget->IsUpdateVisible());
	TestEqual(TEXT("Fixed panel size"), Widget->GetPanelDimensions(), FVector2D(420, 108));
	TestTrue(TEXT("Native widget initializes before layout inspection"), Widget->Initialize());
	const TSharedRef<SWidget> SlateWidget = Widget->TakeWidget();
	UBorder* Panel = Cast<UBorder>(Widget->GetWidgetFromName(TEXT("QuestUpdatePanel")));
	UTextBlock* Detail = Cast<UTextBlock>(Widget->GetWidgetFromName(TEXT("QuestUpdateDetail")));
	TestNotNull(TEXT("Actual native panel exists"), Panel);
	TestNotNull(TEXT("Actual detail text exists"), Detail);
	if (!Panel || !Detail) return false;
	UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(Panel->Slot);
	UCanvasPanelSlot* DetailSlot = Cast<UCanvasPanelSlot>(Detail->Slot);
	TestNotNull(TEXT("Panel is a bounded canvas child"), PanelSlot);
	TestNotNull(TEXT("Detail is a bounded canvas child"), DetailSlot);
	if (!PanelSlot || !DetailSlot) return false;
	TestEqual(TEXT("Rendered panel uses contract size"), PanelSlot->GetSize(), FVector2D(420, 108));
	TestEqual(TEXT("Rendered detail cannot grow"), DetailSlot->GetSize(), FVector2D(332, 40));
	TestEqual(TEXT("Panel clips overflow"), Panel->GetClipping(), EWidgetClipping::ClipToBoundsAlways);
	TestEqual(TEXT("Observer cannot intercept inputs"), Widget->GetVisibility(), ESlateVisibility::HitTestInvisible);
	FEmbermereQuestUpdate LastUpdate;
	Character->QuestLog->OnLiveQuestUpdate.AddLambda([&LastUpdate](const FEmbermereQuestUpdate& Update) { LastUpdate = Update; });
	Character->QuestLog->AcceptQuest(Quest);
	TestTrue(TEXT("Live acceptance visible"), Widget->IsUpdateVisible());
	TestEqual(TEXT("Exact accepted heading"), Widget->GetUpdateHeading().ToString(), FString(TEXT("QUEST ACCEPTED")));
	TestEqual(TEXT("Exact instructions and count"), Widget->GetUpdateDetail().ToString(), FString(TEXT("0 / 3   Complete three steps.")));
	Widget->AdvancePresentation(2.9f);
	Widget->PresentUpdate(LastUpdate);
	Widget->AdvancePresentation(0.11f);
	TestFalse(TEXT("Duplicate does not extend lifetime"), Widget->IsUpdateVisible());
	Character->QuestLog->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId);
	Character->QuestLog->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId);
	Character->QuestLog->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId);
	TestEqual(TEXT("Rapid results replace with latest exact ready state"), Widget->GetUpdateHeading().ToString(), FString(TEXT("READY TO RETURN")));
	TestEqual(TEXT("Latest count displayed"), Widget->GetUpdateDetail().ToString(), FString(TEXT("3 / 3   Complete three steps.")));
	Widget->SetSuppressed(true);
	TestFalse(TEXT("Modal clears active notification"), Widget->IsUpdateVisible());
	Character->QuestLog->TryCompleteQuest(Quest);
	Widget->SetSuppressed(false);
	TestFalse(TEXT("Closing modal never replays suppressed completion"), Widget->IsUpdateVisible());
	Widget->PresentUpdate(LastUpdate);
	TestFalse(TEXT("Suppressed sequence cannot be replayed"), Widget->IsUpdateVisible());
	Widget->BindToQuestLog(nullptr, nullptr);
	TestFalse(TEXT("Unbind clears"), Widget->IsUpdateVisible());
	Character->QuestLog->OnLiveQuestUpdate.Clear();
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereQuestUpdateLifecycleTest,
	"Embermere.UI.QuestUpdateLifecycle",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestUpdateLifecycleTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	UEmbermereQuestUpdateWidget* Widget = NewObject<UEmbermereQuestUpdateWidget>();
	UEmbermereQuestData* Quest = MakeUpdateQuest();
	TestTrue(TEXT("Persistence fixture confirms Human Warrior"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	Widget->BindToQuestLog(Character->QuestLog, Character->Stats);
	Character->QuestLog->AcceptQuest(Quest);
	const TArray<FEmbermereQuestState> Saved = Character->QuestLog->QuestStates;
	Character->QuestLog->RestoreQuestStatesForSaveGame(Saved);
	TestFalse(TEXT("Load clears existing notification immediately"), Widget->IsUpdateVisible());
	Character->QuestLog->RestoreQuestStatesForSaveGame(Saved);
	TestFalse(TEXT("Repeated load remains silent"), Widget->IsUpdateVisible());
	Character->QuestLog->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId);
	TestTrue(TEXT("New live event still works after load"), Widget->IsUpdateVisible());
	Character->Stats->OnDied.Broadcast();
	TestFalse(TEXT("Death signal clears immediately"), Widget->IsUpdateVisible());
	Widget->BindToQuestLog(nullptr, nullptr);
	Character->QuestLog->AddObjectiveProgressForQuest(Quest->QuestId, Quest->ObjectiveId);
	TestFalse(TEXT("Unbound observer stays silent"), Widget->IsUpdateVisible());
	TestEqual(TEXT("Presentation never changes wallet"), Character->Wallet->Copper, 40);
	TestEqual(TEXT("Presentation never grants XP"), Character->Stats->CurrentExperience, 0);
	TestEqual(TEXT("Presentation never grants items"), Character->Inventory->Stacks.Num(), 0);
	TestFalse(TEXT("Presentation never completes quest"), Character->QuestLog->QuestStates[0].bCompleted);

	// Use a real asset so the existing stable-path persistence validator applies.
	AEmbermereCharacter* PersistentCharacter = NewObject<AEmbermereCharacter>();
	PersistentCharacter->TryApplyRaceAndClass(EEmbermereRace::Elf, EEmbermereClass::Wizard);
	UEmbermereQuestData* Mara = LoadObject<UEmbermereQuestData>(nullptr,
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"));
	if (!TestNotNull(TEXT("Saved Mara quest resolves"), Mara)) return false;
	Widget->BindToQuestLog(PersistentCharacter->QuestLog, PersistentCharacter->Stats);
	PersistentCharacter->QuestLog->AcceptQuest(Mara);
	UEmbermereSaveGame* Save = nullptr;
	FText Message;
	TestEqual(TEXT("Whole-character version-3 capture succeeds"),
		UEmbermerePersistenceLibrary::CaptureGameState(PersistentCharacter, {}, Save, Message),
		EEmbermerePersistenceResult::Success);
	if (!Save) return false;
	PersistentCharacter->QuestLog->AddObjectiveProgressForQuest(Mara->QuestId, Mara->ObjectiveId);
	TestTrue(TEXT("Diverged live state has a notification"), Widget->IsUpdateVisible());
	for (int32 Index = 0; Index < 2; ++Index)
	{
		TestEqual(TEXT("Full persistence restore succeeds"),
			UEmbermerePersistenceLibrary::ApplyGameState(PersistentCharacter, {}, Save, Message),
			EEmbermerePersistenceResult::Success);
		TestFalse(TEXT("Full load clears without replay"), Widget->IsUpdateVisible());
		TestEqual(TEXT("Full load restores exact count"), PersistentCharacter->QuestLog->QuestStates[0].CurrentObjectiveCount, 0);
		TestEqual(TEXT("Full load has no XP drift"), PersistentCharacter->Stats->CurrentExperience, 0);
		TestEqual(TEXT("Full load has no wallet drift"), PersistentCharacter->Wallet->Copper, 40);
	}
	Widget->BindToQuestLog(nullptr, nullptr);
	return true;
}

#endif
