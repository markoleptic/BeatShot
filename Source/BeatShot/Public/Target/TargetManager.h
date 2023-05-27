// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target/SphereTarget.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "BeatShot/BeatShot.h"
#include "TargetManager.generated.h"

class ASphereTarget;
class UBoxComponent;
class UReinforcementLearningComponent;

DECLARE_DELEGATE_OneParam(FOnBeatTrackDirectionChanged, const FVector& Vector);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBeatTrackTargetDamaged, const float DamageDelta, const float TotalPossibleDamage);
DECLARE_MULTICAST_DELEGATE(FOnTargetActivated);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated_AimBot, ASphereTarget* Target);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTargetDestroyed, const float TimeAlive, const int32 NewStreak, const FVector& Position);

DECLARE_LOG_CATEGORY_EXTERN(LogTargetManager, Log, All);

/** Class responsible for spawning and managing targets for all game modes */
UCLASS()
class BEATSHOT_API ATargetManager : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class ABSGameMode;

public:
	ATargetManager();

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
	void InitTargetManager(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	/** Called from selected DefaultGameMode */
	void SetShouldSpawn(const bool bShouldSpawn);

	/** Called from GameMode when it's an appropriate time to spawn or activate a target. Takes TargetSpawnCD into account, as well as AudioAnalyzer settings like threshold */
	void OnAudioAnalyzerBeat();

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	TArray<FAccuracyRow> GetLocationAccuracy() const;

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	FCommonScoreInfo GetCommonScoreInfo() const;

	/** Delegate that is executed every time a target has been spawned or activated */
	FOnTargetActivated OnTargetActivatedOrSpawned;

	/** Delegate that is executed every time a target has been spawned, and passes a pointer to that target. Used for AimBot */
	FOnTargetActivated_AimBot OnTargetActivated_AimBot;
	
	/** Used to notify DefaultCharacter when a BeatTrack target has changed directions */
	FOnBeatTrackDirectionChanged OnBeatTrackDirectionChanged;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data. */
	FOnTargetDestroyed OnTargetDestroyed;

	/** Delegate that is executed when a player damages a BeatTrack target. */
	FOnBeatTrackTargetDamaged OnBeatTrackTargetDamaged;

private:
	UFUNCTION()
	void OnGameModeStarted();
	
	/** Initializes the SpawnCounter array */
	FIntPoint InitializeSpawnCounter();
	
	/** Create BeatGrid Targets */
	void InitBeatGrid();

	ASphereTarget* SpawnTarget(const FVector& Location, const FVector& Scale, const bool bBroadcastSpawnEvent = true);

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

	/** Initial BeatTrack target spawn */
	void SpawnChargedTarget();

	/** Change the tracking target direction on beat */
	void UpdateBeatTrackTarget();

	/** Updates the position of the BeatTrack target on tick */
	void OnTick_UpdateTargetLocation(const float DeltaTime);

	/** An array of spawned targets that is used to move targets forward towards the player on tick */
	void MoveTargetForward(ASphereTarget* SpawnTarget, float DeltaTime) const;

	/** The expiration or destruction of any non-BeatTrack target is bound to this function
	*   to keep track of the streak, timing, and location. The DynamicScaleFactor is also changed
	*   based on consecutive targets hit */
	UFUNCTION()
	void OnOnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent);

	/** Called when any actor stops overlapping the OverlapSpawnBox. Used to reverse direction of target when it goes out of bounds */
	UFUNCTION()
	void OnOverlapEnd_OverlapSpawnBox(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Calls functions to get the next target's location and scale */
	void FindNextTargetProperties();

	/** Returns the scale for next target */
	FVector GetNextTargetScale() const;

	/** Find the next spawn location for a target */
	FVector GetNextTargetSpawnLocation(EBoundsScalingMethod BoundsScalingMethod, const FVector& NewTargetScale);

	/** Randomizes a location to set the BeatTrack target to move towards */
	FVector GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const;

	/** Returns an array of valid spawn points */
	TArray<FVector> GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionMethod& TargetDistributionMethod, const EBoundsScalingMethod& BoundsScalingMethod) const;
	
	/** Returns a copy of all spawn locations that were created on initialization */
	TArray<FVector> GetAllSpawnLocations() const;
	
	/** Returns an array of scaled down points where the target overlaps the SpawnBox */
	TArray<FVector> GetOverlappingPoints(const FVector& Center, const FVector& Scale) const;

	/** Returns an array of all points that are occupied by recent targets, readjusted by scale if needed */
	TArray<FVector> GetAllOverlappingPoints(const FVector& Scale) const;

	/** Returns a copy of the RecentTargets, used to determine future target spawn locations */
	TArray<FRecentTarget> GetRecentTargets() const { return RecentTargets; }

	/** Returns a copy of ActiveTargets, used to move targets forward if game mode permits */
	TArray<ASphereTarget*> GetActiveTargets() const { return ActiveTargets; }

	/** Returns a copy of the SpawnCounter */
	TArray<FVectorCounter> GetSpawnCounter() const { return SpawnCounter; }

	/** Returns a copy of ActiveBeatGridGuids */
	TArray<FGuid> GetActiveBeatGridGuids() const { return ActiveBeatGridGuids; }

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

	/** Adds a FRecentTarget to the RecentTargets array */
	int32 AddToRecentTargets(const ASphereTarget* SpawnTarget, const FVector& Scale);

	/** Adds a SphereTarget to the ActiveTargets array */
	int32 AddToActiveTargets(ASphereTarget* SpawnTarget);

	/** Adds a Guid to the ActiveBeatGridGuids array */
	void AddToActiveBeatGridGuids(const FGuid GuidToAdd);

	/** Removes the RecentTargetStruct associated with the GuidToRemove from RecentTargets */
	UFUNCTION()
	void RemoveFromRecentTargets(const FGuid GuidToRemove);

	/** Removes the DestroyedTarget from ActiveTargets */
	void RemoveFromActiveTargets(const FGuid GuidToRemove);

	/** Removes a Guid from the ActiveBeatGridGuids array */
	void RemoveFromActiveBeatGridGuids(const FGuid GuidToRemove);

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

	/** Whether or not the TargetManager is allowed to spawn a target at a given time */
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

	/** The scale to apply to the next/current target */
	FVector TargetScale;

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

	// TODO: Needs to be a distinction between "Active" and spawned/managed & not activated
	/** An array of active SphereTargets that have not been destroyed */
	UPROPERTY()
	TArray<ASphereTarget*> ActiveTargets;

	/** An array of structs where each element holds a reference to the target, the scale, the center point, and an array of points
	 *  Targets get added to this array when they are spawned inside of spawn functions, and removed inside
	 *  OnOnTargetHealthChangedOrExpired */
	TArray<FRecentTarget> RecentTargets;

	/** Stores all possible spawn locations and the total spawns & player hits at each location */
	TArray<FVectorCounter> SpawnCounter;

	/** All Spawn Locations that were generated on initialization */
	TArray<FVector> AllSpawnLocations;
	
	/** Array containing all spawned BeatGrid targets and their indices. Also holds an array of all bordering indices for each index */
	TArray<FBeatGridIndex> BeatGridIndices;

	/** Array to keep track of the recently activated beat grid targets */
	TArray<int32> RecentBeatGridIndices;

	/** Array to keep track of active beat grid targets */
	TArray<FGuid> ActiveBeatGridGuids;

	/** Scale the 2D representation of the spawn area down by this factor, Y-axis */
	float SpawnMemoryScaleY;
	
	/** Scale the 2D representation of the spawn area down by this factor, Z-axis */
	float SpawnMemoryScaleZ;

	/** Minimum overlap radius so that small targets do not overlap due to the spawn memory scale being much higher */
	float MinOverlapRadius;
	
	/** Current speed of tracking target */
	float MovingTargetSpeed;

	/** Delegate used to bind a timer handle to RemoveFromRecentTargets() inside of OnOnTargetHealthChangedOrExpired() */
	FTimerDelegate RemoveFromRecentDelegate;
};
