#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EmbermereLevelUpWidget.generated.h"

class UBorder;
class UCanvasPanel;
class UEmbermereStatsComponent;
class UTextBlock;
class UWidgetTree;

UCLASS(Blueprintable)
class EMBERMERE_API UEmbermereLevelUpWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UEmbermereLevelUpWidget(const FObjectInitializer& ObjectInitializer);

	void BindToStats(UEmbermereStatsComponent* InStats);
	void AdvancePresentation(float DeltaSeconds);
	void ClearPresentation();
	void SynchronizeViewportBounds();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	bool IsLevelUpVisible() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	FText GetLevelUpTitle() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	FText GetLevelUpDetail() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	FVector2D GetPanelDimensions() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|HUD|Progression")
	float GetPresentationLifetimeSeconds() const;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UEmbermereStatsComponent> BoundStats;

	UPROPERTY(Transient)
	TObjectPtr<UBorder> LevelUpPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LevelUpTitleText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> LevelUpDetailText;

	FText CachedTitle;
	FText CachedDetail;
	float PresentationAgeSeconds = 0.0f;
	bool bPresentationVisible = false;
	FVector2D CachedViewportSize = FVector2D::ZeroVector;

	void BuildDefaultLayout();
	void RefreshVisuals();

	UFUNCTION()
	void HandleLevelChanged(int32 PreviousLevel, int32 CurrentLevel);
};
