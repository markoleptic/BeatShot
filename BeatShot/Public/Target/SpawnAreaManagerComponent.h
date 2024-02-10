// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "SaveGamePlayerScore.h"
#include "SpawnArea.h"
#include "Target.h"
#include "SpawnAreaManagerComponent.generated.h"


/** Cardinal direction Index types that are valid to use when searching for GridBlocks */
inline const TSet GridBlockIndexTypes = {
	EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up, EBorderingDirection::Down
};

/** Up-Down only index types */
inline const TSet VerticalIndexTypes = {EBorderingDirection::Up, EBorderingDirection::Down};

/** Left-right only index types */
inline const TSet HorizontalIndexTypes = {EBorderingDirection::Left, EBorderingDirection::Right};

/** All index types */
inline const TSet AllIndexTypes = {
	EBorderingDirection::UpLeft, EBorderingDirection::UpRight, EBorderingDirection::DownLeft,
	EBorderingDirection::DownRight, EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up,
	EBorderingDirection::Down
};

/** Contains the minimum and maximum of a Box, i.e. the bottom left corner location and top right corner location. */
struct FExtrema
{
	/** The min extrema */
	FVector Min;

	/** The max extrema */
	FVector Max;

	FExtrema() = default;
	~FExtrema() = default;

	FExtrema(const FVector& InMin, const FVector& InMax) :
	Min(InMin),
	Max(InMax)
	{
	}
};

/** A block of SpawnArea indices that are all connected. Hashed so that only unique combinations are considered and not
 *  all permutations. */
struct FBlock
{
	TArray<int32> Indices;
	
	FBlock() = default;
	~FBlock() = default;

	void AddBlockIndex(const int32 InIndex)
	{
		Indices.Add(InIndex);
	}

	void RemoveBlockIndex(const int32 InIndex)
	{
		Indices.Remove(InIndex);
	}

	int32 Num() const
	{
		return Indices.Num();
	}

	void Reset()
	{
		Indices.Empty();
	}

