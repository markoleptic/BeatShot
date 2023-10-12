// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Target/Target.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "SpawnAreaManagerComponent.h"
#include "Components/BoxComponent.h"
#include "TargetManager.generated.h"

class UCompositeCurveTable;
class ATarget;
class UReinforcementLearningComponent;

DECLARE_DELEGATE_OneParam(FOnBeatTrackDirectionChanged, const FVector& Vector);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated, const ETargetDamageType& DamageType);
DECLARE_MULTICAST_DELEGATE_OneParam(FPostTargetDamageEvent, const FTargetDamageEvent& Event);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated_AimBot, ATarget* Target);

DECLARE_LOG_CATEGORY_EXTERN(LogTargetManager, Log, All);

/** Class responsible for spawning and managing targets for all game modes */
UCLASS()
class BEATSHOT_API ATargetManager : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class ABSGameMode;
	friend class UBSCheatManager;

public:
	ATargetManager();
	virtual void Tick(float DeltaTime) override;

	/** Delegate that is executed every time a target has been activated */
	FOnTargetActivated OnTargetActivated;

	/** Delegate that is executed every time a target has been spawned, and passes a pointer to that target.
	 *  Used for AimBot */
	FOnTargetActivated_AimBot OnTargetActivated_AimBot;

	/** Used to notify DefaultCharacter when a BeatTrack target has changed directions */
	FOnBeatTrackDirectionChanged OnBeatTrackDirectionChanged;

	/** Delegate that is broadcast when a target takes any type of damage */
	FPostTargetDamageEvent PostTargetDamageEvent;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	/** The 2D spawn area */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnBox;

	/** Three-dimensional spawn area that all targets must fit inside. Used to update boundary boxes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> SpawnVolume;

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
	TObjectPtr<UReinforcementLearningComponent> RLComponent;

	/** Manages spawn points */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USpawnAreaManagerComponent> SpawnAreaManager;

	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ATarget> TargetToSpawn;

	/** Curves to look up values for Dynamic SpawnArea scaling */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCompositeCurveTable* CCT_SpawnArea;

	/** Curves to look up values for Dynamic target scaling */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCompositeCurveTable* CCT_TargetScale;

