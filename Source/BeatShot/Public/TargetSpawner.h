// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "SphereTarget.h"
#include "SaveGameCustomGameMode.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;
class UMaterialInterface;
class AStaticMeshActor;
class UActorComponent;
class AVisualGrid;

/** A struct representing the space in the grid that a recently spawned target occupies */
USTRUCT()
struct FRecentTargetStruct
{
	GENERATED_BODY()
	
	/** A 2D representation of the area the target spawned. This is stored so that the points can be freed when
	 *  the target expires or is destroyed. */
	TArray<FIntPoint> BlockedSpawnPoints;
	
	/** A unique ID for the target, used to find the target when it comes time to free the blocked points of a target */
	FGuid TargetGuid;

	/** The scale of the target, as it is in the world */
	float TargetScale;

	/** The center of the target*/
	FIntPoint Center;

	FRecentTargetStruct()
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		TargetScale = 0.f;
		Center = FIntPoint(-5000, -5000);
	}

	FRecentTargetStruct(const FGuid GuidToRemove)
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		TargetGuid = GuidToRemove;
		TargetScale = 0.f;
		Center = FIntPoint(-5000, -5000);
	}
	
	FRecentTargetStruct(const FGuid NewTargetGuid, const TArray<FIntPoint> NewBlockedSpawnPoints,
	                    const float NewTargetScale, const FIntPoint NewCenter)
	{
		BlockedSpawnPoints = NewBlockedSpawnPoints;
		TargetGuid = NewTargetGuid;
		TargetScale = NewTargetScale;
		Center = NewCenter;
	}

	FORCEINLINE bool operator == (const FRecentTargetStruct& Other) const
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
struct FGridPoint
{
	GENERATED_BODY()
	
	/** The coordinate for GridPoint, in XY space */
	FIntPoint Point;
	
	/** A counter to increment this point */
	int32 Count;
	
	void Init(const FIntPoint InitPoint)
	{
		Point = InitPoint;
	}

	FGridPoint()
	{
		Point = FIntPoint(0,0);
		Count = 0;
	}

	/** Increment the Count */
	FGridPoint operator++(int)
	{
		FGridPoint Temp = *this;
		++Count;
		return Temp;
	}

	FORCEINLINE bool operator == (const FGridPoint& Other) const
	{
		if (Point == Other.Point)
		{
			return true;
		}
		return false;
	}
};

UCLASS()
class BEATSHOT_API ATargetSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATargetSpawner();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

public:
	/* Called from selected DefaultGameMode */
	void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	/* Called from selected GameModeActorBase */
	void SetShouldSpawn(const bool bShouldSpawn) { ShouldSpawn = bShouldSpawn; }

	/* Called from DefaultGameMode */
	void CallSpawnFunction();

private:
	/** Create BeatGrid Targets */
	void InitBeatGrid();
	
	/** Spawn a MultiBeat on beat */
	void SpawnMultiBeatTarget();
	
	/** Spawn a SingleBeat on beat */
	void SpawnSingleBeatTarget();

	/** Active a BeatGrid target on beat */
	void ActivateBeatGridTarget();

	/** Change the tracking target direction on beat */
	void SetNewTrackingDirection();

	/** The expiration or destruction of any non-BeatTrack target is bound to this function
	*   to keep track of the streak, timing, and location. The DynamicScaleFactor is also changed
	*   based on consecutive targets hit */
	UFUNCTION()
	void OnTargetTimeout(bool DidExpire, float TimeAlive, ASphereTarget* DestroyedTarget);
	
	/** Function to reverse direction of target if no longer overlapping the SpawnBox */
	UFUNCTION()
	void OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);
	
	/** Calls functions to get the next target's location and scale */
	void FindNextTargetProperties();

	/** Returns the scale for next target */
	float GetNextTargetScale() const;

	/** Find the next spawn location for a target */
	FVector GetNextTargetSpawnLocation(ESpreadType SpreadType, const float NewTargetScale);

	/** Randomizes a location to set the BeatTrack target to move towards */
	FVector GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const;

	/** An array of spawned targets that is used to move targets forward towards the player on tick */
	void MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const;

	UFUNCTION()
	void RemoveFromRecentTargetArray(const FGuid GuidToRemove);
	
	/** Returns an array of points containing the area of the target that was spawned */
	TArray<FIntPoint> GetBlockedPoints(const FIntPoint Center, const float Scale) const;

	/** Returns an array of valid spawn points by creating a new 2D array and filling out the values based on the
	 *  contents of RecentTargetArray, the scale of the new target to spawn, and the current BoxExtent */
	TArray<FIntPoint> GetValidSpawnPoints(const float Scale, const FVector& BoxExtent, const bool bIsDynamicSpreadType);
	
	/** Returns a copy of the RecentTargetArray, used to determine future target spawn locations */
	TArray<FRecentTargetStruct> GetRecentTargetArray();

	/** Modifies a circle of points in OutSpawnArea. Returns the modified OutSpawnArea */
	std::vector<std::vector<int32>> ResizeCircleInSpawnArea(const FIntPoint Center, const float Scale,
															std::vector<std::vector<int32>>& OutSpawnArea) const;

	/** Simulates lowering the size of a BoxExtent, but since OutSpawnArea is centered at (0,0), the function
	 *  modifies the outer edges by setting the edge point values equal to 2 (Out of bounds).
	 *  Returns the modified OutSpawnArea */
	std::vector<std::vector<int32>> ResizeSpawnAreaBounds(std::vector<std::vector<int32>>& OutSpawnArea,
														  const FVector& BoxExtent) const;

	/** Returns a point in 2D space with origin at (0,0) given an absolute world location */
	FIntPoint ConvertLocationToPoint(const FVector Location) const;

	/** Returns the absolute world location given a point in 2D space with origin at (0,0) based on the original
	 *  BoxExtent from GameModeActorStruct.BoxBounds */
	FVector ConvertPointToLocation(const FIntPoint Point) const;
	
	/** Returns the absolute world location given a point in 2D space with origin at (0,0) based on the current
	 *  BoxExtent from BoxBounds.BoxExtent */
	FVector ConvertPointToLocationSingleBeat(const FIntPoint Point) const;
	
	/** Find the next spawn location for a target */
	//FVector GenerateRandomTargetLocation(ESpreadType SpreadType, const FVector& ScaledBoxExtent) const;

