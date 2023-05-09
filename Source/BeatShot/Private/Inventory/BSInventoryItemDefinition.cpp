// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/BSInventoryItemDefinition.h"

UBSInventoryItemDefinition::UBSInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

const UBSInventoryItemFragment* UBSInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UBSInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const TObjectPtr<UBSInventoryItemFragment> Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment.Get();
			}
		}
	}

	return nullptr;
}

const UBSInventoryItemFragment* UBSInventoryItemDefinition::FindItemDefinitionFragment(TSubclassOf<UBSInventoryItemDefinition> ItemDef, TSubclassOf<UBSInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UBSInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
