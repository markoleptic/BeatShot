// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	AVisualizerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitializeVisualizers(const FPlayerSettings PlayerSettings);

	float GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam);
	
	void UpdateVisualizers(const TArray<float> SpectrumValues);

	void DestroyVisualizers();

	void UpdateVisualizerStates(const FPlayerSettings PlayerSettings);

	void UpdateAASettings(const FAASettingsStruct NewAASettings);

	/* The base Visualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStaticCubeVisualizer> StaticCubeVisualizerClass;

	/** The BeamVisualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeamVisualizer> BeamVisualizerClass;

	/** The spawned Visualizers */
	UPROPERTY(BlueprintReadOnly)
	TArray<AVisualizerBase*> Visualizers;
		
	/* The spawned AATracker object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AATracker;

	/* The spawned AAPlayer object */
	UPROPERTY(BlueprintReadOnly)
	UAudioAnalyzerManager* AAPlayer;

	/* Locally stored AASettings since they must be accessed frequently in OnTick() */
	UPROPERTY()
	FAASettingsStruct AASettings;

	UPROPERTY(VisibleAnywhere)
	TArray<float> SpectrumVariance;

	UPROPERTY(VisibleAnywhere)
	TArray<float> AvgSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentCubeSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> SpectrumPeaks;

	UPROPERTY(VisibleAnywhere)
	TArray<float> SpectrumPeakEnergy;

	UPROPERTY(VisibleAnywhere)
	TArray<float> MaxSpectrumValues;

	const FVector TargetSpawnerLocation = {3730, 0, 750};
	
	const FVector VisualizerLocation = {4000,-1950, 210};
	const FVector Visualizer2Location = {4000, 1950, 210};
	const FRotator VisualizerRotation = {0, 90, 90};

	const FVector BeamVisualizerLocation = {0,1920,1320};
	const FRotator BeamRotation = {0,0,0};
	
	const FActorSpawnParameters SpawnParameters;

};
