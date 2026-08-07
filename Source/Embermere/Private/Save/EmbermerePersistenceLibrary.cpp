#include "Save/EmbermerePersistenceLibrary.h"

#include "Characters/EmbermereCharacter.h"
#include "Components/EmbermereCombatComponent.h"
#include "Components/EmbermereEquipmentComponent.h"
#include "Components/EmbermereInventoryComponent.h"
#include "Components/EmbermereQuestLogComponent.h"
#include "Components/EmbermereStatsComponent.h"
#include "Components/EmbermereVendorComponent.h"
#include "Components/EmbermereWalletComponent.h"
#include "Data/EmbermereItemData.h"
#include "Data/EmbermereQuestData.h"
#include "Data/EmbermereVendorStockData.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/PackageName.h"
#include "Save/EmbermereSaveGame.h"

namespace
{
	struct FResolvedVendorStock
	{
		UEmbermereVendorComponent* Vendor = nullptr;
		TArray<int32> RemainingQuantities;
	};

	struct FResolvedPersistenceState
	{
		TArray<FEmbermereInventoryStack> InventoryStacks;
		TArray<FEmbermereEquippedItem> EquippedItems;
		FEmbermereQuestState QuestState;
		TArray<FResolvedVendorStock> VendorStocks;
	};

	EEmbermerePersistenceResult Fail(
		EEmbermerePersistenceResult Result,
		FText& OutMessage,
		const FString& Message)
	{
		OutMessage = FText::FromString(Message);
		return Result;
	}

	template <typename AssetType>
	AssetType* ResolveAsset(const FSoftObjectPath& AssetPath)
	{
		if (AssetPath.IsNull())
		{
			return nullptr;
		}
		if (UObject* ResolvedObject = AssetPath.ResolveObject())
		{
			return Cast<AssetType>(ResolvedObject);
		}
		if (!FPackageName::DoesPackageExist(AssetPath.GetLongPackageName()))
		{
			return nullptr;
		}
		return Cast<AssetType>(AssetPath.TryLoad());
	}

	FSoftObjectPath GetPersistentAssetPath(const UObject* Asset)
	{
		return Asset && Asset->IsAsset() ? FSoftObjectPath(Asset) : FSoftObjectPath();
	}

	EEmbermerePersistenceResult BuildVendorMap(
		const TArray<UEmbermereVendorComponent*>& Vendors,
		TMap<FName, UEmbermereVendorComponent*>& OutVendors,
		FText& OutMessage)
	{
		OutVendors.Reset();
		for (UEmbermereVendorComponent* Vendor : Vendors)
		{
			if (!Vendor || Vendor->PersistenceId.IsNone() || !Vendor->StockData)
			{
				return Fail(
					EEmbermerePersistenceResult::VendorMismatch,
					OutMessage,
					TEXT("A persistent vendor is missing its stable ID or stock data."));
			}
			if (OutVendors.Contains(Vendor->PersistenceId))
			{
				return Fail(
					EEmbermerePersistenceResult::VendorMismatch,
					OutMessage,
					FString::Printf(
						TEXT("Persistent vendor ID '%s' is duplicated."),
						*Vendor->PersistenceId.ToString()));
			}
			OutVendors.Add(Vendor->PersistenceId, Vendor);
		}
		return EEmbermerePersistenceResult::Success;
	}

