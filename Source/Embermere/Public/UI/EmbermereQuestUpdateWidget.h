#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "EmbermereQuestUpdateWidget.generated.h"

class UBorder;
class UTextBlock;
class UEmbermereStatsComponent;

UCLASS()
class EMBERMERE_API UEmbermereQuestUpdateWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void BindToQuestLog(UEmbermereQuestLogComponent* Log, UEmbermereStatsComponent* Stats);
	void PresentUpdate(const FEmbermereQuestUpdate& Update);
	void AdvancePresentation(float DeltaSeconds);
	void SetSuppressed(bool bSuppress);
	void ClearPresentation();

	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	bool IsUpdateVisible() const { return bVisible; }
	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FText GetUpdateHeading() const;
	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FText GetUpdateTitle() const { return CurrentUpdate.Title; }
	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FText GetUpdateDetail() const;
	UFUNCTION(BlueprintPure, Category = "Embermere|Quest")
	FVector2D GetPanelDimensions() const { return FVector2D(420.0f, 108.0f); }
	static constexpr float LifetimeSeconds = 3.0f;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeDestruct() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UEmbermereQuestLogComponent> BoundLog;
	UPROPERTY(Transient)
	TObjectPtr<UEmbermereStatsComponent> BoundStats;
	UPROPERTY(Transient)
	TObjectPtr<UBorder> Panel;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Heading;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Title;
	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> Detail;

	FEmbermereQuestUpdate CurrentUpdate;
	uint64 LastSequence = 0;
	float AgeSeconds = 0.0f;
	bool bVisible = false;
	bool bSuppressed = false;
	void BuildLayout();
	void RefreshVisuals();
	UFUNCTION()
	void HandleDeath();
};
