// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/EnumTagMap.h"
#include "UObject/ObjectSaveContext.h"

FEnumTagPair::FEnumTagPair()
{
}

FEnumTagPair::FEnumTagPair(const FString& InEnumValue, const uint8 InIndex) : FEnumStringPair(InEnumValue, InIndex)
{
}

FEnumTagPair::FEnumTagPair(const FText& InEnumValue, const uint8 InIndex) : FEnumStringPair(InEnumValue, InIndex)
{
}

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

FEnumTagMapping::FEnumTagMapping() : Enum(nullptr)
{
}

FEnumTagMapping::FEnumTagMapping(const UEnum* InEnum)
{
	Enum = InEnum;
	EnumClass = Enum->CppType;
	for (int64 i = 0; i < Enum->GetMaxEnumValue(); i++)
	{
		const FText EnumValueText = Enum->GetDisplayNameTextByValue(i);
		EnumTagPairs.Emplace(i, FEnumTagPair(EnumValueText, i));
	}
}

UEnumTagMap::UEnumTagMap()
{
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
	for (auto& [Key, Value] : EnumTagMap)
	{
		// Add any Enum Values not present in EnumTagPairs
		for (int64 i = 0; i < Key->GetMaxEnumValue(); i++)
		{
			if (!Value.EnumTagPairs.Find(i))
			{
				const FText EnumValueText = Key->GetDisplayNameTextByValue(i);
				Value.EnumTagPairs.Emplace(i, FEnumTagPair(EnumValueText, i));
			}
			if (!Value.EnumTagPairs[i].DisplayText.IsFromStringTable())
			{
				FString EnumClassName = Key->CppType;
				if (EnumClassName.StartsWith("E"))
				{
					EnumClassName.RemoveAt(0);
				}
				const FString EnumValueString = Key->GetNameStringByValue(i);
				const FString StringTableKey = EnumValueString == "None"
				                               ? EnumValueString
				                               : FString::Format(TEXT("{0}_{1}"), {EnumClassName, EnumValueString});
				const FText StringTableText = FText::FromStringTable("/Game/StringTables/ST_Enums.ST_Enums",
				                                                     StringTableKey);
				if (!StringTableText.IsFromStringTable())
				{
					UE_LOG(LogTemp, Warning, TEXT("Couldn't find string table entry for %s"), *StringTableKey);
				}
				Value.EnumTagPairs[i].DisplayText = StringTableText;
			}
		}
	}

	Super::PostLoad();
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
