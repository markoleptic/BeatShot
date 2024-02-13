// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TargetCommon.generated.h"

enum class ETargetDamageType : uint8;
class ATarget;
struct FTargetDamageEvent;

DECLARE_DELEGATE_RetVal_TwoParams(int32, FRequestRLCSpawnArea, const int32, const TArray<int32>&);
DECLARE_DELEGATE_OneParam(FOnBeatTrackDirectionChanged, const FVector& Vector);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated, const ETargetDamageType& DamageType);
DECLARE_MULTICAST_DELEGATE_OneParam(FPostTargetDamageEvent, const FTargetDamageEvent& Event);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated_AimBot, ATarget* Target);

/** Enum representing the bordering directions for a SpawnArea */
UENUM(BlueprintType)
enum class EBorderingDirection : uint8
{
	None UMETA(DisplayName="None"),
	Left UMETA(DisplayName="Left"),
	Right UMETA(DisplayName="Right"),
	Up UMETA(DisplayName="Up"),
	Down UMETA(DisplayName="Down"),
	UpLeft UMETA(DisplayName="UpLeft"),
	UpRight UMETA(DisplayName="UpRight"),
	DownLeft UMETA(DisplayName="DownLeft"),
	DownRight UMETA(DisplayName="DownRight"),
};

ENUM_RANGE_BY_FIRST_AND_LAST(EBorderingDirection, EBorderingDirection::Left, EBorderingDirection::DownRight);

/** Enum representing the types of Grid Indices */
UENUM(BlueprintType)
enum class EGridIndexType : uint8
{
	None UMETA(DisplayName="None"),
	Corner_TopLeft UMETA(DisplayName="Corner_TopLeft"),
	Corner_TopRight UMETA(DisplayName="Corner_TopRight"),
	Corner_BottomRight UMETA(DisplayName="Corner_BottomRight"),
	Corner_BottomLeft UMETA(DisplayName="Corner_BottomLeft"),
	Border_Top UMETA(DisplayName="Border_Top"),
	Border_Right UMETA(DisplayName="Border_Right"),
	Border_Bottom UMETA(DisplayName="Border_Bottom"),
	Border_Left UMETA(DisplayName="Border_Left"),
	Middle UMETA(DisplayName="Middle"),
};

ENUM_RANGE_BY_FIRST_AND_LAST(EGridIndexType, EGridIndexType::Corner_TopLeft, EGridIndexType::Middle);

namespace IndexTypes
{
	/** Cardinal direction Index types that are valid to use when searching for GridBlocks */
	inline const TSet GridBlock = {
		EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up, EBorderingDirection::Down
	};

	/** Up-Down only index types */
	inline const TSet Vertical = {EBorderingDirection::Up, EBorderingDirection::Down};

	/** Left-right only index types */
	inline const TSet Horizontal = {EBorderingDirection::Left, EBorderingDirection::Right};

	/** All index types */
	inline const TSet All = {
		EBorderingDirection::UpLeft, EBorderingDirection::UpRight, EBorderingDirection::DownLeft,
		EBorderingDirection::DownRight, EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up,
		EBorderingDirection::Down
	};
}

/** Contains the minimum and maximum of a Box, i.e. the bottom left corner location and top right corner location. */
struct FExtrema
{
	/** The min extrema */
	FVector Min;

	/** The max extrema */
	FVector Max;

	FExtrema() = default;
	~FExtrema() = default;

	FExtrema(const FVector& InMin, const FVector& InMax) : Min(InMin), Max(InMax)
	{
	}
};


/** Key used for location-based indexing */
USTRUCT()
struct FAreaKey
{
	GENERATED_BODY()

	FVector Vertex_BottomLeft;
	FVector Vertex_TopRight;

	FAreaKey() = default;

	FAreaKey(const FVector& InBotLeft, const FVector& InTopRight)
	{
		Vertex_BottomLeft = InBotLeft;
		Vertex_TopRight = InTopRight;
	}

	FAreaKey(const FVector& InBotLeft, const FIntVector3& InInc)
	{
		Vertex_BottomLeft = InBotLeft;
		Vertex_TopRight = InBotLeft + FVector(0.f, InInc.Y, InInc.Z);
	}

	bool operator==(const FAreaKey& Other) const
	{
		return FMath::IsNearlyEqual(Vertex_BottomLeft.Y, Other.Vertex_BottomLeft.Y, 0.01f) &&
			FMath::IsNearlyEqual(Vertex_BottomLeft.Z, Other.Vertex_BottomLeft.Z, 0.01f) &&
			FMath::IsNearlyEqual(Vertex_TopRight.Y, Other.Vertex_TopRight.Y, 0.01f) && FMath::IsNearlyEqual(
				Vertex_TopRight.Z, Other.Vertex_TopRight.Z, 0.01f);
	}

	friend FORCEINLINE uint32 GetTypeHash(const FAreaKey& AreaKey)
	{
		uint32 Hash = GetTypeHash(AreaKey.Vertex_BottomLeft.Y);
		Hash = HashCombine(Hash, GetTypeHash(AreaKey.Vertex_BottomLeft.Z));
		Hash = HashCombine(Hash, GetTypeHash(AreaKey.Vertex_TopRight.Y));
		Hash = HashCombine(Hash, GetTypeHash(AreaKey.Vertex_TopRight.Z));
		return Hash;
	}
};