	FORCEINLINE bool operator==(const FBlock& Other) const
	{
		if (Indices.Num() != Other.Indices.Num()) return false;

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

/** Used as the stack inside UpdateLargestRectangle */
struct FRectInfo
{
	/** Index of the element in IndexValidity */
	int32 Index;
	/** Column index */
	int32 ColIndex;
	/** Column height */
	int32 Height;

	FRectInfo() : Index(0), ColIndex(0), Height(0)
	{
	}
	~FRectInfo() = default;

	FRectInfo(const int32 InIndex, const int32 InColIndex, const int32 InHeight):
	Index(InIndex),
	ColIndex(InColIndex),
	Height(InHeight)
	{
	}
};

/** Contains info about the largest valid rectangle in a grid */
struct FLargestRect
{
	/** True if the ActualBlockSize was greater than the product of the factors chosen for the block,
	 *  like when ActualBlockSize is prime number and a smaller grid is chosen */
	bool bNeedsRemainderIndex;

	/** The max area of the largest rectangle found */
	int32 MaxArea;

	/** The start index of the largest rectangle found */
	int32 StartIndex;

	/** The end index of the largest rectangle found */
	int32 EndIndex;

	/** The start row index of the largest rectangle found */
	int32 StartRowIndex;

	/** The start col index of the largest rectangle found */
	int32 StartColIndex;

	/** The end row index of the largest rectangle found */
	int32 EndRowIndex;

	/** The end col index of the largest rectangle found */
	int32 EndColIndex;

	/** The number of rows in the largest rectangle found */
	int32 NumRowsAvailable;

	/** The number of cols in the largest rectangle found */
	int32 NumColsAvailable;

	/** The true size of the block. Set to the min(MaxArea, input BlockSize) */
	int32 ActualBlockSize;

	/** The size of the block used to set the Chosen Start/End Row/Col Indices */
	int32 ChosenBlockSize;

	/** The chosen start row index */
	int32 ChosenStartRowIndex;

	/** The chosen start col index */
	int32 ChosenStartColIndex;

	/** The chosen end row index */
	int32 ChosenEndRowIndex;

	/** The chosen end col index */
	int32 ChosenEndColIndex;

	FLargestRect() : bNeedsRemainderIndex(false), MaxArea(0), StartIndex(-1), EndIndex(-1), StartRowIndex(0),
	                 StartColIndex(0), EndRowIndex(0), EndColIndex(0), NumRowsAvailable(0), NumColsAvailable(0),
	                 ActualBlockSize(0), ChosenBlockSize(0), ChosenStartRowIndex(-1), ChosenStartColIndex(-1),
	                 ChosenEndRowIndex(-1), ChosenEndColIndex(-1)
	{
	}

	~FLargestRect() = default;

	FLargestRect(const int32 InMaxArea, const int32 InStart, const int32 InEnd): bNeedsRemainderIndex(false),
		MaxArea(InMaxArea), StartIndex(InStart), EndIndex(InEnd), StartRowIndex(0), StartColIndex(0), EndRowIndex(0),
		EndColIndex(0), NumRowsAvailable(0), NumColsAvailable(0), ActualBlockSize(0), ChosenBlockSize(0),
		ChosenStartRowIndex(-1), ChosenStartColIndex(-1), ChosenEndRowIndex(-1), ChosenEndColIndex(-1)
	{
	}

	/** Test the NewMaxArea against MaxArea. If larger, takes on all parameter values */
	void TestNewMaxArea(const int32 InNewMaxArea, const int32 InStart, const int32 InEnd)
	{
		if (InNewMaxArea > MaxArea)
		{
			MaxArea = InNewMaxArea;
			StartIndex = InStart;
			EndIndex = InEnd;
		}
	}

	/** Updates Start/End Row and Column indices, NumRowsAvailable, and NumColsAvailable provided that the StartIndex
	 *  and EndIndex have been found */
	void UpdateIndices(const int32 NumCols)
	{
		StartRowIndex = StartIndex / NumCols;
		StartColIndex = StartIndex % NumCols;
		EndRowIndex = EndIndex / NumCols;
		EndColIndex = EndIndex % NumCols;
		NumRowsAvailable = EndRowIndex - StartRowIndex + 1;
		NumColsAvailable = EndColIndex - StartColIndex + 1;
	}

	/** Sets the value of ActualBlockSize to the minimum of InBlockSize and MaxArea. */
	void SetBlockSize(const int32 InBlockSize)
	{
		ActualBlockSize = FMath::Min(InBlockSize, MaxArea);
	}
};

/** A unique pair of factors for a number */
struct FFactor
{
	int32 Factor1;
	int32 Factor2;
	int32 Distance;

	FFactor(): Factor1(-1), Factor2(-1), Distance(-1)
	{
	}

	FFactor(const int32 F1, const int32 F2) : Factor1(F1), Factor2(F2), Distance(abs(F1 - F2))
	{
	}

	explicit FFactor(const int32 InDistance) : Factor1(-1), Factor2(-1), Distance(InDistance)
	{
	}

	bool IsValid() const
	{
		return Factor1 != -1 && Factor2 != -1;
	}

	FORCEINLINE bool operator==(const FFactor& Other) const
	{
		if (Factor1 == Other.Factor1 && Factor2 == Other.Factor2)
		{
			return true;
		}
		if (Factor1 == Other.Factor2 && Factor2 == Other.Factor1)
		{
			return true;
		}
		return false;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FFactor& Factor)
	{
		uint32 Hash = 0;

		const int32 Min = Factor.Factor1 < Factor.Factor2 ? Factor.Factor1 : Factor.Factor2;
		const int32 Max = Factor.Factor1 < Factor.Factor2 ? Factor.Factor2 : Factor.Factor1;

		Hash = HashCombine(Hash, GetTypeHash(Min));
		Hash = HashCombine(Hash, GetTypeHash(Max));

		return Hash;
	}
};

DECLARE_DELEGATE_RetVal_TwoParams(int32, FRequestRLCSpawnArea, const int32, const TArray<int32>&);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEATSHOT_API USpawnAreaManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USpawnAreaManagerComponent();

	virtual void DestroyComponent(bool bPromoteChildren) override;

	/** Initializes basic variables in SpawnAreaManagerComponent */
	void Init(const TSharedPtr<FBSConfig>& InConfig, const FVector& InOrigin, const FVector& InStaticExtents,
		const FExtrema& InStaticExtrema);

	/** Resets all variables */
	void Clear();
	
	/** Returns the (Height, Width) of all SpawnAreas */
	FIntVector3 GetSpawnAreaInc() const { return SpawnAreaInc; }

	/** Returns (x, NumHorizontal, NumVertical) total spawn areas */
	FIntVector3 GetSpawnAreaSize() const { return Size; }

	/** Returns delegate used to request a SpawnArea selection from the RLC */
	FRequestRLCSpawnArea& GetSpawnAreaRequestDelegate() { return RequestRLCSpawnArea; }

	/** Sets whether or not to request a SpawnArea selection from the RLC */
	void SetShouldAskRLCForSpawnAreas(const bool bShould) { bShouldAskRLCForSpawnAreas = bShould; }

	/** Finds a SpawnArea with the matching location and increments TotalTrackingDamagePossible */
	void UpdateTotalTrackingDamagePossible(const FVector& InLocation) const;

	/** Handles dealing with SpawnAreas that correspond to Damage Events */
	void HandleTargetDamageEvent(const FTargetDamageEvent& DamageEvent);
	
	/** Calls FlagSpawnAreaAsRecent, and sets a timer for when the recent flag should be removed */
	void HandleRecentTargetRemoval(USpawnArea* SpawnArea);
	
	/** Called when the BoxBounds of the TargetManager are changed to update CachedExtrema or CachedEdgeOnly sets */
	void OnExtremaChanged(const FExtrema& Extrema);

	int32 GetNumActivated() const { return CachedActivated.Num(); }
	int32 GetNumDeactivated() const { return GetDeactivatedSpawnAreas().Num(); }
	
protected:
	/** Sets the most recently activated SpawnArea. Called at end of FlagSpawnAreaAsActivated. */
	void SetMostRecentSpawnArea(USpawnArea* SpawnArea) { MostRecentSpawnArea = SpawnArea; }
	
	/** Sets InSpawnAreaInc, InSpawnAreaScale */
	void SetAppropriateSpawnMemoryValues(FIntVector3& OutSpawnAreaInc, FVector& OutSpawnAreaScale,
		const FBSConfig* InCfg, const FVector& InStaticExtents);

	/** Initializes the SpawnCounter array */
	void InitializeSpawnAreas();

	/** Returns whether or not to consider Managed SpawnAreas as invalid choices for activation */
	bool ShouldConsiderManagedAsInvalid() const;
	
	/** Returns pointer to TargetManager's BSConfig */
	TSharedPtr<FBSConfig> GetBSConfig() const { return BSConfig; }

	/** Returns the minimum distance between targets, used just to shorten length of call */
	float GetMinDist() const { return BSConfig->TargetConfig.MinDistanceBetweenTargets; }
	
	/* ------------------------------- */
	/* -- SpawnArea finders/getters -- */
	/* ------------------------------- */

public:
	/** Returns reference to SpawnAreas */
	USpawnArea* GetSpawnArea(const int32 Index) const;

	/** Returns the most recent SpawnArea that was set at the end of ATargetManager::ActivateTarget */
	USpawnArea* GetMostRecentSpawnArea() const { return MostRecentSpawnArea; }

	/** Returns the SpawnArea containing the origin */
	USpawnArea* GetOriginSpawnArea() const { return OriginSpawnArea; }

	/** Finds a SpawnArea with the matching InLocation using the AreaKeyMap for lookup */
	USpawnArea* FindSpawnArea(const FVector& InLocation) const;

	/** Finds a SpawnArea with the matching TargetGuid using the GuidMap for lookup */
	USpawnArea* FindSpawnArea(const FGuid& TargetGuid) const;

	/** Returns the oldest SpawnArea flagged as recent */
	USpawnArea* FindOldestRecentSpawnArea() const;

	/** Returns the oldest SpawnArea flagged as deactivated and managed */
	USpawnArea* FindOldestDeactivatedSpawnArea() const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const USpawnArea* InSpawnArea) const;

