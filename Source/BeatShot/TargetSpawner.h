// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "GameModeActorBase.h"
#include "SphereTarget.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;

USTRUCT()
struct FActiveTargetStruct
{
	GENERATED_BODY()
	
	/** A 2D representation of the area the target spawned. This is stored so that the points can be freed when
	 *  the target expires or is destroyed. */
	TArray<FIntPoint> BlockedSpawnPoints;

	/** A reference to the target that was spawned, so that it can be found and its BlockedSpawnPoints can be freed
	 *  when the target expires or is destroyed. */
	//ASphereTarget* ActiveTarget;

	FGuid TargetGuid;

	/** The scale of this target, as it is in the world */
	float TargetScale;

	/** The center of this target*/
	FIntPoint Center;

	FActiveTargetStruct()
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		//ActiveTarget = nullptr;
		TargetScale = 0.f;
		Center = FIntPoint(-5000, -5000);
	}

	FActiveTargetStruct(const FGuid GuidToRemove)
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		TargetGuid = GuidToRemove;
		TargetScale = 0.f;
		Center = FIntPoint(-5000, -5000);
	}
	
	FActiveTargetStruct(const ASphereTarget* TargetToAdd, const TArray<FIntPoint> BlockedSpawnPointsToAdd,
	                    const float TargetScaleToAdd, const FIntPoint CenterToAdd)
	{
		BlockedSpawnPoints = BlockedSpawnPointsToAdd;
		//ActiveTarget = TargetToAdd;
		TargetGuid = TargetToAdd->Guid;
		TargetScale = TargetScaleToAdd;
		Center = CenterToAdd;
	}

	FORCEINLINE bool operator == (const FActiveTargetStruct& Other) const
	{
		if (TargetGuid == Other.TargetGuid)
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

	/** Adds a target to ActiveTargetArray (used for moving targets forward), and RecentTargetArray
	 *  (used to store location and size of recently spawned targets  */
	void AddTargetToTargetArrays(ASphereTarget* Target, FVector LastSpawnLocation, float LastTargetScale);

	UFUNCTION()
	void RemoveFromRecentTargetArray(const FGuid GuidToRemove);
	
	/** Adds a circle to the 2D array SpawnArea by setting all values that make up the area covered by the target
	 *  equal to 1 (occupied). Returns an array of points containing all values that were changed */
	TArray<FIntPoint> GetCircleOfPoints(const FIntPoint Center, const float Scale) const;

	/** Returns an array of valid spawn points by creating a new 2D array and filling out the values based on the
	 *  contents of RecentTargetArray, the scale of the new target to spawn, and the current BoxExtent */
	TArray<FIntPoint> GetValidSpawnPoints(const float Scale, const FVector& BoxExtent, const bool bIsDynamicSpreadType);
	
	/** Returns a copy of the RecentTargetArray, used to determine future target spawn locations */
	TArray<FActiveTargetStruct> GetRecentTargetArray();

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
	FVector GenerateRandomTargetLocation(ESpreadType SpreadType, const FVector& ScaledBoxExtent) const;

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

	/** Distance between floor and center of Back Wall */
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
	TArray<FActiveTargetStruct> RecentTargetArray;

	/** Scale the 2D representation of the spawn area down by this factor */
	int32 SpawnAreaScale;

	FTimerDelegate RemoveFromRecentDelegate;

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
};
