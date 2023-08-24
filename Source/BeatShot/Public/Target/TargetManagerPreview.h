// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TargetManager.h"
#include "GameFramework/Actor.h"
#include "SubMenuWidgets/CustomGameModesWidget/Components/CustomGameModesWidget_Preview.h"
#include "TargetManagerPreview.generated.h"

class UBoxBoundsWidget;

UCLASS()
class BEATSHOT_API ATargetManagerPreview : public ATargetManager
{
	GENERATED_BODY()

public:
	ATargetManagerPreview();

	/** Called from selected DefaultGameMode */
	virtual void Init(const FBSConfig& InBSConfig, const FPlayerSettings_Game& InPlayerSettings) override;
	
	FCreateTargetWidget CreateTargetWidget;
	
	void InitTargetManagerPreview(const TObjectPtr<UBoxBoundsWidget> InTargetWidget, FBSConfig* InBSConfig);

	/** Called from selected DefaultGameMode */
	virtual void SetShouldSpawn(const bool bShouldSpawn) override;

	/** Called when a player moves the CrossHair off a target. Updates target colors if they are vulnerable to tracking damage */
	virtual void OnPlayerStopTrackingTarget() override;

	/** Called from GameMode when it's an appropriate time to spawn or activate a target. This is the main loop that drives this class */
	virtual void OnAudioAnalyzerBeat() override;

protected:
	/** Generic spawn function that all game modes use to spawn a target. Initializes the target, binds to its delegates,
	 *  sets the InSpawnArea's Guid, and adds the target to ManagedTargets */
	virtual ATarget* SpawnTarget(USpawnArea* InSpawnArea) override;

	/** Executes any Target Activation Responses and calls ActivateTarget on InTarget. Flags associated SpawnArea as recent, fires OnActivation delegate,
	 *  and adds to ReinforcementLearningComponent ActiveTargetPairs if active */
	virtual bool ActivateTarget(ATarget* InTarget) const override;
	
	/** Tries to spawn a target if there are less targets in ManagedTargets than MaxNumTargetsAtOnce. Also activates the target */
	virtual void HandleRuntimeSpawnAndActivation() override;

	/** Returns the number of targets that are allowed to be spawned at once, at runtime */
	virtual int32 GetNumberOfRuntimeTargetsToSpawn() const override;

	/** Returns the number of targets that are allowed to be activated at once */
	virtual int32 GetNumberOfTargetsToActivate(const int32 MaxPossibleToActivate) const override;

	/** Activate target(s)/SpawnArea(s) if there are any ManagedTargets that are not activated. Handles permanent and temporary targets */
	virtual void HandleActivateExistingTargets() override;

	/** Handles permanently activated targets so they can still receive activation responses, called in HandleActivateExistingTargets */
	virtual void HandlePermanentlyActiveTargetActivation() const override;

	/** Spawns targets at the beginning of a game mode based on the TargetDistributionPolicy */
	virtual void SpawnUpfrontOnlyTargets() override;

	/** The expiration or destruction of any target is bound to this function, which handles firing delegates, target flags, target removal */
	virtual void OnTargetHealthChangedOrExpired(const FTargetDamageEvent& TargetDamageEvent) override;

	/** Updates ConsecutiveTargetsHit, based on if the target expired or not */
	virtual void UpdateConsecutiveTargetsHit(const float TimeAlive) override;

	/** Updates DynamicSpawnScale, based on if the target expired or not */
	virtual void UpdateDynamicSpawnScale(const float TimeAlive) override;

	/** Broadcasts the appropriate delegate based on the damage type */
	virtual void HandleTargetExpirationDelegate(const ETargetDamageType& DamageType, const FTargetDamageEvent& TargetDamageEvent) const override;

	/** Removes from ManagedTargets based if the TargetDestructionConditions permit */
	virtual void HandleManagedTargetRemoval(const TArray<ETargetDestructionCondition>& TargetDestructionConditions, const FTargetDamageEvent& TargetDamageEvent) override;
	
	/** Calls functions to get the next target's location and scale */
	virtual void FindNextTargetProperties() override;

	/** Returns the scale for next target */
	virtual FVector GetNextTargetScale() const override;

	/** Find the next spawn location for a target */
	virtual USpawnArea* GetNextSpawnArea(EBoundsScalingPolicy BoundsScalingPolicy, const FVector& NewTargetScale) const override;
	
	/** Randomizes a location to set the BeatTrack target to move towards */
	virtual FVector GetRandomMovingTargetEndLocation(const FVector& LocationBeforeChange, const float TargetSpeed, const bool bLastDirectionChangeHorizontal) const override;

	/** Updates the SpawnVolume and all directional boxes to match the current SpawnBox */
	virtual void UpdateSpawnVolume() const override;

	/** Updates the total amount of damage that can be done if a tracking target is damageable */
	virtual void UpdateTotalPossibleDamage() override;
	
	/** Returns true if a target exists that is vulnerable to tracking damage */
	virtual bool TrackingTargetIsDamageable() const override;

	/** Returns the SphereTarget that has the matching Guid, or nullptr if not found in ManagedTargets */
	virtual ATarget* FindManagedTargetByGuid(const FGuid Guid) const override;
	
	/** Returns SpawnBox's BoxExtents as they are in the game, prior to any dynamic changes */
	virtual FVector GetBoxExtents_Static() const override;

	/** Returns SpawnBox's origin, as it is in the game */
	virtual FVector GetBoxOrigin() const override;

	/** Returns a FExtrema struct containing both the min extrema and max extrema */
	virtual FExtrema GetBoxExtrema(const bool bDynamic) const override;

	/** Creates the box extrema for a grid target distribution */
	virtual FExtrema GenerateBoxExtremaGrid() const override;

	/** Adds a SphereTarget to the ManagedTargets array, and updates the associated SpawnArea IsCurrentlyManaged flag */
	virtual int32 AddToManagedTargets(ATarget* SpawnTarget) override;

	/** Removes the DestroyedTarget from ManagedTargets, and updates its associated SpawnArea IsCurrentlyManaged flag */
	virtual void RemoveFromManagedTargets(const FGuid GuidToRemove) override;
	
	/** Sets the SpawnBox's BoxExtents based on the current value of DynamicScaleFactor. This value is snapped to the values of SpawnMemoryScale Y & Z */
	virtual void SetBoxExtents_Dynamic() const override;

	/** Function called from BSGameMode any time a player changes settings. Propagates to all targets currently active */
	virtual void UpdatePlayerSettings(const FPlayerSettings_Game& InPlayerSettings) override;
	
	/** Peeks & Pops TargetPairs and updates the QTable of the RLAgent if not empty. Returns the SpawnArea containing the next target location based on the index that the RLAgent returned */
	virtual USpawnArea* TryGetSpawnAreaFromReinforcementLearningComponent(const TArray<FVector>& OpenLocations) const override;

	FVector2d GetWidgetPositionFromWorldPosition(const FVector& InPosition) const;

	UPROPERTY()
	TObjectPtr<UBoxBoundsWidget> BoxBoundsWidget;

	FBSConfig* ConfigPtr;
};
