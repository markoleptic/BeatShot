// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Visualizers/VisualizerManager.h"
#include "Visualizers/VisualizerBase.h"
#include "Visualizers/StaticCubeVisualizer.h"
#include "Visualizers/BeamVisualizer.h"
#include "Kismet/KismetMathLibrary.h"

AVisualizerManager::AVisualizerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AVisualizerManager::BeginPlay()
{
	Super::BeginPlay();
}

void AVisualizerManager::InitializeVisualizers(const FPlayerSettings_Game& PlayerSettings, const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	AASettings = InAASettings;
	AvgSpectrumValues.Init(0, AASettings.NumBandChannels);
	CurrentSpectrumValues.Init(0, AASettings.NumBandChannels);
	MaxSpectrumValues.Init(1, AASettings.NumBandChannels);
	CurrentCubeSpectrumValues.Init(0, AASettings.NumBandChannels);
	
	/* Default Visualizers */
	CubeVisualizers.Emplace(Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass, &LeftStaticCubeVisualizerLocation, &StaticCubeVisualizerRotation, SpawnParameters)));
	CubeVisualizers.Emplace(Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass, &RightStaticCubeVisualizerLocation, &StaticCubeVisualizerRotation, SpawnParameters)));
	ABeamVisualizer* MiddleRoomBeam = Cast<ABeamVisualizer>(GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation, SpawnParameters));
	MiddleRoomBeam->SetInitialLocation(InitialBeamLightLocation);
	MiddleRoomBeam->SetInitialRotation(BeamRotation);
	MiddleRoomBeam->SetVisualizerOffset(BeamLightOffset);
	MiddleRoomBeam->SetMovingLights(true);
	BeamVisualizers.Add(MiddleRoomBeam);
	
	if (PlayerSettings.bShowLightVisualizers)
	{
		bUpdateVisualizers = true;
	}

	for (const TObjectPtr<ABeamVisualizer> Visualizer : GetBeamVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->InitializeVisualizer(InAASettings);
		}
	}
	for (const TObjectPtr<AStaticCubeVisualizer> Visualizer : GetCubeVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->InitializeVisualizer(InAASettings);
		}
	}
}

void AVisualizerManager::InitializeVisualizersFromWorld(const TArray<TSoftObjectPtr<AVisualizerBase>>& InVisualizers)
{
	for (const TSoftObjectPtr<AVisualizerBase>& Visualizer : InVisualizers)
	{
		if (Cast<ABeamVisualizer>(Visualizer.Get()))
		{
			Visualizer->InitializeVisualizerFromWorld();
			BeamVisualizers.Add(Cast<ABeamVisualizer>(Visualizer.Get()));
		}
		else if (Cast<AStaticCubeVisualizer>(Visualizer.Get()))
		{
			Visualizer->InitializeVisualizerFromWorld();
			CubeVisualizers.Add(Cast<AStaticCubeVisualizer>(Visualizer.Get()));
		}
	}
}

float AVisualizerManager::GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam)
{
	if (bIsBeam)
	{
		return UKismetMathLibrary::MapRangeClamped(CurrentSpectrumValues[Index], 0, MaxSpectrumValues[Index], 0, 1);
	}
	return UKismetMathLibrary::MapRangeClamped(CurrentCubeSpectrumValues[Index] - AvgSpectrumValues[Index], 0, MaxSpectrumValues[Index], 0, 1);
}

void AVisualizerManager::UpdateVisualizers(const TArray<float>& SpectrumValues)
{
	if (!bUpdateVisualizers)
	{
		return;
	}
	
	for (int i = 0; i < SpectrumValues.Num(); i++)
	{
		if (SpectrumValues[i] > MaxSpectrumValues[i])
		{
			MaxSpectrumValues[i] = SpectrumValues[i];
		}
		if (SpectrumValues[i] > CurrentCubeSpectrumValues[i])
		{
			CurrentCubeSpectrumValues[i] = SpectrumValues[i];
		}

		if (SpectrumValues[i] > 0 && CurrentSpectrumValues[i] < 0)
		{
			CurrentSpectrumValues[i] = SpectrumValues[i];
			
			UpdateBeamVisualizers(i, GetNormalizedSpectrumValue(i, true));
		}

		UpdateCubeVisualizers(i, GetNormalizedSpectrumValue(i, false));

		if (CurrentSpectrumValues[i] >= 0)
		{
			CurrentSpectrumValues[i] -= (AvgSpectrumValues[i] / 120);
		}
		if (CurrentCubeSpectrumValues[i] >= 0)
		{
			CurrentCubeSpectrumValues[i] -= 0.005;
		}
		if (MaxSpectrumValues[i] >= 0)
		{
			MaxSpectrumValues[i] -= (AvgSpectrumValues[i] / 500);
		}
	}
	MarkVisualizerRenderStateDirty();
}

void AVisualizerManager::UpdateBeamVisualizers(const int32 Index, const float SpectrumValue)
{
	for (const TObjectPtr<ABeamVisualizer>& BeamVisualizer : GetBeamVisualizers())
	{
		BeamVisualizer->ActivateVisualizer(Index);
	}
}

void AVisualizerManager::UpdateCubeVisualizers(const int32 Index, const float SpectrumValue)
{
	for (const TObjectPtr<AStaticCubeVisualizer>& CubeVisualizer : GetCubeVisualizers())
	{
		CubeVisualizer->UpdateVisualizer(Index, SpectrumValue);
	}
}

void AVisualizerManager::DeactivateVisualizers()
{
	for (const TObjectPtr<ABeamVisualizer> Visualizer : GetBeamVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->DeactivateVisualizers();
		}
	}
	
	for (const TObjectPtr<AStaticCubeVisualizer> Visualizer : GetCubeVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->DeactivateVisualizers();
		}
	}
}

void AVisualizerManager::UpdateVisualizerSettings(const FPlayerSettings_Game& PlayerSettings)
{
	bUpdateVisualizers = PlayerSettings.bShowLightVisualizers;
	if (!bUpdateVisualizers)
	{
		DeactivateVisualizers();
	}
}

void AVisualizerManager::UpdateAASettings(const FPlayerSettings_AudioAnalyzer& NewAASettings)
{
	AASettings = NewAASettings;

	for (const TObjectPtr<ABeamVisualizer> Visualizer : GetBeamVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->UpdateAASettings(NewAASettings);
		}
	}
	
	for (const TObjectPtr<AStaticCubeVisualizer> Visualizer : GetCubeVisualizers())
	{
		if (Visualizer)
		{
			Visualizer->UpdateAASettings(NewAASettings);
		}
	}
}

void AVisualizerManager::MarkVisualizerRenderStateDirty()
{
	for (const TObjectPtr<AStaticCubeVisualizer>& CubeVisualizer : GetCubeVisualizers())
	{
		CubeVisualizer->MarkRenderStateDirty();
	}
}
