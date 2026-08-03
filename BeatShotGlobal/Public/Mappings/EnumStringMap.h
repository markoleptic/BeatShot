// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnumStringMap.generated.h"

/** Display name for an enum. */
USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct BEATSHOTGLOBAL_API FEnumStringPair
{
	GENERATED_BODY()

	FEnumStringPair() = default;

	explicit FEnumStringPair(const FText& InEnumValue, const uint8 InIndex);

	FORCEINLINE bool operator==(const FEnumStringPair& Other) const
	{
		return Index == Other.Index;
	}

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText DisplayText;

	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (NoResetToDefault))
	uint8 Index = MAX_uint8;
};

/** A collection of FEnumStringPair for a particular enum type. */
USTRUCT(BlueprintType, meta=(ShowOnlyInnerProperties))
struct BEATSHOTGLOBAL_API FEnumStringMapping
{
	GENERATED_BODY()

	FEnumStringMapping() = default;

	explicit FEnumStringMapping(const UEnum* InEnum);

	virtual ~FEnumStringMapping() = default;

	FORCEINLINE bool operator==(const FEnumStringMapping& Other) const
	{
		return Enum == Other.Enum;
	}

	FORCEINLINE bool operator<(const FEnumStringMapping& Other) const
	{
		return EnumClass < Other.EnumClass;
	}

	/** UEnum static Enum. */
	UPROPERTY(BlueprintReadOnly)
	const UEnum* Enum = nullptr;

	/** String version of the Enum Class. */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, meta = (NoResetToDefault))
	FString EnumClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta = (TitleProperty="{DisplayText}"))
	TMap<uint8, FEnumStringPair> EnumStringPairs;
};

UCLASS(Blueprintable, BlueprintType, Const)
class BEATSHOTGLOBAL_API UEnumStringMap : public UDataAsset
{
	GENERATED_BODY()

public:
	virtual void PostLoad() override;

	/** Returns a pointer to the entire EnumStringMap. */
	const TMap<UEnum*, FEnumStringMapping>& GetEnumStringMap() const;

	/** Returns the EnumStringMapping associated with a specific enum class. */
	template <typename T>
	const FEnumStringMapping* GetEnumStringMapping() const;

	/** Returns the string associated with a specific full enum name. */
	template <typename T>
	FText FindTextFromEnum(const T& InEnum) const;

	/** Finds the full enum type from the Display Name. */
	template <typename T>
	T FindEnumFromText(const FText& InText) const;
	template <typename T>
	T FindEnumFromString(const FString& InString) const;

protected:
	void PopulateEnumTypes(const TSet<UEnum*>& InTypes);

	UPROPERTY(EditDefaultsOnly)
	TMap<UEnum*, FEnumStringMapping> EnumStringMap;
};

template <typename T>
const FEnumStringMapping* UEnumStringMap::GetEnumStringMapping() const
{
	const UEnum* EnumClass = StaticEnum<T>();
	if (!EnumClass)
	{
		return nullptr;
	}
	return EnumStringMap.Find(EnumClass);
}

template <typename T>
FText UEnumStringMap::FindTextFromEnum(const T& InEnum) const
{
	const FEnumStringMapping* EnumTagMapping = GetEnumStringMapping<T>();
	if (!EnumTagMapping)
	{
		return FText::GetEmpty();
	}
	if (const auto Found = EnumTagMapping->EnumStringPairs.Find(static_cast<uint8>(InEnum)))
	{
		return Found->DisplayText;
	}

	UE_LOG(LogTemp, Warning, TEXT("Didn't find mapping for %d"), static_cast<uint8>(InEnum));
	return FText::GetEmpty();
}

template <typename T>
T UEnumStringMap::FindEnumFromText(const FText& InText) const
{
	return FindEnumFromString<T>(InText.ToString());
}

template <typename T>
T UEnumStringMap::FindEnumFromString(const FString& InString) const
{
	if (const FEnumStringMapping* EnumTagMapping = GetEnumStringMapping<T>())
	{
		for (const auto& [Key, Value] : EnumTagMapping->EnumStringPairs)
		{
			if (Value.DisplayText.ToString().Equals(InString))
			{
				return static_cast<T>(Value.Index);
			}
		}
	}
	ensureMsgf(false, TEXT("Didn't find enum corresponding to string: %s"), *InString);
	return static_cast<T>(0);
}

FText GetEnumText(const UEnum* InEnum, int64 InValue);
