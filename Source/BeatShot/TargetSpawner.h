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
	// Sets default values for this actor's properties
	ATargetSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
public:	

	/* Broadcasts when a target has been spawned */
	UPROPERTY(BlueprintAssignable)
		FOnTargetSpawnSignature OnTargetSpawn;

	/* Broadcasts the current streak */
	UPROPERTY(BlueprintAssignable)
		FOnStreakUpdateSignature OnStreakUpdate;

	/* Reference to Game Instance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		UDefaultGameInstance* GI;

	/*
	 * General Spawning functions available for all game modes to use
	 */ 

	/* Called from selected DefaultGameMode */
	UFUNCTION(BlueprintCallable, Category = "Spawn Properties")
		void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	/* Called from selected GameModeActorBase */
	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SetShouldSpawn(bool bShouldSpawn) {ShouldSpawn = bShouldSpawn;}

	/* Called from selected DefaultGameMode */
	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void CallSpawnFunction();

private:

	/*
	* Game mode specific spawn functions
	*/ 

	/* Single Beat */
	void SpawnSingleBeatTarget();
	
	/* Multi Beat */
	void SpawnMultiBeatTarget();

	/* BeatTrack */
	void ChangeTrackingTargetDirection();

	/* BeatGrid */
	void ActivateBeatGridTarget();

	/* Create BeatGrid Targets */
	void InitBeatGrid();

	/*
	 * General Spawning variables available for all game modes to use
	 */
	
	/* Changed by GameModeActorBase */
		bool ShouldSpawn;

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

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		FBoxSphereBounds BoxBounds;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		bool LastTargetSpawnedCenter = false;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		UBoxComponent* SpawnBox;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		TSubclassOf<ASphereTarget> ActorToSpawn;

	/* the radius to check for recent spawn target collision */ 
		float CheckSpawnRadius;
	
	/* the radius to check for recent spawn target collision */ 
		int32 MaxNumberOfTargetsAtOnce;

	/* initial spawn location at center of BoxBounds */ 
		FVector FirstSpawnLocation;

	/* Location to spawn the next/current target */
		FVector SpawnLocation;

	/* Location to refer to the last target spawned */
		FVector LastSpawnLocation;

	/* The scale applied to the most recently spawned target */
		float LastTargetScale;

	/* Max radius for dynamic targets */
		float MaxRadius;

	/* Recent sphere areas */
		TArray<FSphere> RecentSpawnBounds;

	/* consecutively destroyed targets */
		int32 ConsecutiveTargetsHit;

	/* number used to dynamically change spawn area size and target size, if dynamic settings are enabled */
		int32 DynamicScaleFactor;

	/*
	 * BeatTrack Variables and Functions
	 */ 

	/* Current location of tracking target */
		FVector CurrentTrackerLocation;

	/* Current speed of tracking target */
		float TrackingSpeed;
	
	/* Current direction of tracking target */
		FVector TrackingDirection;

	/* Only one tracking target spawns, so we store a ref to it to manipulate its behavior */
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		ASphereTarget* TrackingTarget;
	
	/* The end of the path that the tracking target will move to */
		FVector EndLocation;

	/* Location just before randomizing a new tracking direction */
		FVector LocationBeforeDirectionChange;

	/* Randomizes tracking target location */
		FVector RandomizeTrackerLocation(FVector LocationBeforeChange);

	UFUNCTION()
		void OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	/*
	 * BeatGrid Variables and Functions
	 */

	/* Currently activated beat grid target */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
		ASphereTarget* ActiveBeatGridTarget;

	/* Array to keep track of the grid of targets that do not despawn */
	UPROPERTY(VisibleDefaultsOnly, Category = "BeatGrid")
		TArray<ASphereTarget*> SpawnedBeatGridTargets;

	/* Array to keep track of the recently activated beat grid targets */
		TArray<int32> RecentBeatGridIndices;

	/* Most recently activated beat grid target */
		int32 LastBeatGridIndex;

	/* Keep track of initial beat grid activation */
		bool InitialBeatGridTargetActivated;

	/*
	 * Local spawning functions
	 */

	/* randomize scale of target */
	float RandomizeScale(ASphereTarget* Target);

	/* randomize location of target */
	void RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue);

	/* dynamically randomize location of target */
	void RandomizeDynamicLocation(FVector FLastSpawnLocation, float LastTargetScaleValue);

	/* dynamically change the target scale */
	float ChangeDynamicScale(ASphereTarget* Target);

	/* Bind the expiration or destruction of target to this function */
	UFUNCTION(Category = "Spawn Properties")
		void OnTargetTimeout(bool DidExpire, FVector Location);
};