public:
	/** Called from BSGameMode */
	void Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	/** Called from MainMenuGameMode */
	void Init(FBSConfig* InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

protected:
	/** Main initialization function */
	void Init_Internal();

	/** Initializes the Composite Curve Tables */
	void Init_Tables();

public:
	/** Called from BSGameMode */
	void SetShouldSpawn(const bool bShouldSpawn);

	/** Called when a player moves the CrossHair off a target. Updates target colors if they are vulnerable
	 *  to tracking damage */
	UFUNCTION()
	void OnPlayerStopTrackingTarget();

	/** Called from GameMode when it's an appropriate time to spawn or activate a target. This is the main loop
	 *  that drives this class */
	void OnAudioAnalyzerBeat();

protected:
	/** Generic spawn function that all game modes use to spawn a target. Initializes the target, binds to its
	 *  delegates, sets the InSpawnArea's Guid, and adds the target to ManagedTargets */
	virtual ATarget* SpawnTarget(USpawnArea* InSpawnArea);

	/** Adds a Target to the ManagedTargets array, and updates the associated SpawnArea IsCurrentlyManaged flag */
	int32 AddToManagedTargets(ATarget* SpawnTarget);

	/** Executes any Target Activation Responses and calls ActivateTarget on InTarget */
	bool ActivateTarget(ATarget* InTarget) const;

	/** Tries to spawn a target if there are less targets in ManagedTargets than MaxNumTargetsAtOnce */
	void HandleRuntimeSpawning();

	/** Spawns targets at the beginning of a game mode based on the TargetDistributionPolicy */
	void HandleUpfrontSpawning();

	/** Activate target(s)/SpawnArea(s) if there are any ManagedTargets that are not activated. Handles permanent
	 *  and temporary targets */
	void HandleTargetActivation();

	/** Handles permanently activated targets so they can still receive activation responses, called in
	 *  HandleTargetActivation */
	void HandlePermanentlyActiveTargetActivation() const;

	/** Returns the number of targets that are allowed to be spawned at once, at runtime */
	int32 GetNumberOfRuntimeTargetsToSpawn() const;

	/** Returns the number of targets that are allowed to be activated at once. Will only return values >= 0 */
	int32 GetNumberOfTargetsToActivateAtOnce(const int32 MaxPossibleToActivate) const;

	/** Calls functions to get the next target's location and scale */
	void FindNextTargetProperties();

	/** Returns the scale for next target */
	FVector FindNextTargetScale() const;

	/** Find the next spawn location for a target */
	USpawnArea* FindNextSpawnArea(const FVector& NewTargetScale) const;

	ETargetDamageType FindNextTargetDamageType();

	/** Peeks & Pops TargetPairs and updates the QTable of the RLAgent if not empty. Returns the SpawnArea containing
	 *  the next target location based on the index that the RLAgent returned */
	USpawnArea* TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const;

	/** The expiration or destruction of any target is bound to this function, which handles firing delegates,
	 *  target flags, target removal */
	UFUNCTION()
	void OnTargetDamageEvent(FTargetDamageEvent Event);

	/** Updates CurrentStreak, based on if the target expired or not */
	void UpdateCurrentStreak(const FTargetDamageEvent& Event);

	/** Updates DynamicLookUpValue_TargetScale and DynamicLookUpValue_SpawnAreaScale,
	 *  based on if the target expired or not */
	void UpdateDynamicLookUpValues(const FTargetDamageEvent& Event);

	/** Removes from ManagedTargets based if the TargetDestructionConditions permit */
	void HandleManagedTargetRemoval(const FTargetDamageEvent& Event);

	/** Removes the DestroyedTarget from ManagedTargets, and updates its associated SpawnArea IsCurrentlyManaged flag */
	void RemoveFromManagedTargets(const FGuid GuidToRemove);

	/** Returns the static location to place the SpawnBox */
	FVector GenerateStaticLocation() const;
	
	/** Returns the ending extents for a game mode. If dynamic, this is the extents that the curves lerp towards from
	 *  StartExtents */
	FVector GenerateStaticExtents() const;
	
	/** Returns the absolute minimum and maximum corners of the spawn volume, based on the Static Extents and SpawnBox
	 *  origin. */
	FExtrema GenerateStaticExtrema() const;

	/** Returns the location to place the SpawnVolume based on the Factor if dynamic or the StaticExtents otherwise.
	 *  X and Y will be the same as the SpawnBox's location */
	FVector GenerateSpawnVolumeLocation(const float Factor = 1.f) const;

	/** Returns the extents to apply to the SpawnVolume based on the Factor if dynamic or the StaticExtents otherwise.
	 *  X and Y will be the same as the SpawnBox's extents */
	FVector GenerateSpawnVolumeExtents(const float Factor = 1.f) const;

	/** Returns SpawnVolume's Location */
	FVector GetSpawnVolumeLocation() const;
	
	/** Returns SpawnVolume's BoxExtents */
	FVector GetSpawnVolumeExtents() const;
	
	/** Returns a FExtrema struct containing both the min extrema and max extrema of the SpawnVolume */
	FExtrema GetSpawnVolumeExtrema() const;

	/** Returns SpawnBox's origin. X will always be 3700 */
	FVector GetSpawnBoxOrigin() const;
	
	/** Returns SpawnBox's current BoxExtents. X will always be 0 */
	FVector GetSpawnBoxExtents() const;
	
	/** Returns the current min and max extrema for the SpawnBox. X will always be 0 */
	FExtrema GetSpawnBoxExtrema() const;
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor */
	void UpdateSpawnBoxExtents(const float Factor) const;

	/** Updates the SpawnVolume and all directional boxes to match the current SpawnBox */
	virtual void UpdateSpawnVolume(const float Factor) const;

	/** Calls GetNewTargetDirection and sets the new direction of the target.
	 *  Spawn = 0, Activation  = 1, Deactivation = 2 */
	void ChangeTargetDirection(ATarget* InTarget, const uint8 InSpawnActivationDeactivation) const;

	/** Returns a new unit vector direction for a target */
	FVector GetNewTargetDirection(const FVector& LocationBeforeChange, const bool bLastDirectionChangeHorizontal) const;

	/** Updates the total amount of damage that can be done if a tracking target is damageable */
	void UpdateTotalPossibleDamage();

	/** Returns true if a target exists that is vulnerable to tracking damage */
	bool TrackingTargetIsDamageable() const;

	/** Returns BSConfig pointer */
	FBSConfig* GetBSConfig() const { return BSConfig; }

	/** Returns a copy of ManagedTargets */
	TArray<TObjectPtr<ATarget>> GetManagedTargets() const { return ManagedTargets; }

	/** Returns the SphereTarget that has the matching Guid, or nullptr if not found in ManagedTargets */
	ATarget* FindManagedTargetByGuid(const FGuid Guid) const;

	/** Evaluates the specified curve at InTime */
	float GetCurveTableValue(const bool bIsSpawnArea, const int32 InTime) const;

	/** Function called from BSGameMode any time a player changes settings.
	 *  Propagates to all targets currently active */
	void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);

