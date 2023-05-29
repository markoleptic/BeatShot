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
	MoveDownByStepOff UMETA(DisplayName="MoveDownByStepOff")};

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

/** A struct representing two consecutively spawned targets, used to keep track of the reward associated between two points */
USTRUCT()
struct FTargetPair
{
	GENERATED_BODY()

	/** The location of the target spawned before Current */
	FVector Previous;

	/** The location spawned after Previous */
	FVector Current;

	/** The reward for spawning a target at Previous and then spawning a target at Current */
	float Reward;

	FTargetPair()
	{
		Previous = FVector::ZeroVector;
		Current = FVector::ZeroVector;
		Reward = 0.f;
	}

	FTargetPair(const FVector& CurrentPoint)
	{
		Previous = FVector::ZeroVector;
		Current = CurrentPoint;
		Reward = 0.f;
	}

	FTargetPair(const FVector& PreviousPoint, const FVector& CurrentPoint)
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

/** A struct representing a spawn point in a 2D grid with information about that point */
USTRUCT()
struct FSpawnPoint
{
	GENERATED_BODY()

	/** Unscaled, world spawn location point. Bottom left of the square sub-area */
	FVector Point;

	/** The center of the square sub-area */
	FVector Center;

	/** The center of the square sub-area */
	FVector Scale;

	/** The chosen point for this vector counter, it might be different than Point, but will be within the sub-area bounded by incrementY and incrementZ */
	FVector ActualChosenPoint;

	/** The total number of target spawns at this point */
	int32 TotalSpawns;

	/** The total number of target hits by player at this point */
	int32 TotalHits;

	/** The index for this SpawnPoint inside an array of SpawnPoints  */
	int32 Index;

	/** The horizontal spacing between the next SpawnPoint */
	float IncrementY;

	/** The vertical spacing between the next SpawnPoint */
	float IncrementZ;

	/** The type of point (corner, border, etc.) */
	EGridIndexType IndexType;

	/** The bordering SpawnPoints adjacent to this SpawnPoint */
	TArray<int32> BorderingIndices;

	/** The points that this target overlapped with */
	TArray<FVector> OverlappingPoints;

	/** The total amount of horizontal SpawnPoints in an array of SpawnPoints. Used for grid */
	int32 Width;

	/** The total number of SpawnPoints in the array this SpawnPoint belongs to */
	int32 Size;

	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid TargetGuid;

	bool bIsActivated;

	bool bIsRecent;
	
	FSpawnPoint()
	{
		Point = FVector(-1);
		ActualChosenPoint = FVector(-1);
		Scale = FVector(1);
		TotalSpawns = INDEX_NONE;
		TotalHits = 0;
		IncrementY = 0.f;
		IncrementZ = 0.f;
		Index = INDEX_NONE;
		IndexType = EGridIndexType::None;
		BorderingIndices = TArray<int32>();
		OverlappingPoints = TArray<FVector>();
		Width = INDEX_NONE;
		Size = INDEX_NONE;
		TargetGuid = FGuid();
		bIsActivated = false;
		bIsRecent = false;
	}

	FSpawnPoint(const int32 NewIndex, const FVector& NewPoint, const float IncY, const float IncZ, const int32 NewWidth = INDEX_NONE, const int32 NewSize = INDEX_NONE)
	{
		Index = NewIndex;
		Point = NewPoint;
		IncrementY = IncY;
		IncrementZ = IncZ;
		Width = NewWidth;
		Size = NewSize;
		IndexType = GetIndexType();
		BorderingIndices = InitBorderingIndices();
		ActualChosenPoint = FVector(-1);
		Scale = FVector(1);
		TotalSpawns = INDEX_NONE;
		TotalHits = 0;
		Center = FVector(Point.X, roundf(Point.Y + IncrementY / 2.f), roundf(Point.Z + IncrementZ / 2.f));
		OverlappingPoints = TArray<FVector>();
		TargetGuid = FGuid();
		bIsActivated = false;
		bIsRecent = false;
	}

	FSpawnPoint(const int32 NewIndex)
	{
		Point = FVector(-1);
		ActualChosenPoint = FVector(-1);
		Scale = FVector(1);
		TotalSpawns = INDEX_NONE;
		TotalHits = 0;
		IncrementY = 0.f;
		IncrementZ = 0.f;
		Index = NewIndex;
		IndexType = EGridIndexType::None;
		BorderingIndices = TArray<int32>();
		OverlappingPoints = TArray<FVector>();
		Width = INDEX_NONE;
		Size = INDEX_NONE;
		TargetGuid = FGuid();
		bIsActivated = false;
		bIsRecent = false;
	}

	FSpawnPoint(const FVector& NewPoint)
	{
		Point = NewPoint;
		ActualChosenPoint = FVector(-1);
		Scale = FVector(1);
		TotalSpawns = INDEX_NONE;
		TotalHits = 0;
		IncrementY = 0.f;
		IncrementZ = 0.f;
		Index = INDEX_NONE;
		IndexType = EGridIndexType::None;
		BorderingIndices = TArray<int32>();
		OverlappingPoints = TArray<FVector>();
		Width = INDEX_NONE;
		Size = INDEX_NONE;
		TargetGuid = FGuid();
		bIsActivated = false;
		bIsRecent = false;
	}
	
	FORCEINLINE bool operator ==(const FSpawnPoint& Other) const
	{
		if (Index != INDEX_NONE && Index == Other.Index)
		{
			return true;
		}
		if (Other.Point.Y >= Point.Y &&
			Other.Point.Y < Point.Y + IncrementY &&
			(Other.Point.Z >= Point.Z && Other.Point.Z < Point.Z + IncrementZ))
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator <(const FSpawnPoint& Other) const
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

	FVector GetRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections) const
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