	/** Returns true if the SpawnArea is contained in SpawnAreas */
	bool IsSpawnAreaValid(const int32 InIndex) const;

	/** Returns a set of SpawnAreas that are flagged as currently managed */
	TSet<USpawnArea*> GetManagedSpawnAreas() const;

	/** Returns a set of SpawnAreas that are flagged as currently managed and not flagged as activated
	 *  (Cached Managed difference Cached Activated) */
	TSet<USpawnArea*> GetDeactivatedSpawnAreas() const;

	/** Returns a set of SpawnAreas containing only SpawnAreas flagged as recent */
	TSet<USpawnArea*> GetRecentSpawnAreas() const;

	/** Returns a set of SpawnAreas containing only SpawnAreas flagged as activated */
	TSet<USpawnArea*> GetActivatedSpawnAreas() const;

	/** Returns a set of SpawnAreas containing only SpawnAreas flagged as activated or recent
	 *  (Cached Activated union Cached Recent)*/
	TSet<USpawnArea*> GetActivatedOrRecentSpawnAreas() const;

	/** Returns a set of SpawnAreas containing SpawnAreas flagged as managed, activated, or recent
	 *  (Cached Managed union Cached Activated union Cached Recent) */
	TSet<USpawnArea*> GetManagedActivatedOrRecentSpawnAreas() const;

