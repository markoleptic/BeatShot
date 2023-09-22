// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GlobalEnums.h"
#include "Engine/DataAsset.h"
#include "EnumTagMap.generated.h"


USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FEnumTagPair
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString EnumClass;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	FString EnumValue;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagContainer Tags;

	FEnumTagPair()
	{
		Tags = FGameplayTagContainer();
	}

	FEnumTagPair(const FString& InEnumClass, const FString& InEnumValue)
	{
		EnumClass = InEnumClass;
		EnumValue = InEnumValue;
	}

	FORCEINLINE bool operator==(const FEnumTagPair& Other) const
	{
		return EnumValue.Equals(Other.EnumValue) && EnumClass.Equals(Other.EnumClass);
	}
};

USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct FEnumTagMapping
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	UEnum* Enum;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
	FString EnumClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta =(TitleProperty="{EnumValue}", H))
	TArray<FEnumTagPair> EnumTagPairs;

	FEnumTagMapping()
	{
		Enum = nullptr;
		EnumClass = FString();
	}

	FEnumTagMapping(UEnum* InEnum)
	{
		Enum = InEnum;
		EnumClass = Enum->CppType;
		CreateEnumTagPairs();
	}

	FEnumTagMapping(UEnum* InEnum, const bool bComparison)
	{
		Enum = InEnum;

		if (bComparison)
		{
			return;
		}
		EnumClass = Enum->CppType;
		CreateEnumTagPairs();
	}

	void CreateEnumTagPairs()
	{
		for (int32 i = 0; i < Enum->GetMaxEnumValue(); i++)
		{
			EnumTagPairs.Emplace(EnumClass, Enum->GetNameStringByValue(i));
		}
	}

	FORCEINLINE bool operator==(const FEnumTagMapping& Other) const
	{
		return Enum == Other.Enum;
	}
};

/**  */
UCLASS(Blueprintable, BlueprintType)
class BEATSHOTGLOBAL_API UEnumTagMap : public UDataAsset
{
	GENERATED_BODY()

public:
	UEnumTagMap();
	
	template<typename T>
	FGameplayTagContainer GetTagsForEnum(const T& InEnum);

	template<typename T>
	const FEnumTagMapping* GetEnumTagMapping();

	const TArray<FEnumTagMapping>* GetEnumTagMappings() const;

protected:
	UPROPERTY(EditDefaultsOnly, meta = (TitleProperty="{EnumClass}"))
	TArray<FEnumTagMapping> EnumTagMappings;
};