	EEmbermerePersistenceResult ResolveSaveState(
		AEmbermereCharacter* Character,
		const TArray<UEmbermereVendorComponent*>& Vendors,
		const UEmbermereSaveGame* SaveGame,
		FResolvedPersistenceState& OutState,
		FText& OutMessage)
	{
		OutState = FResolvedPersistenceState();
		if (!Character || !SaveGame || !Character->Inventory || !Character->Equipment ||
			!Character->QuestLog || !Character->Wallet || !Character->Stats)
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidRequest,
				OutMessage,
				TEXT("The player persistence owners are unavailable."));
		}
		if (SaveGame->FormatVersion != EmbermereSaveGameVersion::Current)
		{
			return Fail(
				EEmbermerePersistenceResult::UnsupportedVersion,
				OutMessage,
				FString::Printf(
					TEXT("Save version %d is unsupported; expected version %d."),
					SaveGame->FormatVersion,
					EmbermereSaveGameVersion::Current));
		}
		if (SaveGame->Copper < 0 || SaveGame->CurrentExperience < 0)
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The save contains a negative wallet or experience value."));
		}

		for (const FEmbermereSavedInventoryStack& SavedStack : SaveGame->InventoryStacks)
		{
			if (SavedStack.ItemId.IsNone() || SavedStack.ItemAsset.IsNull())
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("An inventory record is missing its stable item identity."));
			}
			UEmbermereItemData* Item = ResolveAsset<UEmbermereItemData>(SavedStack.ItemAsset);
			if (!Item)
			{
				return Fail(
					EEmbermerePersistenceResult::MissingAsset,
					OutMessage,
					FString::Printf(
						TEXT("Inventory asset '%s' could not be resolved."),
						*SavedStack.ItemAsset.ToString()));
			}
			if (Item->ItemId != SavedStack.ItemId || SavedStack.Quantity <= 0 ||
				SavedStack.Quantity > Item->MaxStack)
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					FString::Printf(
						TEXT("Inventory record for '%s' has a mismatched ID or invalid quantity."),
						*SavedStack.ItemId.ToString()));
			}

			FEmbermereInventoryStack& ResolvedStack = OutState.InventoryStacks.AddDefaulted_GetRef();
			ResolvedStack.Item = Item;
			ResolvedStack.Quantity = SavedStack.Quantity;
		}
		if (!Character->Inventory->CanRestoreStacksForSaveGame(OutState.InventoryStacks))
		{
			return Fail(
				EEmbermerePersistenceResult::CapacityConflict,
				OutMessage,
				TEXT("The saved inventory does not fit the current bag contract."));
		}

		for (const FEmbermereSavedEquipmentItem& SavedItem : SaveGame->EquippedItems)
		{
			if (SavedItem.Slot == EEmbermereEquipmentSlot::None ||
				SavedItem.ItemId.IsNone() || SavedItem.ItemAsset.IsNull())
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("An equipment record is missing its slot or stable item identity."));
			}
			UEmbermereItemData* Item = ResolveAsset<UEmbermereItemData>(SavedItem.ItemAsset);
			if (!Item)
			{
				return Fail(
					EEmbermerePersistenceResult::MissingAsset,
					OutMessage,
					FString::Printf(
						TEXT("Equipped asset '%s' could not be resolved."),
						*SavedItem.ItemAsset.ToString()));
			}
			if (Item->ItemId != SavedItem.ItemId || Item->EquipmentSlot != SavedItem.Slot)
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					FString::Printf(
						TEXT("Equipped record for '%s' no longer matches its slot contract."),
						*SavedItem.ItemId.ToString()));
			}

			FEmbermereEquippedItem& ResolvedItem = OutState.EquippedItems.AddDefaulted_GetRef();
			ResolvedItem.Slot = SavedItem.Slot;
			ResolvedItem.Item = Item;
		}
		if (!Character->Equipment->CanRestoreEquippedItemsForSaveGame(
			OutState.EquippedItems,
			Character->Stats->Level))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The saved equipment violates current slot or level rules."));
		}

		if (SaveGame->QuestState.bHasActiveQuest)
		{
			if (SaveGame->QuestState.QuestId.IsNone() || SaveGame->QuestState.QuestAsset.IsNull())
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("The active quest record is missing its stable identity."));
			}
			UEmbermereQuestData* Quest = ResolveAsset<UEmbermereQuestData>(SaveGame->QuestState.QuestAsset);
			if (!Quest)
			{
				return Fail(
					EEmbermerePersistenceResult::MissingAsset,
					OutMessage,
					FString::Printf(
						TEXT("Quest asset '%s' could not be resolved."),
						*SaveGame->QuestState.QuestAsset.ToString()));
			}
			if (Quest->QuestId != SaveGame->QuestState.QuestId)
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("The saved quest ID no longer matches its data asset."));
			}
			OutState.QuestState.Quest = Quest;
			OutState.QuestState.CurrentObjectiveCount = SaveGame->QuestState.CurrentObjectiveCount;
			OutState.QuestState.bCompleted = SaveGame->QuestState.bCompleted;
		}
		else if (!SaveGame->QuestState.QuestId.IsNone() ||
			!SaveGame->QuestState.QuestAsset.IsNull() ||
			SaveGame->QuestState.CurrentObjectiveCount != 0 ||
			SaveGame->QuestState.bCompleted)
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("An empty quest record contains contradictory progression data."));
		}
		if (!Character->QuestLog->CanRestoreQuestStateForSaveGame(OutState.QuestState))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The saved quest progress is outside the current objective contract."));
		}

		TMap<FName, UEmbermereVendorComponent*> VendorMap;
		const EEmbermerePersistenceResult VendorMapResult = BuildVendorMap(Vendors, VendorMap, OutMessage);
		if (VendorMapResult != EEmbermerePersistenceResult::Success)
		{
			return VendorMapResult;
		}
		if (VendorMap.Num() != SaveGame->VendorStocks.Num())
		{
			return Fail(
				EEmbermerePersistenceResult::VendorMismatch,
				OutMessage,
				TEXT("The save and current world expose different persistent vendor sets."));
		}

		TSet<FName> SeenVendorIds;
		for (const FEmbermereSavedVendorStock& SavedVendor : SaveGame->VendorStocks)
		{
			if (SavedVendor.VendorId.IsNone() || SeenVendorIds.Contains(SavedVendor.VendorId))
			{
				return Fail(
					EEmbermerePersistenceResult::VendorMismatch,
					OutMessage,
					TEXT("The save contains a missing or duplicate vendor ID."));
			}
			SeenVendorIds.Add(SavedVendor.VendorId);

			UEmbermereVendorComponent* const* VendorPointer = VendorMap.Find(SavedVendor.VendorId);
			UEmbermereVendorComponent* Vendor = VendorPointer ? *VendorPointer : nullptr;
			if (!Vendor)
			{
				return Fail(
					EEmbermerePersistenceResult::VendorMismatch,
					OutMessage,
					FString::Printf(
						TEXT("Persistent vendor '%s' is not present in the current world."),
						*SavedVendor.VendorId.ToString()));
			}

			UEmbermereVendorStockData* SavedStock = ResolveAsset<UEmbermereVendorStockData>(SavedVendor.StockAsset);
			if (!SavedStock)
			{
				return Fail(
					EEmbermerePersistenceResult::MissingAsset,
					OutMessage,
					FString::Printf(
						TEXT("Vendor stock asset '%s' could not be resolved."),
						*SavedVendor.StockAsset.ToString()));
			}
			if (SavedStock != Vendor->StockData ||
				SavedVendor.StockAsset != GetPersistentAssetPath(Vendor->StockData))
			{
				return Fail(
					EEmbermerePersistenceResult::VendorMismatch,
					OutMessage,
					FString::Printf(
						TEXT("Vendor '%s' no longer uses the saved stock contract."),
						*SavedVendor.VendorId.ToString()));
			}
			if (!Vendor->CanRestoreStockForSaveGame(SavedVendor.RemainingQuantities))
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					FString::Printf(
						TEXT("Vendor '%s' contains invalid finite-stock quantities."),
						*SavedVendor.VendorId.ToString()));
			}

			FResolvedVendorStock& ResolvedVendor = OutState.VendorStocks.AddDefaulted_GetRef();
			ResolvedVendor.Vendor = Vendor;
			ResolvedVendor.RemainingQuantities = SavedVendor.RemainingQuantities;
		}

		return EEmbermerePersistenceResult::Success;
	}
}

