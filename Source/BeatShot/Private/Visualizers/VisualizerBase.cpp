// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Visualizers/VisualizerBase.h"

AVisualizerBase::AVisualizerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AVisualizerBase::Destroyed()
{
	Super::Destroyed();
	DeactivateVisualizers();
}

void AVisualizerBase::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	SetActorLocation(GetConfig().StartLocation);
	SetActorRotation(GetConfig().StartRotation);
	SetActorScale3D(GetConfig().StartScale);
	MapAudioAnalyzerChannelsToVisualizerLights(InAASettings.NumBandChannels, GetConfig().NumVisualizerLightsToSpawn);
}

void AVisualizerBase::InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings, const int32 NumSpawnedVisualizers)
{
	MapAudioAnalyzerChannelsToVisualizerLights(InAASettings.NumBandChannels, NumSpawnedVisualizers);
}

void AVisualizerBase::ActivateVisualizer(const int32 Index)
{
}

void AVisualizerBase::DeactivateVisualizers()
{
}

void AVisualizerBase::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
}

void AVisualizerBase::UpdateAASettings(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	InitializeVisualizer(InAASettings);
}

void AVisualizerBase::MarkRenderStateDirty()
{
}

void AVisualizerBase::MapAudioAnalyzerChannelsToVisualizerLights(const int32 NumBandChannels, const int32 NumVisualizers)
{
	BaseConfig.MappedIndices.Init(FChannelToVisualizerMap(), NumBandChannels);

	if (NumVisualizers == NumBandChannels)
	{
		for (int i = 0; i < NumVisualizers; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}

	if ((NumVisualizers > NumBandChannels) &&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::MultiLightPerChannelOnly) &&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::Auto))
	{
		for (int i = 0; i < NumBandChannels; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}

	if ((NumBandChannels > NumVisualizers) &&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::MultiChannelPerLightOnly)&&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::Auto))
	{
		for (int i = 0; i < NumVisualizers; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}
	
	switch (BaseConfig.GroupingMethod)
	{
	case ELightVisualizerGroupingMethod::CombineByProximity:
		{
			const int32 GroupingSize = NumVisualizers / NumBandChannels;
			const int32 LeftToDistribute = NumVisualizers % NumBandChannels;
			int32 CurrentVizIndex = 0;
			for (int i = 0; i < NumBandChannels; i++)
			{
				const int StartIndex = CurrentVizIndex;
				for (int j = StartIndex; j < StartIndex + GroupingSize; j++)
				{
					BaseConfig.MappedIndices[i].AddIndex(j);
					CurrentVizIndex++;
				}
				if (i < LeftToDistribute)
				{
					BaseConfig.MappedIndices[i].AddIndex(CurrentVizIndex);
					CurrentVizIndex++;
				}
			}
		}
		break;
	case ELightVisualizerGroupingMethod::Repeat:
		{
			// Assigning multiple lights to channels
			if (NumVisualizers > NumBandChannels)
			{
				for (int i = 0; i < NumVisualizers; i++)
				{
					const int32 ChannelIndex = i % NumBandChannels;
					BaseConfig.MappedIndices[ChannelIndex].AddIndex(i);
				}
			}
			// Assigning multiple channels to lights
			else if (NumBandChannels > NumVisualizers)
			{
				for (int i = 0; i < NumBandChannels; i++)
				{
					const int32 VizIndex = i % NumVisualizers;
					BaseConfig.MappedIndices[i].AddIndex(VizIndex);
				}
			}
		}
		break;
	}
	
	int Total = 0;
	for (int i = 0; i<  BaseConfig.MappedIndices.Num(); i++)
	{
		int Current = 0;
		for (int j = 0; j < BaseConfig.MappedIndices[i].VisualizerIndices.Num(); j++)
		{
			Current++;
			Total++;
		}
		UE_LOG(LogTemp, Display, TEXT("TotalVizualizers in channel %d: %d"), i, Current);
	}
	UE_LOG(LogTemp, Display, TEXT("NumVisualizers: %d, NumBandChannels: %d"), NumVisualizers, NumBandChannels);
	UE_LOG(LogTemp, Display, TEXT("TotalVizualizers in all channels: %d"), Total);
}

TArray<int32>& AVisualizerBase::GetLightIndices(const int32 ChannelIndex)
{
	return GetConfig().MappedIndices[ChannelIndex].VisualizerIndices;
}
