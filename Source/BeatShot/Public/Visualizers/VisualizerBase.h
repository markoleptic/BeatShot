// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "VisualizerBase.generated.h"

class USceneComponent;
class UAudioAnalyzerManager;

USTRUCT(BlueprintType)
struct FChannelToVisualizerMap
{
	GENERATED_BODY()
	
	TArray<int32> VisualizerIndices;
	
	FChannelToVisualizerMap()
	{
		VisualizerIndices = TArray<int32>();
	}

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
	
	/** Assign multiple channels to each light, or assign multiple lights to each channel, depending on which has more */
	Auto UMETA(DisplayName="Auto"),

	/** Assign multiple channels to each light, but don't assign more than one light to any channel */
	MultiChannelPerLightOnly UMETA(DisplayName="MultiChannelLightOnly"),
	
	/** Assign multiple lights to each channel if there are more lights than channels, but don't assign more than one channel to any light */
	MultiLightPerChannelOnly UMETA(DisplayName="OneChannelPerLight"),

	/** Only assign one channel to any light, and vice versa */
	SinglePairsOnly UMETA(DisplayName="OneChannelPerLight")
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

USTRUCT(BlueprintType)
struct FBaseVisualizerConfig
{
	GENERATED_BODY()
	
	/** Whether or not override the child visualizer settings. This will be false if using settings from individually placed lights in a level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOverrideChildLightConfig = true;
	
	/** Whether or not override each child light's light colors with BeamLightColors, vs use the light color from each child light*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOverrideChildLightColors = true;

	/** Optionally override the color for each light, while keeping all other settings the same */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLinearColor> BeamLightColors = DefaultBeamLightColors;

	/** Number of visualizer lights to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 NumVisualizerLightsToSpawn = DefaultNumVisualizerLightsToSpawn;

	/** Each index in this array represents an AudioAnalyzerChannel, where the element is an array of visualizer indices that should sync to this AudioAnalyzerChannel */
	TArray<FChannelToVisualizerMap> MappedIndices;

	/** How to assign AudioAnalyzer channels to lights */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELightVisualizerAssignmentMethod AssignmentMethod = ELightVisualizerAssignmentMethod::MultiChannelPerLightOnly;

	/** How to group AudioAnalyzer channels and lights together */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ELightVisualizerGroupingMethod GroupingMethod = ELightVisualizerGroupingMethod::CombineByProximity;
	
	FBaseVisualizerConfig()
	{
		bOverrideChildLightConfig = true;
		bOverrideChildLightColors = true;
		BeamLightColors = DefaultBeamLightColors;
		NumVisualizerLightsToSpawn = DefaultNumVisualizerLightsToSpawn;
		AssignmentMethod = ELightVisualizerAssignmentMethod::MultiChannelPerLightOnly;
		GroupingMethod = ELightVisualizerGroupingMethod::CombineByProximity;
	}
};

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
	virtual void InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings);

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
	
	/** Sets the initial visualizer spawn location */
	void SetInitialLocation(const FVector& Location) { InitialVisualizerLocation = Location; }

	/** Sets offset used to space out the visualizers */
	void SetInitialRotation(const FRotator& Rotation) { VisualizerRotation = Rotation; }

	/** Sets offset used to space out the visualizers */
	void SetVisualizerOffset(const FVector& Offset) { VisualizerOffset = Offset; }
	
protected:

	void MapVisualizerToAudioAnalyzerChannels(const int32 NumBandChannels, const int32 NumVisualizerLightsToSpawn);

	TArray<int32>& GetLightIndices(const int32 ChannelIndex);
	
	/** AudioAnalyzer settings */
	FPlayerSettings_AudioAnalyzer AASettings;

	/** Whether the visualizer is spawned through code or placed in the world */
	bool bIsSpawnedThroughCode = true;

	/** Initial visualizer spawn location */
	FVector InitialVisualizerLocation;

	/** Visualizer rotation */
	FRotator VisualizerRotation;

	/** Distance between the visualizers */
	FVector VisualizerOffset;

	const FActorSpawnParameters SpawnParameters;

	/** Inherited config inherited from VisualizerBase */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visualizer Config")
	FBaseVisualizerConfig BaseConfig;
};
