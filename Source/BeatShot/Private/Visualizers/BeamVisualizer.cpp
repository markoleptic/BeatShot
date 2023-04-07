// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Visualizers/BeamVisualizer.h"
#include "Visualizers/SimpleBeamLight.h"

ABeamVisualizer::ABeamVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABeamVisualizer::BeginPlay()
{
	Super::BeginPlay();
	SetInitialLocation(Constants::InitialBeamLightLocation);
	SetRotation(Constants::BeamLightRotation);
	SetVisualizerOffset(Constants::BeamLightOffset);
}

void ABeamVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();

	BeatColors.Emplace(FLinearColor(255 / 255.f, 0 / 255.f, 0 / 255.f));
	BeatColors.Emplace(FLinearColor(255 / 255.f, 127 / 255.f, 0 / 255.f));
	BeatColors.Emplace(FLinearColor(255 / 255.f, 255 / 255.f, 0 / 255.f));
	BeatColors.Emplace(FLinearColor(127 / 255.f, 255 / 255.f, 0 / 255.f));
	BeatColors.Emplace(FLinearColor(0 / 255.f, 255 / 255.f, 0 / 255.f));
	BeatColors.Emplace(FLinearColor(0 / 255.f, 255 / 255.f, 127 / 255.f));
	BeatColors.Emplace(FLinearColor(0 / 255.f, 255 / 255.f, 255 / 255.f));
	BeatColors.Emplace(FLinearColor(0 / 255.f, 127 / 255.f, 255 / 255.f));
	BeatColors.Emplace(FLinearColor(0 / 255.f, 0 / 255.f, 255 / 255.f));
	BeatColors.Emplace(FLinearColor(127 / 255.f, 0 / 255.f, 255 / 255.f));

	FVector CurrentSpawnLoc = InitialVisualizerLocation - VisualizerOffset * (static_cast<float>(AASettings.NumBandChannels - 1) / 2);
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		//CurrentSpawnLoc = i * VisualizerOffset + InitialVisualizerLocation;
		Visualizers.EmplaceAt(i, Cast<ASimpleBeamLight>(GetWorld()->SpawnActor(SimpleBeamLightClass, &CurrentSpawnLoc, &VisualizerRotation, SpawnParameters)));
		Cast<ASimpleBeamLight>(Visualizers[i])->SetLightColor(BeatColors[i]);
		Cast<ASimpleBeamLight>(Visualizers[i])->SetLightIntensities(FLinearColor::Transparent);
		Cast<ASimpleBeamLight>(Visualizers[i])->SetIndex(i);
		CurrentSpawnLoc += VisualizerOffset;
	}
}

void ABeamVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	Cast<ASimpleBeamLight>(Visualizers[Index])->UpdateNiagaraBeam(SpectrumAlpha);
}
