// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class USceneComponent;
class UAudioAnalyzerManager;

/** Base class for Light Visualizers. BeamVisualizer and StaticCubeVisualizer are examples of classes that subclass this one */
UCLASS()
class BEATSHOT_API AVisualizerBase : public AActor
{
	GENERATED_BODY()

public:
	AVisualizerBase();

protected:

	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

public:
	/** Copies the new AudioAnalyzer Settings. Overrides should spawn their specific visualizer implementations. */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings);

	/** Initializes a visualizer already in the level. Called by the GameMode when the level gives it references */
	virtual void InitializeVisualizerFromWorld();

	/** Activates the matching visualizer from the given index */
	virtual void ActivateVisualizer(const int32 Index);
	
	/** Deactivates all visualizers */
	virtual void DeactivateVisualizers();

	/** Updates a visualizer state at Index inside Visualizers array. SpectrumAlpha should be a value between
	 *  0 and 1, with 1 being the maximum visualizer state and 0 being the minimum */
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha);

	/** Called by the VisualizerManager when AudioAnalyzer settings are changed. Calls InitializeVisualizer */
	virtual void UpdateAASettings(const FPlayerSettings_AudioAnalyzer& InAASettings);

	/** If the visualizer is using an instanced static mesh, use this function to rerender the meshes. Should be called by VisualizerManager */
	virtual void MarkRenderStateDirty();
	
	/** Sets the initial visualizer spawn location */
	void SetInitialLocation(const FVector& Location) { InitialVisualizerLocation = Location; }

	/** Sets offset used to space out the visualizers */
	void SetInitialRotation(const FRotator& Rotation) { VisualizerRotation = Rotation; }

	/** Sets offset used to space out the visualizers */
	void SetVisualizerOffset(const FVector& Offset) { VisualizerOffset = Offset; }
	
protected:
	TArray<TObjectPtr<AActor>>& GetVisualizers() { return Visualizers; }
	
	/** AudioAnalyzer settings */
	FPlayerSettings_AudioAnalyzer AASettings;

	/** Whether the visualizer is spawned through code or placed in the world */
	UPROPERTY(EditAnywhere, Category = "Spawnable Visualizer Settings")
	bool bIsSpawnedThroughCode = true;

	/** Initial visualizer spawn location */
	UPROPERTY(EditAnywhere, Category = "Spawnable Visualizer Settings")
	FVector InitialVisualizerLocation;

	/** Visualizer rotation */
	UPROPERTY(EditAnywhere, Category = "Spawnable Visualizer Settings")
	FRotator VisualizerRotation;

	/** Distance between the visualizers */
	UPROPERTY(EditAnywhere, Category = "Spawnable Visualizer Settings")
	FVector VisualizerOffset;

	const FActorSpawnParameters SpawnParameters;

	/** An array of spawned visualizer lights. Populate this if creating a visualizer in a level */
	UPROPERTY(EditAnywhere, Category = "Spawnable Visualizer Settings")
	TArray<TObjectPtr<AActor>> Visualizers;
};
