#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EmbermerePersistenceLibrary.generated.h"

class AEmbermereCharacter;
class UEmbermereSaveGame;
class UEmbermereVendorComponent;
class UWorld;

UENUM(BlueprintType)
enum class EEmbermerePersistenceResult : uint8
{
	Success,
	InvalidRequest,
	UnsupportedVersion,
	MissingAsset,
	InvalidData,
	CapacityConflict,
	VendorMismatch,
	SlotUnavailable
};

UCLASS()
class EMBERMERE_API UEmbermerePersistenceLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static EEmbermerePersistenceResult CaptureGameState(
		AEmbermereCharacter* Character,
		const TArray<UEmbermereVendorComponent*>& Vendors,
		UEmbermereSaveGame*& OutSaveGame,
		FText& OutMessage);

	static EEmbermerePersistenceResult ApplyGameState(
		AEmbermereCharacter* Character,
		const TArray<UEmbermereVendorComponent*>& Vendors,
		const UEmbermereSaveGame* SaveGame,
		FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Persistence")
	static EEmbermerePersistenceResult SaveWorldStateToSlot(
		AEmbermereCharacter* Character,
		const FString& SlotName,
		int32 UserIndex,
		FText& OutMessage);

	UFUNCTION(BlueprintCallable, Category = "Embermere|Persistence")
	static EEmbermerePersistenceResult LoadWorldStateFromSlot(
		AEmbermereCharacter* Character,
		const FString& SlotName,
		int32 UserIndex,
		FText& OutMessage);

	static TArray<UEmbermereVendorComponent*> CollectPersistentVendors(const UWorld* World);
};
