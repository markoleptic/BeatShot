// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class USceneComponent;
class UAudioAnalyzerManager;

UCLASS()
class BEATSHOT_API AVisualizerBase : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	AVisualizerBase();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Destroyed() override;

public:
	/** Destroys any existing components and repopulates based on AASettings.NumBandChannels */
	UFUNCTION()
	virtual void InitializeVisualizer();

	/** Updates a visualizer state at Index inside Visualizers array. SpectrumAlpha should be a value between
	 *  0 and 1, with 1 being the maximum visualizer state and 0 being the minimum */
	UFUNCTION()
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha);

	/** Saves the new AASettings and calls InitializeVisualizer() */
	UFUNCTION()
	virtual void UpdateAASettings(const FPlayerSettings_AudioAnalyzer& AASettingsStruct);

	virtual void MarkRenderStateDirty();

protected:
	/** Sets the initial visualizer spawn location */
	void SetInitialLocation(const FVector& Location) { InitialVisualizerLocation = Location; }

	/** Sets offset used to space out the visualizers */
	void SetRotation(const FRotator& Rotation) { VisualizerRotation = Rotation; }

	/** Sets offset used to space out the visualizers */
	void SetVisualizerOffset(const FVector& Offset) { VisualizerOffset = Offset; }

	/** AudioAnalyzer settings */
	FPlayerSettings_AudioAnalyzer AASettings;

	/** Initial visualizer spawn location */
	FVector InitialVisualizerLocation;

	/** Visualizer rotation */
	FRotator VisualizerRotation;

	/** Distance between the visualizers */
	FVector VisualizerOffset;

	const FActorSpawnParameters SpawnParameters;

	/** An array of spawned visualizers */
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> Visualizers;
};
