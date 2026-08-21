#include "UI/EmbermereCharacterCreationWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Characters/EmbermereCharacter.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/VerticalBox.h"
#include "Components/VerticalBoxSlot.h"
#include "Data/EmbermereRulesData.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"

namespace
{
	constexpr float CharacterCreationPanelWidth = 940.0f;
	constexpr float CharacterCreationPanelHeight = 560.0f;

	const TArray<EEmbermereRace> RaceOrder = {
		EEmbermereRace::Human,
		EEmbermereRace::Elf,
		EEmbermereRace::Dwarf,
		EEmbermereRace::Gnome,
		EEmbermereRace::DarkElf,
		EEmbermereRace::Lizardman,
		EEmbermereRace::Ogre,
		EEmbermereRace::Bullywug,
	};

	const TArray<EEmbermereClass> ClassOrder = {
		EEmbermereClass::Warrior,
		EEmbermereClass::Cleric,
		EEmbermereClass::Ranger,
		EEmbermereClass::Wizard,
	};

	UTextBlock* MakeCreationText(
		UWidgetTree* WidgetTree,
		const FName Name,
		const float FontSize,
		const FLinearColor Color,
		const ETextJustify::Type Justification = ETextJustify::Left)
	{
		UTextBlock* Text = WidgetTree
			? WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name)
			: nullptr;
		if (!Text)
		{
			return nullptr;
		}

		FSlateFontInfo Font = Text->GetFont();
		Font.Size = FontSize;
		Text->SetFont(Font);
		Text->SetColorAndOpacity(FSlateColor(Color));
		Text->SetJustification(Justification);
		Text->SetShadowOffset(FVector2D(1.0f, 1.0f));
		Text->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.92f));
		return Text;
	}

	UVerticalBoxSlot* AddVerticalChild(
		UVerticalBox* Parent,
		UWidget* Child,
		float BottomPadding = 6.0f)
	{
		if (!Parent || !Child)
		{
			return nullptr;
		}

		UVerticalBoxSlot* Slot = Parent->AddChildToVerticalBox(Child);
		if (Slot)
		{
			Slot->SetPadding(FMargin(0.0f, 0.0f, 0.0f, BottomPadding));
		}
		return Slot;
	}

	UBorder* MakeSection(UWidgetTree* WidgetTree, const FName Name, UWidget* Content)
	{
		UBorder* Section = WidgetTree
			? WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), Name)
			: nullptr;
		if (Section)
		{
			Section->SetBrushColor(FLinearColor(0.035f, 0.052f, 0.045f, 0.96f));
			Section->SetPadding(FMargin(14.0f));
			Section->SetContent(Content);
		}
		return Section;
	}
}

void UEmbermereCharacterCreationOptionButton::InitializeOption(
	int32 NewOptionIndex,
	bool bNewRaceOption)
{
	OptionIndex = NewOptionIndex;
	bRaceOption = bNewRaceOption;
	OnClicked.AddUniqueDynamic(this, &UEmbermereCharacterCreationOptionButton::HandleClicked);
}

void UEmbermereCharacterCreationOptionButton::HandleClicked()
{
	OnOptionChosen.Broadcast(OptionIndex, bRaceOption);
}

void UEmbermereCharacterCreationWidget::BindToCharacter(AEmbermereCharacter* Character)
{
	BoundCharacter = Character;
}

bool UEmbermereCharacterCreationWidget::SetPendingRace(EEmbermereRace Race)
{
	FEmbermereRaceDefinition Definition;
	UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	if (bConfirmationComplete || !EffectiveRules ||
		!EffectiveRules->GetRaceDefinition(Race, Definition))
	{
		return false;
	}

	SelectedRace = Race;
	RefreshPresentation();
	return true;
}

bool UEmbermereCharacterCreationWidget::SetPendingClass(EEmbermereClass Class)
{
	FEmbermereClassDefinition Definition;
	UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	if (bConfirmationComplete || !EffectiveRules ||
		!EffectiveRules->GetClassDefinition(Class, Definition) ||
		!IsClassAvailable(Class))
	{
		return false;
	}

	SelectedClass = Class;
	RefreshPresentation();
	return true;
}

