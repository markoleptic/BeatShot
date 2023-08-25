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

	/** Initializes the BoxBounds widget */
	void InitBoxBoundsWidget(const TObjectPtr<UBoxBoundsWidget> InBoxBoundsWidget);

	/** Reinitialize the TargetManager by calling Init */
	void RestartSimulation();

	/** Empties ManagedTargets and removes all TargetWidgets from Viewport */
	void FinishSimulation();

	/** Returns the TargetSpawnCD */
	float GetSimulation_TargetSpawnCD() const;

	/** Sets the values of bSimulatePlayerDestroying and DestroyChance */
	void SetSimulatePlayerDestroyingTargets(const bool bInSimulatePlayerDestroyingTargets, const float InDestroyChance = 0.f);

	/** Broadcast when a target is spawned so that a TargetWidget can also be spawned */
	FCreateTargetWidget CreateTargetWidget;

	/** Whether or not to tell spawned targets to artificially destroy themselves early, simulating a player destroying it */
	bool bSimulatePlayerDestroyingTargets = false;

	/** The chance that the target should simulating a player destroying it */
	float DestroyChance = 0.f;

protected:
	/** Generic spawn function that all game modes use to spawn a target. Initializes the target, binds to its delegates,
	 *  sets the InSpawnArea's Guid, and adds the target to ManagedTargets */
	virtual ATarget* SpawnTarget(USpawnArea* InSpawnArea) override;

	/** Updates the SpawnVolume and all directional boxes to match the current SpawnBox */
	virtual void UpdateSpawnVolume() const override;

	UPROPERTY()
	TObjectPtr<UBoxBoundsWidget> BoxBoundsWidget;
};
