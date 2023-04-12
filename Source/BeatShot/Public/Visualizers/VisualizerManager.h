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
	// Sets default values for this actor's properties
	AVisualizerManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	void InitializeVisualizers(const FPlayerSettings_Game& PlayerSettings);

	float GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam);

	void UpdateVisualizers(const TArray<float>& SpectrumValues);

	void DestroyVisualizers();

	void UpdateVisualizerStates(const FPlayerSettings_Game& PlayerSettings);

	void UpdateAASettings(const FPlayerSettings_AudioAnalyzer& NewAASettings);

	/* The base Visualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AStaticCubeVisualizer> StaticCubeVisualizerClass;

	/** The BeamVisualizer class to spawn */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABeamVisualizer> BeamVisualizerClass;

	/** The spawned Visualizers */
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<AVisualizerBase>> Visualizers;

	/* The spawned AATracker object */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAudioAnalyzerManager> AATracker;

	/* The spawned AAPlayer object */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAudioAnalyzerManager> AAPlayer;

	/* Locally stored AASettings since they must be accessed frequently in OnTick() */
	UPROPERTY()
	FPlayerSettings_AudioAnalyzer AASettings;

	UPROPERTY(VisibleAnywhere)
	TArray<float> AvgSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> CurrentCubeSpectrumValues;

	UPROPERTY(VisibleAnywhere)
	TArray<float> MaxSpectrumValues;

	const FActorSpawnParameters SpawnParameters;
};
