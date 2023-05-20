// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "BSInventoryItemDefinition.generated.h"

class UBSInventoryItemInstance;
struct FFrame;

/**
 * Represents one fragment/part of an item definition such as InventoryFragment_Equippable and InventoryFragment_Stats whereas an InventoryItemDefinition is a collection of fragments
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class BEATSHOT_API UBSInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	/** Called when a BSInventoryItem is created and added to a FBSInventoryList */
	virtual void OnInstanceCreated(UBSInventoryItemInstance* Instance) const {}
};


/**
 * Describes the characteristics of an item in an inventory, composed of individual InventoryItemFragments
 */
UCLASS(Blueprintable, Const, Abstract)
class BEATSHOT_API UBSInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UBSInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UBSInventoryItemFragment>> Fragments;

	UPROPERTY(EditDefaultsOnly, Category=Display)
	FGameplayTagContainer ItemTags;
	
	const UBSInventoryItemFragment* FindFragmentByClass(TSubclassOf<UBSInventoryItemFragment> FragmentClass) const;

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UBSInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UBSInventoryItemDefinition> ItemDef, TSubclassOf<UBSInventoryItemFragment> FragmentClass);
};
