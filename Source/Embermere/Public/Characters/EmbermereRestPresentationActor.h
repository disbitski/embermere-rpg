#pragma once

#include "CoreMinimal.h"
#include "Components/EmbermereRestServiceComponent.h"
#include "GameFramework/Actor.h"
#include "EmbermereRestPresentationActor.generated.h"

class AEmbermereCharacter;
class AEmbermereRestServiceActor;
class UMaterialInstanceDynamic;
class USceneComponent;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EEmbermereRestPresentationPhase : uint8
{
	Hidden,
	Channeling,
	Completion
};

UCLASS()
class EMBERMERE_API AEmbermereRestPresentationActor : public AActor
{
	GENERATED_BODY()

public:
	AEmbermereRestPresentationActor();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Rest Presentation")
	TObjectPtr<AEmbermereRestServiceActor> ObservedRestService;

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Rest")
	void SetObservedRestService(AEmbermereRestServiceActor* NewRestService);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Rest")
	void BindToRestService();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Rest")
	void UnbindFromRestService();

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Rest")
	void AdvanceRestPresentation(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Presentation|Rest")
	void ClearRestPresentation();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	EEmbermereRestPresentationPhase GetPresentationPhase() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	bool IsRestPresentationVisible() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	bool IsBoundToRestService() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	int32 GetRestPresentationSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	int32 GetVisibleRestPresentationSegmentCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	bool AreRestPresentationSegmentsNonColliding() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	float GetRestPresentationRadius() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	float GetCompletionLifetimeSeconds() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	FLinearColor GetRestPresentationColor() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Embermere|Presentation|Rest")
	FString GetRestPresentationMaterialPath() const;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaSeconds) override;

private:
	void EnsurePresentationComponents();
	void StartChannelPresentation(AEmbermereCharacter* Character);
	void StartCompletionPresentation(AEmbermereCharacter* Character);
	void UpdateSegmentPresentation(float DeltaSeconds);

	void HandleRestOutcome(const FEmbermereRestOutcome& Outcome);

	UPROPERTY(VisibleAnywhere, Transient, Category = "Rest Presentation")
	TArray<TObjectPtr<UStaticMeshComponent>> PresentationSegments;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UMaterialInstanceDynamic>> PresentationMaterials;

	TWeakObjectPtr<AEmbermereRestServiceActor> BoundRestService;
	FDelegateHandle RestOutcomeHandle;
	TWeakObjectPtr<AEmbermereCharacter> ActiveCharacter;
	EEmbermereRestPresentationPhase PresentationPhase = EEmbermereRestPresentationPhase::Hidden;
	float PresentationAgeSeconds = 0.0f;
	float PresentationRotationDegrees = 0.0f;
	float PresentationRadius = 0.0f;
	FLinearColor PresentationColor = FLinearColor::Transparent;
	FString PresentationMaterialPath;

	static constexpr int32 SegmentCount = 12;
	static constexpr float ChannelRadius = 46.0f;
	static constexpr float CompletionStartRadius = 52.0f;
	static constexpr float CompletionEndRadius = 120.0f;
	static constexpr float PresentationHeight = 55.0f;
	static constexpr float CompletionLifetimeSeconds = 1.1f;
	static constexpr float RotationSpeedDegreesPerSecond = 42.0f;

#if WITH_DEV_AUTOMATION_TESTS
	int32 ObservedOutcomeCount = 0;
	friend class FEmbermereRestWorldPresentationTest;
#endif
};
