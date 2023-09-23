// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "EnumTagMap.h"
#include "UObject/ObjectSaveContext.h"

UEnumTagMap::UEnumTagMap()
{
	for (const UEnum* Enum : EnumsToInclude)
	{
		EnumTagMappings.AddUnique(Enum);
	}
	EnumTagMappings.Sort();
}

void UEnumTagMap::PreSave(FObjectPreSaveContext ObjectSaveContext)
{
	for (FEnumTagMapping& TagMapping : EnumTagMappings)
	{
		for (FEnumTagPair& TagPair : TagMapping.EnumTagPairs)
		{
			TagPair.AddParentTags(TagMapping.ParentTags);
		}
	}
	Super::PreSave(ObjectSaveContext);
}

void UEnumTagMap::PostLoad()
{
	for (const UEnum* Enum : EnumsToInclude)
	{
		FEnumTagMapping TagMapping(Enum);
		if (!EnumTagMappings.Contains(TagMapping))
		{
			EnumTagMappings.Add(TagMapping);
		}
	}
	EnumTagMappings.Sort();
	Super::PostLoad();
}

const TArray<FEnumTagMapping>* UEnumTagMap::GetEnumTagMappings() const
{
	return &EnumTagMappings;
}