bool UEmbermereCharacterCreationWidget::SetRaceAndClass(
	EEmbermereRace Race,
	EEmbermereClass Class)
{
	UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	if (bConfirmationComplete || !EffectiveRules ||
		!EffectiveRules->IsClassAllowed(Race, Class))
	{
		return false;
	}

	SelectedRace = Race;
	SelectedClass = Class;
	RefreshPresentation();
	return true;
}

void UEmbermereCharacterCreationWidget::ConfirmChoice(AEmbermereCharacter* Character)
{
	TryConfirmChoice(Character);
}

bool UEmbermereCharacterCreationWidget::TryConfirmChoice(AEmbermereCharacter* Character)
{
	if (bConfirmationComplete || !IsCurrentChoiceValid())
	{
		return false;
	}

	AEmbermereCharacter* EffectiveCharacter = Character ? Character : BoundCharacter.Get();
	if (!EffectiveCharacter || !EffectiveCharacter->TryApplyRaceAndClass(SelectedRace, SelectedClass))
	{
		return false;
	}

	bConfirmationComplete = true;
	RefreshPresentation();
	OnCharacterChoiceConfirmed.Broadcast(SelectedRace, SelectedClass);
	return true;
}

bool UEmbermereCharacterCreationWidget::IsClassAvailable(EEmbermereClass Class) const
{
	const UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	return EffectiveRules && EffectiveRules->IsClassAllowed(SelectedRace, Class);
}

bool UEmbermereCharacterCreationWidget::IsCurrentChoiceValid() const
{
	return !bConfirmationComplete && IsClassAvailable(SelectedClass);
}

bool UEmbermereCharacterCreationWidget::IsConfirmationComplete() const
{
	return bConfirmationComplete;
}

int32 UEmbermereCharacterCreationWidget::GetRaceOptionCount() const
{
	const UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	return EffectiveRules ? EffectiveRules->Races.Num() : 0;
}

int32 UEmbermereCharacterCreationWidget::GetClassOptionCount() const
{
	const UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	return EffectiveRules ? EffectiveRules->Classes.Num() : 0;
}

FVector2D UEmbermereCharacterCreationWidget::GetPanelDimensions() const
{
	return FVector2D(CharacterCreationPanelWidth, CharacterCreationPanelHeight);
}

FText UEmbermereCharacterCreationWidget::GetSelectionSummary() const
{
	const UEmbermereRulesData* EffectiveRules = GetEffectiveRules();
	FEmbermereRaceDefinition RaceDefinition;
	FEmbermereClassDefinition ClassDefinition;
	if (!EffectiveRules ||
		!EffectiveRules->GetRaceDefinition(SelectedRace, RaceDefinition) ||
		!EffectiveRules->GetClassDefinition(SelectedClass, ClassDefinition))
	{
		return FText::FromString(TEXT("Character rules unavailable."));
	}

	FString AbilityNames;
	for (const FName AbilityId : ClassDefinition.StarterAbilityIds)
	{
		FEmbermereAbilityDefinition Ability;
		if (EffectiveRules->GetAbilityDefinition(AbilityId, Ability))
		{
			if (!AbilityNames.IsEmpty())
			{
				AbilityNames += TEXT(", ");
			}
			AbilityNames += Ability.DisplayName.ToString();
		}
	}

	const FEmbermereAttributeBlock& Stats = ClassDefinition.StartingAttributes;
	return FText::FromString(FString::Printf(
		TEXT("%s %s\n\n%s\n\n%s\n\nHealth %.0f   Mana %.0f\nStrength %.0f   Spirit %.0f\nAgility %.0f   Intellect %.0f\n\nStarter abilities\n%s"),
		*RaceDefinition.DisplayName.ToString(),
		*ClassDefinition.DisplayName.ToString(),
		*RaceDefinition.Description.ToString(),
		*ClassDefinition.Description.ToString(),
		Stats.MaxHealth,
		Stats.MaxMana,
		Stats.Strength,
		Stats.Spirit,
		Stats.Agility,
		Stats.Intellect,
		*AbilityNames));
}

