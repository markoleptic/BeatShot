// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "SaveGamePlayerScore.h"
#include "Target.h"
#include "BeatShot/BeatShot.h"
#include "UObject/Object.h"
#include "SpawnAreaManagerComponent.generated.h"

/** Enum representing the bordering directions for a SpawnArea */
UENUM(BlueprintType)
enum class EBorderingDirection : uint8
{
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

/** Contains the minimum and maximum of a Box */
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

/** A block of SpawnArea indices that are all connected. Hashed so that only unique combinations are considered and not
 *  all permutations */
USTRUCT()
struct FBlock
{
	GENERATED_BODY()

	int32 BlockSize;
	TArray<int32> Indices;

	FBlock()
	{
		BlockSize = 0;
		Indices = TArray<int32>();
	}

	void AddBlockIndex(const int32 InIndex)
	{
		Indices.Add(InIndex);
		BlockSize++;
	}

	void RemoveBlockIndex(const int32 InIndex)
	{
		const int32 NumRemoved = Indices.Remove(InIndex);
		if (NumRemoved > 0)
		{
			BlockSize--;
		}
	}

	void Reset()
	{
		BlockSize = 0;
		Indices.Empty();
	}

	FORCEINLINE bool operator==(const FBlock& Other) const
	{
		if (BlockSize != Other.BlockSize) return false;
		
		for (const int32 Index : Other.Indices)
		{
			if (Indices.Contains(Index)) continue;
			return false;
		}
		
		return true;
	}
	friend FORCEINLINE uint32 GetTypeHash(const FBlock& Block)
	{
		uint32 Hash = 0;

		// Sort the Indices array to ensure a consistent order
		TArray<int32> SortedIndices = Block.Indices;
		Algo::Sort(SortedIndices);

		for (const int32 Index : SortedIndices)
		{
			Hash = HashCombine(Hash, GetTypeHash(Index));
		}

		return Hash;
	}
};

/** Parameters for FindValidIndexCombinationsDFS */
USTRUCT()
struct FDFSLoopParams
{
	GENERATED_BODY()

	TArray<int32> Valid;
	int32 BlockSize;
	TSet<FBlock> Blocks;
	int32 CurrentDepth;
	FBlock CurrentBlock;
	TSet<int32> Visited;
	TSet<int32> InitialVisited;
	TSet<EBorderingDirection> IndexTypes;

	FDFSLoopParams()
	{
		Valid = TArray<int32>();
		BlockSize = 0;
		Blocks = TSet<FBlock>();
		CurrentDepth = 1;
		CurrentBlock = FBlock();
		Visited = TSet<int32>();
		InitialVisited = TSet<int32>();
		IndexTypes = TSet<EBorderingDirection>();
	}

	FDFSLoopParams(const TArray<int32>& InValid, const TSet<EBorderingDirection>& InGridIndexTypes, const int32 InBlockSize)
	{
		Valid = InValid;
		BlockSize = InBlockSize;
		Blocks = TSet<FBlock>();
		CurrentDepth = 1;
		CurrentBlock = FBlock();
		Visited = TSet<int32>();
		InitialVisited = TSet<int32>();
		IndexTypes = InGridIndexTypes;
	}

	/** Resets CurrentBlock, Visited, and InitialVisited. Sets CurrentDepth to 1 */
	void NextIter(const int32 InNewIndex)
	{
		CurrentBlock.Reset();
		CurrentBlock.AddBlockIndex(InNewIndex);
		Visited.Empty();
		InitialVisited.Empty();
		InitialVisited.Add(InNewIndex);
		CurrentDepth = 1;
	}
};

/** Parameters for initializing a SpawnArea */
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

class USpawnArea;

DECLARE_DELEGATE_RetVal_OneParam(USpawnArea*, FRequestRLCSpawnArea, const TArray<USpawnArea*>&)

/** Key used for location-based indexing */
USTRUCT()
struct FAreaKey
{
	GENERATED_BODY()

	FVector Vertex_BottomLeft;
	FVector Vertex_TopRight;

	FAreaKey()
	{
		Vertex_BottomLeft = FVector();
		Vertex_TopRight = FVector();
	}

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
		return FMath::IsNearlyEqual(Vertex_BottomLeft.Y, Other.Vertex_BottomLeft.Y, 0.1f) &&
			   FMath::IsNearlyEqual(Vertex_BottomLeft.Z, Other.Vertex_BottomLeft.Z, 0.1f) &&
			   FMath::IsNearlyEqual(Vertex_TopRight.Y, Other.Vertex_TopRight.Y, 0.1f) &&
			   FMath::IsNearlyEqual(Vertex_TopRight.Z, Other.Vertex_TopRight.Z, 0.1f);
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

/** A small piece of the total spawn area containing info about its state in relation to targets,
 *  including points that represents where targets have spawned */
UCLASS()
class BEATSHOT_API USpawnArea : public UObject
{
	GENERATED_BODY()