	/** Returns a set of SpawnAreas containing SpawnAreas flagged as managed, not activated, and not recent
	 *  (Cached Managed difference (Cached Activated union Cached Recent)) */
	TSet<USpawnArea*> GetManagedDeactivatedNotRecentSpawnAreas() const;

	/** Returns a set of SpawnAreas containing SpawnAreas not flagged with anything
	 *  (AllSpawnAreas difference Cached Managed difference Cached Activated difference Cached Recent) */
	TSet<USpawnArea*> GetUnflaggedSpawnAreas() const;

	/* ------------------------ */
	/* -- SpawnArea flagging -- */
	/* ------------------------ */

	/** Adds to Managed and Deactivated cache, adds to GuidMap, and flags the SpawnArea as being actively managed by
	 *  TargetManager. Calls SetOccupiedVertices if needed */
	void FlagSpawnAreaAsManaged(USpawnArea* SpawnArea, const FGuid TargetGuid);

	/** Adds to Activated cache, removes from Deactivate cache, and flags the SpawnArea as activated and removes the
	 *  recent flag if present. Calls SetOccupiedVertices if needed */
	void FlagSpawnAreaAsActivated(const FGuid TargetGuid, const FVector& TargetScale);

protected:
	/** Adds to Recent cache and flags the SpawnArea as recent */
	void FlagSpawnAreaAsRecent(USpawnArea* SpawnArea);

	/** Removes from Managed cache, removes from Deactivated cache, removes from GuidMap, and removes the Managed flag,
	 *  meaning the target that the SpawnArea represents is not longer actively managed by TargetManager */
	void RemoveManagedFlagFromSpawnArea(const FGuid TargetGuid);

	/** Removes from Activated cache, adds to Deactivated cache, and removes the activated flag */
	void RemoveActivatedFlagFromSpawnArea(USpawnArea* SpawnArea);

	/** Removes from Recent cache and removes the Recent flag, meaning the SpawnArea is not longer being considered as
	 *  a blocked SpawnArea. SpawnAreas empty their OccupiedVertices when their Recent Flag is removed */
	void RemoveRecentFlagFromSpawnArea(USpawnArea* SpawnArea);

public:
	/** Removes the oldest SpawnArea recent flags if the max number of recent targets has been exceeded */
	void RefreshRecentFlags();

