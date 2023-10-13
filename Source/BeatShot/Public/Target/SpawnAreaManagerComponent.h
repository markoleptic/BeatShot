// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "SaveGamePlayerScore.h"
#include "Target.h"
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

USTRUCT()
struct FSpawnAreaParams
{
	GENERATED_BODY()
	int32 Index;
	FVector BottomLeft;
	int32 IncY;
	int32 IncZ;
	int32 NumVerticalTargets;
	int32 NumHorizontalTargets;
	bool bGrid;

	FSpawnAreaParams()
	{
		Index = -1;
		BottomLeft = FVector();
		IncY = -1;
		IncZ = -1;
		NumVerticalTargets = -1;
		NumHorizontalTargets = -1;
		bGrid = false;
	}

	FSpawnAreaParams& NextIndex(const int32 InIndex, const FVector& InLoc)
	{
		Index = InIndex;
		BottomLeft = InLoc;
		return *this;
	}
};

/** A small piece of the total spawn area containing info about its state in relation to targets,
 *  including points that represents where targets have spawned */
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

	/** Bottom left vertex of the box, used for comparison between SpawnAreas. This Vertex corresponds to a location
	 *  in AllSpawnLocations (in TargetManager) */
	FVector Vertex_BottomLeft;
	FVector Vertex_TopRight;
	FVector Vertex_BottomRight;
	FVector Vertex_TopLeft;

	/** Whether or not this SpawnArea has a target active */
	bool bIsActivated;

	/** Whether or not this SpawnArea can be reactivated while activated */
	bool bAllowActivationWhileActivated;

	/** Whether or not this SpawnArea still corresponds to a managed target */
	bool bIsCurrentlyManaged;

	/** Whether or not this SpawnArea recently had a target occupy its space */
	bool bIsRecent;

	/** The time that this SpawnArea was flagged as recent */
	double TimeSetRecent;

	/** The type of SpawnArea (corner, border, etc.) */
	EGridIndexType IndexType;

	/** A unique ID for the target, used to find the target when it comes time to free the blocked SpawnAreas
	 *  of a target */
	FGuid TargetGuid;

	/** The scale associated with the target if the SpawnArea is currently representing one */
	FVector TargetScale;

	/** The total number of target spawns in this SpawnArea */
	int32 TotalSpawns;

	/** The total number of target hits by player in this SpawnArea */
	int32 TotalHits;

	/** The total amount of tracking damage that was possible at this SpawnArea */
	int32 TotalTrackingDamagePossible;

	/** The total amount of tracking damage that was dealt at this SpawnArea */
	int32 TotalTrackingDamage;

	/** The indices of the SpawnAreas adjacent to this SpawnArea */
	TArray<int32> AdjacentIndices;

	/** The SpawnAreas that the target overlapped with */
	TArray<FVector> OverlappingVertices;

	/** NumHorizontalTargets * NumVerticalTargets */
	int32 Size;