	friend class USpawnAreaManagerComponent;

	FGuid Guid;

	/** The center point of the box */
	FVector CenterPoint;

	/** The point chosen after a successful spawn or activation. Will be inside the sub area */
	FVector ChosenPoint;

	/** The width of the box */
	float Width;

	/** The height of the box */
	float Height;

	/** The unique index for this SpawnArea */
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

	/** The type of Grid Index */
	EGridIndexType GridIndexType;

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
	
	/** The indices of the SpawnAreas adjacent to this SpawnArea, including their direction from this SpawnArea */
	TMap<EBorderingDirection, int32> AdjacentIndexMap;

	/** The vertices of other SpawnAreas that overlap with this SpawnArea based on the target scale, minimum distance
	 *  between targets, minimum overlap radius, and size of the SpawnArea. Set when flagged as managed, updated if
	 *  the target scale changes */
	TSet<FVector> OverlappingVertices;

	/** NumHorizontalTargets * NumVerticalTargets */
	int32 Size;

public:
	USpawnArea();

	void Init(const FSpawnAreaParams& InParams);

	int32 GetIndex() const { return Index; }
	FVector GetChosenPoint() const { return ChosenPoint; }
	FVector GetBottomLeftVertex() const { return Vertex_BottomLeft; }
	bool IsActivated() const { return bIsActivated; }
	bool IsManaged() const { return bIsCurrentlyManaged; }
	bool IsRecent() const { return bIsRecent; }
	bool CanActivateWhileActivated() const { return bAllowActivationWhileActivated; }
	double GetTimeSetRecent() const { return TimeSetRecent; }
	TMap<EBorderingDirection, int32> GetAdjacentIndexMap() const { return AdjacentIndexMap; }
	TSet<FVector> GetOverlappingVertices() const { return OverlappingVertices; }
	EGridIndexType GetIndexType() const { return GridIndexType; }
	int32 GetTotalSpawns() const { return TotalSpawns; }
	int32 GetTotalHits() const { return TotalHits; }
	int32 GetTotalTrackingDamage() const { return TotalTrackingDamage; }
	int32 GetTotalTrackingDamagePossible() const { return TotalTrackingDamagePossible; }
	FVector GetTargetScale() const { return TargetScale; }
	FGuid GetGuid() const { return Guid; }
	void SetGuid(const FGuid InGuid) { Guid = InGuid; }
	void ResetGuid() { Guid.Invalidate(); }
	TArray<int32> GetBorderingIndices() const;
	
	float GetMinOverlapRadius() const;

	/** Sets the TargetScale */
	void SetTargetScale(const FVector& InScale);

	/** Sets the value of ChosenPoint to the output of GenerateRandomPointInSpawnArea */
	void SetRandomChosenPoint();

private:
	/** Returns an array of indices that border the index when looking at the array like a 2D grid */
	void SetAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex,
		const int32 InWidth);

	/** Returns the corresponding index type depending on the InIndex, InSize, and InWidth */
	static EGridIndexType FindIndexType(const int32 InIndex, const int32 InSize, const int32 InWidth);

	/** Returns a random non-overlapping point within the SpawnArea */
	FVector GenerateRandomPointInSpawnArea() const;

	/** Flags this SpawnArea as corresponding to a target being managed by TargetManager */
	void SetIsCurrentlyManaged(const bool bSetIsCurrentlyManaged);
	
	/** Sets the activated state and the persistently activated state for this SpawnArea */
	void SetIsActivated(const bool bActivated, const bool bAllow = false);

	/** Flags this SpawnArea as recent, and records the time it was set as recent. If false, removes flag and
	 *  clears OverlappingVertices */
	void SetIsRecent(const bool bSetIsRecent);

	/** Sets the value of OverlappingVertices */
	void SetInvalidVerts(const TSet<FVector>& InVerts);

	/** Finds and returns the vertices that overlap with SpawnArea by tracing a circle around the SpawnArea based on
	 *  the target scale, minimum distance between targets, minimum overlap radius, and size of the SpawnArea */
	TSet<FVector> MakeInvalidVerts(const float InMinDist, const FVector& InScale,
		TSet<FVector>& OutValid, const bool bAddValidVertices = false) const;

