// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "BeatShot/BeatShot.h"
#include "UObject/Object.h"
#include "SpawnAreaManagerComponent.generated.h"

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

USTRUCT()
struct FExtrema
{
	GENERATED_BODY()

	/** The min extrema */
	FVector Min;

	/** The max extrema */
	FVector Max;

	FExtrema()
	{
		Min = FVector();
		Max = FVector();
	}

	FExtrema(const FVector& InMin, const FVector& InMax)
	{
		Min = InMin;
		Max = InMax;
	}
};

/** A small piece of the total spawn area containing info about its state in relation to targets, including points that represents where targets have spawned */
UCLASS()
class BEATSHOT_API USpawnArea : public UObject
{
	GENERATED_BODY()

	friend class USpawnAreaManagerComponent;
	
	/** The center point of the box */
	FVector CenterPoint;

	/** The point chosen after a successful spawn or activation. Will be inside the sub area */
	FVector ChosenPoint;
	
	/** The width of the box */
	float Width;

	/** The height of the box */
	float Height;
	
	/** The index for this SpawnArea inside an array of SpawnAreas */
	int32 Index;

	/** Bottom left vertex of the box, used for comparison between SpawnAreas. This Vertex corresponds to a location in AllSpawnLocations (in TargetManager) */
	FVector Vertex_BottomLeft;
	FVector Vertex_TopRight;
	FVector Vertex_BottomRight;
	FVector Vertex_TopLeft;
	
	/** Whether or not this SpawnArea has a target active */
	bool bIsActivated;

	/** Whether or not this SpawnArea still corresponds to a managed target */
	bool bIsCurrentlyManaged;
	
	/** Whether or not this SpawnArea recently had a target occupy its space */
	bool bIsRecent;

	/** The time that this SpawnArea was flagged as recent */
	double TimeSetRecent;

	/** The type of SpawnArea (corner, border, etc.) */
	EGridIndexType IndexType;

	/** A unique ID for the target, used to find the target when it comes time to free the blocked SpawnAreas of a target */
	FGuid TargetGuid;

	/** The scale associated with the target if the SpawnArea is currently representing one */
	FVector TargetScale;

	/** The total number of target spawns in this SpawnArea */
	int32 TotalSpawns;

	/** The total number of target hits by player in this SpawnArea */
	int32 TotalHits;
	
	/** The indices of the SpawnAreas adjacent to this SpawnArea */
	TArray<int32> AdjacentIndices;

	/** The SpawnAreas that the target overlapped with */
	TArray<FVector> OverlappingVertices;

public:
	USpawnArea();

	void Init(const int32 InIndex, const FVector& InPoint, const float IncY, const float IncZ,  const int32 InNumHorizontalTargets = INDEX_NONE, const int32 InNumVerticalTargets = INDEX_NONE);

