// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class USceneComponent;
class UAudioAnalyzerManager;

/** Each instance of this struct represents an AudioAnalyzer channel index, containing 0-multiple visualizer indices */
USTRUCT(BlueprintType)
struct BEATSHOT_API FChannelToVisualizerMap
{
	GENERATED_BODY()

	/** An array of visualizer indices that are assigned to this AudioAnalyzer channel index */
	TArray<int32> VisualizerIndices;
	
	FChannelToVisualizerMap()
	{
		VisualizerIndices = TArray<int32>();
	}

	/** Adds a visualizer index to this AudioAnalyzer channel index */
	void AddIndex(const int32 InIndex)
	{
		VisualizerIndices.Add(InIndex);
	}
};

/** How to assign AudioAnalyzer channels to lights if there is an uneven number between the two */
UENUM(BlueprintType)
enum class ELightVisualizerAssignmentMethod : uint8
{
	/** Don't assign channels to any lights (not implemented) */
	None UMETA(DisplayName="None"),
	
	/** Assign a channel to every light, or assign a light to every channel, depending on which has more */
	Auto UMETA(DisplayName="Auto"),

	/** Assign a channel to every light, but don't assign more than one light to any channel */
	MultiChannelPerLightOnly UMETA(DisplayName="MultiChannelPerLightOnly"),
	
	/** Assign a light to every channel, but don't assign more than one channel to any light */
	MultiLightPerChannelOnly UMETA(DisplayName="MultiLightPerChannelOnly"),

	/** Only assign one channel to any light, and vice versa */
	SinglePairsOnly UMETA(DisplayName="SinglePairsOnly")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELightVisualizerAssignmentMethod, ELightVisualizerAssignmentMethod::None, ELightVisualizerAssignmentMethod::SinglePairsOnly);

/** How to group AudioAnalyzer channels and lights together if there is an uneven number between the two */
UENUM(BlueprintType)
enum class ELightVisualizerGroupingMethod : uint8
{
	/** Assigns nearby lights to the same channel if lights > channels,
	 *  or assigns consecutive channels to the same light if channels > lights */
	CombineByProximity UMETA(DisplayName="CombineByProximity"),

	/** Assign in order. If there is an unequal amount of lights and channels, repeat from the start */
	Repeat UMETA(DisplayName="Repeat")
};
ENUM_RANGE_BY_FIRST_AND_LAST(ELightVisualizerGroupingMethod, ELightVisualizerGroupingMethod::CombineByProximity, ELightVisualizerGroupingMethod::Repeat);

/** The base configuration common to all visualizers */
USTRUCT(BlueprintType, Category = "Visualizer Config")
struct BEATSHOT_API FBaseVisualizerConfig
{
	GENERATED_BODY()

	/** Whether or not to spawn visualizer lights, or add already placed lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(InlineEditConditionToggle))
	bool bSpawnVisualizerLights;

	/** Number of visualizer lights to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayName="Spawn lights through code", DisplayPriority=-1200, EditCondition="bSpawnVisualizerLights"))
	int32 NumVisualizerLightsToSpawn;
	
	/** World location to apply to this actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-1100))
	FVector StartLocation;

	/** Rotation to apply to this actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(Units="Degrees"), meta=(DisplayPriority=-1000))
	FRotator StartRotation;

	/** Scale to apply to this actor */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-900))
	FVector StartScale;
	
	/** Whether or not to make the StartTransform the center of the spawned lights or not */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-800))
	bool bGrowFromCenter;
	
	/** Relative offset location to apply between lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-700))
	FVector OffsetLocation;

	/** Relative offset rotation to apply between lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(Units="Degrees"), meta=(DisplayPriority=-600))
	FRotator OffsetRotation;

	/** Relative offset scale to apply between lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Position", meta=(DisplayPriority=-500))
	FVector OffsetScale;
	
	/** How to assign AudioAnalyzer channels to lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Audio Analyzer Linking", meta=(DisplayPriority=-400))
	ELightVisualizerAssignmentMethod AssignmentMethod;

	/** How to group AudioAnalyzer channels and lights together */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Audio Analyzer Linking", meta=(DisplayPriority=-300))
	ELightVisualizerGroupingMethod GroupingMethod;

	/** Whether or not override each child light's light colors with BeamLightColors, vs use the light color from each child light*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(InlineEditConditionToggle))
	bool bOverrideChildLightColors;
	
	/** Optionally override the color for each light, while keeping all other settings the same */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayName="Override light colors", DisplayPriority=-200, EditCondition="bOverrideChildLightColors"))
	TArray<FLinearColor> BeamLightColors;
	
	/** Whether or not to override the child visualizer settings. This will be false if using settings from individually placed lights in a level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config | Light Defaults", meta=(DisplayPriority=-100))
	bool bOverrideChildLightConfig;
	
	/** Each index in this array represents an AudioAnalyzerChannel, where the element is an array of visualizer indices that should sync to this AudioAnalyzerChannel */
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
};

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

	/** Returns the BaseConfig */
	FBaseVisualizerConfig& GetConfig() { return BaseConfig; }
	
protected:

	/** Maps AudioAnalyzer channels to individual visualizer lights */
	virtual void MapAudioAnalyzerChannelsToVisualizerLights(const int32 NumBandChannels, const int32 NumVisualizers = INDEX_NONE);

	/** Returns the visualizer light indices that correspond to AudioAnalyzer ChannelIndex */
	virtual TArray<int32>& GetLightIndices(const int32 ChannelIndex);

	/** The base configuration common to all visualizers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config", meta=(DisplayPriority=-20000))
	FBaseVisualizerConfig BaseConfig;
};
