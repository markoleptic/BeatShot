﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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

	TArray<int32> Indices;

	FBlock()
	{
		Indices = TArray<int32>();
	}

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

	FRectInfo(const int32 InIndex, const int32 InColIndex, const int32 InHeight)
	{
		Index = InIndex;
		ColIndex = InColIndex;
		Height = InHeight;
	}
};

/** Contains info about the largest valid rectangle in a grid */
USTRUCT()
struct FLargestRect
{
	GENERATED_BODY()

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

	FLargestRect(const int32 InMaxArea, const int32 InStart, const int32 InEnd): bNeedsRemainderIndex(false),
		StartRowIndex(0), StartColIndex(0), EndRowIndex(0), EndColIndex(0), NumRowsAvailable(0), NumColsAvailable(0),
		ActualBlockSize(0), ChosenBlockSize(0), ChosenStartRowIndex(-1), ChosenStartColIndex(-1), ChosenEndRowIndex(-1),
		ChosenEndColIndex(-1)
	{
		MaxArea = InMaxArea;
		StartIndex = InStart;
		EndIndex = InEnd;
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

	/** Updates StartRowIndex, StartColIndex, EndRowIndex, EndColIndex, NumRowsAvailable, and NumColsAvailable
	 *  provided that the StartIndex and EndIndex has been found */
	void UpdateIndices(const int32 NumCols)
	{
		StartRowIndex = StartIndex / NumCols;
		StartColIndex = StartIndex % NumCols;
		EndRowIndex = EndIndex / NumCols;
		EndColIndex = EndIndex % NumCols;
		NumRowsAvailable = EndRowIndex - StartRowIndex + 1;
		NumColsAvailable = EndColIndex - StartColIndex + 1;
	}

	void SetBlockSize(const int32 InBlockSize)
	{
		ActualBlockSize = FMath::Min(InBlockSize, MaxArea);
	}
};

/** A unique pair of factors for a number */
USTRUCT()
struct FFactor
{
	GENERATED_BODY()

	int32 Factor1;
	int32 Factor2;
	int32 Distance;

	FFactor(): Factor1(-1), Factor2(-1), Distance(-1)
	{
	}

	FFactor(const int32 F1, const int32 F2)
	{
		Factor1 = F1;
		Factor2 = F2;
		Distance = abs(F1 - F2);
	}