	/* --------------------------------------------------- */
	/* -- Finding Valid SpawnAreas for Spawn/Activation -- */
	/* --------------------------------------------------- */

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Only considers SpawnAreas that
	 *  are linked to a managed target since activatable requires being managed. Also considers the
	 *  Target Activation Selection Policy */
	TSet<USpawnArea*> GetActivatableSpawnAreas(const int32 NumToActivate) const;
	
	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Broadest search since SpawnAreas
	 *  do not have to be linked to a managed target to be considered. Also considers the Target Distribution Policy
	 *  and Bounds Scaling Policy */
	TSet<USpawnArea*> GetSpawnableSpawnAreas(const TArray<FVector>& Scales, const int32 NumToSpawn) const;

protected:
	/** Uses a priority list to return a SpawnArea to activate. Always verifies that the candidate has a valid Guid,
	 *  meaning that it corresponds to a valid target. Priority is origin (setting permitting), reinforcement learning
	 *  component (setting permitting), and lastly chooses a random index of ValidSpawnAreas. */
	USpawnArea* ChooseActivatableSpawnArea(const USpawnArea* PreviousSpawnArea,
		const TSet<USpawnArea*>& ValidSpawnAreas, const TSet<USpawnArea*>& SelectedSpawnAreas) const;

	/** Uses a priority list to return a SpawnArea to spawn. Priority is origin (setting permitting), reinforcement
	 *  learning component (setting permitting), and lastly chooses a random index of ValidSpawnAreas. */
	USpawnArea* ChooseSpawnableSpawnArea(const USpawnArea* PreviousSpawnArea,
		const TSet<USpawnArea*>& ValidSpawnAreas, const TSet<USpawnArea*>& SelectedSpawnAreas) const;
	
