// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target/SphereTarget.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "SpawnPointManagerComponent.h"
#include "Components/BoxComponent.h"
#include "TargetManager.generated.h"

class ASphereTarget;
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

	/** The 2D spawn area */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnBox;

	/** Three-dimensional spawn area that all targets must fit inside. Used to update boundary boxes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnArea;

	/** All directional boxes act as a boundary for moving targets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> TopBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BottomBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> LeftBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> RightBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> ForwardBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> BackwardBox;

	/** Reinforcement learning agent component */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<UReinforcementLearningComponent> ReinforcementLearningComponent;

	/** Manages spawn points */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USpawnPointManagerComponent> SpawnPointManager;
	
	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ASphereTarget> TargetToSpawn;

	/** Curve to look up values for DynamicSpawnScale */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCurveFloat* DynamicSpawnCurve;

public:
	void ShowDebug_SpawnBox(const bool bShow);
	void ShowDebug_SpawnMemory(const bool bShow);
	void ShowDebug_ReinforcementLearningWidget(const bool bShow);
	void ShowDebug_NumRecentNumActivated() const;

	bool IsDebugActive_SpawnBox() const { return bShowDebug_SpawnBox; }
	bool IsDebugActive_SpawnMemory() const { return bShowDebug_SpawnMemory; }
	bool IsDebugActive_ReinforcementLearningWidget() const { return bShowDebug_ReinforcementLearningWidget; }
	
	/** Called from selected DefaultGameMode */
	void InitTargetManager(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	/** Called from selected DefaultGameMode */
	void SetShouldSpawn(const bool bShouldSpawn);

	/** Called when a player moves the CrossHair off a target. Updates target colors if they are vulnerable to tracking damage */
	UFUNCTION()
	void OnPlayerStopTrackingTarget();

	/** Called from GameMode when it's an appropriate time to spawn or activate a target. This is the main loop that drives this class */
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
	FOnTargetDestroyed OnTargetDeactivated;

	/** Delegate that is executed when a player damages a BeatTrack target. */
	FOnBeatTrackTargetDamaged OnBeatTrackTargetDamaged;

private:
	/** Generic spawn function that all game modes use to spawn a target. Initializes the target, binds to its delegates,
	 *  sets the InSpawnPoint's Guid, and adds the target to ManagedTargets */
	ASphereTarget* SpawnTarget(USpawnPoint* InSpawnPoint);

	/** Executes any Target Activation Responses and calls ActivateTarget on InTarget. Flags SpawnPoint as recent, fires OnActivation delegate,
	 *  and adds to ReinforcementLearningComponent ActiveTargetPairs if active */
	bool ActivateTarget(ASphereTarget* InTarget) const;
	
	/** Tries to spawn a target if there are less targets in ManagedTargets than MaxNumTargetsAtOnce. Also activates the target */
	void HandleRuntimeSpawnAndActivation();

	/** Returns the number of targets that are allowed to be spawned at once, at runtime */
	int32 GetNumberOfRuntimeTargetsToSpawn() const;

	/** Returns the number of targets that are allowed to be activated at once */
	int32 GetNumberOfTargetsToActivate() const;

	/** Activate target(s)/SpawnPoint(s) if there are any ManagedTargets that are not activated. Handles permanent and temporary targets */
	void HandleActivateExistingTargets();

	/** Handles permanently activated targets so they can still receive activation responses, called in HandleActivateExistingTargets */
	void HandlePermanentlyActiveTargetActivation() const;

	/** Handles temporarily activated targets, called in HandleActivateExistingTargets */
	void HandleTemporaryTargetActivation();

	/** Spawns targets at the beginning of a game mode based on the TargetDistributionPolicy */
	void SpawnUpfrontOnlyTargets();

	/** The expiration or destruction of any target is bound to this function, which handles firing delegates, target flags, target removal */
	UFUNCTION()
	void OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent);

	/** Updates ConsecutiveTargetsHit, based on if the target expired or not */
	void UpdateConsecutiveTargetsHit(const float TimeAlive);

	/** Updates DynamicSpawnScale, based on if the target expired or not */
	void UpdateDynamicSpawnScale(const float TimeAlive);

	/** Broadcasts the appropriate delegate based on the damage type */
	void HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const;

	/** Removes from ManagedTargets based if the TargetDestructionConditions permit */
	void HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent);
	
	/** Calls functions to get the next target's location and scale */
	void FindNextTargetProperties();

	/** Returns the scale for next target */
	FVector GetNextTargetScale() const;

	/** Find the next spawn location for a target */
	USpawnPoint* GetNextSpawnPoint(EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const;
	
	/** Randomizes a location to set the BeatTrack target to move towards */
	FVector GetRandomMovingTargetEndLocation(const FVector& LocationBeforeChange, const float TargetSpeed, const bool bLastDirectionChangeHorizontal) const;

	/** Returns an array of valid spawn points, filtering locations from AllSpawnLocations based on the
	 *  TargetDistributionPolicy, BoundsScalingPolicy and if needed, the TargetActivationSelectionPolicy */
	TArray<FVector> GetValidSpawnLocations(const FVector& Scale, const ETargetDistributionPolicy& DistributionPolicy, const EBoundsScalingPolicy& BoundsScalingPolicy) const;

	/** Adds valid spawn locations for an edge-only TargetDistributionPolicy */
	void HandleEdgeOnlySpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema& Extrema, const bool bShowDebug = false) const;

	/** Adds valid spawn locations for a full range TargetDistributionPolicy */
	void HandleFullRangeSpawnLocations(TArray<FVector>& ValidSpawnLocations, const FExtrema &Extrema, const bool bShowDebug = false) const;

	/** Adds valid spawn locations for a grid TargetDistributionPolicy, using TargetActivationSelectionPolicy */
	void HandleGridSpawnLocations(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug = false) const;

	/** Adds/filters valid spawn locations for a Bordering TargetActivationSelectionPolicy */
	void HandleBorderingSelectionPolicy(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug = false) const;

	/** Filters out any locations that correspond to recent points flagged as activated */
	void HandleFilterActivated(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug = false) const;

	/** Filters out any locations that correspond to recent points flagged as recent */
	void HandleFilterRecent(TArray<FVector>& ValidSpawnLocations, const bool bShowDebug = false) const;

	/** Updates the SpawnArea and all directional boxes to match the current SpawnBox */
	void UpdateSpawnArea() const;

	/** Updates the total amount of damage that can be done if a tracking target is damageable */
	void UpdateTotalPossibleDamage();
	
	/** Returns true if a target exists that is vulnerable to tracking damage */
	bool TrackingTargetIsDamageable() const;
	
	/** Returns a copy of all spawn locations that were created on initialization */
	TArray<FVector> GetAllSpawnLocations() const { return AllSpawnLocations; }

	/** Returns a copy of ManagedTargets */
	TArray<TObjectPtr<ASphereTarget>> GetManagedTargets() const { return ManagedTargets; }

	/** Returns the SphereTarget that has the matching Guid, or nullptr if not found in ManagedTargets */
	ASphereTarget* FindManagedTargetByGuid(const FGuid Guid) const;
	
	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any dynamic changes */
	FVector GetBoxExtents_Static() const { return StaticExtents; }

	/** Returns SpawnBox's origin, as it is in the game */
	FVector GetBoxOrigin() const { return SpawnBox->Bounds.Origin; }

	/** Returns a FExtrema struct containing both the min extrema and max extrema */
	FExtrema GetBoxExtrema(const bool bDynamic) const;

	/** Creates the box extrema for a grid target distribution */
	FExtrema GenerateBoxExtremaGrid() const;

	/** Adds a SphereTarget to the ManagedTargets array, and updates the associated SpawnPoint IsCurrentlyManaged flag */
	int32 AddToManagedTargets(ASphereTarget* SpawnTarget, const USpawnPoint* AssociatedSpawnPoint);

	/** Removes the DestroyedTarget from ManagedTargets, and updates its associated SpawnPoint IsCurrentlyManaged flag */
	void RemoveFromManagedTargets(const FGuid GuidToRemove);
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor. This value is snapped to the values of SpawnMemoryScale Y & Z */
	void SetBoxExtents_Dynamic() const;

	/** Function called from BSGameMode any time a player changes settings. Propagates to all targets currently active */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);
	
	/** Peeks & Pops TargetPairs and updates the QTable of the RLAgent if not empty. Returns the next target location based on the index that the RLAgent returned */
	USpawnPoint* TryGetSpawnPointFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const;
	
	/** Initialized at start of game mode by DefaultGameMode */
	FBSConfig BSConfig;

	/** Settings that get updated by DefaultGameMode if they change */
	FPlayerSettings_Game PlayerSettings;

	/** Whether or not the last target spawned in center of spawn area, used for SingleBeat */
	bool LastTargetSpawnedCenter = false;

	/** Whether or not the TargetManager is allowed to spawn a target at a given time */
	bool ShouldSpawn = false;

	/** Whether or not to show Debug SpawnBox outline */
	bool bShowDebug_SpawnBox = false;

	/** Whether or not to show Debug components showing the SpawnMemory */
	bool bShowDebug_SpawnMemory = false;

	/** Whether or not to show the RLAgentWidget */
	bool bShowDebug_ReinforcementLearningWidget = false;

	/** SpawnPoint for the next/current target */
	UPROPERTY()
	USpawnPoint* SpawnPoint;

	/** SpawnPoint for the previous target. Assigned the value of SpawnPoint immediately before the next SpawnPoint is chosen in FindNextTargetProperties */
	UPROPERTY()
	USpawnPoint* PreviousSpawnPoint;

	/** The scale to apply to the next/current target */
	FVector TargetScale;
	
	/** The min and max extrema, set during initialization. This value can be different than current BoxBounds extrema if DynamicSpreadType */
	FExtrema StaticExtrema;

	/** The static extents for the spawn box. This is half the value that was passed in with the FBSConfig */
	FVector StaticExtents;

	/** Consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** The time to use when looking up values from DynamicSpawnCurve */
	int32 DynamicSpawnScale;

	/** An array of spawned SphereTargets that are being actively managed by this class. This is the only place references to spawned targets are stored */
	UPROPERTY()
	TArray<TObjectPtr<ASphereTarget>> ManagedTargets;

	/** All Spawn Locations that were generated on initialization */
	TArray<FVector> AllSpawnLocations;

	/** The total amount of ticks while at least one tracking target was damageable */
	double TotalPossibleDamage;
};