EEmbermerePersistenceResult UEmbermerePersistenceLibrary::CaptureGameState(
	AEmbermereCharacter* Character,
	const TArray<UEmbermereVendorComponent*>& Vendors,
	UEmbermereSaveGame*& OutSaveGame,
	FText& OutMessage)
{
	OutSaveGame = nullptr;
	if (!Character || !Character->Inventory || !Character->Equipment ||
		!Character->QuestLog || !Character->Wallet || !Character->Stats)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidRequest,
			OutMessage,
			TEXT("The player persistence owners are unavailable."));
	}

	UEmbermereSaveGame* Candidate = Cast<UEmbermereSaveGame>(
		UGameplayStatics::CreateSaveGameObject(UEmbermereSaveGame::StaticClass()));
	if (!Candidate)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidRequest,
			OutMessage,
			TEXT("The Embermere save object could not be created."));
	}

	Candidate->FormatVersion = EmbermereSaveGameVersion::Current;
	Candidate->Copper = Character->Wallet->Copper;
	Candidate->CurrentExperience = Character->Stats->CurrentExperience;
	for (const FEmbermereInventoryStack& Stack : Character->Inventory->Stacks)
	{
		const FSoftObjectPath ItemPath = GetPersistentAssetPath(Stack.Item);
		if (!Stack.Item || Stack.Item->ItemId.IsNone() || ItemPath.IsNull())
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The live inventory contains an item without a stable data-asset identity."));
		}
		FEmbermereSavedInventoryStack& SavedStack = Candidate->InventoryStacks.AddDefaulted_GetRef();
		SavedStack.ItemId = Stack.Item->ItemId;
		SavedStack.ItemAsset = ItemPath;
		SavedStack.Quantity = Stack.Quantity;
	}
	for (const FEmbermereEquippedItem& EquippedItem : Character->Equipment->EquippedItems)
	{
		const FSoftObjectPath ItemPath = GetPersistentAssetPath(EquippedItem.Item);
		if (!EquippedItem.Item || EquippedItem.Item->ItemId.IsNone() || ItemPath.IsNull())
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The live equipment contains an item without a stable data-asset identity."));
		}
		FEmbermereSavedEquipmentItem& SavedItem = Candidate->EquippedItems.AddDefaulted_GetRef();
		SavedItem.Slot = EquippedItem.Slot;
		SavedItem.ItemId = EquippedItem.Item->ItemId;
		SavedItem.ItemAsset = ItemPath;
	}

	if (Character->QuestLog->ActiveQuest.Quest)
	{
		const UEmbermereQuestData* Quest = Character->QuestLog->ActiveQuest.Quest;
		const FSoftObjectPath QuestPath = GetPersistentAssetPath(Quest);
		if (Quest->QuestId.IsNone() || QuestPath.IsNull())
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The active quest does not have a stable data-asset identity."));
		}
		Candidate->QuestState.bHasActiveQuest = true;
		Candidate->QuestState.QuestId = Quest->QuestId;
		Candidate->QuestState.QuestAsset = QuestPath;
		Candidate->QuestState.CurrentObjectiveCount = Character->QuestLog->ActiveQuest.CurrentObjectiveCount;
		Candidate->QuestState.bCompleted = Character->QuestLog->ActiveQuest.bCompleted;
	}

	TMap<FName, UEmbermereVendorComponent*> VendorMap;
	const EEmbermerePersistenceResult VendorMapResult = BuildVendorMap(Vendors, VendorMap, OutMessage);
	if (VendorMapResult != EEmbermerePersistenceResult::Success)
	{
		return VendorMapResult;
	}
	TArray<FName> VendorIds;
	VendorMap.GetKeys(VendorIds);
	VendorIds.Sort([](const FName& Left, const FName& Right)
	{
		return Left.ToString() < Right.ToString();
	});
	for (const FName VendorId : VendorIds)
	{
		UEmbermereVendorComponent* Vendor = VendorMap.FindChecked(VendorId);
		const FSoftObjectPath StockPath = GetPersistentAssetPath(Vendor->StockData);
		if (StockPath.IsNull())
		{
			return Fail(
				EEmbermerePersistenceResult::VendorMismatch,
				OutMessage,
				FString::Printf(
					TEXT("Vendor '%s' does not use persistent stock data."),
					*VendorId.ToString()));
		}
		FEmbermereSavedVendorStock& SavedVendor = Candidate->VendorStocks.AddDefaulted_GetRef();
		SavedVendor.VendorId = VendorId;
		SavedVendor.StockAsset = StockPath;
		SavedVendor.RemainingQuantities = Vendor->RemainingQuantities;
	}

	FResolvedPersistenceState ValidationState;
	const EEmbermerePersistenceResult ValidationResult = ResolveSaveState(
		Character,
		Vendors,
		Candidate,
		ValidationState,
		OutMessage);
	if (ValidationResult != EEmbermerePersistenceResult::Success)
	{
		return ValidationResult;
	}

	OutSaveGame = Candidate;
	OutMessage = FText::FromString(TEXT("Embermere progress captured."));
	return EEmbermerePersistenceResult::Success;
}

