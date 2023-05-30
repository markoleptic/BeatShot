// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "UObject/Object.h"
#include "SpawnPointManager.generated.h"

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

	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid TargetGuid;

private:
	/** The bordering SpawnPoints adjacent to this SpawnPoint */
	TArray<int32> BorderingIndices;

	/** The points that this target overlapped with */
	TArray<FVector> OverlappingPoints;

	/** Whether or not this point has a target active */
	bool bIsActivated;

	/** Whether or not this point recently had a target occupy its space */
	bool bIsRecent;

	/** The time that this point was flagged as recent */
	double TimeSetRecent;

public:
	FSpawnPoint();

	FSpawnPoint(const int32 NewIndex, const FVector& NewPoint, const float IncY, const float IncZ, const int32 Width = INDEX_NONE, const int32 Size = INDEX_NONE);

	explicit FSpawnPoint(const int32 NewIndex);

	explicit FSpawnPoint(const FVector& NewPoint);

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

	/** Returns whether or not the index is a corner */
	bool IsCornerIndex() const;

	/** Returns whether or not the index is a border */
	bool IsBorderIndex() const;

	/** Returns if this spawn point is activated or not */
	bool IsActivated() const { return bIsActivated; }

	/** Returns whether or not this SpawnPoint is flagged as recent */
	bool IsRecent() const { return bIsRecent; }

	/** Returns the time that the spawn point was flagged as recent */
	double GetTimeSetRecent() const { return TimeSetRecent; }

	/** Returns a random point within the area that this spawn points represents */
	FVector GetRandomSubPoint(const TArray<EBorderingDirection>& BlockedDirections) const;

	/** Returns a copy of the BorderingIndices array */
	TArray<int32> GetBorderingIndices() const { return BorderingIndices; }

	/** Returns a copy of the OverlappingPoints array */
	TArray<FVector> GetOverlappingPoints() const { return OverlappingPoints; }

	/** Returns the target Guid */
	FGuid GetGuid() const { return TargetGuid; }
	
	/** Sets the activated state for this spawn point */
	void SetIsActivated(const bool bSetIsActivated) { bIsActivated = bSetIsActivated; }

	/** Sets and returns the overlapping points, based on the parameters and IncrementY & IncrementZ */
	TArray<FVector>& SetOverlappingPoints(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale,
		const FVector& InOrigin, const FVector& InNegativeExtents, const FVector& InPositiveExtents);
	
	/** Flags this SpawnPoint as recent, and records the time it was set as recent. If false, removes flag and clears overlapping points */
	void SetIsRecent(const bool bSetIsRecent);

	/** Sets the TargetScale */
	void SetScale(const FVector& InScale)
	{
		Scale = InScale;
	}

	/** Sets the Guid */
	void SetGuid(const FGuid InGuid)
	{
		TargetGuid = InGuid;
	}
	
private:
	/** Returns the corresponding index type depending on the InIndex, InSize, and InWidth */
	static EGridIndexType FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth);

	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	static TArray<int32> FindBorderingIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth);
	
	/** Empties the Overlapping Points array */
	void EmptyOverlappingPoints() { OverlappingPoints.Empty(); }
};

UCLASS()
class BEATSHOT_API USpawnPointManager : public UObject
{
	GENERATED_BODY()
	
public:

	void InitSpawnPointManager(const FBSConfig& InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents);

	/** Initializes the SpawnCounter array */
	TArray<FVector> InitializeSpawnPoints(const FVector& NegativeExtents, const FVector& PositiveExtents);
	
	/** Finds a SpawnPoint with the matching InIndex */
	FSpawnPoint* FindSpawnPointFromIndex(const int32 InIndex);

	/** Finds a SpawnPoint with the matching InLocation */
	FSpawnPoint* FindSpawnPointFromLocation(const FVector& InLocation);

	/** Finds a SpawnPoint with the matching InGuid */
	FSpawnPoint* FindSpawnPointFromGuid(const FGuid& InGuid);

	/** Returns the oldest most recent spawn point */
	FSpawnPoint* FindOldestRecentSpawnPoint() const;

	/** Returns a filtered array containing only spawn points flagged as recent */
	TArray<FSpawnPoint> GetRecentSpawnPoints() const;
	
	/** Returns a filtered array containing only spawn points flagged as activated */
	TArray<FSpawnPoint> GetActivatedSpawnPoints() const;

	/** Returns a filtered array containing only spawn points flagged as activated or recent */
	TArray<FSpawnPoint> GetActivatedOrRecentSpawnPoints() const;

	/** Returns an array of all points that are occupied by recent targets, readjusted by scale if needed */
	void RemoveOverlappingPointsFromSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale) const;
	
	TArray<FSpawnPoint> GetSpawnCounter() { return SpawnPoints; }
	int32 GetSpawnPointsWidth() const { return Width; }
	int32 GetSpawnMemoryIncY() const { return SpawnMemoryIncY; }
	int32 GetSpawnMemoryIncZ() const { return SpawnMemoryIncZ; }
	int32 GetSpawnPointsHeight() const { return Height; }

	/** Sets the overlapping points for matching SpawnPoint, based on the parameters and IncrementY & IncrementZ */
	void SetOverlappingPoints(FSpawnPoint& Point, const FVector& Scale) const;

	void FlagSpawnPointAsRecent(const FGuid SpawnPointGuid);
	void FlagSpawnPointAsActivated(const FGuid SpawnPointGuid);
	
	/** Sets the corresponding SpawnPoint as not recent */
	UFUNCTION()
	void RemoveRecentFlagFromSpawnPoint(const FGuid SpawnPointGuid);
	void RemoveActivatedFlagFromSpawnPoint(const FGuid SpawnPointGuid);

	int32 GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const;

private:
	void SetAppropriateSpawnMemoryValues();
	
	/** Initialized at start */
	FBSConfig BSConfig;
	
	/** The total amount of horizontal points in SpawnPoints */
	int32 Width;

	/** The total amount of horizontal points in SpawnPoints */
	int32 Height;

	/** Stores all possible spawn locations and the total spawns & player hits at each location */
	TArray<FSpawnPoint> SpawnPoints;

	/** Incremental step value used to iterate through SpawnPoints locations */
	int32 SpawnMemoryIncY;
	
	/** Incremental step value used to iterate through SpawnPoints locations */
	int32 SpawnMemoryIncZ;

	/** Scale the 2D representation of the spawn area down by this factor, Y-axis */
	float SpawnMemoryScaleY;
	
	/** Scale the 2D representation of the spawn area down by this factor, Z-axis */
	float SpawnMemoryScaleZ;
	
	float MinOverlapRadius;
	
	FVector Origin;
	FVector StaticExtents;
	FVector StaticNegativeExtents;
	FVector StaticPositiveExtents;

	const TArray<int32> PreferredScales = {100, 95, 90, 85, 80, 75, 70, 65, 60, 55, 50, 45, 40, 30, 25, 20, 15, 10, 5};

	bool bShowDebug_SpawnMemory = false;
};