	/** Handles selecting SpawnAreas for runtime Grid distributions, based on the
	 *  RuntimeTargetSpawningLocationSelectionMode. Can call FindRandomBorderingGrid, FindGridBlockUsingLargestRect,
	 *  or none if random */
	TSet<USpawnArea*> GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales, int32 NumToSpawn) const;

	/** Handles selecting SpawnAreas for runtime target distribution that are not Grid. Similar loop to
	 *  GetActivatableSpawnAreas */
	TSet<USpawnArea*> GetSpawnableSpawnAreas_NonGrid(const TArray<FVector>& Scales, int32 NumToSpawn) const;

	/** Performs a depth-first search of ValidSpawnAreas, returning a set of SpawnAreas that are all bordering at
	 *  least one another */
	void FindAdjacentGridUsingDFS(TSet<USpawnArea*>& ValidSpawnAreas, const int32 NumToSpawn) const;

	/** Returns a set of SpawnAreas bordering the inSpawnAreas according to Directions .*/
	TSet<USpawnArea*> GetAdjacentSpawnAreas(TSet<USpawnArea*>& InSpawnAreas,
		const TSet<EBorderingDirection>& Directions) const;

	/** Performs the actual ValidSpawnAreas editing by calling FindLargestValidRectangle and ChooseRectIndices.
	 *  Always empties ValidSpawnAreas before looping through the rectangle. Updates MostRecentGridBlock */
	void FindGridBlockUsingLargestRect(TSet<USpawnArea*>& ValidSpawnAreas, const TArray<int32>& IndexValidity,
		const int32 BlockSize, const bool bBordering = false) const;

	/** Attempts to set the start and end indices of a sub-block within the largest rectangle. Finds the best factors
	 *  for the block size as uses them to guide the arrangement of SpawnAreas in the rectangle. Calls
	 *  FindBorderingRectIndices if bBordering is true, but continues on if it failed to set the indices */
	void ChooseRectIndices(FLargestRect& Rect, const bool bBordering) const;

	/** Attempts to set the start and end indices of a sub-block within the largest rectangle by considering indices
	 *  in MostRecentGridBlock that are also within the current largest rectangle. Returns true if successful, and
	 *  false otherwise */
	bool FindBorderingRectIndices(FLargestRect& Rect, const int32 SubRowSize, const int32 SubColSize) const;

	/** Returns all SpawnAreas that can border the input grid block. If the block had 4 SpawnAreas, a maximum of 12
	 *  SpawnAreas may be returned Only considers input directions */
	TSet<USpawnArea*> GetBorderingGridBlockSpawnAreas(const TSet<USpawnArea*>& GridBlock,
		const TSet<EBorderingDirection>& Directions) const;
	
	/** Removes all SpawnAreas that are occupied by activated, recent targets, and possibly managed targets.
	 *  Recalculates occupied vertices for each spawn area if necessary. Only called when finding Spawnable
	 *  Non-Grid SpawnAreas since grid-based will never have to worry about overlapping. */
	void RemoveOverlappingSpawnAreas(TSet<USpawnArea*>& ValidSpawnAreas, const TSet<USpawnArea*>& ChosenSpawnAreas,
		const FVector& NewScale) const;

	/** Filters out any SpawnAreas that aren't bordering Current */
	int32 RemoveNonAdjacentIndices(TSet<USpawnArea*>& ValidSpawnAreas, const USpawnArea* Current) const;

	/* ------------- */
	/* -- Utility -- */
	/* ------------- */

	/** Creates an array with size equal to the number of SpawnAreas, where each index represents whether or not the
	 *  SpawnArea should be consider valid. Takes an array of invalid SpawnArea indices */
	TArray<int32> CreateIndexValidityArray(const TArray<int32>& RemovedIndices) const;

	/** Creates an array with size equal to the number of SpawnAreas, where each index represents whether or not the
	 *  SpawnArea should be consider valid. Takes a set of valid spawn areas */
	TArray<int32> CreateIndexValidityArray(const TSet<USpawnArea*>& ValidSpawnAreas) const;

	/** Finds the maximum rectangle of valid indices in the matrix. Returns a struct containing the area, start index,
	 *  and end index that correspond to SpawnAreas */
	static FLargestRect FindLargestValidRectangle(const TArray<int32>& IndexValidity, const int32 NumRows,
		const int32 NumCols);

	/** Called for every row inside FindLargestValidRectangle. Iterates through the number of columns
	 *  both forward and backward, updating the values if a new maximum rectangle is found */
	static void UpdateLargestRectangle(TArray<int32>& Heights, FLargestRect& LargestRect, const int32 CurrentRow);

	/** Returns a set of all unique factors for a number */
	static TSet<FFactor> FindAllFactors(const int32 Number);

	/** Finds the two factors for a number with the smallest difference between factors. If the number has no factors
	 *  greater than 1, an invalid FFactor is returned. */
	static FFactor FindLargestFactors(const int32 Number);

	/** Tries to find pairs of factors for a number that fit within the two constraints. Initially tries
	 *  FindLargestFactors and falls back to FindAllFactors if it returned invalid. It then iterates through all
	 *  factors that meet the constraints, and chooses the ones with the smallest difference between factors */
	static TSet<FFactor> FindBestFittingFactors(const int32 Number, const int32 Constraint1, const int32 Constraint2);

	/** Wrapper around FindBestFittingFactors, uses Rect.NumRowsAvailable and Rect.NumColsAvailable as constraints */
	static TSet<FFactor> FindBestFittingFactors(const int32 Number, const FLargestRect& Rect);

	/** Returns an array of (Row, Column) pairs where each is a member of the Bordering set and falls within the
	 *  min and max indices of the rectangle */
	static TArray<std::pair<int32, int32>> FindBorderingIndicesInRect(const TSet<USpawnArea*>& Bordering,
		const FLargestRect& Rect, const int32 NumCols);

	/** Returns whether or not a number is prime */
	static constexpr bool IsPrime(const int32 Number);

	/** Calculates the Manhattan distance between to indices given the number of columns */
	static int32 CalcManhattanDist(const int32 Index1, const int32 Index2, const int32 NumCols);
	
	/** General SpawnAreas filter function that takes in a filter function to apply */
	static TArray<int32> FilterIndices(TArray<USpawnArea*>& ValidSpawnAreas, bool (USpawnArea::*FilterFunc)() const);