EEmbermerePersistenceResult UEmbermerePersistenceLibrary::ApplyGameState(
	AEmbermereCharacter* Character,
	const TArray<UEmbermereVendorComponent*>& Vendors,
	const UEmbermereSaveGame* SaveGame,
	FText& OutMessage)
{
	FResolvedPersistenceState ResolvedState;
	const EEmbermerePersistenceResult ValidationResult = ResolveSaveState(
		Character,
		Vendors,
		SaveGame,
		ResolvedState,
		OutMessage);
	if (ValidationResult != EEmbermerePersistenceResult::Success)
	{
		return ValidationResult;
	}

	// Every record is resolved and preflighted above; the commit phase has no
	// fallible asset lookup or capacity mutation.
	Character->Inventory->RestoreStacksForSaveGame(ResolvedState.InventoryStacks);
	Character->Equipment->RestoreEquippedItemsForSaveGame(ResolvedState.EquippedItems);
	Character->Stats->ApplyEquipmentBonuses(Character->Equipment->GetTotalStatBonuses());
	Character->Wallet->SetCopperForPrototype(SaveGame->Copper);
	Character->Stats->RestoreExperienceForSaveGame(SaveGame->CurrentExperience);
	Character->QuestLog->RestoreQuestStateForSaveGame(ResolvedState.QuestState);
	for (const FResolvedVendorStock& VendorState : ResolvedState.VendorStocks)
	{
		VendorState.Vendor->RestoreStockForSaveGame(VendorState.RemainingQuantities);
	}

	Character->Stats->InitializeVitals();
	if (Character->Combat)
	{
		Character->Combat->SetTarget(nullptr);
	}
	OutMessage = FText::FromString(
		TEXT("Embermere progress loaded. Combat state and buyback history were reset."));
	return EEmbermerePersistenceResult::Success;
}

