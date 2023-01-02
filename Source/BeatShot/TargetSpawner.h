// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameFramework/Actor.h"
#include "TargetSpawner.generated.h"

class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class ASphereTarget;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetSpawnSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStreakUpdateSignature, int32, Streak, FVector, Location);

UCLASS()
class BEATSHOT_API ATargetSpawner : public AActor
{
	GENERATED_BODY()

public:
	/* Sets default values for this actor's properties */
	ATargetSpawner();

protected:
	/* Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	/* Called every frame */
	virtual void Tick(float DeltaTime) override;

public:
	/* Broadcasts when a target has been spawned */
	UPROPERTY()
	FOnTargetSpawnSignature OnTargetSpawn;

	/* Broadcasts the current streak */
	UPROPERTY()
	FOnStreakUpdateSignature OnStreakUpdate;

	/* Reference to Game Instance */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	UDefaultGameInstance* GI;
	
	/* Called from selected DefaultGameMode */
	UFUNCTION(BlueprintCallable, Category = "Spawn Properties")
	void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	/* Called from selected GameModeActorBase */
	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
	void SetShouldSpawn(const bool bShouldSpawn) { ShouldSpawn = bShouldSpawn; }

	/* Called from DefaultGameMode */
	void CallSpawnFunction();

private:
	/* The expiration or destruction of any non-BeatTrack target is bound to this function
	 * to keep track of the streak, timing, and location */
	UFUNCTION(Category = "Spawn Properties")
	void OnTargetTimeout(bool DidExpire, float TimeAlive, FVector Location);

	/* Spawn a SingleBeat on beat */
	void SpawnSingleBeatTarget();

	/* Spawn a MultiBeat on beat */
	void SpawnMultiBeatTarget();

	/* FindNextTargetProperties */
	void FindNextTargetProperties(FVector FLastSpawnLocation, float LastTargetScaleValue);
	
	/* Find the next spawn location for a target */
	FVector FindNextTargetSpawnLocation(ESpreadType SpreadType, const float CollisionSphereRadius);

	/* randomize scale of target */
	float GenerateRandomTargetScale() const;
	
	/* Find the next spawn location for a target */
	FVector GenerateRandomTargetLocation(ESpreadType SpreadType, const FVector& ScaledBoxExtent) const;

#pragma region General Spawning Variables

	/* The spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UBoxComponent* SpawnBox;

	/* The bounds of the spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	FBoxSphereBounds BoxBounds;

	/* Whether or not the last target spawned in center of spawn area, used for SingleBeat */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	bool LastTargetSpawnedCenter = false;

	/* The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ASphereTarget> ActorToSpawn;

	/* Changed by GameModeActorBase */
	bool ShouldSpawn;

	/* Whether or not to skip the spawn of this target if a new Target location was not found */
	bool bSkipNextSpawn;

	/* Base size of the sphere target */
	const float SphereTargetRadius = 50.f;

	/* Minimum distance between floor and bottom of the SpawnBox */
	const float DistanceFromFloor = 110.f;

	/* Distance between floor and center of Back Wall */
	const float CenterBackWallHeight = 750.f;

	/* Distance between floor and HeadshotHeight */
	const float HeadshotHeight = 160.f;

	/* Initialized at start of game mode by DefaultGameMode */
	FGameModeActorStruct GameModeActorStruct;
	
	/* the radius to check for recent spawn target collision */
	float CheckSpawnRadius;

	/* the radius to check for recent spawn target collision */
	int32 MaxNumberOfTargetsAtOnce;

	/* Location to spawn the next/current target */
	FVector SpawnLocation;

	/* The scale to apply to the next/current target */
	float TargetScale;

	/* Location to refer to the last target spawned */
	FVector LastSpawnLocation;

	/* The scale applied to the most recently spawned target */
	float LastTargetScale;

	/* Recent sphere areas */
	TArray<FSphere> RecentSpawnBounds;

	/* consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/* number used to dynamically change spawn area size and target size, if dynamic settings are enabled */
	int32 DynamicScaleFactor;

#pragma endregion

#pragma region BeatTrack

	/* Change the tracking target direction on beat */
	void FindNextTrackingDirection();
	
	/* Function to reverse direction of target if no longer overlapping the SpawnBox */
	UFUNCTION()
	void OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);
	
	/* Only one tracking target spawns, so we store a ref to it to manipulate its behavior */
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
	ASphereTarget* TrackingTarget;
	
	/* Current location of tracking target */
	FVector CurrentTrackerLocation;

	/* Current direction of tracking target */
	FVector TrackingDirection;

	/* Current speed of tracking target */
	float TrackingSpeed;

	/* The end of the path that the tracking target will move to */
	FVector EndLocation;

	/* Location just before randomizing a new tracking direction */
	FVector LocationBeforeDirectionChange;

	/* Randomizes tracking target location */
	FVector GenerateRandomTrackerLocation(const FVector LocationBeforeChange) const;

#pragma endregion
	
#pragma region BeatGrid

	/* Active a BeatGrid target on beat */
	void ActivateBeatGridTarget();

	/* Create BeatGrid Targets */
	void InitBeatGrid();
	
	/* Currently activated beat grid target */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
	ASphereTarget* ActiveBeatGridTarget;

	/* Array to keep track of the grid of targets that do not de-spawn */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
	TArray<ASphereTarget*> SpawnedBeatGridTargets;

	/* Array to keep track of the recently activated beat grid targets */
	TArray<int32> RecentBeatGridIndices;

	/* Index of the most recently activated beat grid target */
	int32 LastBeatGridIndex;

	/* Whether or not the first BeatGrid target has been activated */
	bool InitialBeatGridTargetActivated;

#pragma endregion
	
};
