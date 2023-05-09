// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/BSInventoryItemInstance.h"
#include "Inventory/BSInventoryItemDefinition.h"
#include "Containers/Array.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Class.h"

class FLifetimeProperty;

UBSInventoryItemInstance::UBSInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

void UBSInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void UBSInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UBSInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UBSInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UBSInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return (StatTags.GetStackCount(Tag) == 0) ? false : true;
}

const UBSInventoryItemFragment* UBSInventoryItemInstance::FindFragmentByClass(TSubclassOf<UBSInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UBSInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

void UBSInventoryItemInstance::SetItemDef(TSubclassOf<UBSInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}
