// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamVisualizer.h"
#include "SimpleBeamLight.h"

ABeamVisualizer::ABeamVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABeamVisualizer::Destroyed()
{
	Super::Destroyed();
	for (AActor* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}
}

void ABeamVisualizer::BeginPlay()
{
	Super::BeginPlay();
	SetInitialLocation(InitialBeamLightLocation);
	SetRotation(BeamLightRotation);
	SetVisualizerOffset(BeamLightOffset);
}

void ABeamVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeamVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();

	BeatColors.Emplace(FLinearColor(255/255.f, 0/255.f, 0/255.f));
	BeatColors.Emplace(FLinearColor(255/255.f, 127/255.f, 0/255.f));
	BeatColors.Emplace(FLinearColor(255/255.f, 255/255.f, 0/255.f));
	BeatColors.Emplace(FLinearColor(127/255.f, 255/255.f, 0/255.f));
	BeatColors.Emplace(FLinearColor(0/255.f, 255/255.f, 0/255.f));
	BeatColors.Emplace(FLinearColor(0/255.f, 255/255.f, 127/255.f));
	BeatColors.Emplace(FLinearColor(0/255.f, 255/255.f, 255/255.f));
	BeatColors.Emplace(FLinearColor(0/255.f, 127/255.f, 255/255.f));
	BeatColors.Emplace(FLinearColor(0/255.f, 0/255.f, 255/255.f));
	BeatColors.Emplace(FLinearColor(127/255.f, 0/255.f, 255/255.f));
	
	FVector CurrentSpawnLoc = InitialVisualizerLocation - VisualizerOffset * (static_cast<float>(AASettings.NumBandChannels - 1) / 2);
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		//CurrentSpawnLoc = i * VisualizerOffset + InitialVisualizerLocation;
		Visualizers.EmplaceAt(i, Cast<ASimpleBeamLight>(
			                      GetWorld()->SpawnActor(SimpleBeamLightClass, &CurrentSpawnLoc, &VisualizerRotation,
			                                             SpawnParameters)));
		Cast<ASimpleBeamLight>(Visualizers[i])->SetColor(BeatColors[i]);
		Cast<ASimpleBeamLight>(Visualizers[i])->SetIndex(i);
		CurrentSpawnLoc += VisualizerOffset;
	}
}

void ABeamVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	Cast<ASimpleBeamLight>(Visualizers[Index])->UpdateNiagaraBeam(SpectrumAlpha);
}