TSharedRef<SWidget> UEmbermereCharacterCreationWidget::RebuildWidget()
{
	BuildDefaultLayout();
	return Super::RebuildWidget();
}

void UEmbermereCharacterCreationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	BuildDefaultLayout();
	RefreshPresentation();
	SetKeyboardFocus();
}

FReply UEmbermereCharacterCreationWidget::NativeOnPreviewKeyDown(
	const FGeometry& InGeometry,
	const FKeyEvent& InKeyEvent)
{
	const FKey Key = InKeyEvent.GetKey();
	if (Key == EKeys::Tab)
	{
		bKeyboardFocusOnClasses = !bKeyboardFocusOnClasses;
		return FReply::Handled();
	}
	if (Key == EKeys::Up)
	{
		bKeyboardFocusOnClasses ? MoveClassSelection(-1) : MoveRaceSelection(-1);
		return FReply::Handled();
	}
	if (Key == EKeys::Down)
	{
		bKeyboardFocusOnClasses ? MoveClassSelection(1) : MoveRaceSelection(1);
		return FReply::Handled();
	}
	if (Key == EKeys::Left)
	{
		bKeyboardFocusOnClasses = false;
		MoveRaceSelection(-1);
		return FReply::Handled();
	}
	if (Key == EKeys::Right)
	{
		bKeyboardFocusOnClasses = true;
		MoveClassSelection(1);
		return FReply::Handled();
	}
	if (Key == EKeys::Enter)
	{
		TryConfirmChoice();
		return FReply::Handled();
	}

	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

UEmbermereRulesData* UEmbermereCharacterCreationWidget::GetEffectiveRules() const
{
	if (RulesData)
	{
		return RulesData;
	}
	if (!RuntimeRulesData)
	{
		const_cast<UEmbermereCharacterCreationWidget*>(this)->RuntimeRulesData =
			NewObject<UEmbermereRulesData>(const_cast<UEmbermereCharacterCreationWidget*>(this));
	}
	return RuntimeRulesData;
}

void UEmbermereCharacterCreationWidget::BuildDefaultLayout()
{
	if (!WidgetTree || WidgetTree->RootWidget)
	{
		return;
	}

	UOverlay* Root = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("CharacterCreationRoot"));
	UBorder* Dimmer = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CharacterCreationDimmer"));
	USizeBox* PanelSize = WidgetTree->ConstructWidget<USizeBox>(USizeBox::StaticClass(), TEXT("CharacterCreationPanelSize"));
	UBorder* Panel = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("CharacterCreationPanel"));
	UVerticalBox* MainStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CharacterCreationMainStack"));
	if (!Root || !Dimmer || !PanelSize || !Panel || !MainStack)
	{
		return;
	}

	WidgetTree->RootWidget = Root;
	Dimmer->SetBrushColor(FLinearColor(0.002f, 0.006f, 0.008f, 0.94f));
	if (UOverlaySlot* DimmerSlot = Root->AddChildToOverlay(Dimmer))
	{
		DimmerSlot->SetHorizontalAlignment(HAlign_Fill);
		DimmerSlot->SetVerticalAlignment(VAlign_Fill);
	}

	PanelSize->SetWidthOverride(CharacterCreationPanelWidth);
	PanelSize->SetHeightOverride(CharacterCreationPanelHeight);
	PanelSize->SetContent(Panel);
	Panel->SetBrushColor(FLinearColor(0.018f, 0.032f, 0.027f, 1.0f));
	Panel->SetPadding(FMargin(22.0f));
	Panel->SetContent(MainStack);
	if (UOverlaySlot* PanelSlot = Root->AddChildToOverlay(PanelSize))
	{
		PanelSlot->SetHorizontalAlignment(HAlign_Center);
		PanelSlot->SetVerticalAlignment(VAlign_Center);
	}

	UTextBlock* Title = MakeCreationText(WidgetTree, TEXT("CharacterCreationTitle"), 27.0f, FLinearColor(1.0f, 0.78f, 0.28f, 1.0f), ETextJustify::Center);
	if (Title)
	{
		Title->SetText(FText::FromString(TEXT("Begin Your Embermere Journey")));
	}
	AddVerticalChild(MainStack, Title, 2.0f);

	UTextBlock* Subtitle = MakeCreationText(WidgetTree, TEXT("CharacterCreationSubtitle"), 13.0f, FLinearColor(0.72f, 0.78f, 0.72f, 1.0f), ETextJustify::Center);
	if (Subtitle)
	{
		Subtitle->SetText(FText::FromString(TEXT("Choose a race and starting class")));
	}
	AddVerticalChild(MainStack, Subtitle, 16.0f);

	UHorizontalBox* Body = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterCreationBody"));
	if (UVerticalBoxSlot* BodySlot = AddVerticalChild(MainStack, Body, 12.0f))
	{
		BodySlot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
	}

	UVerticalBox* RaceStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("RaceOptionStack"));
	UVerticalBox* ClassStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ClassOptionStack"));
	UVerticalBox* DetailStack = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("CreationDetailStack"));
	UBorder* RaceSection = MakeSection(WidgetTree, TEXT("RaceSection"), RaceStack);
	UBorder* ClassSection = MakeSection(WidgetTree, TEXT("ClassSection"), ClassStack);
	UBorder* DetailSection = MakeSection(WidgetTree, TEXT("CreationDetailSection"), DetailStack);
	if (Body)
	{
		if (UHorizontalBoxSlot* Slot = Body->AddChildToHorizontalBox(RaceSection))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			Slot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		}
		if (UHorizontalBoxSlot* Slot = Body->AddChildToHorizontalBox(ClassSection))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			Slot->SetPadding(FMargin(0.0f, 0.0f, 8.0f, 0.0f));
		}
		if (UHorizontalBoxSlot* Slot = Body->AddChildToHorizontalBox(DetailSection))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
		}
	}

	UTextBlock* RaceHeading = MakeCreationText(WidgetTree, TEXT("RaceHeading"), 17.0f, FLinearColor(0.92f, 0.86f, 0.68f, 1.0f));
	if (RaceHeading)
	{
		RaceHeading->SetText(FText::FromString(TEXT("Race")));
	}
	AddVerticalChild(RaceStack, RaceHeading, 10.0f);
	UUniformGridPanel* RaceGrid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), TEXT("RaceOptionsGrid"));
	AddVerticalChild(RaceStack, RaceGrid, 0.0f);
	RaceButtons.Reset();
	for (int32 Index = 0; Index < RaceOrder.Num(); ++Index)
	{
		FEmbermereRaceDefinition Definition;
		GetEffectiveRules()->GetRaceDefinition(RaceOrder[Index], Definition);
		UEmbermereCharacterCreationOptionButton* Button = WidgetTree->ConstructWidget<UEmbermereCharacterCreationOptionButton>(
			UEmbermereCharacterCreationOptionButton::StaticClass(),
			*FString::Printf(TEXT("RaceOption_%02d"), Index));
		UTextBlock* Label = MakeCreationText(WidgetTree, *FString::Printf(TEXT("RaceOptionLabel_%02d"), Index), 13.0f, FLinearColor::White, ETextJustify::Center);
		if (Button && Label && RaceGrid)
		{
			Label->SetText(Definition.DisplayName);
			Button->SetContent(Label);
			Button->SetToolTipText(Definition.Description);
			Button->InitializeOption(Index, true);
			Button->OnOptionChosen.AddUniqueDynamic(this, &UEmbermereCharacterCreationWidget::HandleOptionChosen);
			RaceGrid->AddChildToUniformGrid(Button, Index / 2, Index % 2);
			RaceButtons.Add(Button);
		}
	}

	UTextBlock* ClassHeading = MakeCreationText(WidgetTree, TEXT("ClassHeading"), 17.0f, FLinearColor(0.92f, 0.86f, 0.68f, 1.0f));
	if (ClassHeading)
	{
		ClassHeading->SetText(FText::FromString(TEXT("Starting Class")));
	}
	AddVerticalChild(ClassStack, ClassHeading, 10.0f);
	ClassButtons.Reset();
	for (int32 Index = 0; Index < ClassOrder.Num(); ++Index)
	{
		FEmbermereClassDefinition Definition;
		GetEffectiveRules()->GetClassDefinition(ClassOrder[Index], Definition);
		UEmbermereCharacterCreationOptionButton* Button = WidgetTree->ConstructWidget<UEmbermereCharacterCreationOptionButton>(
			UEmbermereCharacterCreationOptionButton::StaticClass(),
			*FString::Printf(TEXT("ClassOption_%02d"), Index));
		UTextBlock* Label = MakeCreationText(WidgetTree, *FString::Printf(TEXT("ClassOptionLabel_%02d"), Index), 14.0f, FLinearColor::White, ETextJustify::Center);
		if (Button && Label)
		{
			Label->SetText(Definition.DisplayName);
			Button->SetContent(Label);
			Button->SetToolTipText(Definition.Description);
			Button->InitializeOption(Index, false);
			Button->OnOptionChosen.AddUniqueDynamic(this, &UEmbermereCharacterCreationWidget::HandleOptionChosen);
			AddVerticalChild(ClassStack, Button, 7.0f);
			ClassButtons.Add(Button);
		}
	}

	UTextBlock* DetailHeading = MakeCreationText(WidgetTree, TEXT("CreationDetailHeading"), 17.0f, FLinearColor(0.92f, 0.86f, 0.68f, 1.0f));
	if (DetailHeading)
	{
		DetailHeading->SetText(FText::FromString(TEXT("Your Path")));
	}
	AddVerticalChild(DetailStack, DetailHeading, 10.0f);
	DetailText = MakeCreationText(WidgetTree, TEXT("CreationDetailText"), 13.0f, FLinearColor(0.9f, 0.92f, 0.86f, 1.0f));
	if (DetailText)
	{
		DetailText->SetAutoWrapText(true);
	}
	AddVerticalChild(DetailStack, DetailText, 0.0f);

	UHorizontalBox* Footer = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("CharacterCreationFooter"));
	AddVerticalChild(MainStack, Footer, 0.0f);
	ValidationText = MakeCreationText(WidgetTree, TEXT("CharacterCreationValidation"), 13.0f, FLinearColor::White);
	ConfirmButton = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("CharacterCreationConfirmButton"));
	UTextBlock* ConfirmLabel = MakeCreationText(WidgetTree, TEXT("CharacterCreationConfirmLabel"), 16.0f, FLinearColor::White, ETextJustify::Center);
	if (ConfirmLabel)
	{
		ConfirmLabel->SetText(FText::FromString(TEXT("Enter Embermere")));
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetContent(ConfirmLabel);
		ConfirmButton->OnClicked.AddUniqueDynamic(this, &UEmbermereCharacterCreationWidget::HandleConfirmClicked);
	}
	if (Footer)
	{
		if (UHorizontalBoxSlot* Slot = Footer->AddChildToHorizontalBox(ValidationText))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Fill));
			Slot->SetVerticalAlignment(VAlign_Center);
		}
		if (UHorizontalBoxSlot* Slot = Footer->AddChildToHorizontalBox(ConfirmButton))
		{
			Slot->SetSize(FSlateChildSize(ESlateSizeRule::Automatic));
			Slot->SetPadding(FMargin(12.0f, 0.0f, 0.0f, 0.0f));
		}
	}

	RefreshPresentation();
}