public:
	/** Gathers all total hits and total spawns for the game mode session and converts them into a 5X5 matrix using
	 *  GetAveragedAccuracyData. Calls UpdateAccuracy once the values are copied over, and returns the struct */
	FAccuracyData GetLocationAccuracy();
	
	/** Preferred SpawnMemory increments */
	UPROPERTY(EditAnywhere, Category="SpawnArea")
	TArray<int32> PreferredSpawnAreaIncScales = {50, 45, 40, 30, 25, 20, 15, 10, 5};

	/* ----------- */
	/* -- Debug -- */
	/* ----------- */

	/** Debug box color for all spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_AllSpawnAreas = FColor::Cyan;

	/** Debug box color for spawn areas removed due to overlap */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_InvalidOverlap = FColor::Red;

	/** Debug box color for spawn areas not removed due to overlap */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_ValidOverlap = FColor::Emerald;

	/** Debug box color for spawnable spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_SpawnableSpawnAreas = FColor::Emerald;

	/** Debug box color for activatable spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_ActivatableSpawnAreas = FColor::Emerald;

	/** Debug box color for activated spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_ActivatedSpawnAreas = FColor::Blue;

	/** Debug box color for deactivated managed spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_DeactivatedSpawnAreas = FColor::Purple;

	/** Debug box color for deactivated managed spawn areas. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_RecentSpawnAreas = FColor::Orange;

	/** Debug box color for spawn areas removed due to the BoxBounds. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_RemovedFromExtremaChange = FColor::Red;

	/** Debug box color for spawn areas removed due to being non adjacent. */
	UPROPERTY(EditAnywhere, Category="Debug")
	FColor DebugColor_NonAdjacent = FColor::Yellow;

	/** Line thickness for all debug boxes. */
	UPROPERTY(EditAnywhere, Category="Debug")
	float DebugBoxLineThickness = 4.f;

	/** Size of the debug point for occupied vertices and non-occupied vertices. */
	UPROPERTY(EditAnywhere, Category="Debug")
	float DebugVertexSize = 24.f;

	UPROPERTY(EditAnywhere, Category="Debug")
	float DebugBoxXOffset = 2.f;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 DebugSphereSegments = 12;

#if !UE_BUILD_SHIPPING
	/** Clears all debug persistent lines/boxes and draws new ones based on debug bool variables. */
	void RefreshDebugBoxes() const;
	
	/** Draws debug boxes using SpawnAreas. */
	void DrawDebug_Boxes(const TSet<USpawnArea*>& InSpawnAreas, const FColor& Color, const int32 Thickness,
		bool bPersistent) const;

	/** Draws debug points for the spawn areas' occupied vertices and non-occupied vertices as well as a debug sphere. */
	void DrawDebug_Vertices(const TSet<USpawnArea*>& InSpawnAreas, const bool bGenerateNew, const bool bDrawSphere) const;

	/** Prints the number of activated, recent, and managed targets. */
	void PrintDebug_SpawnAreaStateInfo() const;
	
	/** Prints debug info about a SpawnArea. */
	static void PrintDebug_SpawnArea(const USpawnArea* SpawnArea);

	/** Prints debug info about SpawnArea distance. */
	void PrintDebug_SpawnAreaDist(const USpawnArea* SpawnArea) const;

	/** Prints debug info about Largest Rectangular area found. */
	static void PrintDebug_GridLargestRect(const FLargestRect& LargestRect, const int32 NumCols);

	/** Prints a formatted matrix (upside down from how indexes appear in SpawnAreas so that it matches in game). */
	static void PrintDebug_Matrix(const TArray<int32>& Matrix, const int32 NumRows, const int32 NumCols);

	/** Toggles showing debug boxes for all SpawnAreas. */
	bool bShowDebug_AllSpawnAreas;
	
	/** Toggles showing debug boxes for valid spawnable spawn areas. */
	bool bShowDebug_SpawnableSpawnAreas;

	/** Toggles showing debug boxes for valid activatable spawn areas. */
	bool bShowDebug_ActivatableSpawnAreas;

	/** Toggles showing debug boxes for activated spawn areas. */
	bool bShowDebug_ActivatedSpawnAreas;

	/** Toggles showing debug boxes for deactivated managed spawn areas. */
	bool bShowDebug_DeactivatedSpawnAreas;

	/** Toggles showing debug boxes for spawn areas removed due to overlap. */
	bool bShowDebug_ValidInvalidSpawnAreas;

	/** Toggles showing debug boxes for recent spawn areas. */
	bool bShowDebug_RecentSpawnAreas;

	/** Toggles showing debug boxes for spawn areas removed due to the BoxBounds. */
	bool bShowDebug_RemovedFromExtremaChange;

	/** Toggles showing debug boxes for spawn areas removed due to being non adjacent. */
	bool bShowDebug_NonAdjacent;

	/** Toggles showing debug points and a debug sphere for overlapping vertices of activated spawn areas. */
	int32 ShowDebug_Vertices;
	
	/** Toggles printing the number managed, activated, and recent Spawn Areas. */
	bool bPrintDebug_SpawnAreaStateInfo;

	/** Toggles printing various grid-distribution related info. */
	bool bPrintDebug_Grid;
	
	// ReSharper disable twice CppUE4ProbableMemoryIssuesWithUObjectsInContainer
	mutable TSet<USpawnArea*> DebugCached_SpawnableValidSpawnAreas;
	mutable TSet<USpawnArea*> DebugCached_NonAdjacentSpawnAreas;

