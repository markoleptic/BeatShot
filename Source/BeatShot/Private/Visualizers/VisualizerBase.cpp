// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Visualizers/VisualizerBase.h"

AVisualizerBase::AVisualizerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AVisualizerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualizerBase::Destroyed()
{
	Super::Destroyed();
	DeactivateVisualizers();
}

void AVisualizerBase::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	AASettings = InAASettings;
	MapVisualizerToAudioAnalyzerChannels(InAASettings.NumBandChannels, BaseConfig.NumVisualizerLightsToSpawn);
}

void AVisualizerBase::InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	AASettings = InAASettings;
	MapVisualizerToAudioAnalyzerChannels(InAASettings.NumBandChannels, BaseConfig.NumVisualizerLightsToSpawn);
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

void AVisualizerBase::MapVisualizerToAudioAnalyzerChannels(const int32 NumBandChannels, const int32 NumVisualizerLightsToSpawn)
{
	BaseConfig.MappedIndices.Init(FChannelToVisualizerMap(), NumBandChannels);

	if (NumVisualizerLightsToSpawn == NumBandChannels)
	{
		for (int i = 0; i < NumVisualizerLightsToSpawn; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}

	if (NumVisualizerLightsToSpawn > NumBandChannels &&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::MultiLightPerChannelOnly && BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::Auto))
	{
		for (int i = 0; i < NumBandChannels; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}

	if (NumBandChannels > NumVisualizerLightsToSpawn &&
		(BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::MultiChannelPerLightOnly && BaseConfig.AssignmentMethod != ELightVisualizerAssignmentMethod::Auto))
	{
		for (int i = 0; i < NumVisualizerLightsToSpawn; i++)
		{
			BaseConfig.MappedIndices[i].AddIndex(i);
		}
		return;
	}
	
	switch (BaseConfig.GroupingMethod)
	{
	case ELightVisualizerGroupingMethod::CombineByProximity:
		// Assigning multiple lights to channels
		if (NumVisualizerLightsToSpawn > NumBandChannels)
		{
			const int32 GroupingSize = NumVisualizerLightsToSpawn / NumBandChannels;
			const int32 LeftToDistribute = NumVisualizerLightsToSpawn % NumBandChannels;
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
		// Assigning multiple channels to lights
		else if (NumBandChannels > NumVisualizerLightsToSpawn)
		{
			const int32 GroupingSize = NumBandChannels / NumVisualizerLightsToSpawn;
			const int32 LeftToDistribute = NumBandChannels % NumVisualizerLightsToSpawn;
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
		// Assigning multiple lights to channels
		if (NumVisualizerLightsToSpawn > NumBandChannels)
		{
			for (int i = 0; i < NumVisualizerLightsToSpawn; i++)
			{
				const int32 ChannelIndex = i % NumBandChannels;
				BaseConfig.MappedIndices[ChannelIndex].AddIndex(i);
			}
		}
		// Assigning multiple channels to lights
		else if (NumBandChannels > NumVisualizerLightsToSpawn)
		{
			for (int i = 0; i < NumBandChannels; i++)
			{
				const int32 VizIndex = i % NumVisualizerLightsToSpawn;
				BaseConfig.MappedIndices[i].AddIndex(VizIndex);
			}
		}
		break;
	}

	UE_LOG(LogTemp, Display, TEXT("NumVisualizerLightsToSpawn: %d, NumBandChannels: %d"), NumVisualizerLightsToSpawn, NumBandChannels);
	int Total = 0;
	for (int i = 0; i<  BaseConfig.MappedIndices.Num(); i++)
	{
		int Current = 0;
		for (int j = 0; j < BaseConfig.MappedIndices[i].VisualizerIndices.Num(); j++)
		{
			UE_LOG(LogTemp, Display, TEXT("Assigned VizNum %d to channel: %d"), BaseConfig.MappedIndices[i].VisualizerIndices[j], i);
			Current++;
			Total++;
		}
		UE_LOG(LogTemp, Display, TEXT("TotalVizualizers in channel: %d"), Current);
	}
	UE_LOG(LogTemp, Display, TEXT("TotalVizualizers in all channels: %d"), Total);
}

TArray<int32>& AVisualizerBase::GetLightIndices(const int32 ChannelIndex)
{
	return BaseConfig.MappedIndices[ChannelIndex].VisualizerIndices;
}
