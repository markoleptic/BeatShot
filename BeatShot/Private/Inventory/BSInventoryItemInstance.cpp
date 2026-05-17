// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Inventory/BSInventoryItemInstance.h"
#include "GameplayTagContainer.h"
#include "Containers/Array.h"
#include "Inventory/BSInventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"
#include "UObject/Class.h"

class FLifetimeProperty;

void UBSInventoryItemInstance::AddIdentifierTags(const FGameplayTagContainer& Tags)
{
	IdentifierTags.AppendTags(Tags);
}

void UBSInventoryItemInstance::RemoveIdentifierTag(const FGameplayTag& Tag)
{
	IdentifierTags.RemoveTag(Tag);
}

bool UBSInventoryItemInstance::HasIdentifierTag(const FGameplayTag& Tag)
{
	return IdentifierTags.HasTag(Tag);
}

void UBSInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void UBSInventoryItemInstance::AddStatTagStack(const FGameplayTag Tag, const int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UBSInventoryItemInstance::RemoveStatTagStack(const FGameplayTag Tag, const int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UBSInventoryItemInstance::GetStatTagStackCount(const FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UBSInventoryItemInstance::HasStatTag(const FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag) != 0;
}

const UBSInventoryItemFragment* UBSInventoryItemInstance::FindFragmentByClass(
	const TSubclassOf<UBSInventoryItemFragment> FragmentClass) const
{
	if (ItemDef != nullptr && FragmentClass != nullptr)
	{
		return GetDefault<UBSInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

void UBSInventoryItemInstance::SetItemDef(const TSubclassOf<UBSInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}
