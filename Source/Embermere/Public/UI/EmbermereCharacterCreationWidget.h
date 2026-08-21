#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Types/EmbermereTypes.h"
#include "EmbermereCharacterCreationWidget.generated.h"

class AEmbermereCharacter;
class UEmbermereRulesData;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FEmbermereCharacterChoiceSignature,
	EEmbermereRace,
	Race,
	EEmbermereClass,
	Class);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FEmbermereCreationOptionChosenSignature,
	int32,
	OptionIndex,
	bool,
	bRaceOption);

UCLASS()
class EMBERMERE_API UEmbermereCharacterCreationOptionButton : public UButton
{
	GENERATED_BODY()

public:
	void InitializeOption(int32 NewOptionIndex, bool bNewRaceOption);

	UPROPERTY(BlueprintAssignable, Category = "Embermere|Character Creation")
	FEmbermereCreationOptionChosenSignature OnOptionChosen;

private:
	UFUNCTION()
	void HandleClicked();

	int32 OptionIndex = INDEX_NONE;
	bool bRaceOption = false;
};

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermereCharacterCreationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rules")
	TObjectPtr<UEmbermereRulesData> RulesData;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	EEmbermereRace SelectedRace = EEmbermereRace::Human;

	UPROPERTY(BlueprintReadOnly, Category = "Selection")
	EEmbermereClass SelectedClass = EEmbermereClass::Warrior;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FEmbermereCharacterChoiceSignature OnCharacterChoiceConfirmed;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	void BindToCharacter(AEmbermereCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	bool SetPendingRace(EEmbermereRace Race);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	bool SetPendingClass(EEmbermereClass Class);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	bool SetRaceAndClass(EEmbermereRace Race, EEmbermereClass Class);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	void ConfirmChoice(AEmbermereCharacter* Character);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Character Creation")
	bool TryConfirmChoice(AEmbermereCharacter* Character = nullptr);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	bool IsClassAvailable(EEmbermereClass Class) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	bool IsCurrentChoiceValid() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	bool IsConfirmationComplete() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	int32 GetRaceOptionCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	int32 GetClassOptionCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	FVector2D GetPanelDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Character Creation")
	FText GetSelectionSummary() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	UPROPERTY(Transient)
	TObjectPtr<AEmbermereCharacter> BoundCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UEmbermereRulesData> RuntimeRulesData;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereCharacterCreationOptionButton>> RaceButtons;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UEmbermereCharacterCreationOptionButton>> ClassButtons;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DetailText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> ValidationText;

	UPROPERTY(Transient)
	TObjectPtr<UButton> ConfirmButton;

	bool bConfirmationComplete = false;
	bool bKeyboardFocusOnClasses = false;

	UEmbermereRulesData* GetEffectiveRules() const;
	void BuildDefaultLayout();
	void RefreshPresentation();
	void MoveRaceSelection(int32 Direction);
	void MoveClassSelection(int32 Direction);

	UFUNCTION()
	void HandleOptionChosen(int32 OptionIndex, bool bRaceOption);

	UFUNCTION()
	void HandleConfirmClicked();
};
