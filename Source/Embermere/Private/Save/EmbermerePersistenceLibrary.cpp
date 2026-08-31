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
#include "Data/EmbermereRulesData.h"
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
		EEmbermereRace Race = EEmbermereRace::Human;
		EEmbermereClass Class = EEmbermereClass::Warrior;
		int32 Level = 1;
		TArray<FEmbermereInventoryStack> InventoryStacks;
		TArray<FEmbermereEquippedItem> EquippedItems;
		TArray<FEmbermereQuestState> QuestStates;
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

	bool ResolveSavedCharacterIdentity(
		const UEmbermereSaveGame* SaveGame,
		EEmbermereRace& OutRace,
		EEmbermereClass& OutClass,
		FString& OutError)
	{
		OutRace = EEmbermereRace::Human;
		OutClass = EEmbermereClass::Warrior;
		OutError.Reset();
		if (!SaveGame)
		{
			OutError = TEXT("The save record is unavailable.");
			return false;
		}
		if (SaveGame->FormatVersion == EmbermereSaveGameVersion::ProgressionOnly)
		{
			return true;
		}
		if (SaveGame->FormatVersion < EmbermereSaveGameVersion::CharacterIdentity ||
			SaveGame->RaceId.IsNone() || SaveGame->ClassId.IsNone())
		{
			OutError = TEXT("The save is missing its stable character identity.");
			return false;
		}
		if (!UEmbermereRulesData::TryResolveStableRaceId(SaveGame->RaceId, OutRace))
		{
			OutError = FString::Printf(
				TEXT("Saved race ID '%s' is unknown."),
				*SaveGame->RaceId.ToString());
			return false;
		}
		if (!UEmbermereRulesData::TryResolveStableClassId(SaveGame->ClassId, OutClass))
		{
			OutError = FString::Printf(
				TEXT("Saved class ID '%s' is unknown."),
				*SaveGame->ClassId.ToString());
			return false;
		}
		return true;
	}

	bool IsLegacyQuestStateEmpty(const FEmbermereSavedQuestState& QuestState)
	{
		return !QuestState.bHasActiveQuest && QuestState.QuestId.IsNone() &&
			QuestState.QuestAsset.IsNull() && QuestState.CurrentObjectiveCount == 0 &&
			!QuestState.bCompleted;
	}

	EEmbermerePersistenceResult ResolveQuestRecord(
		FName QuestId,
		const FSoftObjectPath& QuestAsset,
		FName ObjectiveId,
		int32 CurrentObjectiveCount,
		bool bCompleted,
		bool bRequireSavedObjectiveId,
		FEmbermereQuestState& OutQuestState,
		FText& OutMessage)
	{
		OutQuestState = FEmbermereQuestState();
		if (QuestId.IsNone() || QuestAsset.IsNull() ||
			(bRequireSavedObjectiveId && ObjectiveId.IsNone()))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("A quest record is missing its stable quest or objective identity."));
		}

		UEmbermereQuestData* Quest = ResolveAsset<UEmbermereQuestData>(QuestAsset);
		if (!Quest)
		{
			return Fail(
				EEmbermerePersistenceResult::MissingAsset,
				OutMessage,
				FString::Printf(
					TEXT("Quest asset '%s' could not be resolved."),
					*QuestAsset.ToString()));
		}
		if (Quest->QuestId.IsNone() || Quest->ObjectiveId.IsNone() ||
			Quest->RequiredObjectiveCount <= 0 || Quest->RewardExperience < 0 ||
			Quest->RewardCopper < 0 || Quest->QuestId != QuestId ||
			(bRequireSavedObjectiveId && Quest->ObjectiveId != ObjectiveId))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				FString::Printf(
					TEXT("Quest record '%s' no longer matches its authored identity."),
					*QuestId.ToString()));
		}
		if (CurrentObjectiveCount < 0 ||
			CurrentObjectiveCount > Quest->RequiredObjectiveCount ||
			(bCompleted && CurrentObjectiveCount != Quest->RequiredObjectiveCount))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				FString::Printf(
					TEXT("Quest record '%s' has invalid progress or completion state."),
					*QuestId.ToString()));
		}

		OutQuestState.Quest = Quest;
		OutQuestState.CurrentObjectiveCount = CurrentObjectiveCount;
		OutQuestState.bCompleted = bCompleted;
		return EEmbermerePersistenceResult::Success;
	}

	EEmbermerePersistenceResult ResolveSavedQuestStates(
		const UEmbermereSaveGame* SaveGame,
		TArray<FEmbermereQuestState>& OutQuestStates,
		FText& OutMessage)
	{
		OutQuestStates.Reset();
		if (!SaveGame)
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidRequest,
				OutMessage,
				TEXT("The save record is unavailable."));
		}

		if (SaveGame->FormatVersion == EmbermereSaveGameVersion::MultiQuestLedger)
		{
			if (!IsLegacyQuestStateEmpty(SaveGame->QuestState))
			{
				return Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("A version-3 save contains contradictory legacy quest state."));
			}
			if (SaveGame->QuestStates.Num() > UEmbermereQuestLogComponent::MaxTrackedQuests)
			{
				return Fail(
					EEmbermerePersistenceResult::CapacityConflict,
					OutMessage,
					TEXT("The saved quest ledger exceeds the current bounded capacity."));
			}

			TSet<FName> SeenQuestIds;
			for (const FEmbermereSavedQuestRecord& SavedQuest : SaveGame->QuestStates)
			{
				if (SavedQuest.QuestId.IsNone() || SeenQuestIds.Contains(SavedQuest.QuestId))
				{
					return Fail(
						EEmbermerePersistenceResult::InvalidData,
						OutMessage,
						TEXT("The saved quest ledger contains a missing or duplicate quest ID."));
				}
				SeenQuestIds.Add(SavedQuest.QuestId);

				FEmbermereQuestState& ResolvedQuest = OutQuestStates.AddDefaulted_GetRef();
				const EEmbermerePersistenceResult QuestResult = ResolveQuestRecord(
					SavedQuest.QuestId,
					SavedQuest.QuestAsset,
					SavedQuest.ObjectiveId,
					SavedQuest.CurrentObjectiveCount,
					SavedQuest.bCompleted,
					true,
					ResolvedQuest,
					OutMessage);
				if (QuestResult != EEmbermerePersistenceResult::Success)
				{
					return QuestResult;
				}
			}
			return EEmbermerePersistenceResult::Success;
		}

		if (!SaveGame->QuestStates.IsEmpty())
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("A legacy save contains unsupported version-3 quest records."));
		}
		if (!SaveGame->QuestState.bHasActiveQuest)
		{
			return IsLegacyQuestStateEmpty(SaveGame->QuestState)
				? EEmbermerePersistenceResult::Success
				: Fail(
					EEmbermerePersistenceResult::InvalidData,
					OutMessage,
					TEXT("An empty legacy quest record contains contradictory progression data."));
		}

		FEmbermereQuestState& ResolvedQuest = OutQuestStates.AddDefaulted_GetRef();
		return ResolveQuestRecord(
			SaveGame->QuestState.QuestId,
			SaveGame->QuestState.QuestAsset,
			NAME_None,
			SaveGame->QuestState.CurrentObjectiveCount,
			SaveGame->QuestState.bCompleted,
			false,
			ResolvedQuest,
			OutMessage);
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
		if (!EmbermereSaveGameVersion::IsSupported(SaveGame->FormatVersion))
		{
			return Fail(
				EEmbermerePersistenceResult::UnsupportedVersion,
				OutMessage,
				FString::Printf(
					TEXT("Save version %d is unsupported; this build supports versions %d through %d."),
					SaveGame->FormatVersion,
					EmbermereSaveGameVersion::ProgressionOnly,
					EmbermereSaveGameVersion::Current));
		}
		if (SaveGame->Copper < 0 || SaveGame->CurrentExperience < 0)
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The save contains a negative wallet or experience value."));
		}

		FString IdentityError;
		if (!ResolveSavedCharacterIdentity(
			SaveGame,
			OutState.Race,
			OutState.Class,
			IdentityError))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				IdentityError.IsEmpty()
					? TEXT("The saved character identity is malformed.")
					: IdentityError);
		}
		if (!Character->CanRestoreCharacterProgressionForSaveGame(
			OutState.Race,
			OutState.Class,
			SaveGame->CurrentExperience,
			OutState.Level))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The saved identity and experience cannot resolve under current progression rules."));
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
			OutState.Level))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The saved equipment violates current slot or level rules."));
		}

		const EEmbermerePersistenceResult QuestResult = ResolveSavedQuestStates(
			SaveGame,
			OutState.QuestStates,
			OutMessage);
		if (QuestResult != EEmbermerePersistenceResult::Success)
		{
			return QuestResult;
		}
		if (!Character->QuestLog->CanRestoreQuestStatesForSaveGame(OutState.QuestStates))
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("The saved quest ledger is outside the current runtime contract."));
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
	if (!Character->bHasDeliberateCharacterChoice ||
		!Character->CanRestoreRaceAndClassForSaveGame(Character->Race, Character->Class))
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutMessage,
			TEXT("Confirm a valid character identity before saving the journey."));
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
	Candidate->RaceId = UEmbermereRulesData::GetStableRaceId(Character->Race);
	Candidate->ClassId = UEmbermereRulesData::GetStableClassId(Character->Class);
	if (Candidate->RaceId.IsNone() || Candidate->ClassId.IsNone())
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutMessage,
			TEXT("The confirmed character identity has no stable save ID."));
	}
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

	TArray<FEmbermereQuestState> QuestStates = Character->QuestLog->GetQuestStatesForSaveGame();
	if (!Character->QuestLog->CanRestoreQuestStatesForSaveGame(QuestStates))
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutMessage,
			TEXT("The live quest ledger is outside the current persistence contract."));
	}
	QuestStates.Sort([](const FEmbermereQuestState& Left, const FEmbermereQuestState& Right)
	{
		return Left.Quest && Right.Quest
			? Left.Quest->QuestId.ToString() < Right.Quest->QuestId.ToString()
			: Left.Quest != nullptr;
	});
	for (const FEmbermereQuestState& QuestState : QuestStates)
	{
		const UEmbermereQuestData* Quest = QuestState.Quest;
		const FSoftObjectPath QuestPath = GetPersistentAssetPath(Quest);
		if (!Quest || Quest->QuestId.IsNone() || Quest->ObjectiveId.IsNone() || QuestPath.IsNull())
		{
			return Fail(
				EEmbermerePersistenceResult::InvalidData,
				OutMessage,
				TEXT("A tracked quest does not have a stable data-asset identity."));
		}
		FEmbermereSavedQuestRecord& SavedQuest = Candidate->QuestStates.AddDefaulted_GetRef();
		SavedQuest.QuestId = Quest->QuestId;
		SavedQuest.QuestAsset = QuestPath;
		SavedQuest.ObjectiveId = Quest->ObjectiveId;
		SavedQuest.CurrentObjectiveCount = QuestState.CurrentObjectiveCount;
		SavedQuest.bCompleted = QuestState.bCompleted;
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
	if (!Character->TryRestoreCharacterProgressionForSaveGame(
		ResolvedState.Race,
		ResolvedState.Class,
		SaveGame->CurrentExperience))
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutMessage,
			TEXT("The validated character identity could not be restored."));
	}
	Character->Inventory->RestoreStacksForSaveGame(ResolvedState.InventoryStacks);
	Character->Equipment->RestoreEquippedItemsForSaveGame(ResolvedState.EquippedItems);
	Character->Stats->ApplyEquipmentBonuses(Character->Equipment->GetTotalStatBonuses());
	Character->Wallet->SetCopperForPrototype(SaveGame->Copper);
	Character->QuestLog->RestoreQuestStatesForSaveGame(ResolvedState.QuestStates);
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
		TEXT("Embermere identity and progress loaded. Combat state and buyback history were reset."));
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
	if (!EmbermereSaveGameVersion::IsSupported(SaveGame->FormatVersion))
	{
		return Fail(
			EEmbermerePersistenceResult::UnsupportedVersion,
			OutSummary,
			FString::Printf(
				TEXT("Save version %d is unsupported; this build supports versions %d through %d."),
				SaveGame->FormatVersion,
				EmbermereSaveGameVersion::ProgressionOnly,
				EmbermereSaveGameVersion::Current));
	}

	EEmbermereRace SavedRace = EEmbermereRace::Human;
	EEmbermereClass SavedClass = EEmbermereClass::Warrior;
	FString IdentityError;
	const UEmbermereRulesData* Rules = GetDefault<UEmbermereRulesData>();
	if (!ResolveSavedCharacterIdentity(SaveGame, SavedRace, SavedClass, IdentityError) ||
		!Rules || SaveGame->CurrentExperience < 0)
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutSummary,
			IdentityError.IsEmpty()
				? TEXT("The saved character identity is illegal under current rules.")
				: IdentityError);
	}
	FEmbermereAttributeBlock SavedAttributes;
	int32 SavedLevel = 1;
	if (!Rules->ResolveProgressionAttributes(
		SavedRace,
		SavedClass,
		SaveGame->CurrentExperience,
		SavedAttributes,
		SavedLevel))
	{
		return Fail(
			EEmbermerePersistenceResult::InvalidData,
			OutSummary,
			TEXT("The saved identity and experience cannot resolve under current progression rules."));
	}
	FEmbermereRaceDefinition RaceDefinition;
	FEmbermereClassDefinition ClassDefinition;
	Rules->GetRaceDefinition(SavedRace, RaceDefinition);
	Rules->GetClassDefinition(SavedClass, ClassDefinition);
	FString IdentitySummary = FString::Printf(
		TEXT("%s %s"),
		*RaceDefinition.DisplayName.ToString(),
		*ClassDefinition.DisplayName.ToString());
	if (SaveGame->FormatVersion == EmbermereSaveGameVersion::ProgressionOnly)
	{
		IdentitySummary += TEXT("  |  legacy v1 fallback");
	}
	IdentitySummary += FString::Printf(TEXT("  |  Level %d"), SavedLevel);

	TArray<FEmbermereQuestState> ResolvedQuestStates;
	FText QuestValidationMessage;
	const EEmbermerePersistenceResult QuestResult = ResolveSavedQuestStates(
		SaveGame,
		ResolvedQuestStates,
		QuestValidationMessage);
	if (QuestResult != EEmbermerePersistenceResult::Success)
	{
		OutSummary = QuestValidationMessage;
		return QuestResult;
	}

	FString QuestSummary = TEXT("No tracked quests");
	if (ResolvedQuestStates.Num() == 1)
	{
		QuestSummary = ResolvedQuestStates[0].bCompleted
			? TEXT("Quest complete")
			: FString::Printf(
				TEXT("Quest progress %d"),
				ResolvedQuestStates[0].CurrentObjectiveCount);
	}
	else if (ResolvedQuestStates.Num() > 1)
	{
		int32 CompletedQuestCount = 0;
		for (const FEmbermereQuestState& QuestState : ResolvedQuestStates)
		{
			CompletedQuestCount += QuestState.bCompleted ? 1 : 0;
		}
		QuestSummary = FString::Printf(
			TEXT("%d quests | %d complete"),
			ResolvedQuestStates.Num(),
			CompletedQuestCount);
	}
	OutSummary = FText::FromString(FString::Printf(
		TEXT("%s\n%d copper  |  %d XP\n%d bag stacks  |  %d equipped  |  %s"),
		*IdentitySummary,
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
