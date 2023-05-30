// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target/SphereTarget.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "SpawnPointManager.h"
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

	ASphereTarget* SpawnTarget(FSpawnPoint& InSpawnPoint, const bool bBroadcastSpawnEvent = true);
	
	void ActivateTarget();

	/** Initial BeatTrack target spawn */
	void SpawnChargedTarget();

	/** Change the tracking target direction on beat */
	void UpdateBeatTrackTarget();

	/** Updates the position of the BeatTrack target on tick */
	void OnTick_UpdateTargetLocation(const float DeltaTime);

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
	FSpawnPoint* GetNextTargetSpawnLocation(EBoundsScalingPolicy BoundsScalingMethod, const FVector& NewTargetScale);

	/** Randomizes a location to set the BeatTrack target to move towards */
	FVector GetRandomBeatTrackLocation(const FVector& LocationBeforeChange) const;

	/** Returns an array of valid spawn points */
	TArray<FVector> GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionPolicy& TargetDistributionMethod, const EBoundsScalingPolicy& BoundsScalingMethod) const;
	
	/** Returns a copy of all spawn locations that were created on initialization */
	TArray<FVector> GetAllSpawnLocations() const;

	/** Returns a copy of ManagedTargets */
	TArray<ASphereTarget*> GetManagedTargets() const { return ManagedTargets; }

	/** Returns SpawnBox's origin, as it is in the game */
	FVector GetBoxOrigin() const;

	/** Returns a vector representing the BoxBounds extrema. If PositiveExtrema is equal to 1, the positive extrema is returned. Otherwise the negative extrema is returned */
	FVector GetBoxExtrema(const int32 PositiveExtrema, const bool bDynamic) const;
	
	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any dynamic changes */
	FVector GetBoxExtents_Static() const;

	/** Returns an array of directions that contain all directions where the location point does not have an adjacent point in that direction */
	TArray<EBorderingDirection> GetBorderingDirections(const TArray<FVector>& ValidLocations, const FVector& Location) const;

	/** Adds a SphereTarget to the ManagedTargets array */
	int32 AddToManagedTargets(ASphereTarget* SpawnTarget);

	/** Removes the DestroyedTarget from ManagedTargets */
	void RemoveFromManagedTargets(const FGuid GuidToRemove);

	/** Removes points from the InArray that don't have an adjacent point to the top and to the left. Used so that it's safe to spawn a target within a square area */
	void RemoveEdgePoints(TArray<FVector>& In) const;
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor. This value is snapped to the values of SpawnMemoryScale Y & Z */
	void SetBoxExtents_Dynamic() const;

	/** Function called from BSGameMode any time a player changes settings. Propagates to all targets currently active */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);

	UPROPERTY()
	USpawnPointManager* SpawnPointManager;

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

	/** SpawnPoint for the next/current target */
	FSpawnPoint* SpawnPoint;

	/** SpawnPoint for the previous target */
	FSpawnPoint* PreviousSpawnPoint;

	/** The scale to apply to the next/current target */
	FVector TargetScale;
	
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

	/** An array of spawned SphereTargets that are being actively managed by this class. This is the only place references to spawned targets are stored */
	UPROPERTY()
	TArray<ASphereTarget*> ManagedTargets;

	/** All Spawn Locations that were generated on initialization */
	TArray<FVector> AllSpawnLocations;
	
	/** Current speed of tracking target */
	float MovingTargetSpeed;

	/** Delegate used to bind a timer handle to RemoveRecentFlagFromSpawnPoint() inside of OnOnTargetHealthChangedOrExpired() */
	FTimerDelegate RemoveFromRecentDelegate;
};