void UEmbermereCharacterCreationWidget::RefreshPresentation()
{
	for (int32 Index = 0; Index < RaceButtons.Num() && Index < RaceOrder.Num(); ++Index)
	{
		const bool bSelected = RaceOrder[Index] == SelectedRace;
		RaceButtons[Index]->SetIsEnabled(!bConfirmationComplete);
		RaceButtons[Index]->SetBackgroundColor(bSelected
			? FLinearColor(0.18f, 0.48f, 0.36f, 1.0f)
			: FLinearColor(0.12f, 0.15f, 0.13f, 1.0f));
	}
	for (int32 Index = 0; Index < ClassButtons.Num() && Index < ClassOrder.Num(); ++Index)
	{
		const bool bAvailable = IsClassAvailable(ClassOrder[Index]);
		const bool bSelected = ClassOrder[Index] == SelectedClass;
		ClassButtons[Index]->SetIsEnabled(bAvailable && !bConfirmationComplete);
		ClassButtons[Index]->SetBackgroundColor(bSelected
			? (bAvailable ? FLinearColor(0.52f, 0.34f, 0.08f, 1.0f) : FLinearColor(0.36f, 0.08f, 0.06f, 1.0f))
			: (bAvailable ? FLinearColor(0.12f, 0.15f, 0.13f, 1.0f) : FLinearColor(0.055f, 0.06f, 0.055f, 1.0f)));
	}

	if (DetailText)
	{
		DetailText->SetText(GetSelectionSummary());
	}
	const bool bValid = IsCurrentChoiceValid();
	if (ValidationText)
	{
		ValidationText->SetText(bConfirmationComplete
			? FText::FromString(TEXT("Journey initialized."))
			: (bValid
				? FText::FromString(TEXT("Tab changes column; arrows choose; Enter confirms."))
				: FText::FromString(TEXT("That class is unavailable to this race. Choose another class."))));
		ValidationText->SetColorAndOpacity(FSlateColor(bValid || bConfirmationComplete
			? FLinearColor(0.58f, 0.92f, 0.62f, 1.0f)
			: FLinearColor(1.0f, 0.38f, 0.26f, 1.0f)));
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bValid);
		ConfirmButton->SetBackgroundColor(bValid
			? FLinearColor(0.68f, 0.38f, 0.08f, 1.0f)
			: FLinearColor(0.08f, 0.08f, 0.08f, 1.0f));
	}
}