	/** Empties the OverlappingVertices array */
	void EmptyOverlappingVertices();

	/** Increments the total amount of spawns in this SpawnArea, including handling special case where it has not
	 *  spawned there yet */
	void IncrementTotalSpawns();

	/** Increments the total amount of hits in this SpawnArea */
	void IncrementTotalHits();

	/** Increments TotalTrackingDamagePossible */
	void IncrementTotalTrackingDamagePossible();

	/** Increments TotalTrackingDamage */
	void IncrementTotalTrackingDamage();

public:
	
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

	friend FORCEINLINE uint32 GetTypeHash(const USpawnArea& SpawnArea)
	{
		return GetTypeHash(SpawnArea.GetIndex());
	}
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

	/** Sets the most recently activated SpawnArea. Should be called from TargetManager at end of ActivateTarget */
	void SetMostRecentSpawnArea(USpawnArea* SpawnArea);

	/** Returns the most recent SpawnArea that was set at the end of ATargetManager::ActivateTarget */
	USpawnArea* GetMostRecentSpawnArea() const;

	/** Returns the SpawnArea containing the origin */
	USpawnArea* GetOriginSpawnArea() const;

	/** Finds a SpawnArea with the matching InLocation using the AreaKeyMap for lookup */
	USpawnArea* FindSpawnAreaFromLocation(const FVector& InLocation) const;

	/** Finds a SpawnArea with the matching TargetGuid using the GuidMap for lookup */
	USpawnArea* FindSpawnAreaFromGuid(const FGuid& TargetGuid) const;

	/** Returns the oldest SpawnArea flagged as recent */
	USpawnArea* FindOldestRecentSpawnArea() const;

