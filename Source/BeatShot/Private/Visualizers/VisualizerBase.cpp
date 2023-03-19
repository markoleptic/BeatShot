﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Visualizers/VisualizerBase.h"

AVisualizerBase::AVisualizerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AVisualizerBase::BeginPlay()
{
	Super::BeginPlay();
	AASettings = LoadAASettings();
}

void AVisualizerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualizerBase::Destroyed()
{
	Super::Destroyed();
	for (AActor* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}
	Visualizers.Empty();
}

void AVisualizerBase::InitializeVisualizer()
{
	for (AActor* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}
	Visualizers.Empty();
}

void AVisualizerBase::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
}

void AVisualizerBase::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	AASettings = AASettingsStruct;
	InitializeVisualizer();
}