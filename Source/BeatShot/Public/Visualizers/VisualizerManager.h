// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "GameFramework/Actor.h"
#include "VisualizerManager.generated.h"

class AVisualizerBase;
class AStaticCubeVisualizer;
class ABeamVisualizer;
class UAudioAnalyzerManager;

UCLASS()
class BEATSHOT_API AVisualizerManager : public AActor
{
	GENERATED_BODY()

public:
	AVisualizerManager();

protected:
	virtual void BeginPlay() override;

public:
	/** Spawns all default visualizers, adds them to their respective arrays, calls InitializeVisualizer on each one */
	void InitializeVisualizers(const FPlayerSettings_Game& PlayerSettings, const FPlayerSettings_AudioAnalyzer& InAASettings);

	/** Calls InitializeVisualizer on any visualizers already in a level and adds them to their respective arrays */
	void InitializeVisualizersFromWorld(const TArray<TSoftObjectPtr<AVisualizerBase>>& InVisualizers);

	/** Returns a normalized spectrum value based on recent spectrum values */
	float GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam);

	/** Main function to update all visualizers, called on tick in GameMode */
	void UpdateVisualizers(const TArray<float>& SpectrumValues);

	/** Updates any Beam Visualizers in BeamVisualizers */
	void UpdateBeamVisualizers(const int32 Index, const float SpectrumValue);

	/** Updates any Cube Visualizers in CubeVisualizers */
	void UpdateCubeVisualizers(const int32 Index, const float SpectrumValue);

	/** Deactivates all visualizers */
	void DeactivateVisualizers();

	/** Updates visualizers based on player game settings */
	void UpdateVisualizerSettings(const FPlayerSettings_Game& PlayerSettings);

	/** Updates visualizers based on player AudioAnalyzer settings */
	void UpdateAASettings(const FPlayerSettings_AudioAnalyzer& NewAASettings);

	/** Marks the render state dirty on any visualizers using instanced static meshes */
	void MarkVisualizerRenderStateDirty();

	UPROPERTY(VisibleAnywhere)
	TArray<float> AvgSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentCubeSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> MaxSpectrumValues;

protected:
	TArray<TObjectPtr<ABeamVisualizer>>& GetBeamVisualizers() { return BeamVisualizers; }
	TArray<TObjectPtr<AStaticCubeVisualizer>>& GetCubeVisualizers() { return CubeVisualizers; }

	/* The base Visualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStaticCubeVisualizer> StaticCubeVisualizerClass;

	/** The BeamVisualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeamVisualizer> BeamVisualizerClass;

	/** The spawned BeamVisualizers */
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<ABeamVisualizer>> BeamVisualizers;

	/** The spawned StaticCubeVisualizers */
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<AStaticCubeVisualizer>> CubeVisualizers;

	/* Locally stored AASettings since they must be accessed frequently in OnTick() */
	UPROPERTY()
	FPlayerSettings_AudioAnalyzer AASettings;

	const FActorSpawnParameters SpawnParameters;

	UPROPERTY(VisibleAnywhere)
	bool bUpdateVisualizers;
};
