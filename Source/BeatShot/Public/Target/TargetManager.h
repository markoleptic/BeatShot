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
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnBeatTrackTargetDamaged, const float DamageDelta, const float TotalPossibleDamage);
DECLARE_MULTICAST_DELEGATE(FOnTargetActivated);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetActivated_AimBot, ATarget* Target);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTargetDestroyed, const float TimeAlive, const int32 NewStreak, const FTransform& Transform);

DECLARE_LOG_CATEGORY_EXTERN(LogTargetManager, Log, All);

/** Class responsible for spawning and managing targets for all game modes */
UCLASS()
class BEATSHOT_API ATargetManager : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class ABSGameMode;

public:
	ATargetManager();

	virtual void Tick(float DeltaTime) override;

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
	TObjectPtr<UReinforcementLearningComponent> ReinforcementLearningComponent;

	/** Manages spawn points */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<USpawnAreaManagerComponent> SpawnAreaManager;
	
	/** The target actor to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	TSubclassOf<ATarget> TargetToSpawn;

	/** Curves to look up values for Dynamic SpawnArea scaling */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCompositeCurveTable* CompositeCurveTable_SpawnArea;

	/** Curves to look up values for Dynamic target scaling */
	UPROPERTY(EditDefaultsOnly, Category = "Spawn Properties")
	UCompositeCurveTable* CompositeCurveTable_TargetScale;

public:
	void ShowDebug_SpawnBox(const bool bShow);
	void ShowDebug_SpawnMemory(const bool bShow);
	void ShowDebug_ReinforcementLearningWidget(const bool bShow);
	void ShowDebug_NumRecentNumActivated() const;
	void ShowDebug_OverlappingVertices(const bool bShow);
	
	/** Called from selected DefaultGameMode */
	void Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings);

	/** Called from MainMenuGameMode */
	virtual void Init(FBSConfig* InBSConfig, const FPlayerSettings_Game& InPlayerSettings);
	
	/** Called from selected DefaultGameMode */
	virtual void SetShouldSpawn(const bool bShouldSpawn);

	/** Called when a player moves the CrossHair off a target. Updates target colors if they are vulnerable to tracking damage */
	UFUNCTION()
	virtual void OnPlayerStopTrackingTarget();

	/** Called from GameMode when it's an appropriate time to spawn or activate a target. This is the main loop that drives this class */
	virtual void OnAudioAnalyzerBeat();

	/** Called from DefaultGameMode, returns the player accuracy matrix */
	FAccuracyData GetLocationAccuracy() const;
	
	/** Saves the QTable inside InCommonScoreInfo */
	void SaveQTable(FCommonScoreInfo& InCommonScoreInfo) const;

	/** Delegate that is executed every time a target has been activated */
	FOnTargetActivated OnTargetActivated;

	/** Delegate that is executed every time a target has been spawned, and passes a pointer to that target. Used for AimBot */
	FOnTargetActivated_AimBot OnTargetActivated_AimBot;
	
	/** Used to notify DefaultCharacter when a BeatTrack target has changed directions */
	FOnBeatTrackDirectionChanged OnBeatTrackDirectionChanged;

	/** Delegate that is executed when a player destroys a target. Passes the time the target was alive as payload data. */
	FOnTargetDestroyed OnTargetDeactivated;

	/** Delegate that is executed when a player damages a BeatTrack target. */
	FOnBeatTrackTargetDamaged OnBeatTrackTargetDamaged;

