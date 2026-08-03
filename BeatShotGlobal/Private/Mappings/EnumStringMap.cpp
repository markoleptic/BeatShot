// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Mappings/EnumStringMap.h"

FEnumStringPair::FEnumStringPair(const FText& InEnumValue, const uint8 InIndex) : DisplayText(InEnumValue),
	Index(InIndex)
{
}

FEnumStringMapping::FEnumStringMapping(const UEnum* InEnum)
{
	Enum = InEnum;
	EnumClass = Enum->CppType;
	for (int32 Index = 0; Index < Enum->NumEnums(); Index++)
	{
		const int64 ValueByIndex = Enum->GetValueByIndex(Index);
		// Skip hidden/max values if needed
		if (Enum->HasMetaData(TEXT("Hidden"), Index) || Enum->GetMaxEnumValue() == ValueByIndex)
		{
			continue;
		}
		EnumStringPairs.Emplace(ValueByIndex, FEnumStringPair(GetEnumText(Enum, ValueByIndex), ValueByIndex));
	}
}

void UEnumStringMap::PostLoad()
{
	Super::PostLoad();

	for (auto& [Key, Value] : EnumStringMap)
	{
		// Add any Enum Values not present in EnumTagPairs
		for (int32 Index = 0; Index < Key->NumEnums(); Index++)
		{
			const int64 ValueByIndex = Key->GetValueByIndex(Index);
			// Skip hidden/max values if needed
			if (Key->HasMetaData(TEXT("Hidden"), Index) || Key->GetMaxEnumValue() == ValueByIndex)
			{
				continue;
			}
			if (!Value.EnumStringPairs.Find(ValueByIndex))
			{
				Value.EnumStringPairs.Emplace(ValueByIndex,
				                              FEnumStringPair(GetEnumText(Key, ValueByIndex), ValueByIndex));
			}
			else if (!Value.EnumStringPairs[ValueByIndex].DisplayText.IsFromStringTable())
			{
				Value.EnumStringPairs[ValueByIndex].DisplayText = GetEnumText(Key, ValueByIndex);
			}
		}
	}
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

FText GetEnumText(const UEnum* InEnum, const int64 InValue)
{
	FString EnumClassName = InEnum->CppType;
	if (EnumClassName.StartsWith("E") || EnumClassName.StartsWith("U"))
	{
		EnumClassName.RemoveAt(0);
	}
	const FString EnumValueString = InEnum->GetNameStringByValue(InValue);
	const FString StringTableKey = EnumValueString == "None"
	                               ? EnumValueString
	                               : FString::Format(TEXT("{0}_{1}"), {EnumClassName, EnumValueString});
	const FText StringTableText = FText::FromStringTable("/Game/StringTables/ST_Enums.ST_Enums", StringTableKey);
	if (!StringTableText.IsFromStringTable())
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find string table entry for %s"), *StringTableKey);
	}
	return StringTableText;
}
