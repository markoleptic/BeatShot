#include "VisualizerBase.h"

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

void AVisualizerBase::InitializeVisualizer()
{
}

void AVisualizerBase::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
}

void AVisualizerBase::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	AASettings = AASettingsStruct;
	InitializeVisualizer();
}
