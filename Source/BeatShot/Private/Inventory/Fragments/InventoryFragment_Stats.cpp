// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/Fragments/InventoryFragment_Stats.h"
#include "Inventory/BSInventoryItemInstance.h"

void UInventoryFragment_Stats::OnInstanceCreated(UBSInventoryItemInstance* Instance) const
{
	Super::OnInstanceCreated(Instance);

	for (const TTuple<FGameplayTag, int>& ItemStat : InitialItemStats)
	{
		Instance->AddStatTagStack(ItemStat.Key, ItemStat.Value);
	}
}

int32 UInventoryFragment_Stats::GetItemStatByTag(const FGameplayTag& Tag) const
{
	if (const int32* StatPtr = InitialItemStats.Find(Tag))
	{
		return *StatPtr;
	}

	return 0;
}