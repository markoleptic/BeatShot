// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "GameModeActorBase.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;

struct FActiveTargetStruct
{
	/** A 2D representation of the area the target spawned. This is stored so that the points can be freed when
	 *  the target expires or is destroyed. */
	TArray<FIntPoint> BlockedSpawnPoints;

	/** A reference to the target that was spawned, so that it can be found and its BlockedSpawnPoints can be freed
	 *  when the target expires or is destroyed. */
	ASphereTarget* ActiveTarget;

	/** The radius of this target*/
	float TargetScale;

	/** The center of this target*/
	FIntPoint Center;

	FActiveTargetStruct()
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		ActiveTarget = nullptr;
		TargetScale = 0.f;
		Center = FIntPoint();
	}

	FActiveTargetStruct(ASphereTarget* TargetToAdd)
	{
		BlockedSpawnPoints = TArray<FIntPoint>();
		ActiveTarget = TargetToAdd;
		TargetScale = 0.f;
		Center = FIntPoint();
	}

	FORCEINLINE bool operator==(const FActiveTargetStruct& Other) const
	{
		if (ActiveTarget == Other.ActiveTarget)
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
	/** The expiration or destruction of any non-BeatTrack target is bound to this function
	 *  to keep track of the streak, timing, and location. The DynamicScaleFactor is also changed
	 *  based on consecutive targets hit */
	UFUNCTION()
	void OnTargetTimeout(bool DidExpire, float TimeAlive, ASphereTarget* DestroyedTarget);

	/** Spawn a SingleBeat on beat */
	void SpawnSingleBeatTarget();

	/** Spawn a MultiBeat on beat */
	void SpawnMultiBeatTarget();

	/** FindNextTargetProperties */
	void FindNextTargetProperties(FVector LastSpawnLocation, float LastTargetScale);
	
	/** Find the next spawn location for a target */
	FVector FindNextTargetSpawnLocation(ESpreadType SpreadType, const float NewTargetScale);

	/** Returns the scale for next target */
	float GenerateTargetScale() const;
	
	/** Find the next spawn location for a target */
	FVector GenerateRandomTargetLocation(ESpreadType SpreadType, const FVector& ScaledBoxExtent) const;

	/** An array of spawned targets that is used to move targets forward towards the player on tick */
	void MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const;

	/** Adds a circle to the 2D array SpawnArea by setting all values that make up the area covered by the target
	 *  equal to 1 (occupied). Returns an array of points containing all values that were changed. */
	TArray<FIntPoint> SetSpawnAreaOccupied(const FIntPoint Center, const float Scale);

	std::vector<std::vector<int32>> SetTempSpawnAreaOccupied(const FIntPoint Center, const float Scale, std::vector<std::vector<int32>>& TempSpawnArea);

	FIntPoint ConvertLocationToPoint(const FVector Location) const;

	FVector ConvertPointToLocation(const FIntPoint Point) const;

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
	
	/** the radius to check for recent spawn target collision */
	float CheckSpawnRadius;

	/** the radius to check for recent spawn target collision */
	int32 MaxNumberOfTargetsAtOnce;

	/** Location to spawn the next/current target */
	FVector SpawnLocation;

	/** The scale to apply to the next/current target */
	float TargetScale;

	/** Recent sphere areas */
	TArray<FSphere> RecentSpawnBounds;

	/** consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** number used to dynamically change spawn area size and target size, if dynamic settings are enabled */
	int32 DynamicScaleFactor;
	
	/** An array of structs where each element holds a reference to the target, the radius, and an array of points
	 *  that the target occupies in 2D space. */
	TArray<FActiveTargetStruct> ActiveTargetArray;

	/** A 2D array representation of the space that the spawn area occupies. Initializes with size equal to twice the
	 *  box bounds. 0's represent available spawn locations, 1's represent occupied spawn locations, and 2's represent
	 *  the box bounds shrinking during dynamic spread types */
	std::vector<std::vector<int32>> SpawnArea;

#pragma endregion

#pragma region BeatTrack

	/** Change the tracking target direction on beat */
	void FindNextTrackingDirection();
	
	/** Function to reverse direction of target if no longer overlapping the SpawnBox */
	UFUNCTION()
	void OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);
	
	/** Only one tracking target spawns, so we store a ref to it to manipulate its behavior */
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
	ASphereTarget* TrackingTarget;
	
	/** Current location of tracking target */
	FVector CurrentTrackerLocation;

	/** Current direction of tracking target */
	FVector TrackingDirection;

	/** Current speed of tracking target */
	float TrackingSpeed;

	/** The end of the path that the tracking target will move to */
	FVector EndLocation;

	/** Location just before randomizing a new tracking direction */
	FVector LocationBeforeDirectionChange;

	/** Randomizes tracking target location */
	FVector GenerateRandomTrackerLocation(const FVector LocationBeforeChange) const;

#pragma endregion
	
#pragma region BeatGrid

	/** Active a BeatGrid target on beat */
	void ActivateBeatGridTarget();

	/** Create BeatGrid Targets */
	void InitBeatGrid();
	
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