protected:
	void Init_Internal();

	/** Initializes the Composite Curve Tables */
	void Init_Tables();

	/** Returns the location to spawn the SpawnBox at */
	FVector GenerateSpawnBoxLocation() const;

	/** Returns the actual BoxBounds that the TargetManager sets its 2D BoxBounds to */
	FVector GenerateBoxExtentsStatic() const;
	
	/** Generic spawn function that all game modes use to spawn a target. Initializes the target, binds to its delegates,
	 *  sets the InSpawnArea's Guid, and adds the target to ManagedTargets */
	virtual ATarget* SpawnTarget(USpawnArea* InSpawnArea);

	/** Executes any Target Activation Responses and calls ActivateTarget on InTarget. Flags associated SpawnArea as recent, fires OnActivation delegate,
	 *  and adds to ReinforcementLearningComponent ActiveTargetPairs if active */
	virtual bool ActivateTarget(ATarget* InTarget) const;
	
	/** Tries to spawn a target if there are less targets in ManagedTargets than MaxNumTargetsAtOnce. Also activates the target */
	virtual void HandleRuntimeSpawnAndActivation();

	/** Returns the number of targets that are allowed to be spawned at once, at runtime */
	virtual int32 GetNumberOfRuntimeTargetsToSpawn() const;

	/** Returns the number of targets that are allowed to be activated at once */
	virtual int32 GetNumberOfTargetsToActivate(const int32 MaxPossibleToActivate) const;

	/** Activate target(s)/SpawnArea(s) if there are any ManagedTargets that are not activated. Handles permanent and temporary targets */
	virtual void HandleActivateExistingTargets();

	/** Handles permanently activated targets so they can still receive activation responses, called in HandleActivateExistingTargets */
	virtual void HandlePermanentlyActiveTargetActivation() const;

	/** Spawns targets at the beginning of a game mode based on the TargetDistributionPolicy */
	virtual void SpawnUpfrontOnlyTargets();

	/** The expiration or destruction of any target is bound to this function, which handles firing delegates, target flags, target removal */
	UFUNCTION()
	virtual void OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent);

	/** Updates ConsecutiveTargetsHit, based on if the target expired or not */
	virtual void UpdateConsecutiveTargetsHit(const float TimeAlive);

	/** Updates DynamicLookUpValue_TargetScale and DynamicLookUpValue_SpawnAreaScale, based on if the target expired or not */
	virtual void UpdateDynamicLookUpValues(const float TimeAlive);

	/** Broadcasts the appropriate delegate based on the damage type */
	virtual void HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const;

	/** Removes from ManagedTargets based if the TargetDestructionConditions permit */
	virtual void HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent);
	
	/** Calls functions to get the next target's location and scale */
	virtual void FindNextTargetProperties();

	/** Returns the scale for next target */
	virtual FVector GetNextTargetScale() const;

	/** Find the next spawn location for a target */
	virtual USpawnArea* GetNextSpawnArea(EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const;

	/** Calls GetNewTargetDirection and sets the new direction of the target. Spawn = 0, Activation  = 1, Deactivation = 2 */
	void ChangeTargetDirection(ATarget* InTarget, const uint8 InSpawnActivationDeactivation) const;
	
	/** Returns a new unit vector direction for a target */
	virtual FVector GetNewTargetDirection(const FVector& LocationBeforeChange, const bool bLastDirectionChangeHorizontal) const;

	/** Updates the total amount of damage that can be done if a tracking target is damageable */
	virtual void UpdateTotalPossibleDamage();
	
	/** Returns true if a target exists that is vulnerable to tracking damage */
	virtual bool TrackingTargetIsDamageable() const;

	/** Returns BSConfig pointer */
	virtual FBSConfig* GetBSConfig() const { return BSConfig; }

	/** Returns a copy of ManagedTargets */
	virtual TArray<TObjectPtr<ATarget>> GetManagedTargets() const { return ManagedTargets; }

	/** Returns the SphereTarget that has the matching Guid, or nullptr if not found in ManagedTargets */
	virtual ATarget* FindManagedTargetByGuid(const FGuid Guid) const;
	
	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any dynamic changes */
	virtual FVector GetBoxExtents_Static() const { return StaticExtents; }

	/** Returns max extents that the SpawnBox could have */
	virtual FVector GetBoxExtents_Max() const;

	/** Returns max extrema that the SpawnBox could have */
	virtual FExtrema GetBoxExtrema_Max() const;

	/** Returns SpawnBox's origin, as it is in the game */
	virtual FVector GetBoxOrigin() const { return SpawnBox->Bounds.Origin; }

	/** Returns a FExtrema struct containing both the min extrema and max extrema */
	virtual FExtrema GetBoxExtrema(const bool bDynamic) const;

	/** Creates the box extrema for a grid target distribution */
	virtual FExtrema GenerateBoxExtremaGrid() const;

	/** Adds a SphereTarget to the ManagedTargets array, and updates the associated SpawnArea IsCurrentlyManaged flag */
	virtual int32 AddToManagedTargets(ATarget* SpawnTarget);

	/** Removes the DestroyedTarget from ManagedTargets, and updates its associated SpawnArea IsCurrentlyManaged flag */
	virtual void RemoveFromManagedTargets(const FGuid GuidToRemove);
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor. This value is snapped to the values of SpawnMemoryScale Y & Z */
	virtual void UpdateSpawnBox() const;

	/** Updates the SpawnVolume and all directional boxes to match the current SpawnBox */
	virtual void UpdateSpawnVolume() const;

	/** Function called from BSGameMode any time a player changes settings. Propagates to all targets currently active */
	virtual void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings);
	
	/** Peeks & Pops TargetPairs and updates the QTable of the RLAgent if not empty. Returns the SpawnArea containing the next target location based on the index that the RLAgent returned */
	virtual USpawnArea* TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const;

	/** Bound to ReinforcementLearningComponent's OnSpawnAreaValidityRequest delegate */
	bool OnSpawnAreaValidityRequested(const int32 Index);

	/** Evaluates the specified curve at InTime */
	float GetDynamicValueFromCurveTable(const bool bIsSpawnArea, const int32 InTime) const;
	
	/** Initialized at start of game mode by DefaultGameMode */
	FBSConfig BSConfigLocal;
	FBSConfig* BSConfig;

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

	/** SpawnArea for the next/current target */
	UPROPERTY()
	USpawnArea* CurrentSpawnArea;

	/** SpawnArea for the previous target. Assigned the value of CurrentSpawnArea immediately before the CurrentSpawnArea is chosen in FindNextTargetProperties */
	UPROPERTY()
	mutable USpawnArea* PreviousSpawnArea;

	/** The scale to apply to the next/current target */
	FVector CurrentTargetScale;
	
	/** The min and max extrema, set during initialization. This value can be different than current BoxBounds extrema if DynamicSpreadType */
	FExtrema StaticExtrema;

	/** The static extents for the spawn box. This is half the value that was passed in with the FBSConfig */
	FVector StaticExtents;

	/** Consecutively destroyed targets */
	int32 ConsecutiveTargetsHit;

	/** The time to use when looking up values from CompositeCurveTable_TargetScale. Incremented by for each consecutive target hit, decremented by setting value */
	int32 DynamicLookUpValue_TargetScale;

	/** The time to use when looking up values from CompositeCurveTable_SpawnArea. Incremented by for each consecutive target hit, decremented by setting value  */
	int32 DynamicLookUpValue_SpawnAreaScale;

	/** An array of spawned SphereTargets that are being actively managed by this class. This is the only place references to spawned targets are stored */
	UPROPERTY()
	TArray<TObjectPtr<ATarget>> ManagedTargets;

	/** The total amount of ticks while at least one tracking target was damageable */
	double TotalPossibleDamage;

	/** Whether or not the last activated target direction change was horizontal */
	mutable bool bLastActivatedTargetDirectionChangeHorizontal;

	/** Whether or not the last spawned target direction change was horizontal */
	mutable bool bLastSpawnedTargetDirectionChangeHorizontal;
};