	FORCEINLINE bool operator ==(const USpawnArea& Other) const
	{
		if (Index != INDEX_NONE && Index == Other.Index)
		{
			return true;
		}
		if ((Other.Vertex_BottomLeft.Y >= Vertex_BottomLeft.Y) &&
			(Other.Vertex_BottomLeft.Z >= Vertex_BottomLeft.Z) &&
			(Other.Vertex_BottomLeft.Y < Vertex_TopRight.Y - 0.01) &&
			(Other.Vertex_BottomLeft.Z < Vertex_TopRight.Z - 0.01))
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator <(const USpawnArea& Other) const
	{
		if (Vertex_BottomLeft.Z < Other.Vertex_BottomLeft.Z)
		{
			return true;
		}
		if (Vertex_BottomLeft.Z == Other.Vertex_BottomLeft.Z && Vertex_BottomLeft.Y < Other.Vertex_BottomLeft.Y)
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator ==(const USpawnArea* Other) const
	{
		if (Index != INDEX_NONE && Index == Other->Index)
		{
			return true;
		}
		if ((Other->Vertex_BottomLeft.Y >= Vertex_BottomLeft.Y) &&
			(Other->Vertex_BottomLeft.Z >= Vertex_BottomLeft.Z) &&
			(Other->Vertex_BottomLeft.Y < Vertex_TopRight.Y - 0.01) &&
			(Other->Vertex_BottomLeft.Z < Vertex_TopRight.Z - 0.01))
		{
			return true;
		}
		return false;
	}

	FORCEINLINE bool operator <(const USpawnArea* Other) const
	{
		if (Vertex_BottomLeft.Z < Other->Vertex_BottomLeft.Z)
		{
			return true;
		}
		if (Vertex_BottomLeft.Z == Other->Vertex_BottomLeft.Z && Vertex_BottomLeft.Y < Other->Vertex_BottomLeft.Y)
		{
			return true;
		}
		return false;
	}

	int32 GetIndex() const { return Index; }
	FVector GetChosenPoint() const { return ChosenPoint; }
	FVector GetBottomLeftVertex() const { return Vertex_BottomLeft; }
	bool IsActivated() const { return bIsActivated; }
	bool IsCurrentlyManaged() const { return bIsCurrentlyManaged; }
	bool IsRecent() const { return bIsRecent; }
	double GetTimeSetRecent() const { return TimeSetRecent; }
	TArray<int32> GetBorderingIndices() const { return AdjacentIndices; }
	TArray<FVector> GetOverlappingVertices() const { return OverlappingVertices; }
	FGuid GetTargetGuid() const { return TargetGuid; }
	EGridIndexType GetIndexType() const { return IndexType; }
	int32 GetTotalSpawns() const { return TotalSpawns; }
	int32 GetTotalHits() const { return TotalHits; }
	FVector GetTargetScale() const { return TargetScale; }
	
	/** Sets the TargetScale */
	void SetTargetScale(const FVector& InScale)
	{
		TargetScale = InScale;
	}

	/** Sets the Guid */
	void SetTargetGuid(const FGuid InGuid)
	{
		TargetGuid = InGuid;
	}

	/** Sets the value of ChosenPoint to the output of GenerateRandomPointInSpawnArea */
	void SetRandomChosenPoint();
	
	/** Returns an array of directions that contain all directions where the location point does not have an adjacent point in that direction. */
	/*TArray<EBorderingDirection> GetBorderingEdges(const TArray<FVector>& ValidLocations, const FExtrema& InExtrema) const;*/

private:
	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	static TArray<int32> FindAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth);

	/** Returns the corresponding index type depending on the InIndex, InSize, and InWidth */
	static EGridIndexType FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth);
	
	/** Returns a random non-overlapping point within the SpawnArea */
	FVector GenerateRandomPointInSpawnArea() const;

	/** Flags this SpawnArea as corresponding to a target being managed by TargetManager */
	void SetIsCurrentlyManaged(const bool bSetIsCurrentlyManaged)
	{
		bIsCurrentlyManaged = bSetIsCurrentlyManaged;
	}
	
	/** Sets the activated state for this SpawnArea */
	void SetIsActivated(const bool bSetIsActivated)
	{
		bIsActivated = bSetIsActivated;
	}

	/** Flags this SpawnArea as recent, and records the time it was set as recent. If false, removes flag and clears OverlappingVertices */
	void SetIsRecent(const bool bSetIsRecent);
	
	/** Sets the value of OverlappingVertices */
	void SetOverlappingVertices(const TArray<FVector>& InOverlappingVertices);

	/** Finds and returns the OverlappingVertices, based on the parameters, Width, & Height */
	TArray<FVector> GenerateOverlappingVertices(const float InMinTargetDistance, const float InMinOverlapRadius, const FVector& InScale, TArray<FVector>& DebugVertices, const bool bAddDebugVertices = false) const;

	/** Empties the OverlappingVertices array */
	void EmptyOverlappingVertices() { OverlappingVertices.Empty(); }
	
	/** Increments the total amount of spawns in this SpawnArea, including handling special case where it has not spawned there yet */
	void IncrementTotalSpawns();

	/** Increments the total amount of hits in this SpawnArea */
	void IncrementTotalHits();
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEATSHOT_API USpawnAreaManagerComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
	USpawnAreaManagerComponent();

	virtual void DestroyComponent(bool bPromoteChildren) override;
	
	/** Initializes basic variables in SpawnAreaManagerComponent */
	void Init(FBSConfig* InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents, const FExtrema& InStaticExtrema);
	
	/** Finds a SpawnArea with the matching InIndex */
	USpawnArea* FindSpawnAreaFromIndex(const int32 InIndex) const;

	/** Finds a SpawnArea with the matching InLocation */
	USpawnArea* FindSpawnAreaFromLocation(const FVector& InLocation) const;

	/** Finds a SpawnArea with the matching TargetGuid */
	USpawnArea* FindSpawnAreaFromGuid(const FGuid& TargetGuid) const;

