#if WITH_DEV_AUTOMATION_TESTS

#include "Characters/EmbermereCharacter.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#include "Save/EmbermerePersistenceLibrary.h"
#include "Save/EmbermereSaveGame.h"
#include "UI/EmbermerePlayerHudWidget.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FEmbermereDialoguePresentationTest,
	"Embermere.UI.DialoguePresentation",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FEmbermereDialoguePresentationTest::RunTest(const FString& Parameters)
{
	AEmbermereCharacter* Character = NewObject<AEmbermereCharacter>();
	TestTrue(TEXT("Fixture confirms identity"),
		Character->TryApplyRaceAndClass(EEmbermereRace::Human, EEmbermereClass::Warrior));
	UEmbermereSaveGame* Before = nullptr;
	FText Message;
	TestEqual(TEXT("Capture before presentation"),
		UEmbermerePersistenceLibrary::CaptureGameState(Character, {}, Before, Message),
		EEmbermerePersistenceResult::Success);
	TArray<uint8> BeforeBytes;
	TestTrue(TEXT("Baseline serializes"), UGameplayStatics::SaveGameToMemory(Before, BeforeBytes));

	UEmbermerePlayerHudWidget* Hud = NewObject<UEmbermerePlayerHudWidget>();
	TestTrue(TEXT("Initialize real widget tree"), Hud->Initialize());
	const TSharedRef<SWidget> SlateWidget = Hud->TakeWidget();
	Hud->BindToCharacter(Character);
	Hud->ToggleInventoryPanel();
	UBorder* Panel = Cast<UBorder>(Hud->GetWidgetFromName(TEXT("DialoguePanel")));
	UTextBlock* Body = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("DialogueText")));
	UTextBlock* Speaker = Cast<UTextBlock>(Hud->GetWidgetFromName(TEXT("DialogueSpeaker")));
	USizeBox* Hotbar = Cast<USizeBox>(Hud->GetWidgetFromName(TEXT("HotbarSlotSize_0")));
	if (!TestNotNull(TEXT("Dialogue panel exists"), Panel) ||
		!TestNotNull(TEXT("Dialogue body exists"), Body) ||
		!TestNotNull(TEXT("Speaker has its own fixed region"), Speaker) ||
		!TestNotNull(TEXT("Existing hotbar slot exists"), Hotbar)) return false;
	UCanvasPanelSlot* PanelSlot = Cast<UCanvasPanelSlot>(Panel->Slot);
	UCanvasPanelSlot* SpeakerSlot = Cast<UCanvasPanelSlot>(Speaker->Slot);
	UCanvasPanelSlot* BodySlot = Cast<UCanvasPanelSlot>(Body->Slot);
	if (!TestNotNull(TEXT("Panel canvas slot"), PanelSlot) ||
		!TestNotNull(TEXT("Speaker canvas slot"), SpeakerSlot) ||
		!TestNotNull(TEXT("Body canvas slot"), BodySlot)) return false;
	TestEqual(TEXT("Fixed panel"), PanelSlot->GetSize(), FVector2D(620, 120));
	TestEqual(TEXT("Bottom center anchor"), PanelSlot->GetAnchors().Minimum, FVector2D(0.5, 1));
	TestEqual(TEXT("Bottom center alignment"), PanelSlot->GetAlignment(), FVector2D(0.5, 1));
	TestEqual(TEXT("Fixed bottom clearance"), PanelSlot->GetPosition(), FVector2D(0, -116));
	TestEqual(TEXT("Fixed speaker bounds"), SpeakerSlot->GetSize(), FVector2D(596, 28));
	TestEqual(TEXT("Speaker begins at origin"), SpeakerSlot->GetPosition(), FVector2D::ZeroVector);
	TestEqual(TEXT("Fixed body bounds"), BodySlot->GetSize(), FVector2D(596, 72));
	TestEqual(TEXT("Body begins below speaker"), BodySlot->GetPosition(), FVector2D(0, 28));
	TestEqual(TEXT("Hotbar width unchanged"), Hotbar->GetWidthOverride(), 92.0f);
	TestEqual(TEXT("Hotbar height unchanged"), Hotbar->GetHeightOverride(), 64.0f);
	UWidget* BottomStack = Hud->GetWidgetFromName(TEXT("HudBottomStack"));
	UCanvasPanelSlot* BottomSlot = BottomStack ? Cast<UCanvasPanelSlot>(BottomStack->Slot) : nullptr;
	if (!TestNotNull(TEXT("Existing bottom stack"), BottomSlot)) return false;
	TestEqual(TEXT("Hotbar bottom margin unchanged"), BottomSlot->GetPosition().Y, -36.0);
	for (const double Height : {720.0, 1080.0, 1440.0})
	{
		const double DialogueBottom = Height + PanelSlot->GetPosition().Y;
		const double HotbarTop = Height + BottomSlot->GetPosition().Y - Hotbar->GetHeightOverride();
		TestEqual(TEXT("Sixteen pixel gap at every viewport height"), HotbarTop - DialogueBottom, 16.0);
	}
	for (UWidget* Widget : {static_cast<UWidget*>(Panel), static_cast<UWidget*>(Speaker), static_cast<UWidget*>(Body)})
	{
		TestEqual(TEXT("Text cannot paint outside its region"), Widget->GetClipping(), EWidgetClipping::ClipToBoundsAlways);
	}
	TestEqual(TEXT("Body uses bounded wrap width"), Body->GetWrapTextAt(), 596.0f);
	TestEqual(TEXT("Body remains readable fixed-size text"), Body->GetFont().Size, 14.0f);
	TestEqual(TEXT("Speaker retains established size"), Speaker->GetFont().Size, 16.0f);

	const TArray<FString> AuthoredBodies = {
		TEXT("You picked a lively day to arrive. Something is stirring near the old stones east of town. Cull a few of those things and come back whole."),
		TEXT("Stand still beside the communal well to restore your health and mana.")
	};
	for (const FString& Copy : AuthoredBodies)
	{
		Hud->ShowDialogue(FText::FromString(TEXT("Mara Fenwatch")), FText::FromString(Copy));
		Hud->ForceLayoutPrepass();
		TestTrue(TEXT("Speaker fits its fixed height"), Speaker->GetDesiredSize().Y <= 28.0f);
		TestEqual(TEXT("Speaker payload stays separate"), Speaker->GetText().ToString(), FString(TEXT("Mara Fenwatch")));
		TestEqual(TEXT("Dialogue payload stays exact"), Body->GetText().ToString(), Copy);
		TestTrue(FString::Printf(TEXT("Starter copy fits fixed body height (measured %.1f x %.1f): %s"),
			Body->GetDesiredSize().X, Body->GetDesiredSize().Y, *Copy), Body->GetDesiredSize().Y <= 72.0f);
		TestEqual(TEXT("Dialogue cannot intercept input"), Panel->GetVisibility(), ESlateVisibility::HitTestInvisible);
	}
	const FString ExtremeCopy = FString::ChrN(2000, TEXT('W')) + TEXT("\nSecond line\nThird line\nFourth line\nFifth line");
	Hud->ShowDialogue(FText::FromString(ExtremeCopy), FText::FromString(ExtremeCopy));
	Hud->ForceLayoutPrepass();
	TestEqual(TEXT("Long payload is retained without rules or copy rewriting"), Body->GetText().ToString(), ExtremeCopy);
	TestEqual(TEXT("Long payload cannot grow its panel"), PanelSlot->GetSize(), FVector2D(620, 120));
	TestEqual(TEXT("Long payload cannot grow its body region"), BodySlot->GetSize(), FVector2D(596, 72));
	Hud->ShowDialogue(FText::FromString(TEXT("Fenwatch")), FText::FromString(TEXT("Return safely.")));
	TestEqual(TEXT("Latest speaker replaces previous"), Speaker->GetText().ToString(), FString(TEXT("Fenwatch")));
	TestEqual(TEXT("Latest body replaces previous"), Body->GetText().ToString(), FString(TEXT("Return safely.")));
	UEmbermereSaveGame* After = nullptr;
	TestEqual(TEXT("Capture after presentation"),
		UEmbermerePersistenceLibrary::CaptureGameState(Character, {}, After, Message),
		EEmbermerePersistenceResult::Success);
	TArray<uint8> AfterBytes;
	TestTrue(TEXT("Result serializes"), UGameplayStatics::SaveGameToMemory(After, AfterBytes));
	TestTrue(TEXT("All durable owners remain unchanged"), BeforeBytes == AfterBytes);
	return true;
}

#endif
