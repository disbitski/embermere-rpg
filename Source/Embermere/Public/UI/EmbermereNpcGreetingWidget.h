#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmbermereNpcGreetingWidget.generated.h"

class UBorder;
class UTextBlock;

UENUM(BlueprintType)
enum class EEmbermereNpcGreetingState : uint8
{
	Hidden,
	Available,
	Active,
	ReadyToTurnIn,
	Completed
};

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermereNpcGreetingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Embermere|NPC|Greeting")
	void SetGreetingState(EEmbermereNpcGreetingState NewState, const FText& NewGreetingText);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Greeting")
	EEmbermereNpcGreetingState GetGreetingState() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Greeting")
	FText GetGreetingText() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|NPC|Greeting")
	FVector2D GetPanelDimensions() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StateMarkerText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> GreetingTextBlock;

	EEmbermereNpcGreetingState GreetingState = EEmbermereNpcGreetingState::Hidden;
	FText GreetingText;

	void BuildDefaultLayout();
	void UpdatePresentation();
};
