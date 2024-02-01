// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/Fragments/StatFragment.h"
#include "Inventory/BSInventoryItemInstance.h"

void UStatFragment::OnInstanceCreated(UBSInventoryItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);

	for (const TTuple<FGameplayTag, int>& ItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(ItemStat.Key, ItemStat.Value);
	}
}

int32 UStatFragment::GetItemStatByTag(const FGameplayTag& Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}