	/** Returns whether or not the index is a corner */
	bool IsCornerIndex() const
	{
		if (IndexType == EGridIndexType::Corner_TopLeft || IndexType == EGridIndexType::Corner_TopRight || IndexType == EGridIndexType::Corner_BottomRight || IndexType ==
			EGridIndexType::Corner_BottomLeft)
		{
			return true;
		}
		return false;
	}

	/** Returns whether or not the index is a border */
	bool IsBorderIndex() const
	{
		if (IndexType == EGridIndexType::Border_Top || IndexType == EGridIndexType::Border_Right || IndexType == EGridIndexType::Border_Bottom || IndexType == EGridIndexType::Border_Left)
		{
			return true;
		}
		return false;
	}

	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	TArray<int32> InitBorderingIndices() const
	{
		TArray<int32> ReturnArray = TArray<int32>();

		const int32 TopLeft = Index + Width - 1;
		const int32 Top = Index + Width;
		const int32 TopRight = Index + Width + 1;
		const int32 Right = Index + 1;
		const int32 BottomRight = Index - Width + 1;
		const int32 Bottom = Index - Width;
		const int32 BottomLeft = Index - Width - 1;
		const int32 Left = Index - 1;

		switch (IndexType)
		{
		case EGridIndexType::None:
			break;
		case EGridIndexType::Corner_TopLeft:
			ReturnArray.Add(Right);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomRight);
			break;
		case EGridIndexType::Corner_TopRight:
			ReturnArray.Add(Left);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomLeft);
			break;
		case EGridIndexType::Corner_BottomRight:
			ReturnArray.Add(Left);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopLeft);
			break;
		case EGridIndexType::Corner_BottomLeft:
			ReturnArray.Add(Right);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			break;
		case EGridIndexType::Border_Top:
			ReturnArray.Add(Left);
			ReturnArray.Add(Right);
			ReturnArray.Add(BottomRight);
			ReturnArray.Add(Bottom);
			ReturnArray.Add(BottomLeft);
			break;
		case EGridIndexType::Border_Right:
			ReturnArray.Add(TopLeft);
			ReturnArray.Add(Top);
			ReturnArray.Add(Left);
			ReturnArray.Add(BottomLeft);
			ReturnArray.Add(Bottom);
			break;
		case EGridIndexType::Border_Bottom:
			ReturnArray.Add(TopLeft);
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			ReturnArray.Add(Right);
			ReturnArray.Add(Left);
			break;
		case EGridIndexType::Border_Left:
			ReturnArray.Add(Top);
			ReturnArray.Add(TopRight);
			ReturnArray.Add(Right);
			ReturnArray.Add(BottomRight);
			ReturnArray.Add(Bottom);
			break;
		case EGridIndexType::Middle:
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
	EGridIndexType GetIndexType() const
	{
		const int32 MaxIndex = Size - 1;
		const int32 BottomRowFirstIndex = Width - 1;
		const int32 TopRowFirstIndex = Size - Width;
		if (Index == 0)
		{
			return EGridIndexType::Corner_BottomLeft;
		}
		if (Index == BottomRowFirstIndex)
		{
			return EGridIndexType::Corner_BottomRight;
		}
		if (Index == MaxIndex)
		{
			return EGridIndexType::Corner_TopRight;
		}
		if (Index == TopRowFirstIndex)
		{
			return EGridIndexType::Corner_TopLeft;
		}
		
		// top
		if (Index > 0 && Index < BottomRowFirstIndex)
		{
			return EGridIndexType::Border_Bottom;
		}
		// right
		if ((Index + 1) % Width == 0 && Index < MaxIndex)
		{
			return EGridIndexType::Border_Right;
		}
		// bottom
		if (Index > TopRowFirstIndex && Index < MaxIndex)
		{
			return EGridIndexType::Border_Top;
		}
		// left	
		if (Index % Width == 0 && Index < TopRowFirstIndex)
		{
			return EGridIndexType::Border_Left;
		}
		return EGridIndexType::Middle;
	}

	void SetIsActivated(const bool bSetIsActivated)
	{
		bIsActivated = bSetIsActivated;
	}

	bool IsActivated() const { return bIsActivated; }

	void SetIsRecent(const bool bSetIsRecent)
	{
		bIsRecent = bSetIsRecent;
	}

	bool IsRecent() const { return bIsRecent; }

	void SetScale(const FVector& InScale)
	{
		Scale = InScale;
	}
	
	void SetGuid(const FGuid InGuid)
	{
		TargetGuid = InGuid;
	}
};

/** Finds a SpawnPoint with the matching InIndex */
FSpawnPoint* FindSpawnPointFromIndex(TArray<FSpawnPoint>& InSpawnPointArray, const int32 InIndex);

/** Finds a SpawnPoint with the matching InLocation */
FSpawnPoint* FindSpawnPointFromLocation(TArray<FSpawnPoint>& InSpawnPointArray, const FVector& InLocation);

/** Finds a SpawnPoint with the matching InGuid */
FSpawnPoint* FindSpawnPointFromGuid(TArray<FSpawnPoint>& InSpawnPointArray, const FGuid& InGuid);

TArray<FSpawnPoint> GetRecentSpawnPoints(const TArray<FSpawnPoint>& InSpawnPointArray);

class FBeatShot : public FDefaultGameModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
