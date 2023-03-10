// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SphereTarget.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "GameFramework/Actor.h"
#include "BeatShot/BeatShot.h"
#include "TargetSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;
class UMaterialInterface;
class AStaticMeshActor;
class UActorComponent;

DECLARE_DELEGATE_OneParam(FOnBeatTrackDirectionChanged, const FVector);

UCLASS()
class BEATSHOT_API ATargetSpawner : public AActor
{
	GENERATED_BODY()

public:
	ATargetSpawner();

	/** Used to notify DefaultCharacter when a BeatTrack target has changed directions */
	FOnBeatTrackDirectionChanged OnBeatTrackDirectionChanged;

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void ShowDebug_SpawnBox();

	UFUNCTION(CallInEditor)
	void HideDebug_SpawnBox();

	UFUNCTION(CallInEditor)
	void ShowDebug_SpawnMemory();

	UFUNCTION(CallInEditor)
	void HideDebug_SpawnMemory();

public:
	/** Called from selected DefaultGameMode */
	void InitializeGameModeActor(FGameModeActorStruct NewGameModeActor);

	/** Called from selected DefaultGameMode */
	void SetShouldSpawn(const bool bShouldSpawn) { ShouldSpawn = bShouldSpawn; }

	/** Called from DefaultGameMode */
	void CallSpawnFunction();

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	TArray<F2DArray> GetLocationAccuracy();

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

	/** Adds a FRecentTarget to the RecentTargets array */
	int32 AddToRecentTargets(const ASphereTarget* SpawnTarget, const float Scale);

	/** Adds a SphereTarget to the ActiveTargets array */
	int32 AddToActiveTargets(ASphereTarget* SpawnTarget);

	/** Removes the RecentTargetStruct associated with the GuidToRemove from RecentTargets */
	UFUNCTION()
	void RemoveFromRecentTargets(const FGuid GuidToRemove);

	/** Removes the DestroyedTarget from ActiveTargets */
	void RemoveFromActiveTargets(ASphereTarget* SpawnTarget);

	/** Returns an array of valid spawn points */
	TArray<FVector> GetValidSpawnLocations(const float Scale) const;

	/** Returns a copy of the RecentTargets, used to determine future target spawn locations */
	TArray<FRecentTarget> GetRecentTargets() const { return RecentTargets; }

	/** Returns a copy of ActiveTargets, used to move targets forward if game mode permits */
	TArray<ASphereTarget*> GetActiveTargets() const { return ActiveTargets; }

	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any scaling or dynamic changes */
	FVector GetBoxExtents_Unscaled_Static() const;

	/** Returns SpawnBox's current BoxExtents, scaled by SpawnMemoryScaleY and SpawnMemoryScaleZ */
	FVector GetBoxExtents_Scaled_Current() const;

	/** Returns SpawnBox's original BoxExtents, scaled by SpawnMemoryScaleY and SpawnMemoryScaleZ */
	FVector GetBoxExtents_Scaled_Static() const;

	/** Returns SpawnBox's origin, as it is in the game */
	FVector GetBoxOrigin_Unscaled() const;

	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor */
	void SetBoxExtents_Dynamic() const;

	/** Returns an array of scaled down points where the target overlaps the SpawnBox */
	TArray<FVector> GetOverlappingPoints(const FVector Center, const float Scale) const;

	/** Initializes the SpawnCounter array */
	void InitializeSpawnCounter();

	/** Returns a string representation of an accuracy row */
	void AppendStringLocAccRow(const F2DArray Row, FString& StringToWriteTo);

#pragma region General Spawning Variables

	/** The spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UBoxComponent* SpawnBox;

	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ASphereTarget> ActorToSpawn;

	/** The spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCurveFloat* DynamicSpawnCurve;

	/** Initialized at start of game mode by DefaultGameMode */
	FGameModeActorStruct GameModeActorStruct;

	/** Whether or not the last target spawned in center of spawn area, used for SingleBeat */
	bool LastTargetSpawnedCenter = false;

	/** Changed by GameModeActorBase */
	bool ShouldSpawn = false;

	/** Whether or not to skip the spawn of this target if a new Target location was not found */
	bool bSkipNextSpawn = false;

	/** Whether or not to show Debug SpawnBox outline */
	bool bShowDebug_SpawnBox = false;

	/** Whether or not to show Debug components showing the SpawnMemory */
	bool bShowDebug_SpawnMemory = false;

	/** Location to spawn the next/current target */
	FVector SpawnLocation;

	/** Consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** The time to use when looking up values from DynamicSpawnCurve */
	int32 DynamicSpawnScale;

	/** An array of active SphereTargets that have not been destroyed, used to move targets forward */
	UPROPERTY()
	TArray<ASphereTarget*> ActiveTargets;

	/** An array of structs where each element holds a reference to the target, the scale, the center point, and an array of points
	 *  Targets get added to this array when they are spawned inside of spawn functions, and removed inside
	 *  OnTargetTimeout */
	UPROPERTY()
	TArray<FRecentTarget> RecentTargets;

	/** Stores all possible spawn locations and the total spawns & player hits at each location */
	TArray<FVectorCounter> SpawnCounter;

	/** The scale to apply to the next/current target */
	float TargetScale;

	/** Scale the 2D representation of the spawn area down by this factor, Y-axis */
	float SpawnMemoryScaleY;

	/** Scale the 2D representation of the spawn area down by this factor, Z-axis */
	float SpawnMemoryScaleZ;

	/** Delegate used to bind a timer handle to RemoveFromRecentTargets() inside of OnTargetTimeout() */
	FTimerDelegate RemoveFromRecentDelegate;

	/** Spawn parameters for ASphereTarget */
	FActorSpawnParameters TargetSpawnParams;

	/** The default location to spawn the SpawnBox */
	const FVector StartingSpawnBoxLocation = {3700.f, 0.f, 160.f};

	/** Base size of the sphere target */
	const float SphereTargetRadius = 50.f;

	/** Minimum distance between floor and bottom of the SpawnBox */
	const float DistanceFromFloor = 110.f;

	/** Distance between floor and center of designated Back Wall */
	const float CenterBackWallHeight = 750.f;

	/** Distance between floor and HeadshotHeight */
	const float HeadshotHeight = 160.f;

	const FVector MaxBoxExtent = {0, 1600, 500};

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
