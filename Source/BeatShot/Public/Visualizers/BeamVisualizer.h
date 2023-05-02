// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "VisualizerBase.h"
#include "BeamVisualizer.generated.h"

class ASimpleBeamLight;

/** Light Visualizer that displays beams of light. Controls instances of ASimpleBeamLights */
UCLASS()
class BEATSHOT_API ABeamVisualizer : public AVisualizerBase
{
	GENERATED_BODY()

public:
	ABeamVisualizer();

	/** Calls the parent implementation and spawns a new array of visualizers given the current AASettings */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings) override;

	/** Initializes a visualizer already in the level */
	virtual void InitializeVisualizerFromWorld() override;

	/** Activates the matching visualizer from the given index if it isn't already */
	virtual void ActivateVisualizer(const int32 Index) override;

	/** Deactivates all visualizers */
	virtual void DeactivateVisualizers() override;

	/** Called by child beam lights to remove them from ActiveLightIndices */
	UFUNCTION()
	void OnBeamLightLifetimeCompleted(const int32 IndexToRemove);

	/** Does nothing for beam visualizer */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

	/** Sets whether or not lights will be moving */
	void SetMovingLights(const bool bMoveLights) { bMovingLights = bMoveLights; }
	
	UFUNCTION(CallInEditor)
	void SpawnBeamLight(const FLinearColor& Color, const float InLightDuration);

	UFUNCTION(CallInEditor)
	void AddBeamLightFromWorld(const TSoftObjectPtr<ASimpleBeamLight>& InBeamLight);

protected:
	virtual void BeginPlay() override;

	TArray<TObjectPtr<ASimpleBeamLight>>& GetSimpleBeamLights() { return BeamLights; }

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;

	/** The duration of the each light after activation */
	UPROPERTY(EditAnywhere)
	TArray<float> BeamLightLifetimes = DefaultBeamLightLifetimes;

	/** The color for each light */
	UPROPERTY(EditAnywhere)
	TArray<FLinearColor> BeamLightColors = DefaultBeamLightColors;

	/** Whether or not lights will be moving */
	UPROPERTY(EditAnywhere)
	bool bMovingLights = false;

	/** Array of spawned beam lights */
	TArray<TObjectPtr<ASimpleBeamLight>> BeamLights;

	/** Array of beam light indices that are currently active (Niagara particles are active) */
	TArray<int32> ActiveLightIndices;
};