EEmbermerePersistenceResult UEmbermerePersistenceLibrary::SaveWorldStateToSlot(
	AEmbermereCharacter* Character,
	const FString& SlotName,
	int32 UserIndex,
	FText& OutMessage)
{
	if (!Character || SlotName.IsEmpty() || UserIndex < 0)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidRequest,
			OutMessage,
			TEXT("A player, save-slot name, and non-negative user index are required."));
	}

	UEmbermereSaveGame* SaveGame = nullptr;
	const TArray<UEmbermereVendorComponent*> Vendors = CollectPersistentVendors(Character->GetWorld());
	const EEmbermerePersistenceResult CaptureResult = CaptureGameState(
		Character,
		Vendors,
		SaveGame,
		OutMessage);
	if (CaptureResult != EEmbermerePersistenceResult::Success)
	{
		return CaptureResult;
	}

	if (!UGameplayStatics::SaveGameToSlot(SaveGame, SlotName, UserIndex))
	{
		return Fail(
			EEmbermerePersistenceResult::SlotUnavailable,
			OutMessage,
			FString::Printf(TEXT("Save slot '%s' could not be written."), *SlotName));
	}
	OutMessage = FText::FromString(FString::Printf(TEXT("Progress saved to %s."), *SlotName));
	return EEmbermerePersistenceResult::Success;
}

