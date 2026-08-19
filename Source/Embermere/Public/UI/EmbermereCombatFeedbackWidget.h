#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EmbermereCombatComponent.h"
#include "EmbermereCombatFeedbackWidget.generated.h"

class UBorder;
class UCanvasPanel;
class UTextBlock;
class UWidgetTree;

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermereCombatFeedbackWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UEmbermereCombatFeedbackWidget(const FObjectInitializer& ObjectInitializer);

	void PresentCombatResult(const FEmbermereCombatResult& Result);
	void AdvancePresentation(float DeltaSeconds);
	void ClearForTarget(const AActor* Target);
	void ClearAllFeedback();
	void SynchronizeViewportBounds();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Combat Feedback")
	int32 GetActiveFeedbackCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Combat Feedback")
	FText GetFeedbackDisplayText(int32 EntryIndex) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Combat Feedback")
	FVector2D GetFeedbackSlotDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Combat Feedback")
	int32 GetMaximumFeedbackEntries() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Combat Feedback")
	float GetFeedbackLifetimeSeconds() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	struct FFeedbackEntry
	{
		TWeakObjectPtr<AActor> Target;
		FText DisplayText;
		FLinearColor Color = FLinearColor::White;
		float AgeSeconds = 0.0f;
	};

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBorder>> FeedbackPanels;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UTextBlock>> FeedbackTexts;

	TArray<FFeedbackEntry> ActiveEntries;
	FVector2D CachedViewportSize = FVector2D::ZeroVector;

	void BuildDefaultLayout();
	void BuildFeedbackPanels(UWidgetTree* LayoutTree, UCanvasPanel* HostCanvas);
	void RefreshFeedbackVisuals();
};
