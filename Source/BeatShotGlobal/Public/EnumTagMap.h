// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobalEnums.h"
#include "Engine/DataAsset.h"
#include "EnumTagMap.generated.h"


class UGameModeCategoryTagWidget;

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FEnumTagPair
{
	GENERATED_BODY()

	/** String version of the Enum Value */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (NoResetToDefault))
	FString EnumValue;

	/** Gameplay Tags inherited from the Enum Class */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories="GameModeCategory"))
	FGameplayTagContainer ParentTags;

	/** Gameplay Tags associated with the Enum Value */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories="GameModeCategory"))
	FGameplayTagContainer Tags;

	FEnumTagPair()
	{
		Tags = FGameplayTagContainer();
	}

	FEnumTagPair(const FString& InEnumValue)
	{
		EnumValue = InEnumValue;
		Tags = FGameplayTagContainer();
	}

	void AddParentTags(const FGameplayTagContainer& InParentTags)
	{
		ParentTags = InParentTags;
		
		for (const FGameplayTag& Tag : InParentTags)
		{
			if (Tags.HasTagExact(Tag))
			{
				Tags.RemoveTag(Tag);
			}
		}
	}

	FORCEINLINE bool operator==(const FEnumTagPair& Other) const
	{
		return EnumValue.Equals(Other.EnumValue);
	}
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FEnumTagMapping
{
	GENERATED_BODY()

	/** UEnum static Enum */
	UPROPERTY(BlueprintReadOnly)
	const UEnum* Enum;

	/** String version of the Enum Class */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (NoResetToDefault))
	FString EnumClass;

	/** Any EnumTagPairs inherit these tags. Must save to show changes in editor */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (Categories="GameModeCategory"))
	FGameplayTagContainer ParentTags;

	/** Gameplay Tags associated with an Enum Value */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (TitleProperty="{EnumValue}"))
	TArray<FEnumTagPair> EnumTagPairs;

	FEnumTagMapping()
	{
		Enum = nullptr;
		EnumClass = FString();
	}

	FEnumTagMapping(const UEnum* InEnum, const bool bComparison = false)
	{
		Enum = InEnum;

		if (!bComparison)
		{
			EnumClass = Enum->CppType;
			CreateEnumTagPairs();
		}
	}

	void CreateEnumTagPairs()
	{
		for (int64 i = 0; i < Enum->GetMaxEnumValue(); i++)
		{
			const FText EnumValueText = Enum->GetDisplayNameTextByValue(i);
			EnumTagPairs.Emplace(EnumValueText.ToString());
		}
	}

	FORCEINLINE bool operator==(const FEnumTagMapping& Other) const
	{
		return Enum == Other.Enum;
	}
	FORCEINLINE bool operator<(const FEnumTagMapping& Other) const
	{
		return EnumClass < Other.EnumClass;
	}
};

/** Since the combo boxes in the custom game mode menu are usually populated with enums,
 *  this data asset allows editing GameplayTags associated with each enum in blueprint.
 *  Enums are populated in the constructor with one line of code. */
UCLASS(Blueprintable, BlueprintType, Const)
class BEATSHOTGLOBAL_API UEnumTagMap : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Adds enums to the EnumTagMappings array */
	UEnumTagMap();

	virtual void PreSave(FObjectPreSaveContext ObjectSaveContext) override;
	virtual void PostLoad() override;

	/** Returns the GameplayTags associated with a specific full enum name */
	template<typename T>
	FGameplayTagContainer GetTagsForEnum(const T& InEnum);

	/** Returns the EnumTagMapping associated with a specific enum class,
	 *  which contains an array of FEnumTagPairs for each enum value in the class */
	template<typename T>
	const FEnumTagMapping* GetEnumTagMapping();

	/** Returns a pointer to the entire EnumTagMappings array */
	const TArray<FEnumTagMapping>* GetEnumTagMappings() const;

protected:
	UPROPERTY(EditDefaultsOnly, meta = (TitleProperty="{EnumClass}"))
	TArray<FEnumTagMapping> EnumTagMappings;

	TArray<UEnum*> EnumsToInclude = {
		StaticEnum<EBoundsScalingPolicy>(),
		StaticEnum<EMovingTargetDirectionMode>(),
		StaticEnum<EConsecutiveTargetScalePolicy>(),
		StaticEnum<ETargetDamageType>(),
		StaticEnum<ETargetActivationSelectionPolicy>(),
		StaticEnum<ERecentTargetMemoryPolicy>(),
		StaticEnum<ETargetDeactivationCondition>(),
		StaticEnum<ETargetDestructionCondition>(),
		StaticEnum<ETargetActivationResponse>(),
		StaticEnum<ETargetDeactivationResponse>(),
		StaticEnum<EDynamicBoundsScalingPolicy>(),
		StaticEnum<ETargetSpawningPolicy>(),
		StaticEnum<ETargetDistributionPolicy>()
	};
};

template <typename T>
FGameplayTagContainer UEnumTagMap::GetTagsForEnum(const T& InEnum)
{
	const FEnumTagMapping* EnumTagMapping = GetEnumTagMapping<T>();
	if (!EnumTagMapping)
	{
		return FGameplayTagContainer();
	}
	
	const FText EnumValueText = EnumTagMapping->Enum->GetDisplayNameTextByValue(static_cast<int64>(InEnum));
	const int32 Index = EnumTagMapping->EnumTagPairs.Find(FEnumTagPair(EnumValueText.ToString()));

	if (!EnumTagMapping->EnumTagPairs.IsValidIndex(Index))
	{
		return FGameplayTagContainer();
	}
	
	return EnumTagMapping->EnumTagPairs[Index].Tags;
}

template <typename T>
const FEnumTagMapping* UEnumTagMap::GetEnumTagMapping()
{
	const UEnum* EnumClass = StaticEnum<T>();
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