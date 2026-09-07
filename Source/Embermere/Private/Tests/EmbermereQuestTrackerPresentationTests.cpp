#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Data/EmbermereQuestData.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Save/EmbermereSaveGame.h"
#include "UI/EmbermerePlayerHudWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereQuestTrackerTextFitTest,
	"Embermere.UI.QuestTrackerTextFit",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereQuestTrackerTextFitTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestTrue(TEXT("Fixture confirms identity"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	TestTrue(TEXT("Initialize real widget tree"), Hud->Initialize());
	const TSharedRef<SWidget> SlateWidget = Hud->TakeWidget();
	Hud->BindToCharacter(Character);
	UTextBlock* Tracker = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("QuestText")));
	USizeBox* Bounds = Cast<USizeBox>(Hud->GetWidgetFromName(TEXT("QuestTrackerSize")));
	if (!TestNotNull(TEXT("Tracker exists"), Tracker) ||
		!TestNotNull(TEXT("Fixed tracker bounds exist"), Bounds)) return false;
	UVerticalBoxSlot* BoundsSlot = Cast<UVerticalBoxSlot>(Bounds->Slot);
	if (!TestNotNull(TEXT("Tracker column slot exists"), BoundsSlot)) return false;
	TestEqual(TEXT("Wider target names cannot stretch the tracker cell"),
		BoundsSlot->GetHorizontalAlignment(), HAlign_Left);

	for (const TCHAR* Path : {
		TEXT("/Game/Data/Quests/DQ_FirstSignsAtTheRuin.DQ_FirstSignsAtTheRuin"),
		TEXT("/Game/Data/Quests/DQ_FenwatchStillWaters.DQ_FenwatchStillWaters")})
	{
		UEmbermereQuestData* Quest = LoadObject<UEmbermereQuestData>(nullptr, Path);
		if (!TestNotNull(TEXT("Use real saved quest copy"), Quest)) return false;
		TestTrue(TEXT("Accept through quest authority"), Character->QuestLog->AcceptQuest(Quest));
		TestTrue(TEXT("Focus through quest authority"), Character->QuestLog->FocusQuest(Quest->QuestId));
		UEmbermereSaveGame* Before = nullptr;
		FText Message;
		TestEqual(TEXT("Capture before rendering"),
			UEmbermerePersistenceLibrary::CaptureGameState(Character, {}, Before, Message),
			EEmbermerePersistenceResult::Success);
		TArray<uint8> BeforeBytes;
		TestTrue(TEXT("Baseline serializes"), UGameplayStatics::SaveGameToMemory(Before, BeforeBytes));

		Hud->BindToCharacter(Character);
		Hud->ForceLayoutPrepass();
		const FVector2D Desired = Tracker->GetDesiredSize();
		AddInfo(FString::Printf(TEXT("%s tracker measured %.1f x %.1f"),
			*Quest->QuestId.ToString(), Desired.X, Desired.Y));
		TestTrue(FString::Printf(TEXT("%s authored copy fits 260x68 (measured %.1f x %.1f)"),
			*Quest->QuestId.ToString(), Desired.X, Desired.Y),
			Desired.X > 0 && Desired.Y > 0 && Desired.X <= 260 && Desired.Y <= 68);
		TestTrue(TEXT("Exact title remains present"),
			Tracker->GetText().ToString().Contains(Quest->Title.ToString()));
		TestTrue(TEXT("Exact instruction remains present"),
			Tracker->GetText().ToString().Contains(Quest->ObjectiveInstructions.ToString()));
		TestEqual(TEXT("Fixed width"), Bounds->GetWidthOverride(), 260.0f);
		TestEqual(TEXT("Fixed height"), Bounds->GetHeightOverride(), 68.0f);
		TestEqual(TEXT("Explicit wrapping does not depend on a prior viewport tick"), Tracker->GetWrapTextAt(), 260.0f);
		TestEqual(TEXT("Fixed font size"), Tracker->GetFont().Size, 13.0f);
		TestEqual(TEXT("Overflow remains clipped"), Tracker->GetClipping(), EWidgetClipping::ClipToBoundsAlways);
		UEmbermereSaveGame* After = nullptr;
		TestEqual(TEXT("Capture after rendering"),
			UEmbermerePersistenceLibrary::CaptureGameState(Character, {}, After, Message),
			EEmbermerePersistenceResult::Success);
		TArray<uint8> AfterBytes;
		TestTrue(TEXT("Result serializes"), UGameplayStatics::SaveGameToMemory(After, AfterBytes));
		TestTrue(TEXT("Rendering preserves all durable owners"), BeforeBytes == AfterBytes);
	}
	return true;
}

#endif
