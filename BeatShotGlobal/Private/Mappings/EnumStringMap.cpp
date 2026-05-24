// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/EnumStringMap.h"

UEnumStringMap::UEnumStringMap()
{
}

void UEnumStringMap::PostLoad()
{
	for (auto& [Key, Value] : EnumStringMap)
	{
		// Add any Enum Values not present in EnumTagPairs
		for (int64 i = 0; i < Key->GetMaxEnumValue(); i++)
		{
			if (!Value.EnumStringPairs.Find(i))
			{
				const FText EnumValueText = Key->GetDisplayNameTextByValue(i);
				Value.EnumStringPairs.Emplace(i, FEnumStringPair(EnumValueText.ToString(), i));
			}
			if (!Value.EnumStringPairs[i].DisplayText.IsFromStringTable())
			{
				FString EnumClassName = Key->CppType;
				if (EnumClassName.StartsWith("E"))
				{
					EnumClassName.RemoveAt(0);
				}
				const FString EnumValueString = Key->GetNameStringByValue(i);
				const FString StringTableKey = FString::Format(TEXT("{0}_{1}"), {EnumClassName, EnumValueString});
				const FText StringTableText = FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets",
				                                                     StringTableKey);
				if (!StringTableText.IsFromStringTable())
				{
					UE_LOG(LogTemp, Warning, TEXT("Couldn't find string table entry for %s"), *StringTableKey);
				}
				Value.EnumStringPairs[i].DisplayText = StringTableText;
			}
		}
	}

	Super::PostLoad();
}

const TMap<UEnum*, FEnumStringMapping>& UEnumStringMap::GetEnumStringMap() const
{
	return EnumStringMap;
}

void UEnumStringMap::PopulateEnumTypes(const TSet<UEnum*>& InTypes)
{
	for (UEnum* Enum : InTypes)
	{
		if (!EnumStringMap.Contains(Enum))
		{
			EnumStringMap.Emplace(Enum, FEnumStringMapping(Enum));
		}
	}
}
