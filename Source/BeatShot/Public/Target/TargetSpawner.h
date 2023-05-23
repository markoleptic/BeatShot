// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target/SphereTarget.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "BeatShot/BeatShot.h"
#include "TargetSpawner.generated.h"

class ASphereTarget;
class UBoxComponent;
class UReinforcementLearningComponent;

DECLARE_DELEGATE_OneParam(FOnBeatTrackDirectionChanged, const FVector& Vector);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnBeatTrackTargetDamaged, const float OldValue, const float NewValue, const float TotalPossibleDamage);
DECLARE_MULTICAST_DELEGATE(FOnTargetSpawned);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetSpawned_AimBot, ASphereTarget* Target);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTargetDestroyed, const float TimeAlive, const int32 NewStreak, const FVector& Position);

DECLARE_LOG_CATEGORY_EXTERN(LogTargetSpawner, Log, All);

/** Class responsible for spawning and managing targets for all game modes */
UCLASS()
class BEATSHOT_API ATargetSpawner : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class ABSGameMode;

public:
	ATargetSpawner();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

	/** The spawn area */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBoxComponent* SpawnBox;

	/** The spawn area for BeatTrack, used to reverse the direction if the BeatTrack target stops overlapping it */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UBoxComponent* OverlapSpawnBox;

	/** Reinforcement learning agent component */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UReinforcementLearningComponent* ReinforcementLearningComponent;

	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ASphereTarget> ActorToSpawn;

	/** Curve to look up values for DynamicSpawnScale */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCurveFloat* DynamicSpawnCurve;