public:
	/** Called from DefaultGameMode, returns the player accuracy matrix */
	FAccuracyData GetLocationAccuracy() const;

	/** Saves the QTable inside InCommonScoreInfo */
	void SaveQTable(FCommonScoreInfo& InCommonScoreInfo) const;

	/** Prints the number of activated, recent, and managed targets to log */
	void PrintDebug_NumRecentNumActive();

protected:
	/** Initialized at start of game mode by DefaultGameMode */
	FBSConfig BSConfigLocal;
	FBSConfig* BSConfig;

	/** Settings that get updated by DefaultGameMode if they change */
	FPlayerSettings_Game PlayerSettings;

	/** Whether or not the TargetManager is allowed to spawn a target at a given time */
	bool ShouldSpawn;

	/** Whether or not to print NumRecent NumActive */
	bool bPrintDebug_NumRecentNumActive;

	/** Whether or not to print information about each activated Spawn Area */
	bool bPrintDebug_SpawnAreaInfo;

	/** Whether or not to print information about Spawn Areas spawning targets too close together */
	bool bPrintDebug_SpawnAreaDistance;

	/** SpawnArea for the next/current target */
	UPROPERTY()
	USpawnArea* CurrentSpawnArea;

	/** SpawnArea for the previous target. Used for RLC */
	UPROPERTY()
	mutable USpawnArea* PreviousSpawnArea;

	/** The type of damage that the last target was vulnerable to */
	ETargetDamageType LastTargetDamageType;

	mutable FGuid PreviousTargetGuid;

	/** The scale to apply to the next/current target */
	FVector CurrentTargetScale;

	/** The min and max extrema, set during initialization. This value can be different than current
	 *  BoxBounds extrema if DynamicSpreadType */
	FExtrema StaticExtrema;

	/** The static extents for the spawn box. This is half the value that was passed in with the FBSConfig */
	FVector StaticExtents;

	/** Consecutively destroyed targets */
	int32 CurrentStreak;

	/** The time to use when looking up values from CCT_TargetScale. Incremented by for each consecutive target hit,
	 *  decremented by setting value */
	int32 DynamicLookUpValue_TargetScale;

	/** The time to use when looking up values from CCT_SpawnArea. Incremented by for each consecutive target hit,
	 *  decremented by setting value  */
	int32 DynamicLookUpValue_SpawnAreaScale;

	/** An array of spawned Targets that are being actively managed by this class. This is the only place where
	 *  references to spawned targets are stored */
	UPROPERTY()
	TArray<TObjectPtr<ATarget>> ManagedTargets;

	/** The total amount of ticks while at least one tracking target was damageable */
	double TotalPossibleDamage;

	/** Whether or not the last activated target direction change was horizontal */
	mutable bool bLastActivatedTargetDirectionChangeHorizontal;

	/** Whether or not the last spawned target direction change was horizontal */
	mutable bool bLastSpawnedTargetDirectionChangeHorizontal;
};
