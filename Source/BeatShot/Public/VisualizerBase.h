// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class USceneComponent;

UCLASS()
class BEATSHOT_API AVisualizerBase : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AVisualizerBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Destroys any existing components and repopulates based on AASettings.NumBandChannels */
	UFUNCTION()
	virtual void InitializeVisualizer();

	/** Called by the owning object (DefaultGameMode) every time an update should be propagated */
	UFUNCTION()
	virtual void UpdateVisualizer(const int32 Index, const float SpectrumAlpha);

	/** Saves the new AASettings and calls InitializeVisualizer() */
	UFUNCTION()
	virtual void UpdateAASettings(const FAASettingsStruct AASettingsStruct);

protected:

	/** Sets the initial visualizer spawn location */
	void SetInitialLocation(const FVector Location) { InitialVisualizerLocation = Location; }

	/** Sets offset used to space out the visualizers */
	void SetRotation(const FRotator Rotation) { VisualizerRotation = Rotation; }

	/** Sets offset used to space out the visualizers */
	void SetVisualizerOffset(const FVector Offset) { VisualizerOffset = Offset; }

	/** AudioAnalyzer settings */
	FAASettingsStruct AASettings;

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