public:
	USpawnArea();

	void Init(const FSpawnAreaParams& InParams);

	FORCEINLINE bool operator ==(const USpawnArea& Other) const
	{
		if (Index != INDEX_NONE && Index == Other.Index)
		{
			return true;
		}
		if ((Other.Vertex_BottomLeft.Y >= Vertex_BottomLeft.Y) && (Other.Vertex_BottomLeft.Z >= Vertex_BottomLeft.Z) &&
			(Other.Vertex_BottomLeft.Y < Vertex_TopRight.Y - 0.01) && (Other.Vertex_BottomLeft.Z < Vertex_TopRight.Z -
				0.01))
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
		if ((Other->Vertex_BottomLeft.Y >= Vertex_BottomLeft.Y) && (Other->Vertex_BottomLeft.Z >= Vertex_BottomLeft.Z)
			&& (Other->Vertex_BottomLeft.Y < Vertex_TopRight.Y - 0.01) && (Other->Vertex_BottomLeft.Z < Vertex_TopRight.
				Z - 0.01))
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
	bool CanActivateWhileActivated() const { return bAllowActivationWhileActivated; }
	bool IsCurrentlyManaged() const { return bIsCurrentlyManaged; }
	bool IsRecent() const { return bIsRecent; }
	double GetTimeSetRecent() const { return TimeSetRecent; }
	TArray<int32> GetBorderingIndices() const { return AdjacentIndices; }
	TArray<FVector> GetOverlappingVertices() const { return OverlappingVertices; }
	FGuid GetTargetGuid() const { return TargetGuid; }
	EGridIndexType GetIndexType() const { return IndexType; }
	int32 GetTotalSpawns() const { return TotalSpawns; }
	int32 GetTotalHits() const { return TotalHits; }
	int32 GetTotalTrackingDamage() const { return TotalTrackingDamage; }
	int32 GetTotalTrackingDamagePossible() const { return TotalTrackingDamagePossible; }
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

private:
	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	static TArray<int32> FindAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex,
		const int32 InWidth);

	/** Returns the corresponding index type depending on the InIndex, InSize, and InWidth */
	static EGridIndexType FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth);

	/** Returns a random non-overlapping point within the SpawnArea */
	FVector GenerateRandomPointInSpawnArea() const;

	/** Flags this SpawnArea as corresponding to a target being managed by TargetManager */
	void SetIsCurrentlyManaged(const bool bSetIsCurrentlyManaged)
	{
		bIsCurrentlyManaged = bSetIsCurrentlyManaged;
	}

	/** Sets the activated state and the persistently activated state for this SpawnArea */
	void SetIsActivated(const bool bActivated, const bool bAllow = false)
	{
		bIsActivated = bActivated;
		bAllowActivationWhileActivated = bAllow;
	}

	/** Flags this SpawnArea as recent, and records the time it was set as recent. If false, removes flag and
	 *  clears OverlappingVertices */
	void SetIsRecent(const bool bSetIsRecent);

	/** Sets the value of OverlappingVertices */
	void SetOverlappingVertices(const TArray<FVector>& InOverlappingVertices);

	/** Finds and returns the OverlappingVertices, based on the parameters, Width, & Height */
	TArray<FVector> GenerateOverlappingVertices(const float InMinTargetDistance, const float InMinOverlapRadius,
		const FVector& InScale, TArray<FVector>& DebugVertices, const bool bAddDebugVertices = false) const;

	/** Empties the OverlappingVertices array */
	void EmptyOverlappingVertices() { OverlappingVertices.Empty(); }

	/** Increments the total amount of spawns in this SpawnArea, including handling special case where it has not
	 *  spawned there yet */
	void IncrementTotalSpawns();

	/** Increments the total amount of hits in this SpawnArea */
	void IncrementTotalHits();

	/** Increments TotalTrackingDamagePossible */
	void IncrementTotalTrackingDamagePossible();

	/** Increments TotalTrackingDamage */
	void IncrementTotalTrackingDamage();
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEATSHOT_API USpawnAreaManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpawnAreaManagerComponent();

	virtual void DestroyComponent(bool bPromoteChildren) override;

	/** Initializes basic variables in SpawnAreaManagerComponent */
	void Init(FBSConfig* InBSConfig, const FVector& InOrigin, const FVector& InStaticExtents,
		const FExtrema& InStaticExtrema);

	/** Resets all variables */
	void Clear();

	/** Finds a SpawnArea with the matching location and increments TotalTrackingDamagePossible */
	void UpdateTotalTrackingDamagePossible(const FVector& InLocation) const;
	
	/** Handles dealing with SpawnAreas that correspond to Damage Events */
	void HandleTargetDamageEvent(const FTargetDamageEvent& DamageEvent);
	
	/** Calls FlagSpawnAreaAsRecent, and sets a timer for when the recent flag should be removed */
	void HandleRecentTargetRemoval(USpawnArea* SpawnArea);
	
	/** Returns true if bSpawnEveryOtherTargetInCenter is true and the previous SpawnArea is not the Origin SpawnArea */
	bool ShouldForceSpawnAtOrigin() const;

	/** Sets the most recently activated SpawnArea */
	void SetMostRecentSpawnArea(USpawnArea* SpawnArea);

	/** Returns the most recently activated SpawnArea */
	USpawnArea* GetMostRecentSpawnArea() const;

	/** Returns the SpawnArea containing the origin */
	USpawnArea* GetOriginSpawnArea() const;

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

	/** Removes all SpawnAreas that are occupied by activated and recent targets, readjusted by scale if needed */
	void RemoveOverlappingSpawnAreas(TArray<USpawnArea*>& ValidSpawnAreas, const FVector& Scale) const;

	TArray<USpawnArea*>& GetSpawnAreasRef() { return SpawnAreas; }
	TArray<USpawnArea*> GetSpawnAreas() const { return SpawnAreas; }

	/** Returns the (Height, Width) of all SpawnAreas */
	FIntVector3 GetSpawnAreaInc() const { return SpawnAreaInc; }

	/** Returns (x, NumHorizontal, NumVertical) total spawn areas */
	FIntVector3 GetSpawnAreaSize() const { return Size; }

	/** Flags the SpawnArea as being actively managed by TargetManager. Calls SetOverlappingVertices if needed */
	void FlagSpawnAreaAsManaged(const FGuid TargetGuid) const;

	/** Flags the SpawnArea as activated and removes the recent flag if present. Calls SetOverlappingVertices
	 *  if needed */
	void FlagSpawnAreaAsActivated(const FGuid TargetGuid, const bool bCanActivateWhileActivated) const;

	/** Flags the SpawnArea as recent */
	static void FlagSpawnAreaAsRecent(USpawnArea* SpawnArea);

	/** Removes the Managed flag, meaning the target that the SpawnArea represents is not longer actively managed by
	 *  TargetManager */
	void RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid) const;

	/** Called after deactivation of a target to remove the activated flag */
	void RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea) const;

	/** Removes the Recent flag, meaning the SpawnArea is not longer being considered as a blocked SpawnArea.
	 *  SpawnAreas empty their OverlappingVertices when their Recent Flag is removed */
	void RemoveRecentFlagFromSpawnArea(USpawnArea* SpawnArea) const;

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array based on the
	*   TargetDistributionPolicy, BoundsScalingPolicy, and if needed, the TargetActivationSelectionPolicy */
	TArray<USpawnArea*> GetValidSpawnAreas(const FVector& Scale, const FExtrema& InCurrentExtrema) const;

	/** Adds valid SpawnAreas for an edge-only TargetDistributionPolicy */
	void HandleEdgeOnlySpawnLocations(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const;

	/** Adds valid SpawnAreas for a full range TargetDistributionPolicy */
	void HandleFullRangeSpawnLocations(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const;

	/** Adds valid SpawnAreas for a grid TargetDistributionPolicy, using TargetActivationSelectionPolicy */
	void HandleGridSpawnLocations(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** Filters out any SpawnAreas that aren't bordering the CurrentSpawnArea */
	void FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** Filters out any SpawnAreas that are flagged as recent */
	void FilterRecentIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** Filters out any SpawnAreas that are flagged as activated */
	void FilterActivatedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** Filters out any locations that are flagged as managed */
	void FilterManagedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** General SpawnAreas filter function that takes in a filter function to apply */
	void FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas, bool (USpawnArea::*FilterFunc)() const,
			const bool bShowDebug, const FColor& DebugColor) const;

	/** Gathers all total hits and total spawns for the game mode session and converts them into a 5X5 matrix using
	 *  GetAveragedAccuracyData. Calls UpdateAccuracy once the values are copied over, and returns the struct */
	FAccuracyData GetLocationAccuracy();

	/** Shows the grid of spawn areas drawn as debug boxes */
	void DrawDebug_AllSpawnAreas() const;

	/** Removes the grid of spawn areas drawn as debug boxes */
	void ClearDebug_AllSpawnAreas() const;

	/** Draws debug boxes, converting the open locations to center points using SpawnMemory values */
	void DrawDebug_Boxes(const TArray<FVector>& InLocations, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws debug boxes using SpawnAreas */
	void DrawDebug_Boxes(const TArray<const USpawnArea*>& InSpawnAreas, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws debug boxes using SpawnAreas */
	void DrawDebug_Boxes(const TArray<USpawnArea*>& InSpawnAreas, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws a debug sphere where the overlapping vertices were traced from, and draws debug points for
	 *  the vertices if they were recalculated */
	void DrawOverlappingVertices(const USpawnArea* SpawnArea, const FVector& Scale, const TArray<FVector>& DebugVertices = TArray<FVector>()) const;

	/** Prints debug info about a SpawnArea */
	static void PrintDebug_SpawnArea(const USpawnArea* SpawnArea);

	/** Prints debug info about SpawnArea distance */
	void PrintDebug_SpawnAreaDist(const USpawnArea* SpawnArea) const;

	/** Toggles showing green debug boxes for valid spawn locations at the end of the GetValidSpawnAreas function */
	bool bShowDebug_ValidSpawnLocations;

	/** Toggles showing red debug boxes for removed spawn locations */
	bool bShowDebug_RemovedSpawnLocations;

	/** Toggles showing turquoise debug boxes for filtered recent SpawnAreas */
	bool bShowDebug_FilteredRecentIndices;

	/** Toggles showing cyan debug boxes for filtered activated SpawnAreas */
	bool bShowDebug_FilteredActivatedIndices;

	/** Toggles showing blue debug boxes for filtered managed SpawnAreas */
	bool bShowDebug_FilteredManagedIndices;

	/** Shows the overlapping vertices generated when SpawnArea was flagged as Managed as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices, and green Debug Boxes for valid */
	bool bShowDebug_OverlappingVertices_OnFlaggedManaged;

	/** Shows the overlapping vertices generated when SpawnArea was flagged as Activated as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices, and green Debug Boxes for valid */
	bool bShowDebug_OverlappingVertices_OnFlaggedActivated;

	/** Shows the overlapping vertices generated during RemoveOverlappingSpawnAreas as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices */
	bool bShowDebug_OverlappingVertices_Dynamic;

private:
	bool ShouldConsiderManagedAsInvalid() const;
	
	/** Sets SpawnMemoryInY & Z, SpawnMemoryScaleY & Z, MinOverlapRadius, and bLocationsAreCorners */
	void SetAppropriateSpawnMemoryValues();

	/** Initializes the SpawnCounter array */
	TArray<FVector> InitializeSpawnAreas();

	/** Returns an array containing the BottomLeftVertex of all SpawnAreas */
	TArray<FVector> GetAllBottomLeftVertices() const { return AllBottomLeftVertices; }

	/** Returns pointer to TargetManager's BSConfig */
	FBSConfig* GetBSConfig() const { return BSConfig; }

	/** Pointer to TargetManager's BSConfig */
	FBSConfig* BSConfig;

	/** The total amount of (-, horizontal, vertical) SpawnAreas in SpawnAreas */
	FIntVector3 Size;

	/** Stores all possible spawn locations and the total spawns & player hits at each location. Array is filled
	 *  bottom-up, left-to-right */
	UPROPERTY()
	TArray<USpawnArea*> SpawnAreas;

	UPROPERTY()
	USpawnArea* MostRecentSpawnArea;

	UPROPERTY()
	USpawnArea* OriginSpawnArea;

	/** An array containing the BottomLeftVertex of all SpawnAreas */
	TArray<FVector> AllBottomLeftVertices;
	
	/** Incremental (horizontal, vertical) step values used to iterate through SpawnAreas locations */
	FIntVector3 SpawnAreaInc;
	
	/** Scale the representation of the spawn area down by this factor */
	FVector SpawnAreaScale;

	/** Radius used when finding overlapping SpawnAreas */
	float MinOverlapRadius;

	/** BoxBounds origin */
	FVector Origin;

	/** The largest the BoxExtents will be */
	FVector StaticExtents;

	/** The largest min and max extrema for the SpawnBox */
	FExtrema StaticExtrema;

	/** Preferred SpawnMemory increments */
	const TArray<int32> PreferredScales = {50, 45, 40, 30, 25, 20, 15, 10, 5};

	/** Delegate used to bind a timer handle to RemoveRecentFlagFromSpawnArea() */
	FTimerDelegate RemoveFromRecentDelegate;
};