	/** Returns the oldest SpawnArea flagged as deactivated and managed */
	USpawnArea* FindOldestDeactivatedManagedSpawnArea() const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const USpawnArea* InSpawnArea) const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const int32 InIndex) const;

	/** Returns an array of SpawnAreas that are flagged as currently managed */
	TSet<USpawnArea*> GetManagedSpawnAreas() const;

	/** Returns an array of SpawnAreas that are flagged as currently managed and not flagged as activated */
	TSet<USpawnArea*> GetDeactivatedManagedSpawnAreas() const;

	/** Returns a filtered array containing only SpawnAreas flagged as recent */
	TSet<USpawnArea*> GetRecentSpawnAreas() const;

	/** Returns a filtered array containing only SpawnAreas flagged as activated */
	TSet<USpawnArea*> GetActivatedSpawnAreas() const;

	/** Returns a filtered array containing only SpawnAreas flagged as activated or recent */
	TSet<USpawnArea*> GetActivatedOrRecentSpawnAreas() const;

	/** Returns a filtered array containing SpawnAreas flagged as managed, activated, or recent */
	TSet<USpawnArea*> GetManagedActivatedOrRecentSpawnAreas() const;

	/** Removes all SpawnAreas that are occupied by activated and recent targets, readjusted by scale if needed.
	 *  This is a more intensive version of FilterRecentIndices and FilterActivatedIndices */
	void RemoveOverlappingSpawnAreas(TArray<USpawnArea*>& ValidSpawnAreas, const FVector& Scale) const;

	TArray<USpawnArea*>& GetSpawnAreasRef() { return SpawnAreas; }

	/** Returns the (Height, Width) of all SpawnAreas */
	FIntVector3 GetSpawnAreaInc() const { return SpawnAreaInc; }

	/** Returns (x, NumHorizontal, NumVertical) total spawn areas */
	FIntVector3 GetSpawnAreaSize() const { return Size; }

	/** Adds to Managed and Deactivated cache, adds to GuidMap, and flags the SpawnArea as being actively managed by
	 *  TargetManager. Calls SetInvalidVerts if needed */
	void FlagSpawnAreaAsManaged(USpawnArea* SpawnArea, const FGuid TargetGuid);

	/** Adds to Activated cache, removes from Deactivate cache, and flags the SpawnArea as activated and removes the
	 *  recent flag if present. Calls SetInvalidVerts if needed */
	void FlagSpawnAreaAsActivated(const FGuid TargetGuid, const bool bCanActivateWhileActivated);

	/** Adds to Recent cache and flags the SpawnArea as recent */
	void FlagSpawnAreaAsRecent(USpawnArea* SpawnArea);

	/** Removes from Managed cache, removes from Deactivated cache, removes from GuidMap, and removes the Managed flag,
	 *  meaning the target that the SpawnArea represents is not longer actively managed by TargetManager */
	void RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid);

	/** Removes from Activated cache, adds to Deactivated cache, and removes the activated flag */
	void RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea);

	/** Removes from Recent cache and removes the Recent flag, meaning the SpawnArea is not longer being considered as
	 *  a blocked SpawnArea. SpawnAreas empty their OverlappingVertices when their Recent Flag is removed */
	void RemoveRecentFlagFromSpawnArea(USpawnArea* SpawnArea);

	/** Removes the oldest SpawnArea recent flags if the max number of recent targets has been exceeded */
	void RefreshRecentFlags();

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Only considers SpawnAreas that
	 *  are linked to a managed target since activatable requires being managed. Also considers the
	 *  Target Activation Selection Policy */
	TArray<USpawnArea*> GetActivatableSpawnAreas(const int32 NumToActivate) const;

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Broadest search since SpawnAreas
	 *  do not have to be linked to a managed target to be considered. Also considers the Target Distribution Policy
	 *  and Bounds Scaling Policy */
	TArray<USpawnArea*> GetSpawnableSpawnAreas(const TArray<FVector>& Scales, const FExtrema& Extrema, int32 NumToSpawn,
		const ERuntimeTargetSpawningLocationSelectionMode Mode) const;

	TArray<USpawnArea*> GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales, int32 NumToSpawn,
		const ERuntimeTargetSpawningLocationSelectionMode Mode) const;
	
	/** Calls FindValidIndexCombinationsDFS for each SpawnArea in ValidSpawnAreas. If at least one valid block was
	 *  found, ValidSpawnAreas is emptied, a random block of indices is chosen, and the SpawnAreas corresponding
	 *  to the indices are added to ValidSpawnAreas */
	void FindRandomGridBlock(TArray<USpawnArea*>& ValidSpawnAreas, const TArray<int32>& IndexValidity, const int32 BlockSize) const;

	/** Performs a depth-first search to find unique combinations of SpawnArea indices (Params.Blocks) of a
	 *  specific size. Checks to see if the SpawnArea is a valid index using Params.Valid. */
	void FindValidIndexCombinationsDFS(const int32 StartIndex, FDFSLoopParams& Params) const;

	/** Filters the SpawnAreas array directly using FilterByPredicate and removing SpawnAreas outside of the Extrema */
	void FilterByExtrema(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const;

	/** Filters out any SpawnAreas that aren't along the edge of the current Extrema */
	void FilterByEdgeOnly(TArray<USpawnArea*>& ValidSpawnAreas, const FExtrema& Extrema) const;

	/** Filters out any locations that are flagged as managed */
	TArray<int32> FilterManagedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;

	/** Filters out any SpawnAreas that are flagged as activated */
	TArray<int32> FilterActivatedIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;
	
	/** Filters out any SpawnAreas that aren't bordering Current */
	TArray<int32> FilterBorderingIndices(TArray<USpawnArea*>& ValidSpawnAreas, const USpawnArea* Current) const;

	/** Filters out any SpawnAreas that are flagged as recent */
	TArray<int32> FilterRecentIndices(TArray<USpawnArea*>& ValidSpawnAreas) const;
	
	/** General SpawnAreas filter function that takes in a filter function to apply */
	TArray<int32> FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas, bool (USpawnArea::*FilterFunc)() const,
		const bool bShowDebug, const FColor& DebugColor) const;
	
	/** Creates an array with size equal to the number of SpawnAreas, where each index represents whether or not the
	 *  SpawnArea should be consider valid */
	TArray<int32> CreateIndexValidityArray(const TArray<int32>& RemovedIndices) const;
	
	/** Gathers all total hits and total spawns for the game mode session and converts them into a 5X5 matrix using
	 *  GetAveragedAccuracyData. Calls UpdateAccuracy once the values are copied over, and returns the struct */
	FAccuracyData GetLocationAccuracy();

	/** Shows the grid of spawn areas drawn as debug boxes */
	void DrawDebug_AllSpawnAreas() const;

	/** Removes the grid of spawn areas drawn as debug boxes */
	void ClearDebug_AllSpawnAreas() const;

	/** Draws debug boxes using SpawnAreas Indices */
	void DrawDebug_Boxes(const TArray<int32>& InIndices, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws debug boxes using SpawnAreas */
	void DrawDebug_Boxes(const TArray<const USpawnArea*>& InSpawnAreas, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws debug boxes using SpawnAreas */
	void DrawDebug_Boxes(const TArray<USpawnArea*>& InSpawnAreas, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws a debug sphere where the overlapping vertices were traced from, and draws debug points for
	 *  the vertices if they were recalculated */
	void DrawVerticesOverlap(const USpawnArea* SpawnArea, const FVector& Scale,
		const TSet<FVector>& Valid, const TSet<FVector>& Invalid) const;

	void DrawVerticesOverlap(const TSet<USpawnArea*>& InSpawnAreas, const FVector& Scale,
	const TSet<FVector>& Valid, const TSet<FVector>& Invalid) const;

	/** Prints debug info about a SpawnArea */
	static void PrintDebug_SpawnArea(const USpawnArea* SpawnArea);

	/** Prints debug info about SpawnArea distance */
	void PrintDebug_SpawnAreaDist(const USpawnArea* SpawnArea) const;

	FRequestRLCSpawnArea RequestRLCSpawnArea;

	/** Toggles showing green debug boxes for valid spawn locations at the end of the GetValidSpawnAreas function */
	bool bDebug_Valid;

	/** Toggles showing red debug boxes for removed spawn locations */
	bool bDebug_Removed;

	/** Toggles showing turquoise debug boxes for filtered recent SpawnAreas */
	bool bDebug_FilterRecent;

	/** Toggles showing cyan debug boxes for filtered activated SpawnAreas */
	bool bDebug_FilterActivated;

	/** Toggles showing blue debug boxes for filtered managed SpawnAreas */
	bool bDebug_FilterManaged;

	/** Shows the overlapping vertices generated when SpawnArea was flagged as Managed as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices, and green Debug Boxes for valid */
	bool bDebug_ManagedVerts;

	/** Shows the overlapping vertices generated when SpawnArea was flagged as Activated as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices, and green Debug Boxes for valid */
	bool bDebug_ActivatedVerts;

	/** Shows the overlapping vertices generated during RemoveOverlappingSpawnAreas as red DebugPoints.
	 *  Draws a magenta Debug Sphere showing the target that was used to generate the overlapping points.
	 *  Draws red Debug Boxes for the removed overlapping vertices */
	bool bDebug_AllVerts;

		
	bool bShouldAskRLCForSpawnAreas = false;

private:
	bool ShouldConsiderManagedAsInvalid() const;

	/** Sets SpawnMemoryInY & Z, SpawnMemoryScaleY & Z, MinOverlapRadius, and bLocationsAreCorners */
	void SetAppropriateSpawnMemoryValues();

	/** Initializes the SpawnCounter array */
	void InitializeSpawnAreas();

	/** Returns pointer to TargetManager's BSConfig */
	FBSConfig* GetBSConfig() const { return BSConfig; }

	const FBS_TargetConfig& GetTc() const { return BSConfig->TargetConfig; }

	float GetMinDist() const { return BSConfig->TargetConfig.MinDistanceBetweenTargets; }

	/** Pointer to TargetManager's BSConfig */
	FBSConfig* BSConfig;

	/** The total amount of (-, horizontal, vertical) SpawnAreas in SpawnAreas */
	FIntVector3 Size;

	/** All SpawnArea objects inside the larger total spawn area. Set is filled bottom-up, left-to-right */
	UPROPERTY()
	TArray<USpawnArea*> SpawnAreas;

	/** Maps each location in the SpawnBox to a unique SpawnArea */
	UPROPERTY()
	TMap<FAreaKey, USpawnArea*> AreaKeyMap;

	/** Maps each Target Guid to a unique SpawnArea. Requires SpawnArea to be managed */
	UPROPERTY()
	TMap<FGuid, USpawnArea*> GuidMap;

	/** A set of the currently managed SpawnAreas */
	UPROPERTY()
	TSet<USpawnArea*> CachedManaged;

	/** A set of the currently activated SpawnAreas */
	UPROPERTY()
	TSet<USpawnArea*> CachedActivated;

	/** A set of the currently managed but deactivated SpawnAreas */
	UPROPERTY()
	TSet<USpawnArea*> CachedDeactivated;

	/** A set of the currently recent SpawnAreas */
	UPROPERTY()
	TSet<USpawnArea*> CachedRecent;

	/** The most recently activated SpawnArea */
	UPROPERTY()
	USpawnArea* MostRecentSpawnArea;

	/** The SpawnArea that contains the origin */
	UPROPERTY()
	USpawnArea* OriginSpawnArea;

	/** Incremental (horizontal, vertical) step values used to iterate through SpawnAreas locations */
	FIntVector3 SpawnAreaInc;

	/** Scale the representation of the spawn area down by this factor */
	FVector SpawnAreaScale;

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
