// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BeatShot.generated.h"

#define ACTOR_ROLE_FSTRING *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(GetLocalRole()))
#define GET_ACTOR_ROLE_FSTRING(Actor) *(FindObject<UEnum>(nullptr, TEXT("/Script/Engine.ENetRole"), true)->GetNameStringByValue(Actor->GetLocalRole()))

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQTableUpdate, const TArray<float>& UpdatedQTable);
DECLARE_DELEGATE(FOnShotFired);

/** Enum representing the different times of the day */
UENUM(BlueprintType)
enum class ETimeOfDay : uint8
{
	Day UMETA(DisplayName="Day"),
	Night UMETA(DisplayName="Night"),
	DayToNight UMETA(DisplayName="DayToNight"),
	NightToDay UMETA(DisplayName="NightToDay"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(ETimeOfDay, ETimeOfDay::Day, ETimeOfDay::NightToDay);

/** Enum representing the ways in which the MovablePlatform can move */
UENUM(BlueprintType)
enum class EPlatformTransitionType : uint8
{
	None UMETA(DisplayName="MoveUpByInteract"),
	MoveUpByInteract UMETA(DisplayName="MoveUpByInteract"),
	MoveDownByInteract UMETA(DisplayName="MoveDownByInteract"),
	MoveDownByStepOff UMETA(DisplayName="MoveDownByStepOff")
};
ENUM_RANGE_BY_FIRST_AND_LAST(EPlatformTransitionType, EPlatformTransitionType::None, EPlatformTransitionType::MoveDownByStepOff);

/** Enum representing the bordering directions for a target */
UENUM(BlueprintType)
enum class EBorderingDirection : uint8
{
	Left UMETA(DisplayName="Left"),
	Right UMETA(DisplayName="Right"),
	Up UMETA(DisplayName="Up"),
	Down UMETA(DisplayName="Down"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBorderingDirection, EBorderingDirection::Left, EBorderingDirection::Down);

/** Enum representing the types of BeatGrid Indices */
UENUM(BlueprintType)
enum class EBeatGridIndexType : uint8
{
	None UMETA(DisplayName="None"),
	Corner_TopLeft UMETA(DisplayName="Corner_TopLeft"),
	Corner_TopRight UMETA(DisplayName="Corner_TopRight"),
	Corner_BottomRight UMETA(DisplayName="Corner_BottomRight"),
	Corner_BottomLeft UMETA(DisplayName="Corner_BottomLeft"),
	Border_Top UMETA(DisplayName="Border_Top"),
	Border_Right UMETA(DisplayName="Border_Top"),
	Border_Bottom UMETA(DisplayName="Border_Top"),
	Border_Left UMETA(DisplayName="Border_Top"),
	Middle UMETA(DisplayName="Middle"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EBeatGridIndexType, EBeatGridIndexType::Corner_TopLeft, EBeatGridIndexType::Middle);

/** A struct representing two consecutively spawned targets */
USTRUCT()
struct FTargetPair
{
	GENERATED_BODY()

	/** The location of the target spawned before Current */
	FVector Previous;

	/** The location spawned after Previous */
	FVector Current;

	float Reward;

	FTargetPair()
	{
		Previous = FVector::ZeroVector;
		Current = FVector::ZeroVector;
		Reward = 0.f;
	}

	FTargetPair(const FVector CurrentPoint)
	{
		Previous = FVector::ZeroVector;
		Current = CurrentPoint;
		Reward = 0.f;
	}

	FTargetPair(const FVector PreviousPoint, const FVector CurrentPoint)
	{
		Previous = PreviousPoint;
		Current = CurrentPoint;
		Reward = 0.f;
	}

	FORCEINLINE bool operator ==(const FTargetPair& Other) const
	{
		if (Current.Y == Other.Current.Y && Current.Z == Other.Current.Z)
		{
			return true;
		}
		return false;
	}
};

/** A struct representing the space in the grid that a recently spawned target occupies */
USTRUCT()
struct FRecentTarget
{
	GENERATED_BODY()

	/** An array of points that were inside the SpawnBox and inside the target */
	TArray<FVector> OverlappingPoints;

	/** The location of the center of the target */
	FVector CenterVector;

	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid TargetGuid;

	/** The scale of the target relative to the world */
	float TargetScale;

	FRecentTarget()
	{
		CenterVector = FVector::ZeroVector;
		TargetScale = 0.f;
	}

	explicit FRecentTarget(const FGuid Guid)
	{
		CenterVector = FVector::ZeroVector;
		TargetGuid = Guid;
		TargetScale = 0.f;
	}

	FRecentTarget(const FGuid NewTargetGuid, const TArray<FVector> Points, const float NewTargetScale, const FVector NewCenter)
	{
		TargetGuid = NewTargetGuid;
		OverlappingPoints = Points;
		TargetScale = NewTargetScale;
		CenterVector = NewCenter;
	}

	FORCEINLINE bool operator ==(const FRecentTarget& Other) const
	{
		if (TargetGuid == Other.TargetGuid)
		{
			return true;
		}
		return false;
	}
};

/** A struct representing a point in a 2D grid with information about that point */
USTRUCT()
struct FVectorCounter
{
	GENERATED_BODY()

	/** Unscaled, world spawn location point. Bottom left of the square sub-area */
	FVector Point;

	/** The center of the square sub-area */
	FVector Center;

	/** The chosen point for this vector counter, it might be different than Point, but will be within the sub-area bounded by incrementY and incrementZ */
	FVector ActualChosenPoint;

	/** The total number of target spawns at this point */
	int32 TotalSpawns;

	/** The total number of target hits by player at this point */
	int32 TotalHits;

	/** The index inside SpawnCounter for this VectorCounter */
	int32 Index;

	/** The horizontal spacing between the next VectorCounter point */
	float IncrementY;

	/** The vertical spacing between the next VectorCounter point */
	float IncrementZ;

	FVectorCounter()
	{
		Point = FVector();
		ActualChosenPoint = FVector();
		TotalSpawns = -1;
		TotalHits = 0;
		IncrementY = 0.f;
		IncrementZ = 0.f;
		Index = -1;
	}

	FVectorCounter(const FVector NewPoint)
	{
		Point = NewPoint;
		ActualChosenPoint = FVector();
		TotalSpawns = -1;
		TotalHits = 0;
		IncrementY = 0.f;
		IncrementZ = 0.f;
		Index = -1;
	}

	FVectorCounter(const int32 NewIndex, const FVector NewPoint, const float IncY, const float IncZ)
	{
		Index = NewIndex;
		Point = NewPoint;
		ActualChosenPoint = FVector();
		TotalSpawns = -1;
		TotalHits = 0;
		IncrementY = IncY;
		IncrementZ = IncZ;
		Center = FVector(Point.X, roundf(Point.Y + IncrementY / 2.f), roundf(Point.Z + IncrementZ / 2.f));
	}

	FORCEINLINE bool operator ==(const FVectorCounter& Other) const
	{
		if (Other.Point.Y >= Point.Y && Other.Point.Y < Point.Y + IncrementY
			&& (Other.Point.Z >= Point.Z && Other.Point.Z < Point.Z + IncrementZ))
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator <(const FVectorCounter& Other) const
	{
		if (Point.Z < Other.Point.Z)
		{
			return true;
		}
		if (Point.Z == Other.Point.Z && Point.Y < Other.Point.Y)
		{
			return true;
		}
		return false;
	}

	FVector GetRandomSubPoint(const TArray<EBorderingDirection> BlockedDirections) const
	{
		float MinY = Point.Y;
		float MaxY = Point.Y + IncrementY;
		float MinZ = Point.Z;
		float MaxZ = Point.Z + IncrementZ;
		if (BlockedDirections.Contains(EBorderingDirection::Left))
		{
			MinY = Center.Y;
		}
		if (BlockedDirections.Contains(EBorderingDirection::Right))
		{
			MaxY = Center.Y;
		}
		if (BlockedDirections.Contains(EBorderingDirection::Down))
		{
			MinZ = Center.Z;
		}
		if (BlockedDirections.Contains(EBorderingDirection::Up))
		{
			MaxZ = Center.Z;
		}
		
		const float Y = roundf(FMath::FRandRange(MinY, MaxY - 1.f));
		const float Z = roundf(FMath::FRandRange(MinZ, MaxZ - 1.f));
		return FVector(Point.X, Y, Z);
	}
};

/** A struct representing a BeatGrid target index. Stores info about bordering indices */
USTRUCT()
struct FBeatGridIndex
{
	GENERATED_BODY()
	
	EBeatGridIndexType IndexType;
	TArray<int32> BorderingIndices;
	int32 Index;
	int32 Width;
	int32 Size;

	FBeatGridIndex()
	{
		Index = -1;
		Width = -1;
		Size = -1;
		IndexType = EBeatGridIndexType::None;
		BorderingIndices = TArray<int32>();
	}

	explicit FBeatGridIndex(const int32 CheckIndex)
	{
		Index = CheckIndex;
		Width = -1;
		Size = -1;
		IndexType = EBeatGridIndexType::None;
		BorderingIndices = TArray<int32>();
	}

	FBeatGridIndex(const int32 NewIndex, const int32 NewWidth, const int32 NewSize)
	{
		Index = NewIndex;
		Width = NewWidth;
		Size = NewSize;
		IndexType = GetIndexType();
		BorderingIndices = InitBorderingIndices();
	}

	FORCEINLINE bool operator ==(const FBeatGridIndex& Other) const
	{
		if (Index == Other.Index)
		{
			return true;
		}
		return false;
	}

	/** Returns whether or not the index is a corner */
	bool IsCornerIndex() const
	{
		if (IndexType == EBeatGridIndexType::Corner_TopLeft ||
			IndexType == EBeatGridIndexType::Corner_TopRight ||
			IndexType == EBeatGridIndexType::Corner_BottomRight ||
			IndexType == EBeatGridIndexType::Corner_BottomLeft)
		{
			return true;
		}
		return false;
	}

	/** Returns whether or not the index is a border */
	bool IsBorderIndex() const
	{
		if (IndexType == EBeatGridIndexType::Border_Top ||
			IndexType == EBeatGridIndexType::Border_Right ||
			IndexType == EBeatGridIndexType::Border_Bottom ||
			IndexType == EBeatGridIndexType::Border_Left)
		{
			return true;
		}
		return false;
	}
	
	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	TArray<int32> InitBorderingIndices() const
	{
		TArray<int32> ReturnArray = TArray<int32>();

		const int32 TopLeft = Index - Width - 1;
		const int32 Top = Index - Width;
		const int32 TopRight = Index - Width + 1;
		const int32 Right = Index + 1;
		const int32 BottomRight = Index + Width + 1;
		const int32 Bottom = Index + Width;
		const int32 BottomLeft = Index + Width - 1;
		const int32 Left = Index - 1;
		
		switch (IndexType)
		{
		case EBeatGridIndexType::None:
			break;
		case EBeatGridIndexType::Corner_TopLeft:
			ReturnArray.Add(Right);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomRight);
			break;
		case EBeatGridIndexType::Corner_TopRight:
			ReturnArray.Add(Left);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomLeft);
			break;
		case EBeatGridIndexType::Corner_BottomRight:
			ReturnArray.Add(Left);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopLeft);
			break;
		case EBeatGridIndexType::Corner_BottomLeft:
			ReturnArray.Add(Right);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			break;
		case EBeatGridIndexType::Border_Top:
			ReturnArray.Add(Left);
			ReturnArray.Add(Right);
			ReturnArray.Add(BottomRight);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomLeft);
			break;
		case EBeatGridIndexType::Border_Right:
			ReturnArray.Add(TopLeft);
			ReturnArray.Add(Top);
			ReturnArray.Add(Left);
			ReturnArray.Add(BottomLeft);
			ReturnArray.Add(Bottom);
			break;
		case EBeatGridIndexType::Border_Bottom:
			ReturnArray.Add(TopLeft);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			ReturnArray.Add(Right);
			ReturnArray.Add(Left);
			break;
		case EBeatGridIndexType::Border_Left:
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			ReturnArray.Add(Right);
			ReturnArray.Add(BottomRight);
			ReturnArray.Add(Bottom);
			break;
		case EBeatGridIndexType::Middle:
			ReturnArray.Add(TopLeft);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			ReturnArray.Add(Right);
			ReturnArray.Add(BottomRight);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomLeft);
			ReturnArray.Add(Left);
			break;
		}
		return ReturnArray;
	}