	/** Returns the oldest most recent SpawnArea */
	USpawnArea* FindOldestRecentSpawnArea() const;

	/** NOT BEING USED, Returns the first index of GetDeactivatedManagedSpawnAreas */
	USpawnArea* FindOldestDeactivatedManagedSpawnArea() const;

	/** NOT BEING USED, Returns the first index of GetDeactivatedManagedSpawnAreas */
	USpawnArea* FindBorderingDeactivatedManagedSpawnArea(const USpawnArea* InSpawnArea) const;

	/** Returns the SpawnArea index corresponding to a world location, or INDEX_NONE if not found */
	int32 FindSpawnAreaIndexFromLocation(const FVector& InLocation) const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const USpawnArea* InSpawnArea) const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const int32 InIndex) const;

	/** Returns an array of SpawnAreas that are flagged as currently managed */
	TArray<USpawnArea*> GetManagedSpawnAreas() const;

	/** Returns an array of SpawnAreas that are flagged as currently managed and not flagged as activated */
	TArray<USpawnArea*> GetDeactivatedManagedSpawnAreas() const;

	/** Returns a filtered array containing only SpawnAreas flagged as recent */
	TArray<USpawnArea*> GetRecentSpawnAreas() const;
	
	/** Returns a filtered array containing only SpawnAreas flagged as activated */
	TArray<USpawnArea*> GetActivatedSpawnAreas() const;

	/** Returns a filtered array containing only SpawnAreas flagged as activated or recent */
	TArray<USpawnArea*> GetActivatedOrRecentSpawnAreas() const;

	/** Returns a filtered array containing SpawnAreas flagged as managed, activated, or recent */
	TArray<USpawnArea*> GetManagedActivatedOrRecentSpawnAreas() const;
	
	/** Removes the oldest SpawnArea recent flags if the max number of recent targets has been exceeded */
	void RefreshRecentFlags() const;

	/** Removes all locations that are occupied by activated and recent targets, readjusted by scale if needed */
	void RemoveOverlappingSpawnLocations(TArray<FVector>& SpawnLocations, const FVector& Scale) const;

	TArray<USpawnArea*>& GetSpawnAreasRef() { return SpawnAreas; }
	TArray<USpawnArea*> GetSpawnAreas() const { return SpawnAreas; }
	int32 GetSpawnMemoryIncY() const { return SpawnMemoryIncY; }
	int32 GetSpawnMemoryIncZ() const { return SpawnMemoryIncZ; }
	int32 GetSpawnAreasWidth() const { return Width; }
	int32 GetSpawnAreasHeight() const { return Height; }

	/** Flags the SpawnArea as being actively managed by TargetManager. Calls SetOverlappingVertices if needed */
	void FlagSpawnAreaAsManaged(const FGuid TargetGuid) const;
	
	/** Flags the SpawnArea as activated and removes the recent flag if present. Calls SetOverlappingVertices if needed */
	void FlagSpawnAreaAsActivated(const FGuid TargetGuid) const;
	
	/** Flags the SpawnArea as recent */
	void FlagSpawnAreaAsRecent(const FGuid TargetGuid) const;
	
	/** Calls RemoveActivatedFlagFromSpawnArea, calls FlagSpawnAreaAsRecent, and sets a timer for when the recent flag should be removed */
	void HandleRecentTargetRemoval(const ERecentTargetMemoryPolicy& RecentTargetMemoryPolicy, const FTargetDamageEvent& TargetDamageEvent);

	/** Removes the Managed flag, meaning the target that the SpawnArea represents is not longer actively managed by TargetManager */
	void RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid) const;
	
	/** Called after deactivation of a target. Increments TotalsSpawns and TotalHits if necessary, and removes activated flag */
	void RemoveActivatedFlagFromSpawnArea(const FTargetDamageEvent& TargetDamageEvent) const;
	
	/** Removes the Recent flag, meaning the SpawnArea is not longer being considered as a blocked SpawnArea.
	 *  SpawnAreas empty their OverlappingVertices when their Recent Flag is removed */
	void RemoveRecentFlagFromSpawnArea(const FGuid TargetGuid) const;

	/** Returns an array of valid spawn points, filtering locations from AllSpawnLocations based on the
	*   TargetDistributionPolicy, BoundsScalingPolicy and if needed, the TargetActivationSelectionPolicy */
	TArray<FVector> GetValidSpawnLocations(const FVector& Scale, const FExtrema &InCurrentExtrema, const USpawnArea* CurrentSpawnArea) const;

	/** Adds valid spawn locations for an edge-only TargetDistributionPolicy */
	void HandleEdgeOnlySpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema &Extrema) const;

	/** Adds valid spawn locations for a full range TargetDistributionPolicy */
	void HandleFullRangeSpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema &Extrema) const;

	/** Adds valid spawn locations for a grid TargetDistributionPolicy, using TargetActivationSelectionPolicy */
	void HandleGridSpawnLocations(TArray<FVector>& ValidSpawnLocations, const USpawnArea* CurrentSpawnArea) const;

	/** Filters out any locations that aren't bordering the CurrentSpawnArea */
	void FilterBorderingIndices(TArray<FVector>& ValidSpawnLocations, const USpawnArea* CurrentSpawnArea) const;

	/** Filters out any locations that correspond to areas flagged as recent */
	void FilterRecentIndices(TArray<FVector>& ValidSpawnLocations) const;

	/** Filters out any locations that correspond to areas flagged as activated */
	void FilterActivatedIndices(TArray<FVector>& ValidSpawnLocations) const;

	/** Filters out any locations that correspond to areas flagged as managed */
	void FilterManagedIndices(TArray<FVector>& ValidSpawnLocations) const;
		
	int32 GetOutArrayIndexFromSpawnAreaIndex(const int32 SpawnAreaIndex) const;

	/** Draws debug boxes, converting the open locations to center points using SpawnMemory values */
	void DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor, const int32 InThickness, const int32 InDepthPriority) const;

	/** Prints debug info about a SpawnArea */
	void PrintDebug_SpawnArea(const USpawnArea* SpawnArea) const;

	bool bShowDebug_SpawnMemory;

	/** Shows the overlapping vertices generated when a target was flagged as managed */
	bool bShowDebug_OverlappingVertices;

	/** Shows the overlapping vertices during RemoveOverlappingSpawnLocations */
	bool bShowDebug_OverlappingVertices_All;

