// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/EnumTagMap.h"
#include "UObject/ObjectSaveContext.h"


void FEnumTagPair::AddParentTags(const FGameplayTagContainer& InParentTags)
{
	ParentTags.AppendTags(InParentTags);

	for (const FGameplayTag& Tag : InParentTags)
	{
		if (Tags.HasTagExact(Tag))
		{
			Tags.RemoveTag(Tag);
		}
	}
}

FEnumTagMapping::FEnumTagMapping(const UEnum* InEnum)
{
	Enum = InEnum;
	EnumClass = Enum->CppType;
	for (int32 Index = 0; Index < Enum->NumEnums(); Index++)
	{
		const int64 ValueByIndex = Enum->GetValueByIndex(Index);
		// Skip hidden values if needed
#if WITH_METADATA
		if (Enum->HasMetaData(TEXT("Hidden"), Index))
		{
			continue;
		}
#endif
		// Skip max values if needed
		if (Enum->GetMaxEnumValue() == ValueByIndex)
		{
			continue;
		}
		EnumTagPairs.Emplace(ValueByIndex, FEnumTagPair(GetEnumText(Enum, ValueByIndex), ValueByIndex));
	}
}

void UEnumTagMap::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	for (auto& [Enum, EnumTageMapping] : EnumTagMap)
	{
		for (auto& [Index, EnumTagPair] : EnumTageMapping.EnumTagPairs)
		{
			EnumTagPair.AddParentTags(EnumTageMapping.ParentTags);
		}
	}

	Super::PreSave(ObjectSaveContext);
}

void UEnumTagMap::PostLoad()
{
	Super::PostLoad();

	for (auto& [Key, Value] : EnumTagMap)
	{
		// Add any Enum Values not present in EnumTagPairs
		for (int32 Index = 0; Index < Key->NumEnums(); Index++)
		{
			const int64 ValueByIndex = Key->GetValueByIndex(Index);
			// Skip hidden values if needed
#if WITH_METADATA
			if (Key->HasMetaData(TEXT("Hidden"), Index))
			{
				continue;
			}
#endif
			// Skip max values if needed
			if (Key->GetMaxEnumValue() == ValueByIndex)
			{
				continue;
			}
			if (!Value.EnumTagPairs.Find(ValueByIndex))
			{
				Value.EnumTagPairs.Emplace(ValueByIndex, FEnumTagPair(GetEnumText(Key, ValueByIndex), ValueByIndex));
			}
			else if (!Value.EnumTagPairs[ValueByIndex].DisplayText.IsFromStringTable())
			{
				Value.EnumTagPairs[ValueByIndex].DisplayText = GetEnumText(Key, ValueByIndex);
			}
		}
	}
}

const TMap<UEnum*, FEnumTagMapping>& UEnumTagMap::GetEnumTagMap() const
{
	return EnumTagMap;
}

void UEnumTagMap::PopulateEnumTypes(const TSet<UEnum*>& InTypes)
{
	for (UEnum* Enum : InTypes)
	{
		if (!EnumTagMap.Contains(Enum))
		{
			EnumTagMap.Emplace(Enum, FEnumTagMapping(Enum));
		}
	}
}
