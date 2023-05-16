// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSVisualizerDefinition.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class UBSVisualizerDefinition;
class USplineComponent;
class USceneComponent;
class UAudioAnalyzerManager;

/*/** The base configuration common to all visualizers #1#
USTRUCT(BlueprintType, Category = "Visualizer Config")
struct BEATSHOT_API FBaseVisualizerConfig
{
	GENERATED_BODY()

	/** Whether or not to spawn visualizer lights, or add already placed lights #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(InlineEditConditionToggle))
	bool bSpawnVisualizerLights;

	/** Number of visualizer lights to spawn #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayName="Spawn lights through code", DisplayPriority=-1200, EditCondition="bSpawnVisualizerLights"))
	int32 NumVisualizerLightsToSpawn;
	
	/** World location to apply to this actor #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-1100))
	FVector StartLocation;

	/** Rotation to apply to this actor #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(Units="Degrees"), meta=(DisplayPriority=-1000))
	FRotator StartRotation;

	/** Scale to apply to this actor #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-900))
	FVector StartScale;
	
	/** Whether or not to make the StartTransform the center of the spawned lights or not #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-800))
	bool bGrowFromCenter;
	
	/** Relative offset location to apply between lights #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-700))
	FVector OffsetLocation;

	/** Relative offset rotation to apply between lights #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(Units="Degrees"), meta=(DisplayPriority=-600))
	FRotator OffsetRotation;

	/** Relative offset scale to apply between lights #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-500))
	FVector OffsetScale;
	
	/** How to assign AudioAnalyzer channels to lights #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Audio Analyzer Linking", meta=(DisplayPriority=-400))
	ELightVisualizerAssignmentMethod AssignmentMethod;

	/** How to group AudioAnalyzer channels and lights together #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Audio Analyzer Linking", meta=(DisplayPriority=-300))
	ELightVisualizerGroupingMethod GroupingMethod;

	/** Whether or not override each child light's light colors with BeamLightColors, vs use the light color from each child light#1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(InlineEditConditionToggle))
	bool bOverrideChildLightColors;
	
	/** Optionally override the color for each light, while keeping all other settings the same #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayName="Override light colors", DisplayPriority=-200, EditCondition="bOverrideChildLightColors"))
	TArray<FLinearColor> BeamLightColors;
	
	/** Whether or not to override the child visualizer settings. This will be false if using settings from individually placed lights in a level #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayPriority=-100))
	bool bOverrideChildLightConfig;
	
	/** Each index in this array represents an AudioAnalyzerChannel, where the element is an array of visualizer indices that should sync to this AudioAnalyzerChannel #1#
	TArray<FChannelToVisualizerMap> MappedIndices;

	FBaseVisualizerConfig()
	{
		bSpawnVisualizerLights = false;
		bGrowFromCenter = true;
		StartLocation = FVector(0);
		StartRotation = FRotator(0);
		StartScale = FVector(1.f);
		OffsetLocation = FVector(0);
		OffsetRotation = FRotator(0);
		OffsetScale = FVector(1.f);
		bOverrideChildLightConfig = true;
		bOverrideChildLightColors = true;
		BeamLightColors = DefaultBeamLightColors;
		NumVisualizerLightsToSpawn = DefaultNumVisualizerLightsToSpawn;
		AssignmentMethod = ELightVisualizerAssignmentMethod::MultiLightPerChannelOnly;
		GroupingMethod = ELightVisualizerGroupingMethod::CombineByProximity;
		MappedIndices = TArray<FChannelToVisualizerMap>();
	}
};*/

/** Base class for Light Visualizers. BeamVisualizer and StaticCubeVisualizer are examples of classes that subclass this one */
UCLASS(Abstract)
class BEATSHOT_API AVisualizerBase : public AActor
{
	GENERATED_BODY()
	
	friend class AVisualizerManager;

public:
	AVisualizerBase();

protected:

	virtual void Destroyed() override;

	UPROPERTY(EditInstanceOnly, Category="Spline")
	USplineComponent* VisualizerPositioning;

public:
	/** Copies the new AudioAnalyzer Settings. Overrides should spawn their specific visualizer implementations. */
	virtual void InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings);

	/** Initializes a visualizer already in the level */
	virtual void InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings, const int32 NumSpawnedVisualizers);

	/** Activates the matching visualizer from the given AudioAnalyzer channel index */
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

	/** Returns the definition for this visualizer */
	UBSVisualizerDefinition& GetVisualizerDefinition() const { return *VisualizerDefinition.GetDefaultObject(); }

	/** The number of points to add to the spline (how many visualizers will be spawned) */
	UPROPERTY(EditInstanceOnly, Category="Spline")
	int32 NumberOfPointsToAdd;

	/** The direction to distribute the splint points towards */
	UPROPERTY(EditInstanceOnly, Category="Spline")
	FVector PointAddDirection;

	/** The spacing in between spline points */
	UPROPERTY(EditInstanceOnly, Category="Spline")
	float Offset;

	/** Whether or not to distribute the points centered from the spline component location */
	UPROPERTY(EditInstanceOnly, Category="Spline")
	bool bDistributeFromCenter;

	/** Clears all exiting spline points, and adds points according to NumberOfPointsToAdd, PointAddDirection, Offset, and DistributeFromCenter */
	UFUNCTION(CallInEditor, Category="Spline")
	void AddVisualizerPointsToSpline();

	UPROPERTY(EditInstanceOnly, Category="Spline")
	TSubclassOf<AActor> TestVisualizerToSpawn;

	UPROPERTY(Transient)
	AActor* SpawnedTestVisualizer;

	/** The direction to distribute the splint points towards */
	UPROPERTY(EditInstanceOnly, Category="Spline")
	FRotator TestVisualizerRotation;

	/** Spawns a visualizer specified in TestVisualizerToSpawn to determine what location and rotation is needed for alignment */
	UFUNCTION(CallInEditor, Category="Spline")
	void AddTestVisualizer();
	
protected:

	/** Maps AudioAnalyzer channels to individual visualizer lights */
	virtual void MapAudioAnalyzerChannelsToVisualizerLights(const int32 NumBandChannels, const int32 NumVisualizers = INDEX_NONE);

	/** Returns the visualizer light indices that correspond to AudioAnalyzer ChannelIndex */
	virtual TArray<int32>& GetLightIndices(const int32 ChannelIndex);

	/** Returns the locations to spawn visualizer lights based on the spline component's points */
	TArray<FVector> GetSplinePointLocations() const;

	/** The data asset that specifies the configuration for this visualizer */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Definition", meta=(DisplayPriority=-20000))
	TSubclassOf<UBSVisualizerDefinition> VisualizerDefinition;
};