void UEmbermereCharacterCreationWidget::MoveRaceSelection(int32 Direction)
{
	const int32 CurrentIndex = RaceOrder.IndexOfByKey(SelectedRace);
	const int32 NewIndex = (CurrentIndex + Direction + RaceOrder.Num()) % RaceOrder.Num();
	SetPendingRace(RaceOrder[NewIndex]);
}

void UEmbermereCharacterCreationWidget::MoveClassSelection(int32 Direction)
{
	const int32 CurrentIndex = ClassOrder.IndexOfByKey(SelectedClass);
	for (int32 Step = 1; Step <= ClassOrder.Num(); ++Step)
	{
		const int32 NewIndex = (CurrentIndex + Direction * Step + ClassOrder.Num() * 2) % ClassOrder.Num();
		if (IsClassAvailable(ClassOrder[NewIndex]))
		{
			SetPendingClass(ClassOrder[NewIndex]);
			return;
		}
	}
}

void UEmbermereCharacterCreationWidget::HandleOptionChosen(int32 OptionIndex, bool bRaceOption)
{
	if (bRaceOption && RaceOrder.IsValidIndex(OptionIndex))
	{
		SetPendingRace(RaceOrder[OptionIndex]);
	}
	else if (!bRaceOption && ClassOrder.IsValidIndex(OptionIndex))
	{
		SetPendingClass(ClassOrder[OptionIndex]);
	}
}

void UEmbermereCharacterCreationWidget::HandleConfirmClicked()
{
	TryConfirmChoice();
}