public:
	void ShowDebug_SpawnBox(const bool bShow);
	void ShowDebug_SpawnMemory(const bool bShow);
	void ShowDebug_ReinforcementLearningWidget(const bool bShow);

	bool IsDebugActive_SpawnBox() const { return bShowDebug_SpawnBox; }
	bool IsDebugActive_SpawnMemory() const { return bShowDebug_SpawnMemory; }
	bool IsDebugActive_ReinforcementLearningWidget() const { return bShowDebug_ReinforcementLearningWidget; }
	
	/** Called from selected DefaultGameMode */
	void InitTargetSpawner(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	/** Called from selected DefaultGameMode */
	void SetShouldSpawn(const bool bShouldSpawn);

	/** Called from DefaultGameMode */
	void CallSpawnFunction();

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	TArray<FAccuracyRow> GetLocationAccuracy() const;

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	FCommonScoreInfo GetCommonScoreInfo() const;

	/** Delegate that is executed every time a target has been spawned. */
	FOnTargetSpawned OnTargetSpawned;

	/** Delegate that is executed every time a target has been spawned, and passes a pointer to that target. Used for AimBot */
	FOnTargetSpawned_AimBot OnTargetSpawned_AimBot;
	
	/** Used to notify DefaultCharacter when a BeatTrack target has changed directions */
	FOnBeatTrackDirectionChanged OnBeatTrackDirectionChanged;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data. */
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when a player damages a BeatTrack target. */
	FOnBeatTrackTargetDamaged OnBeatTrackTargetDamaged;

private:
	/** Initializes the SpawnCounter array */
	FIntPoint InitializeSpawnCounter();
	
	/** Create BeatGrid Targets */
	void InitBeatGrid();

	/** Spawn a MultiBeat on beat */
	void SpawnMultiBeatTarget();

	/** Spawn a SingleBeat on beat */
	void SpawnSingleBeatTarget();

	/** Active a BeatGrid target on beat */
	void ActivateBeatGridTarget();

	/** Active a target with multiple charges */
	void ActivateChargedTarget();

	/** Initial BeatTrack target spawn */
	void SpawnBeatTrackTarget();

	/** Change the tracking target direction on beat */
	void SetNewTrackingDirection();

	/** Updates the position of the BeatTrack target on tick */
	void OnTick_UpdateBeatTrackTargetLocation(const float DeltaTime);

	/** The expiration or destruction of any non-BeatTrack target is bound to this function
	*   to keep track of the streak, timing, and location. The DynamicScaleFactor is also changed
	*   based on consecutive targets hit */
	UFUNCTION()
	void OnTargetTimeout(bool DidExpire, float TimeAlive, ASphereTarget* DestroyedTarget);

	/** Broadcasts OnBeatTrackTargetDamaged when a BeatTrack target was damaged by a player */
	UFUNCTION()
	void OnBeatTrackTargetHealthChanged(AActor* ActorInstigator, const float OldValue, const float NewValue, const float TotalPossibleDamage);

	/** Function to reverse direction of target if no longer overlapping the SpawnBox */
	UFUNCTION()
	void OnBeatTrackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Calls functions to get the next target's location and scale */
	void FindNextTargetProperties();

	/** Returns the scale for next target */
	float GetNextTargetScale() const;

	/** Find the next spawn location for a target */
	FVector GetNextTargetSpawnLocation(EBoundsScalingMethod BoundsScalingMethod, const float NewTargetScale);

	/** Randomizes a location to set the BeatTrack target to move towards */
	FVector GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const;

	/** Returns an array of valid spawn points */
	TArray<FVector> GetValidSpawnLocations(const float Scale, const ETargetDistributionMethod& TargetDistributionMethod, const EBoundsScalingMethod& BoundsScalingMethod) const;
	
	/** Returns a copy of all spawn locations that were created on initialization */
	TArray<FVector> GetAllSpawnLocations() const;
	
	/** Returns an array of scaled down points where the target overlaps the SpawnBox */
	TArray<FVector> GetOverlappingPoints(const FVector& Center, const float Scale) const;

	/** Returns an array of all points that are occupied by recent targets, readjusted by scale if needed */
	TArray<FVector> GetAllOverlappingPoints(const float Scale) const;

	/** Returns a copy of the RecentTargets, used to determine future target spawn locations */
	TArray<FRecentTarget> GetRecentTargets() const { return RecentTargets; }

	/** Returns a copy of ActiveTargets, used to move targets forward if game mode permits */
	TArray<ASphereTarget*> GetActiveTargets() const { return ActiveTargets; }

	/** Returns a copy of the SpawnCounter */
	TArray<FVectorCounter> GetSpawnCounter() const { return SpawnCounter; }

	/** Returns SpawnBox's origin, as it is in the game */
	FVector GetBoxOrigin() const;

	/** Returns a vector representing the BoxBounds extrema. If PositiveExtrema is equal to 1, the positive extrema is returned. Otherwise the negative extrema is returned */
	FVector GetBoxExtrema(const int32 PositiveExtrema, const bool bDynamic) const;
	
	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any dynamic changes */
	FVector GetBoxExtents_Static() const;

	/** Returns an array of directions that contain all directions where the location point does not have an adjacent point in that direction */
	TArray<EBorderingDirection> GetBorderingDirections(const TArray<FVector>& ValidLocations, const FVector& Location) const;

	/** Returns the VectorCounter corresponding to the point */
	FVectorCounter GetVectorCounterFromPoint(const FVector& Point) const;
	
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

	/** Removes points from the InArray that don't have an adjacent point to the top and to the left. Used so that it's safe to spawn a target within a square area */
	void RemoveEdgePoints(TArray<FVector>& In) const;
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor. This value is snapped to the values of SpawnMemoryScale Y & Z */
	void SetBoxExtents_Dynamic() const;

	/** Function called from BSGameMode any time a player changes settings. Propagates to all targets currently active */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);

	/** Converts a SpawnCounter index to a 5x5 index that can be passed to CommonScoreInfo or LocationAccuracy */
	int32 GetOutArrayIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const;

	/** Peeks & Pops TargetPairs and updates the QTable of the RLAgent if not empty. Returns the next target location based on the index that the RLAgent returned */
	int32 TryGetSpawnLocationFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const;

	/** Initialized at start of game mode by DefaultGameMode */
	FBSConfig BSConfig;

	/** Settings that get updated by DefaultGameMode if they change */
	FPlayerSettings_Game PlayerSettings;

	/** Whether or not the last target spawned in center of spawn area, used for SingleBeat */
	bool LastTargetSpawnedCenter = false;

	/** Whether or not the TargetSpawner is allowed to spawn a target at a given time */
	bool ShouldSpawn = false;

	/** Whether or not to skip the spawn of this target if a new Target location was not found */
	bool bSkipNextSpawn = false;

	/** Whether or not to show Debug SpawnBox outline */
	bool bShowDebug_SpawnBox = false;

	/** Whether or not to show Debug components showing the SpawnMemory */
	bool bShowDebug_SpawnMemory = false;

	/** Whether or not to show the RLAgentWidget */
	bool bShowDebug_ReinforcementLearningWidget = false;

	/** Location to spawn the next/current target */
	FVector SpawnLocation;

	/** Location of the previously spawned target, in world space */
	FVector PreviousSpawnLocation;

	/** The smallest possible extrema, set during initialization. This value can be different than current BoxBounds extrema if DynamicSpreadType */
	FVector StaticMinExtrema;

	/** The largest possible extrema, set during initialization. This value can be different than current BoxBounds extrema if DynamicSpreadType */
	FVector StaticMaxExtrema;

	/** The static extents for the spawn box. This is half the value that was passed in with the FBSConfig */
	FVector StaticExtents;

	/** Current location of tracking target */
	FVector CurrentMovingTargetLocation;

	/** Current direction of tracking target */
	FVector CurrentMovingTargetDirection;

	/** The end of the path that the tracking target will move to */
	FVector EndLocation;

	/** Location just before randomizing a new tracking direction */
	FVector LocationBeforeDirectionChange;

	/** Consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** The time to use when looking up values from DynamicSpawnCurve */
	int32 DynamicSpawnScale;

	/** Incremental step value used to iterate through SpawnCounter locations */
	int32 SpawnMemoryIncY;
	
	/** Incremental step value used to iterate through SpawnCounter locations */
	int32 SpawnMemoryIncZ;

	/** The number of columns in the SpawnCounter */
	int32 SpawnCounterHeight;

	/** The number of rows in the SpawnCounter */
	int32 SpawnCounterWidth;

	/** Index of the most recently activated beat grid target */
	int32 LastBeatGridIndex;

	/** An array of active SphereTargets that have not been destroyed, used to move targets forward */
	UPROPERTY()
	TArray<ASphereTarget*> ActiveTargets;

	/** Array to keep track of the grid of targets that do not de-spawn */
	UPROPERTY()
	TArray<ASphereTarget*> SpawnedBeatGridTargets;

	/** An array of structs where each element holds a reference to the target, the scale, the center point, and an array of points
	 *  Targets get added to this array when they are spawned inside of spawn functions, and removed inside
	 *  OnTargetTimeout */
	TArray<FRecentTarget> RecentTargets;

	/** Stores all possible spawn locations and the total spawns & player hits at each location */
	TArray<FVectorCounter> SpawnCounter;

	/** All Spawn Locations that were generated on initialization */
	TArray<FVector> AllSpawnLocations;
	
	/** Array containing all spawned BeatGrid targets and their indices. Also holds an array of all bordering indices for each index */
	TArray<FBeatGridIndex> BeatGridIndices;

	/** Array to keep track of the recently activated beat grid targets */
	TArray<int32> RecentBeatGridIndices;
	
	/** The scale to apply to the next/current target */
	float TargetScale;

	/** Scale the 2D representation of the spawn area down by this factor, Y-axis */
	float SpawnMemoryScaleY;
	
	/** Scale the 2D representation of the spawn area down by this factor, Z-axis */
	float SpawnMemoryScaleZ;

	/** Minimum overlap radius so that small targets do not overlap due to the spawn memory scale being much higher */
	float MinOverlapRadius;
	
	/** Current speed of tracking target */
	float MovingTargetSpeed;

	/** Delegate used to bind a timer handle to RemoveFromRecentTargets() inside of OnTargetTimeout() */
	FTimerDelegate RemoveFromRecentDelegate;
};
