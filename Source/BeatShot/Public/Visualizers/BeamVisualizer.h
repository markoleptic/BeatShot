// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SimpleBeamLight.h"
#include "VisualizerBase.h"
#include "BeamVisualizer.generated.h"

USTRUCT(BlueprintType)
struct FBeamVisualizerConfig
{
	GENERATED_BODY()
	
	/** The configuration to apply to all SimpleBeamLights this visualizer controls if overriding */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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

	/** Calls the parent implementation and spawns a new array of visualizers given the current AASettings */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings) override;

	/** Initializes a visualizer already in the level */
	virtual void InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings) override;

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
	void SetMovingLights(const bool bMoveLights) { BeamVisualizerConfig.SimpleBeamLightConfig.bIsMovingLight = bMoveLights; }

protected:
	virtual void BeginPlay() override;

	TArray<TObjectPtr<ASimpleBeamLight>>& GetSimpleBeamLights() { return SimpleBeamLights; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visualizer Config")
	TSubclassOf<ASimpleBeamLight> SimpleBeamLightClass;

	/** The configuration for this BeamVisualizer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config")
	FBeamVisualizerConfig BeamVisualizerConfig;
	
	/** Array of spawned beam lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config")
	TArray<TObjectPtr<ASimpleBeamLight>> SimpleBeamLights;

	/** Array of beam light indices that are currently active (Niagara particles are active) */
	TArray<int32> ActiveLightIndices;

	UFUNCTION(CallInEditor)
	void SpawnBeamLight(const FLinearColor& Color, const float InLightDuration);

	UFUNCTION(CallInEditor)
	void AddBeamLightFromWorld(const TSoftObjectPtr<ASimpleBeamLight>& InBeamLight);
};
