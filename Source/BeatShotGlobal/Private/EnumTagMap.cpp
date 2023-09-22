// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "EnumTagMap.h"

UEnumTagMap::UEnumTagMap()
{
	EnumTagMappings.Emplace(StaticEnum<EBoundsScalingPolicy>());
	EnumTagMappings.Emplace(StaticEnum<EMovingTargetDirectionMode>());
	EnumTagMappings.Emplace(StaticEnum<EConsecutiveTargetScalePolicy>());
	EnumTagMappings.Emplace(StaticEnum<ETargetDamageType>());
	EnumTagMappings.Emplace(StaticEnum<ETargetActivationSelectionPolicy>());
	EnumTagMappings.Emplace(StaticEnum<ERecentTargetMemoryPolicy>());
	EnumTagMappings.Emplace(StaticEnum<ETargetDeactivationCondition>());
	EnumTagMappings.Emplace(StaticEnum<ETargetDestructionCondition>());
	EnumTagMappings.Emplace(StaticEnum<ETargetActivationResponse>());
	EnumTagMappings.Emplace(StaticEnum<ETargetDeactivationResponse>());
	EnumTagMappings.Emplace(StaticEnum<EDynamicBoundsScalingPolicy>());
}

template <typename T>
FGameplayTagContainer UEnumTagMap::GetTagsForEnum(const T& InEnum)
{
	const FEnumTagMapping* EnumTagMapping = GetEnumTagMapping<T>();
	if (!EnumTagMapping)
	{
		return FGameplayTagContainer();
	}
	const int32 Index = EnumTagMapping->EnumTagPairs.Find(FEnumTagPair(EnumTagMapping->Enum->GetNameStringByValue(TEnumAsByte<T>(InEnum))));

	if (!EnumTagMapping->EnumTagPairs.IsValidIndex(Index))
	{
		return FGameplayTagContainer();
	}
	
	return EnumTagMapping->EnumTagPairs[Index].Tags;
}

template <typename T>
const FEnumTagMapping* UEnumTagMap::GetEnumTagMapping()
{
	UEnum* EnumClass = StaticEnum<T>();
	if (!EnumClass)
	{
		return nullptr;
	}
	const int32 Index = EnumTagMappings.Find(FEnumTagMapping(EnumClass, true));

	if (!EnumTagMappings.IsValidIndex(Index))
	{
		return nullptr;
	}
	return &EnumTagMappings[Index];
}

const TArray<FEnumTagMapping>* UEnumTagMap::GetEnumTagMappings() const
{
	return &EnumTagMappings;
}