	explicit FFactor(const int32 InDistance)
	{
		Factor1 = -1;
		Factor2 = -1;
		Distance = InDistance;
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

/** Parameters for FindValidIndexCombinationsDFS */
USTRUCT()
struct FDFSLoopParams
{
	GENERATED_BODY()

	/** Whether or not a suitable block has been found */
	bool bFound;

	/** The minimum among a completed block's maximum distance between any two indices in the block */
	int32 BestMaxDistance;

	/** The minimum among a completed block's total distance between all indices in the block */
	int32 BestTotalDistance;

	/** The size of the block to create */
	int32 BlockSize;

	/** Number of columns in the SpawnArea grid */
	int32 NumCols;

	/** Recursions skipped */
	int32 SkippedRecursions;

	/** Total iterations performed */
	int32 TotalIterations;

	/** Total recursions performed */
	int32 TotalRecursions;

	/** The current block of indices */
	FBlock CurrentBlock;

	/** A set of suitable blocks, usually just one since often exit after first found */
	TSet<FBlock> Blocks;

	/** Types of indexes that should be allowed to link SpawnAreas */
	TSet<EBorderingDirection> IndexTypes;

	/** An array with size equal to number of SpawnAreas, where each index corresponds to a SpawnArea index. A value
	 *  of 1 indicates the index can be spawned at, while an index of 0 indicates it cannot be spawned at */
	TArray<int32> Valid;

	/** A set of indices already visited, after the first index of a block */
	TSet<int32> Visited;

	/** A set of first block indices already visited. Added before entering the recursion */
	TSet<int32> InitialVisited;

	FDFSLoopParams()
	{
		bFound = false;
		BestMaxDistance = 99999;
		BestTotalDistance = 99999;
		BlockSize = 0;
		NumCols = 0;
		SkippedRecursions = 0;
		TotalIterations = 0;
		TotalRecursions = 0;

		CurrentBlock = FBlock();
		Blocks = TSet<FBlock>();
		IndexTypes = TSet<EBorderingDirection>();
		Valid = TArray<int32>();
		Visited = TSet<int32>();
		InitialVisited = TSet<int32>();
	}

	FDFSLoopParams(const TArray<int32>& InValid, const TSet<EBorderingDirection>& InGridIndexTypes,
		const int32 InBlockSize, const int32 InNumCols)
	{
		bFound = false;
		BestMaxDistance = 99999;
		BestTotalDistance = 99999;
		BlockSize = InBlockSize;
		NumCols = InNumCols;
		SkippedRecursions = 0;
		TotalIterations = 0;
		TotalRecursions = 0;

		CurrentBlock = FBlock();
		Blocks = TSet<FBlock>();
		IndexTypes = InGridIndexTypes;
		Valid = InValid;
		Visited = TSet<int32>();
		InitialVisited = TSet<int32>();
	}

	/** Resets CurrentBlock, Visited, and InitialVisited. Sets CurrentDepth to 1 */
	void NextIter(const int32 InNewIndex)
	{
		CurrentBlock.Reset();
		CurrentBlock.AddBlockIndex(InNewIndex);
		Visited.Empty();
		InitialVisited.Empty();
		InitialVisited.Add(InNewIndex);
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

DECLARE_DELEGATE_RetVal_TwoParams(USpawnArea*, FRequestRLCSpawnArea, const TArray<USpawnArea*>&, const USpawnArea*)

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
			FMath::IsNearlyEqual(Vertex_TopRight.Y, Other.Vertex_TopRight.Y, 0.1f) && FMath::IsNearlyEqual(
				Vertex_TopRight.Z, Other.Vertex_TopRight.Z, 0.1f);
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

	/** Guid associated with a managed target */
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
	void SetAdjacentIndices(const EGridIndexType InGridIndexType, const int32 InIndex, const int32 InWidth);

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
	TSet<FVector> MakeInvalidVerts(const float InMinDist, const FVector& InScale, TSet<FVector>& OutValid,
		const bool bAddValidVertices = false) const;

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

private:
	/** Sets InSpawnAreaInc, InSpawnAreaScale */
	static void SetAppropriateSpawnMemoryValues(FIntVector3& InSpawnAreaInc, FVector& InSpawnAreaScale,
		const FBSConfig* InCfg, const FVector& InStaticExtents);

	/** Initializes the SpawnCounter array */
	void InitializeSpawnAreas();

	/** Returns whether or not to consider Managed SpawnAreas as invalid choices for activation */
	bool ShouldConsiderManagedAsInvalid() const;

	/** Returns true if bSpawnEveryOtherTargetInCenter is true and the previous SpawnArea is not the Origin SpawnArea */
	bool ShouldForceSpawnAtOrigin() const;

public:
	/** Returns reference to SpawnAreas */
	TArray<USpawnArea*>& GetSpawnAreasRef() { return SpawnAreas; }

	/** Returns the (Height, Width) of all SpawnAreas */
	FIntVector3 GetSpawnAreaInc() const { return SpawnAreaInc; }

	/** Returns (x, NumHorizontal, NumVertical) total spawn areas */
	FIntVector3 GetSpawnAreaSize() const { return Size; }

	/** Returns delegate used to request a SpawnArea selection from the RLC */
	FRequestRLCSpawnArea& GetSpawnAreaRequestDelegate() { return RequestRLCSpawnArea; }

private:
	/** Returns pointer to TargetManager's BSConfig */
	FBSConfig* GetBSConfig() const { return BSConfig; }

	/** Returns pointer to TargetManager's BSConfig.TargetConfig */
	const FBS_TargetConfig& GetTargetCfg() const { return BSConfig->TargetConfig; }

	/** Returns the minimum distance between targets, used just to shorten length of call */
	float GetMinDist() const { return BSConfig->TargetConfig.MinDistanceBetweenTargets; }

public:
	/** Sets whether or not to request a SpawnArea selection from the RLC */
	void SetShouldAskRLCForSpawnAreas(const bool bShould) { bShouldAskRLCForSpawnAreas = bShould; }

	/** Sets the most recently activated SpawnArea. Should be called from TargetManager at end of ActivateTarget */
	void SetMostRecentSpawnArea(USpawnArea* SpawnArea) { MostRecentSpawnArea = SpawnArea; }

	/** Finds a SpawnArea with the matching location and increments TotalTrackingDamagePossible */
	void UpdateTotalTrackingDamagePossible(const FVector& InLocation) const;

	/** Handles dealing with SpawnAreas that correspond to Damage Events */
	void HandleTargetDamageEvent(const FTargetDamageEvent& DamageEvent);

	/** Calls FlagSpawnAreaAsRecent, and sets a timer for when the recent flag should be removed */
	void HandleRecentTargetRemoval(USpawnArea* SpawnArea);

	/* ------------------------------- */
	/* -- SpawnArea finders/getters -- */
	/* ------------------------------- */

	/** Returns the most recent SpawnArea that was set at the end of ATargetManager::ActivateTarget */
	USpawnArea* GetMostRecentSpawnArea() const { return MostRecentSpawnArea; }

	/** Returns the SpawnArea containing the origin */
	USpawnArea* GetOriginSpawnArea() const { return OriginSpawnArea; }

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

	/* ------------------------------------ */
	/* -- TSet SpawnArea finders/getters -- */
	/* ------------------------------------ */

	/** Returns a set of SpawnAreas that are flagged as currently managed */
	TSet<USpawnArea*> GetManagedSpawnAreas() const;

	/** Returns a set of SpawnAreas that are flagged as currently managed and not flagged as activated
	 *  (Cached Managed difference Cached Activated) */
	TSet<USpawnArea*> GetDeactivatedManagedSpawnAreas() const;

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

	/* ----------------------------------- */
	/* -- Finding Valid Spawn Locations -- */
	/* ----------------------------------- */

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Only considers SpawnAreas that
	 *  are linked to a managed target since activatable requires being managed. Also considers the
	 *  Target Activation Selection Policy */
	TArray<USpawnArea*> GetActivatableSpawnAreas(const int32 NumToActivate) const;

	/** Returns an array of valid SpawnAreas filtered from the SpawnAreas array. Broadest search since SpawnAreas
	 *  do not have to be linked to a managed target to be considered. Also considers the Target Distribution Policy
	 *  and Bounds Scaling Policy */
	TArray<USpawnArea*> GetSpawnableSpawnAreas(const TArray<FVector>& Scales, int32 NumToSpawn) const;

	/** Handles selecting SpawnAreas for runtime Grid distributions, based on the
	 *  RuntimeTargetSpawningLocationSelectionMode. Can call FindRandomBorderingGrid, FindGridBlockUsingLargestRect,
	 *  or none if random */
	TArray<USpawnArea*> GetSpawnableSpawnAreas_Grid(const TArray<FVector>& Scales, int32 NumToSpawn) const;

	/** Handles selecting SpawnAreas for runtime target distribution that are not Grid. Similar loop to
	 *  GetActivatableSpawnAreas */
	TArray<USpawnArea*> GetSpawnableSpawnAreas_NonGrid(const TArray<FVector>& Scales, int32 NumToSpawn) const;

	/** DOESNT WORK Kinda scuffed method that finds a random chain of bordering targets from the most recent SpawnArea */
	void FindRandomBorderingGrid(TArray<USpawnArea*>& ValidSpawnAreas, int32 NumToSpawn) const;

	/** Performs the actual ValidSpawnAreas editing by calling FindLargestValidRectangle and ChooseRectIndices.
	 *  Always empties ValidSpawnAreas before looping through the rectangle. Updates MostRecentGridBlock */
	void FindGridBlockUsingLargestRect(TArray<USpawnArea*>& ValidSpawnAreas, const TArray<int32>& IndexValidity,
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

	/** Calls FindValidIndexCombinationsDFS for each SpawnArea in ValidSpawnAreas. If at least one valid block was
	 *  found, ValidSpawnAreas is emptied, a random block of indices is chosen, and the SpawnAreas corresponding
	 *  to the indices are added to ValidSpawnAreas */
	void FindGridBlockUsingDFS(TArray<USpawnArea*>& ValidSpawnAreas, const TArray<int32>& IndexValidity,
		const TSet<EBorderingDirection>& Directions, const int32 BlockSize) const;

	/** Called when the BoxBounds of the TargetManager are changed to update CachedExtrema or CachedEdgeOnly sets */
	void OnExtremaChanged(const FExtrema& Extrema);

private:
	/** Removes all SpawnAreas that are occupied by activated and recent targets, readjusted by scale if needed.
	 *  This is a more intensive version of FilterRecentIndices and FilterActivatedIndices */
	void RemoveOverlappingSpawnAreas(TArray<USpawnArea*>& ValidSpawnAreas, TArray<USpawnArea*>& ChosenSpawnAreas,
		const FVector& Scale) const;

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
	static bool IsPrime(const int32 Number);

	/** Calculates the Manhattan distance between to indices given the number of columns */
	static int32 CalcManhattanDist(const int32 Index1, const int32 Index2, const int32 NumCols);

	/** Estimates the BestTotalDistance and MaxTotalDistance of an FDFSLoopParams struct by simulating an index block */
	static void EstimateDistances(FDFSLoopParams& Params);

	/** Deprecated since it just takes way too many iterations and using the largest rectangle is much more efficient.
	 *  Performs a depth-first search to find unique combinations of SpawnArea indices (Params.Blocks) of a
	 *  specific size. Checks to see if the SpawnArea is a valid index using Params.Valid. */
	void FindValidIndexCombinationsDFS(const int32 StartIndex, FDFSLoopParams& Params) const;

public:
	/** Gathers all total hits and total spawns for the game mode session and converts them into a 5X5 matrix using
	 *  GetAveragedAccuracyData. Calls UpdateAccuracy once the values are copied over, and returns the struct */
	FAccuracyData GetLocationAccuracy();

	/* ----------- */
	/* -- Debug -- */
	/* ----------- */

	/** Shows the grid of spawn areas drawn as debug boxes */
	void DrawDebug_AllSpawnAreas() const;

	/** Removes the grid of spawn areas drawn as debug boxes */
	void ClearDebug_AllSpawnAreas() const;

	/** Draws debug boxes using SpawnAreas Indices */
	void DrawDebug_Boxes(const TArray<int32>& InIndices, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws debug boxes using SpawnAreas */
	void DrawDebug_Boxes(const TArray<USpawnArea*>& InSpawnAreas, const FColor& InColor, const int32 InThickness,
		const int32 InDepthPriority, bool bPersistantLines = false) const;

	/** Draws a debug sphere where the overlapping vertices were traced from, and draws debug points for
	 *  the vertices if they were recalculated */
	void DrawVerticesOverlap(const USpawnArea* SpawnArea, const FVector& Scale, const TSet<FVector>& Valid,
		const TSet<FVector>& Invalid) const;

	/** Draws a debug sphere where the overlapping vertices were traced from, and draws debug points for
	 *  the vertices if they were recalculated */
	void DrawVerticesOverlap(const TSet<USpawnArea*>& InSpawnAreas, const FVector& Scale, const TSet<FVector>& Valid,
		const TSet<FVector>& Invalid) const;

	/** Prints debug info about a SpawnArea */
	static void PrintDebug_SpawnArea(const USpawnArea* SpawnArea);

	/** Prints debug info about SpawnArea distance */
	void PrintDebug_SpawnAreaDist(const USpawnArea* SpawnArea) const;

	/** Prints debug info about GridBlocks */
	static void PrintDebug_GridDFS(const FDFSLoopParams& LoopParams);

	/** Prints debug info about Largest Rectangular area found */
	static void PrintDebug_GridLargestRect(const FLargestRect& LargestRect, const int32 NumCols);

	/** Prints a formatted matrix (upside down from how indexes appear in SpawnAreas so that it matches in game)  */
	static void PrintDebug_Matrix(const TArray<int32>& Matrix, const int32 NumRows, const int32 NumCols);

	/** Toggles showing green debug boxes for valid spawn locations at the end of the GetValidSpawnAreas function */
	bool bDebug_Valid;

	/** Toggles showing red debug boxes for removed spawn due to the BoxBounds */
	bool bDebug_Removed;

	/** Toggles showing turquoise debug boxes for filtered recent SpawnAreas */
	bool bDebug_FilterRecent;

	/** Toggles showing cyan debug boxes for filtered activated SpawnAreas */
	bool bDebug_FilterActivated;

	/** Toggles showing blue debug boxes for filtered managed SpawnAreas */
	bool bDebug_FilterManaged;

	/** Toggles showing yellow debug boxes for filtered bordering SpawnAreas */
	bool bDebug_FilterBordering;

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

	/** Prints various grid-distribution related info to log */
	bool bDebug_Grid;

private:
	/** Whether or not to broadcast the RequestRLCSpawnArea when finding SpawnAreas */
	bool bShouldAskRLCForSpawnAreas;
	
	/** Pointer to TargetManager's BSConfig */
	FBSConfig* BSConfig;

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
	TArray<USpawnArea*> SpawnAreas;

	/** All SpawnArea objects inside the larger total spawn area. Set is filled bottom-up, left-to-right. Does not
	 *  change throughout game mode */
	UPROPERTY()
	TSet<USpawnArea*> AllSpawnAreas;

	/** Maps each location in the SpawnBox to a unique SpawnArea. Does not change throughout game mode */
	UPROPERTY()
	TMap<FAreaKey, USpawnArea*> AreaKeyMap;

	/** Maps each Target Guid to a unique SpawnArea. Added when the SpawnArea is flagged as managed, and removed
	 *  when the managed flag is removed */
	UPROPERTY()
	TMap<FGuid, USpawnArea*> GuidMap;
	
	/** A set of SpawnAreas that fall within the current BoxBounds. All are added initially, updated when the SpawnBox
	 *  extents changes through the OnExtremaChanged function*/
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

	/** Cardinal direction Index types that are valid to use when searching for GridBlocks */
	const TSet<EBorderingDirection> GridBlockIndexTypes = {
		EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up, EBorderingDirection::Down
	};

	/** Up-Down only index types */
	const TSet<EBorderingDirection> VerticalIndexTypes = {EBorderingDirection::Up, EBorderingDirection::Down};

	/** Left-right only index types */
	const TSet<EBorderingDirection> HorizontalIndexTypes = {EBorderingDirection::Left, EBorderingDirection::Right};

	/** All index types */
	const TSet<EBorderingDirection> AllIndexTypes = {
		EBorderingDirection::UpLeft, EBorderingDirection::UpRight, EBorderingDirection::DownLeft,
		EBorderingDirection::DownRight, EBorderingDirection::Left, EBorderingDirection::Right, EBorderingDirection::Up,
		EBorderingDirection::Down
	};

	/** Delegate used to bind a timer handle to RemoveRecentFlagFromSpawnArea() */
	FTimerDelegate RemoveFromRecentDelegate;

	/** Delegate used to request a SpawnArea selection from the RLC */
	FRequestRLCSpawnArea RequestRLCSpawnArea;
};

inline TSet<FFactor> USpawnAreaManagerComponent::FindBestFittingFactors(const int32 Number, const FLargestRect& Rect)
{
	return FindBestFittingFactors(Number, Rect.NumRowsAvailable, Rect.NumColsAvailable);
}