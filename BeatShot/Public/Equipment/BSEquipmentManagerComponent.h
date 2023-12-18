// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Containers/Array.h"
#include "Containers/ArrayView.h"
#include "Containers/Map.h"
#include "Containers/Set.h"
#include "Containers/SparseArray.h"
#include "Containers/UnrealString.h"
#include "HAL/Platform.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Engine/NetSerialization.h"
#include "AbilitySystem/Globals/BSAbilitySet.h"
#include "Templates/SubclassOf.h"
#include "Templates/UnrealTemplate.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "BSEquipmentManagerComponent.generated.h"

class UActorComponent;
class UBSAbilitySystemComponent;
class UBSEquipmentManagerComponent;
class UBSEquipmentDefinition;
class UBSEquipmentInstance;
class UObject;
struct FFrame;
struct FBSEquipmentList;
struct FNetDeltaSerializeInfo;
struct FReplicationFlags;

/** A single piece of applied equipment, which contains a pointer to EquipmentInstance and the EquipmentDefinition */
USTRUCT(BlueprintType)
struct FBSAppliedEquipmentEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBSAppliedEquipmentEntry()
	{
	}

	FString GetDebugString() const;

private:
	friend FBSEquipmentList;
	friend UBSEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UBSEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<UBSEquipmentInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FBSAbilitySet_GrantedHandles GrantedHandles;
};


/** An array of FBSAppliedEquipmentEntries, which each contain a pointer to EquipmentInstance and the EquipmentDefinition */
USTRUCT(BlueprintType)
struct FBSEquipmentList : public FFastArraySerializer
{
	GENERATED_BODY()

	FBSEquipmentList() : OwnerComponent(nullptr)
	{
	}

	FBSEquipmentList(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FBSAppliedEquipmentEntry, FBSEquipmentList>(Entries,
			DeltaParms, *this);
	}

	/** Creates and adds a new FBSAppliedEquipmentEntry, also granting any associated abilities */
	UBSEquipmentInstance* AddEntry(TSubclassOf<UBSEquipmentDefinition> EquipmentDefinition);

	/** Removes an existing FBSAppliedEquipmentEntry, also removing any associated abilities */
	void RemoveEntry(UBSEquipmentInstance* Instance);

private:
	UBSAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UBSEquipmentManagerComponent;

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FBSAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template <>
struct TStructOpsTypeTraits<FBSEquipmentList> : public TStructOpsTypeTraitsBase2<FBSEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


/** The component responsible for equipment management. Contains an FBSEquipmentList, which holds an array of FBSAppliedEquipmentEntries.
 *  Each FBSInventoryEntry contains a pointer to EquipmentInstance and the EquipmentDefinition. Each InventoryItemInstance contains the
 *  item definition and the stack count */
UCLASS(BlueprintType, Const)
class BEATSHOT_API UBSEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UBSEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UBSEquipmentInstance* EquipItem(TSubclassOf<UBSEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(UBSEquipmentInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch,
		FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	UBSEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UBSEquipmentInstance> InstanceType);

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<UBSEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UBSEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)(GetFirstInstanceOfType(T::StaticClass()));
	}

private:
	UPROPERTY(Replicated)
	FBSEquipmentList EquipmentList;
};
