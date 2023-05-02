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
}

void AVisualizerBase::InitializeVisualizerFromWorld()
{
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