#pragma region General Spawning Variables

	/** The spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UBoxComponent* SpawnBox;

	/** The bounds of the spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	FBoxSphereBounds BoxBounds;

	/** Whether or not the last target spawned in center of spawn area, used for SingleBeat */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	bool LastTargetSpawnedCenter = false;

	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ASphereTarget> ActorToSpawn;

	/** Changed by GameModeActorBase */
	bool ShouldSpawn;

	/** Whether or not to skip the spawn of this target if a new Target location was not found */
	bool bSkipNextSpawn;

	/** Base size of the sphere target */
	const float SphereTargetRadius = 50.f;

	/** Minimum distance between floor and bottom of the SpawnBox */
	const float DistanceFromFloor = 110.f;

	/** Distance between floor and center of designated Back Wall */
	const float CenterBackWallHeight = 750.f;

	/** Distance between floor and HeadshotHeight */
	const float HeadshotHeight = 160.f;

	/** Initialized at start of game mode by DefaultGameMode */
	FGameModeActorStruct GameModeActorStruct;

	/** Location to spawn the next/current target */
	FVector SpawnLocation;

	/** The scale to apply to the next/current target */
	float TargetScale;

	/** consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** number used to dynamically change spawn area size and target size, if dynamic settings are enabled */
	int32 DynamicScaleFactor;

	/** An array of SphereTargets used to move targets forward after they have spawned (if game mode settings permit).
	 *  Targets get added to this array when they are spawned inside of spawn functions, and removed inside
	 *  OnTargetTimeout */
	UPROPERTY()
	TArray<ASphereTarget*> ActiveTargetArray;

	/** An array of structs where each element holds a reference to the target, the scale, the center point, and an array of points
	 *  Targets get added to this array when they are spawned inside of spawn functions, and removed inside
	 *  OnTargetTimeout */
	UPROPERTY()
	TArray<FRecentTargetStruct> RecentTargetArray;

	/** Scale the 2D representation of the spawn area down by this factor */
	int32 SpawnAreaScale;

	FTimerDelegate RemoveFromRecentDelegate;

	FActorSpawnParameters TargetSpawnParams;

	const FVector StartingSpawnBoxLocation = {3700.f, 0.f, 160.f};

	int32 NumRowsGrid;

	int32 NumColsGrids;
	
	std::vector<std::vector<int32>> SpawnAreaTotals;
	std::vector<std::vector<int32>> SpawnAreaHits;
	
	UPROPERTY()
	TArray<FGridPoint> SpawnAreaTotalsPoints;
	UPROPERTY()
	TArray<FGridPoint> SpawnAreaHitsPoints;

#pragma endregion

#pragma region BeatTrack Variables
	
	/** Only one tracking target spawns, so we store a ref to it to manipulate its behavior */
	UPROPERTY()
	ASphereTarget* BeatTrackTarget;

	/** Current location of tracking target */
	FVector BeatTrackTargetLocation;

	/** Current direction of tracking target */
	FVector BeatTrackTargetDirection;

	/** Current speed of tracking target */
	float BeatTrackTargetSpeed;

	/** The end of the path that the tracking target will move to */
	FVector EndLocation;

	/** Location just before randomizing a new tracking direction */
	FVector LocationBeforeDirectionChange;

#pragma endregion

#pragma region BeatGrid Variables

	/** Currently activated beat grid target */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
	ASphereTarget* ActiveBeatGridTarget;

	/** Array to keep track of the grid of targets that do not de-spawn */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
	TArray<ASphereTarget*> SpawnedBeatGridTargets;

	/** Array to keep track of the recently activated beat grid targets */
	TArray<int32> RecentBeatGridIndices;

	/** Index of the most recently activated beat grid target */
	int32 LastBeatGridIndex;

	/** Whether or not the first BeatGrid target has been activated */
	bool InitialBeatGridTargetActivated;

#pragma endregion

protected:

#pragma region Experimental
	
	UPROPERTY(EditDefaultsOnly)
	AVisualGrid* VisualGrid;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AVisualGrid> VisualGridClass;

#pragma endregion
};
