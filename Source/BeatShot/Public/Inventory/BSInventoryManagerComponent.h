// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "Containers/SparseArray.h"
#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Templates/SubclassOf.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "BSInventoryManagerComponent.generated.h"

class UBSEquipmentManagerComponent;
class UBSEquipmentInstance;
class UBSInventoryItemDefinition;
class UBSInventoryItemInstance;
class UBSInventoryManagerComponent;
class UObject;
struct FFrame;
struct FBSInventoryList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FBSInventoryChangeMessage
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<UActorComponent> InventoryOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UBSInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 Delta = 0;
};






/** A single entry in an inventory, which contains a pointer to the InventoryItemInstance and the stack count */
USTRUCT(BlueprintType)
struct FBSInventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBSInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FBSInventoryList;
	friend UBSInventoryManagerComponent;

	UPROPERTY()
	TObjectPtr<UBSInventoryItemInstance> Instance = nullptr;

	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(NotReplicated)
	int32 LastObservedCount = INDEX_NONE;
};





/** An array of FBSInventoryEntries, which each contain a pointer to the InventoryItemInstance and the stack count */
USTRUCT(BlueprintType)
struct FBSInventoryList : public FFastArraySerializer
{
	GENERATED_BODY()

	FBSInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FBSInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	TArray<UBSInventoryItemInstance*> GetAllItems() const;
	
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBSInventoryEntry, FBSInventoryList>(Entries, DeltaParms, *this);
	}

	UBSInventoryItemInstance* AddEntry(TSubclassOf<UBSInventoryItemDefinition> ItemDef, int32 StackCount);
	void RemoveEntry(UBSInventoryItemInstance* Instance);

private:
	void BroadcastChangeMessage(FBSInventoryEntry& Entry, int32 OldCount, int32 NewCount);
	
	friend UBSInventoryManagerComponent;
	
	// Replicated list of items
	UPROPERTY()
	TArray<FBSInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};





template<>
struct TStructOpsTypeTraits<FBSInventoryList> : public TStructOpsTypeTraitsBase2<FBSInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};





/** The component responsible for inventory management. Contains an FBSInventoryList, which holds an array of FBSInventoryEntries.
 *  Each FBSInventoryEntry contains a pointer to InventoryItemInstance and the stack count. Each InventoryItemInstance contains the
 *  item definition and the stack count */
UCLASS(BlueprintType)
class BEATSHOT_API UBSInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBSInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	bool CanAddItemDefinition(TSubclassOf<UBSInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	UBSInventoryItemInstance* AddItemDefinition(TSubclassOf<UBSInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void AddItemInstance(UBSInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventory")
	void RemoveItemInstance(UBSInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintPure=false)
	TArray<UBSInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory", BlueprintPure)
	UBSInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UBSInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UBSInventoryItemDefinition> ItemDef) const;
	bool ConsumeItemsByDefinition(TSubclassOf<UBSInventoryItemDefinition> ItemDef, int32 NumToConsume);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY(Replicated)
	FBSInventoryList InventoryList;

public:
	UFUNCTION(BlueprintCallable,  Category = "Inventory | Slots")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category = "Inventory | Slots")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category= "Inventory | Slots")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category= "Inventory | Slots")
	TArray<UBSInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category= "Inventory | Slots")
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= "Inventory | Slots")
	int32 GetLastSlotIndex() const { return LastSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category= "Inventory | Slots")
	UBSInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= "Inventory | Slots")
	UBSInventoryItemInstance* GetLastSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category= "Inventory | Slots")
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= "Inventory | Slots")
	void AddItemToSlot(int32 SlotIndex, UBSInventoryItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= "Inventory | Slots")
	UBSInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= "Inventory | Slots")
	UBSEquipmentInstance* GetEquippedItem() const { return EquippedItem; };
	
	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UBSEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY()
	int32 NumSlots = 3;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<UBSInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	int32 LastSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<UBSEquipmentInstance> EquippedItem;
};