private:
	/** Sets SpawnMemoryInY & Z, SpawnMemoryScaleY & Z, MinOverlapRadius, and bLocationsAreCorners */
	void SetAppropriateSpawnMemoryValues();
	
	/** Initializes the SpawnCounter array */
	TArray<FVector> InitializeSpawnAreas();
	
	TArray<FVector> GetAllBottomLeftVertices() const { return AllBottomLeftVertices; }

	FBSConfig* GetBSConfig() const { return BSConfig; }
	
	/** Pointer to TargetManager's BSConfig */
	FBSConfig* BSConfig;
	
	/** The total amount of horizontal SpawnAreas in SpawnAreas */
	int32 Width;

	/** The total amount of vertical SpawnAreas in SpawnAreas */
	int32 Height;

	/** Stores all possible spawn locations and the total spawns & player hits at each location */
	UPROPERTY()
	TArray<USpawnArea*> SpawnAreas;
	
	/** An array containing the BottomLeftVertex of all SpawnAreas */
	TArray<FVector> AllBottomLeftVertices;

	/** Incremental step value used to iterate through SpawnAreas locations */
	int32 SpawnMemoryIncY;
	
	/** Incremental step value used to iterate through SpawnAreas locations */
	int32 SpawnMemoryIncZ;

	/** Scale the 2D representation of the spawn area down by this factor, Y-axis */
	float SpawnMemoryScaleY;
	
	/** Scale the 2D representation of the spawn area down by this factor, Z-axis */
	float SpawnMemoryScaleZ;

	/** Radius used when finding overlapping SpawnAreas */
	float MinOverlapRadius;

	/** BoxBounds origin */
	FVector Origin;

	/** The largest the BoxExtents will be */
	FVector StaticExtents;

	/** The largest min and max extrema for the SpawnBox */
	FExtrema StaticExtrema;

	/** Preferred SpawnMemory increments */
	const TArray<int32> PreferredScales = {/*100, 95, 90, 85, 80, 75, 70, 65, 60, 55,*/ 50, 45, 40, 30, 25, 20, 15, 10, 5};
	
	/** Delegate used to bind a timer handle to RemoveRecentFlagFromSpawnArea() inside of OnTargetHealthChangedOrExpired() */
	FTimerDelegate RemoveFromRecentDelegate;
};
