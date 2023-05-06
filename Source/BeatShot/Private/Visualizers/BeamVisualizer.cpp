// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Visualizers/BeamVisualizer.h"
#include "Visualizers/SimpleBeamLight.h"

ABeamVisualizer::ABeamVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void ABeamVisualizer::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	Super::InitializeVisualizer(InAASettings);
	FBaseVisualizerConfig VisualizerConfig = GetConfig();
	
	if (GetSimpleBeamLights().Num() >= VisualizerConfig.NumVisualizerLightsToSpawn)
	{
		return;
	}
	
	FVector LocationOffset = VisualizerConfig.StartLocation;
	FRotator RotationOffset = VisualizerConfig.StartRotation;
	FVector ScaleOffset = VisualizerConfig.StartScale;
	
	/* Spawn "from left to right" in whatever context that is */
	if (VisualizerConfig.bGrowFromCenter)
	{
		LocationOffset = LocationOffset - VisualizerConfig.OffsetLocation * (static_cast<float>(VisualizerConfig.NumVisualizerLightsToSpawn - 1) / 2);
		RotationOffset = RotationOffset - VisualizerConfig.OffsetRotation * (static_cast<float>(VisualizerConfig.NumVisualizerLightsToSpawn - 1) / 2);
		float Exponent = static_cast<float>(VisualizerConfig.NumVisualizerLightsToSpawn - 1) / 2;
		ScaleOffset = FVector(FMath::Pow(VisualizerConfig.OffsetScale.X, Exponent), FMath::Pow(VisualizerConfig.OffsetScale.Y, Exponent), FMath::Pow(VisualizerConfig.OffsetScale.Z, Exponent));
	}

	FTransform CurrentOffsetTransform(RotationOffset, LocationOffset, ScaleOffset);
	for (int i = 0; i < VisualizerConfig.NumVisualizerLightsToSpawn; i++)
	{
		ASimpleBeamLight* Light = GetWorld()->SpawnActorDeferred<ASimpleBeamLight>(BeamVisualizerConfig.SimpleBeamLightClass, CurrentOffsetTransform, this);
		FSimpleBeamLightConfig Config;
		Config = BeamVisualizerConfig.SimpleBeamLightConfig;
		Config.Index = i;
		if (VisualizerConfig.bOverrideChildLightColors)
		{
			Config.LightColor = VisualizerConfig.BeamLightColors[Config.Index % VisualizerConfig.BeamLightColors.Num()];
		}
		Light->InitSimpleBeamLight(Config);
		BeamVisualizerConfig.SimpleBeamLights.AddUnique(Light);
		Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
		Light->FinishSpawning(CurrentOffsetTransform, true);
		
		CurrentOffsetTransform = FTransform(CurrentOffsetTransform.Rotator() + VisualizerConfig.OffsetRotation,
			CurrentOffsetTransform.GetLocation() + VisualizerConfig.OffsetLocation, 
			CurrentOffsetTransform.GetScale3D() * VisualizerConfig.OffsetScale);
	}
}

void ABeamVisualizer::InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings, const int32 NumSpawnedVisualizers)
{
	Super::InitializeVisualizerFromWorld(InAASettings, GetBeamVisualizerConfig().SimpleBeamLights.Num());
	FBaseVisualizerConfig VisualizerConfig = GetConfig();
	int i = 0;
	for (const TObjectPtr<ASimpleBeamLight> Light : GetSimpleBeamLights())
	{
		FSimpleBeamLightConfig Config = Light->GetSimpleBeamLightConfig();
		if (VisualizerConfig.bOverrideChildLightConfig)
		{
			Config = BeamVisualizerConfig.SimpleBeamLightConfig;
		}
		
		Config.Index = i;
		if (VisualizerConfig.bOverrideChildLightColors)
		{
			Config.LightColor = VisualizerConfig.BeamLightColors[Config.Index % VisualizerConfig.BeamLightColors.Num()];
		}
		
		Light->InitSimpleBeamLight(Config);
		Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
		i++;
	}
}

void ABeamVisualizer::ActivateVisualizer(const int32 Index)
{
	for (const int32 LightIndex : GetLightIndices(Index))
	{
		if (ActiveLightIndices.Contains(LightIndex))
		{
			return;
		}
		GetSimpleBeamLights()[LightIndex]->ActivateLightComponents();
		ActiveLightIndices.Add(LightIndex);
	}
}

void ABeamVisualizer::DeactivateVisualizers()
{
	for (const TObjectPtr<ASimpleBeamLight> Light : GetSimpleBeamLights())
	{
		Light->DeactivateLightComponents();
	}
	ActiveLightIndices.Empty();
}

void ABeamVisualizer::OnBeamLightLifetimeCompleted(const int32 IndexToRemove)
{
	if (const int32 Index = ActiveLightIndices.Find(IndexToRemove); Index != INDEX_NONE)
	{
		ActiveLightIndices.Remove(IndexToRemove);
	}
}

void ABeamVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
}
