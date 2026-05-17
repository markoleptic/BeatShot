// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/BSInventoryItemDefinition.h"

const UBSInventoryItemFragment* UBSInventoryItemDefinition::FindFragmentByClass(
	const TSubclassOf<UBSInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const auto& Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment.Get();
			}
		}
	}

	return nullptr;
}
