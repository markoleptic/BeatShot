// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualizerManager.h"
#include "VisualizerBase.h"
#include "StaticCubeVisualizer.h"
#include "BeamVisualizer.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AVisualizerManager::AVisualizerManager()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AVisualizerManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AVisualizerManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualizerManager::InitializeVisualizers(const FPlayerSettings PlayerSettings)
{
	AvgSpectrumValues.SetNum(AASettings.NumBandChannels);
	CurrentSpectrumValues.SetNum(AASettings.NumBandChannels);
	MaxSpectrumValues.SetNum(AASettings.NumBandChannels);
	MaxSpectrumValues.Init(1, AASettings.NumBandChannels);
	CurrentCubeSpectrumValues.SetNum(AASettings.NumBandChannels);

	Visualizers.Empty();
	Visualizers.EmplaceAt(0, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass,
		                      &VisualizerLocation,
		                      &VisualizerRotation,
		                      SpawnParameters)));
	Visualizers.EmplaceAt(1, Cast<AStaticCubeVisualizer>(GetWorld()->SpawnActor(StaticCubeVisualizerClass,
		                      &Visualizer2Location,
		                      &VisualizerRotation,
		                      SpawnParameters)));
	if (PlayerSettings.bShowLightVisualizers)
	{
		Visualizers.EmplaceAt(2, Cast<ABeamVisualizer>(
			                      GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation,
			                                             SpawnParameters)));
	}

	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->InitializeVisualizer();
	}
}

float AVisualizerManager::GetNormalizedSpectrumValue(const int32 Index, const bool bIsBeam)
{
	if (bIsBeam)
	{
		return UKismetMathLibrary::MapRangeClamped(CurrentSpectrumValues[Index], 0, MaxSpectrumValues[Index], 0, 1);
	}
	return UKismetMathLibrary::MapRangeClamped(CurrentCubeSpectrumValues[Index] - AvgSpectrumValues[Index], 0,
	                                           MaxSpectrumValues[Index], 0, 1);
}

void AVisualizerManager::UpdateVisualizers(const TArray<float> SpectrumValues)
{
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
	Visualizers.Empty();
}

void AVisualizerManager::UpdateVisualizerStates(const FPlayerSettings PlayerSettings)
{
	if (Visualizers.IsEmpty())
	{
		return;
	}
	if (!PlayerSettings.bShowLightVisualizers)
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
	const int32 NewIndex = Visualizers.Emplace(Cast<ABeamVisualizer>(
		GetWorld()->SpawnActor(BeamVisualizerClass, &BeamVisualizerLocation, &BeamRotation,
		                       SpawnParameters)));
	Visualizers[NewIndex]->InitializeVisualizer();
}

void AVisualizerManager::UpdateAASettings(const FAASettingsStruct NewAASettings)
{
	AASettings = NewAASettings;
	for (AVisualizerBase* Visualizer : Visualizers)
	{
		Visualizer->UpdateAASettings(NewAASettings);
	}
}
