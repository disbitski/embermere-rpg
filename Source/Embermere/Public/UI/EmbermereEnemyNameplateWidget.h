#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmbermereEnemyNameplateWidget.generated.h"

class UBorder;
class UProgressBar;
class UTextBlock;

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermereEnemyNameplateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Embermere|Nameplate")
	void SetNameplateState(const FText& DisplayName, float CurrentHealth, float MaxHealth, const FLinearColor& AccentColor, bool bIsSelected);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Nameplate")
	bool IsNameplateSelected() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UBorder> RootPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> SelectedMarkerText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> HealthText;

	UPROPERTY(Transient)
	TObjectPtr<UProgressBar> HealthBar;

	bool bSelected = false;

	void BuildDefaultLayout();
	void UpdateVisibility();
};