#endif
	
private:
	/** Whether or not to broadcast the RequestRLCSpawnArea delegate when finding SpawnAreas */
	bool bShouldAskRLCForSpawnAreas;
	
	/** Pointer to TargetManager's BSConfig */
	TSharedPtr<FBSConfig> BSConfig;
	
	/** BSConfig->TargetConfig */
	FBS_TargetConfig& TargetConfig() const { return BSConfig->TargetConfig; };

	/** The total amount of (-, horizontal, vertical) SpawnAreas in SpawnAreas */
	FIntVector3 Size;
	
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

	/** All SpawnArea objects inside the larger total spawn area. Set is filled bottom-up, left-to-right. Does not
	 *  change throughout game mode */
	UPROPERTY()
	TSet<USpawnArea*> SpawnAreas;

	/** Maps each location in the SpawnBox to a unique SpawnArea. Does not change throughout game mode */
	UPROPERTY()
	TMap<FAreaKey, USpawnArea*> AreaKeyMap;

	/** Maps each Target Guid to a unique SpawnArea. Added when the SpawnArea is flagged as managed, and removed
	 *  when the managed flag is removed */
	UPROPERTY()
	TMap<FGuid, USpawnArea*> GuidMap;
	
	/** A set of SpawnAreas that fall within the current BoxBounds. All are added initially, updated when the SpawnBox
	 *  extents changes through the OnExtremaChanged function */
	UPROPERTY()
	TSet<USpawnArea*> CachedExtrema;

	/** A set of the currently managed SpawnAreas. Added when the SpawnArea is flagged as managed, and removed
	 *  when the managed flag is removed */
	UPROPERTY()
	TSet<USpawnArea*> CachedManaged;

	/** A set of the currently activated SpawnAreas. Added when the SpawnArea is flagged as activated, and removed
	 *  when the activated flag is removed */
	UPROPERTY()
	TSet<USpawnArea*> CachedActivated;

	/** A set of the currently recent SpawnAreas. Added when flagged as recent, and removed when
	 *  the recent flag is removed */
	UPROPERTY()
	TSet<USpawnArea*> CachedRecent;

	/** A set of the most recently spawned grid block of SpawnAreas */
	UPROPERTY()
	mutable TSet<USpawnArea*> MostRecentGridBlock;

	/** The most recently activated SpawnArea */
	UPROPERTY()
	USpawnArea* MostRecentSpawnArea;

	/** The SpawnArea that contains the origin */
	UPROPERTY()
	USpawnArea* OriginSpawnArea;
	
	/** Delegate used to bind a timer handle to RemoveRecentFlagFromSpawnArea() */
	FTimerDelegate RemoveFromRecentDelegate;

	/** Delegate used to request a SpawnArea selection from the RLC */
	FRequestRLCSpawnArea RequestRLCSpawnArea;
};

inline TSet<FFactor> USpawnAreaManagerComponent::FindBestFittingFactors(const int32 Number, const FLargestRect& Rect)
{
	return FindBestFittingFactors(Number, Rect.NumRowsAvailable, Rect.NumColsAvailable);
}