EEmbermerePersistenceResult UEmbermerePersistenceLibrary::LoadWorldStateFromSlot(
	AEmbermereCharacter* Character,
	const FString& SlotName,
	int32 UserIndex,
	FText& OutMessage)
{
	if (!Character || SlotName.IsEmpty() || UserIndex < 0)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidRequest,
			OutMessage,
			TEXT("A player, save-slot name, and non-negative user index are required."));
	}
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return Fail(
			EEmbermerePersistenceResult::SlotUnavailable,
			OutMessage,
			FString::Printf(TEXT("Save slot '%s' does not exist."), *SlotName));
	}

	const UEmbermereSaveGame* SaveGame = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!SaveGame)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutMessage,
			FString::Printf(TEXT("Save slot '%s' is not an Embermere save."), *SlotName));
	}

	return ApplyGameState(
		Character,
		CollectPersistentVendors(Character->GetWorld()),
		SaveGame,
		OutMessage);
}

EEmbermerePersistenceResult UEmbermerePersistenceLibrary::InspectSaveSlot(
	const FString& SlotName,
	int32 UserIndex,
	FText& OutSummary)
{
	if (SlotName.IsEmpty() || UserIndex < 0)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidRequest,
			OutSummary,
			TEXT("A save-slot name and non-negative user index are required."));
	}
	if (!UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		return Fail(
			EEmbermerePersistenceResult::SlotUnavailable,
			OutSummary,
			TEXT("No saved journey yet."));
	}

	const UEmbermereSaveGame* SaveGame = Cast<UEmbermereSaveGame>(
		UGameplayStatics::LoadGameFromSlot(SlotName, UserIndex));
	if (!SaveGame)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutSummary,
			TEXT("The local save cannot be read as an Embermere journey."));
	}
	if (SaveGame->FormatVersion != EmbermereSaveGameVersion::Current)
	{
		return Fail(
			EEmbermerePersistenceResult::UnsupportedVersion,
			OutSummary,
			FString::Printf(
				TEXT("Save version %d is unsupported; this build expects version %d."),
				SaveGame->FormatVersion,
				EmbermereSaveGameVersion::Current));
	}

	FString QuestSummary = TEXT("No active quest");
	if (SaveGame->QuestState.bHasActiveQuest)
	{
		QuestSummary = SaveGame->QuestState.bCompleted
			? TEXT("Quest complete")
			: FString::Printf(TEXT("Quest progress %d"), SaveGame->QuestState.CurrentObjectiveCount);
	}
	OutSummary = FText::FromString(FString::Printf(
		TEXT("%d copper  |  %d XP\n%d bag stacks  |  %d equipped  |  %s"),
		SaveGame->Copper,
		SaveGame->CurrentExperience,
		SaveGame->InventoryStacks.Num(),
		SaveGame->EquippedItems.Num(),
		*QuestSummary));
	return EEmbermerePersistenceResult::Success;
}

TArray<UEmbermereVendorComponent*> UEmbermerePersistenceLibrary::CollectPersistentVendors(
	const UWorld* World)
{
	TArray<UEmbermereVendorComponent*> Vendors;
	if (!World)
	{
		return Vendors;
	}

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		if (UEmbermereVendorComponent* Vendor = It->FindComponentByClass<UEmbermereVendorComponent>())
		{
			if (!Vendor->PersistenceId.IsNone())
			{
				Vendors.Add(Vendor);
			}
		}
	}
	Vendors.Sort([](const UEmbermereVendorComponent& Left, const UEmbermereVendorComponent& Right)
	{
		return Left.PersistenceId.ToString() < Right.PersistenceId.ToString();
	});
	return Vendors;
}