	/** Returns the BorderingIndices array */
	TArray<int32> GetBorderingIndices() const
	{
		return BorderingIndices;
	}

	/** Returns the corresponding index type depending on the index, size, and width */
	EBeatGridIndexType GetIndexType() const
	{
		const int32 MaxIndex = Size - 1;
		const int32 FirstRowLastIndex = Width - 1;
		const int32 LastRowFirstIndex = Size - Width;
		if (Index == 0) {
			return EBeatGridIndexType::Corner_TopLeft;
		}
		if (Index == FirstRowLastIndex)
		{
			return EBeatGridIndexType::Corner_TopRight;
		}
		if (Index == MaxIndex)
		{
			return EBeatGridIndexType::Corner_BottomRight;
		}
		if (Index == LastRowFirstIndex)
		{
			return EBeatGridIndexType::Corner_BottomLeft;
		}
		// top
		if (Index > 0 && Index < FirstRowLastIndex)
		{
			return EBeatGridIndexType::Border_Top;
		}
		// right
		if ((Index + 1) % Width == 0 && Index < MaxIndex)
		{
			return EBeatGridIndexType::Border_Right;
		}
		// bottom
		if (Index > LastRowFirstIndex && Index < MaxIndex)
		{
			return EBeatGridIndexType::Border_Bottom;
		}
		// left	
		if (Index % Width == 0 && Index < LastRowFirstIndex)
		{
			return EBeatGridIndexType::Border_Left;
		}
		return EBeatGridIndexType::Middle;
	}
};

class FBeatShot : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};