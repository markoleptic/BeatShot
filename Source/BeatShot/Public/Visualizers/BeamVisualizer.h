// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimpleBeamLight.h"
#include "VisualizerBase.h"
#include "BeamVisualizer.generated.h"

USTRUCT(BlueprintType, Category = "Visualizer Config")
struct BEATSHOT_API FBeamVisualizerConfig
{
	GENERATED_BODY()
	
	/** The SimpleBeamLight Class */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visualizer Config | Simple Beam Light Config", meta=(DisplayPriority=-5000))
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;

	/** Array of lights that have already been placed in a level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Simple Beam Light Config", meta=(DisplayName="Add lights from a level", DisplayPriority=-1150, EditCondition="bSpawnVisualizerLights==false"))
	TArray<TObjectPtr<ASimpleBeamLight>> SimpleBeamLights;

	/** The configuration to apply to all SimpleBeamLights this visualizer controls if overriding */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Simple Beam Light Config", meta=(DisplayPriority=-99, EditCondition="bOverrideChildLightConfig", EditConditionHides))
	FSimpleBeamLightConfig SimpleBeamLightConfig;
	
	FBeamVisualizerConfig()
	{
		SimpleBeamLightConfig = FSimpleBeamLightConfig();
	}
};

/** Light Visualizer that displays beams of light. Controls instances of ASimpleBeamLights */
UCLASS()
class BEATSHOT_API ABeamVisualizer : public AVisualizerBase
{
	GENERATED_BODY()

public:
	ABeamVisualizer();

	FBeamVisualizerConfig& GetBeamVisualizerConfig() { return BeamVisualizerConfig; }

	/** Calls the parent implementation and spawns a new array of visualizers given the current AASettings */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings) override;

	/** Initializes a visualizer already in the level */
	virtual void InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings, const int32 NumSpawnedVisualizers = INDEX_NONE) override;

	/** Activates the matching visualizer from the given index if it isn't already */
	virtual void ActivateVisualizer(const int32 Index) override;

	/** Deactivates all visualizers */
	virtual void DeactivateVisualizers() override;

	/** Called by child beam lights to remove them from ActiveLightIndices */
	UFUNCTION()
	void OnBeamLightLifetimeCompleted(const int32 IndexToRemove);

	/** Does nothing for beam visualizer */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha) override;

protected:

	TArray<TObjectPtr<ASimpleBeamLight>>& GetSimpleBeamLights() { return BeamVisualizerConfig.SimpleBeamLights; }
	
	/** The configuration for this BeamVisualizer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config", meta=(DisplayPriority=-10000))
	FBeamVisualizerConfig BeamVisualizerConfig;
	
	/** Array of beam light indices that are currently active (Niagara particles are active) */
	TArray<int32> ActiveLightIndices;
};
