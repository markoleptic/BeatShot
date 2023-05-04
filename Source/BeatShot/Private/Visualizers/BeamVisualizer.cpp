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
}

void ABeamVisualizer::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	Super::InitializeVisualizer(InAASettings);
	
	FVector CurrentSpawnLoc = InitialVisualizerLocation - VisualizerOffset * (static_cast<float>(BaseConfig.NumVisualizerLightsToSpawn - 1) / 2);
	for (int i = 0; i < BaseConfig.NumVisualizerLightsToSpawn; i++)
	{
		ASimpleBeamLight* Light = GetWorld()->SpawnActorDeferred<ASimpleBeamLight>(SimpleBeamLightClass, FTransform(VisualizerRotation, CurrentSpawnLoc), this);
		FSimpleBeamLightConfig Config;
		Config = BeamVisualizerConfig.SimpleBeamLightConfig;
		Config.Index = i;
		
		if (BaseConfig.bOverrideChildLightColors)
		{
			Config.LightColor = BaseConfig.BeamLightColors[Config.Index % BaseConfig.BeamLightColors.Num()];
		}
		
		Light->InitSimpleBeamLight(Config);
		SimpleBeamLights.AddUnique(Light);
		Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
		Light->FinishSpawning(FTransform(), true);
		CurrentSpawnLoc += VisualizerOffset;
	}
}

void ABeamVisualizer::InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	Super::InitializeVisualizerFromWorld(InAASettings);
	int i = 0;
	for (const TObjectPtr<ASimpleBeamLight> Light : GetSimpleBeamLights())
	{
		FSimpleBeamLightConfig Config = Light->GetSimpleBeamLightConfig();
		if (BaseConfig.bOverrideChildLightConfig)
		{
			Config = BeamVisualizerConfig.SimpleBeamLightConfig;
		}
		
		Config.Index = i;
		if (BaseConfig.bOverrideChildLightColors)
		{
			Config.LightColor = BaseConfig.BeamLightColors[Config.Index % BaseConfig.BeamLightColors.Num()];
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
		UE_LOG(LogTemp, Display, TEXT("Index To Activate: %d"), LightIndex);
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

void ABeamVisualizer::SpawnBeamLight(const FLinearColor& Color, const float InLightDuration)
{
	ASimpleBeamLight* Light = GetWorld()->SpawnActorDeferred<ASimpleBeamLight>(SimpleBeamLightClass, FTransform(VisualizerRotation, GetActorLocation() + VisualizerOffset), this);
	FSimpleBeamLightConfig Config = BeamVisualizerConfig.SimpleBeamLightConfig;
	Config.Index = GetSimpleBeamLights().Num();
	
	if (BaseConfig.bOverrideChildLightColors)
	{
		Config.LightColor = BaseConfig.BeamLightColors[Config.Index % BaseConfig.BeamLightColors.Num()];
	}
	
	Light->InitSimpleBeamLight(Config);
	Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
	SimpleBeamLights.AddUnique(Light);
	Light->FinishSpawning(FTransform(), true);
}

void ABeamVisualizer::AddBeamLightFromWorld(const TSoftObjectPtr<ASimpleBeamLight>& InBeamLight)
{
	InBeamLight->InitSimpleBeamLight(InBeamLight->GetSimpleBeamLightConfig());
	InBeamLight->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
	SimpleBeamLights.AddUnique(InBeamLight.Get());
}
