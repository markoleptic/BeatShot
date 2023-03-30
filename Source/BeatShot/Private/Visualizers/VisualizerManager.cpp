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

void AVisualizerManager::InitializeVisualizers(const FPlayerSettings& PlayerSettings)
{
	AvgSpectrumValues.Init(0, AASettings.NumBandChannels);
	CurrentSpectrumValues.Init(0, AASettings.NumBandChannels);
	MaxSpectrumValues.Init(1, AASettings.NumBandChannels);
	CurrentCubeSpectrumValues.Init(0, AASettings.NumBandChannels);

	Visualizers.Empty();
	Visualizers.EmplaceAt(0, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass, &VisualizerLocation, &VisualizerRotation, SpawnParameters)));
	Visualizers.EmplaceAt(1, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass, &Visualizer2Location, &VisualizerRotation, SpawnParameters)));
	if (PlayerSettings.Game.bShowLightVisualizers)
	{
		Visualizers.EmplaceAt(2, Cast<ABeamVisualizer>(GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation, SpawnParameters)));
	}

	for (const TObjectPtr<AVisualizerBase> Visualizer : Visualizers)
	{
		if (Visualizer)
		{
			Visualizer->InitializeVisualizer();
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
	if (Visualizers.IsEmpty())
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
			if (Visualizers.Num() > 2)
			{
				Visualizers[2]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, true));
			}
		}

		if (Visualizers[0])
		{
			Visualizers[0]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, false));
		}
		if (Visualizers[1])
		{
			Visualizers[1]->UpdateVisualizer(i, GetNormalizedSpectrumValue(i, false));
		}

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
}

void AVisualizerManager::DestroyVisualizers()
{
	if (Visualizers.IsEmpty())
	{
		return;
	}
	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}
	Visualizers.Empty();
}

void AVisualizerManager::UpdateVisualizerStates(const FPlayerSettings& PlayerSettings)
{
	if (Visualizers.IsEmpty())
	{
		return;
	}
	if (!PlayerSettings.Game.bShowLightVisualizers)
	{
		ABeamVisualizer* FoundBeamVisualizer = nullptr;
		if (Visualizers.FindItemByClass(&FoundBeamVisualizer))
		{
			Visualizers.Remove(FoundBeamVisualizer);
			FoundBeamVisualizer->Destroy();
		}
		Visualizers.Shrink();
		return;
	}

	if (Visualizers.Num() > 2)
	{
		return;
	}
	const int32 NewIndex = Visualizers.Emplace(Cast<ABeamVisualizer>(GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation, SpawnParameters)));
	Visualizers[NewIndex]->InitializeVisualizer();
}

void AVisualizerManager::UpdateAASettings(const FAASettingsStruct& NewAASettings)
{
	AASettings = NewAASettings;
	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->UpdateAASettings(NewAASettings);
	}
}
