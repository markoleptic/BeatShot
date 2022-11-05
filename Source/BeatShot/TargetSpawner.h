// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameModeActorStruct.h"
#include "TargetSpawner.generated.h"

class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class ASphereTarget;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetSpawnSignature);

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Broadcasts when a target has been spawned
	UPROPERTY(BlueprintAssignable)
		FOnTargetSpawnSignature OnTargetSpawn;

	// Reference to Game Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		UDefaultGameInstance* GI;

	/*
	 * General Spawning functions available for all game modes to use
	 */ 

	// Called from selected GameModeActor
	UFUNCTION(BlueprintCallable, Category = "Spawn Properties")
		void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SetTargetSpawnCD(float NewTargetSpawnCD);

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		bool GetShouldSpawn();

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SetShouldSpawn(bool bShouldSpawn);

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SpawnActor();

	/*
	 * Game mode specific spawn functions
	 */ 

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SpawnSingleActor();

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void SpawnTracker();

	UFUNCTION(BlueprintCallable, Category = "Spawn Functions")
		void ActivateBeatGridTarget();

private:

	/*
	 * General Spawning variables available for all game modes to use
	 */ 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Properties", meta = (AllowPrivateAccess = true))
		bool ShouldSpawn;

	// Base size of the sphere target
		const float SphereTargetRadius = 50.f;

	// Minimum distance between floor and bottom of the SpawnBox
		const float DistanceFromFloor = 110.f;

	// Distance between floor and center of Back Wall
		const float CenterBackWallHeight = 750.f;

	// Distance between floor and HeadshotHeight
		const float HeadshotHeight = 160.f;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		FBoxSphereBounds BoxBounds;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		bool LastTargetSpawnedCenter = false;

	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
		UBoxComponent* SpawnBox;

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
		TSubclassOf<ASphereTarget> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
		float CheckSpawnRadius;

	UPROPERTY(EditAnywhere, Category = "Spawn Properties")
		float MaxNumberOfTargetsAtOnce;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector FirstSpawnLocation;

	// Location to spawn the next/current target
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector SpawnLocation;

	// Location to refer to the last target spawned
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector LastSpawnLocation;

	// The scale applied to the most recently spawned target
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		float LastTargetScale;

	// Max radius for dynamic targets
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		float MaxRadius;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		TArray<FVector> RecentSpawnLocations;

	// Recent sphere areas
		TArray<FSphere> RecentSpawnBounds;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		int32 ConsecutiveTargetsHit;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		int32 DynamicScaleFactor;

	/*
	 * BeatTrack Variables and Functions
	 */ 

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector CurrentTrackerLocation;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		float TrackingSpeed;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector TrackingDirection;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		ASphereTarget* TrackingTarget;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector EndLocation;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		FVector LocationBeforeDirectionChange;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		bool IsTrackerMoving;

	UFUNCTION()
		FVector RandomizeTrackerLocation(FVector LocationBeforeChange);

	UFUNCTION()
		void OnBeatTrackOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

	/*
	 * BeatGrid Variables and Functions
	 */

	// not using atm
	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		TArray<int32> RecentBeatGridIndices;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		int32 LastBeatGridIndex;

	UPROPERTY(VisibleAnywhere, Category = "Spawn Properties")
		bool InitialBeatSpawned;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid")
		TArray<ASphereTarget*> SpawnedBeatGridTargets;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatGrid")
		ASphereTarget* ActiveBeatGridTarget;

	UFUNCTION(BlueprintCallable, Category = "BeatGrid")
		void InitBeatGrid();

private:

	/*
	 * Local spawning functions
	 */ 

	// not using atm
	UFUNCTION(Category = "Spawn Properties")
		void OnTargetDestroyed(AActor* DestroyedActor);

	UFUNCTION(Category = "Spawn Properties")
		float RandomizeScale(ASphereTarget* Target);

	UFUNCTION(Category = "Spawn Properties")
		void RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue);

	UFUNCTION(Category = "Spawn Properties")
		void RandomizeDynamicLocation(FVector FLastSpawnLocation, float LastTargetScaleValue);

	UFUNCTION(Category = "Spawn Properties")
		float ChangeDynamicScale(ASphereTarget* Target);

	UFUNCTION(Category = "Spawn Properties")
		void OnTargetTimeout(bool DidExpire);
};
