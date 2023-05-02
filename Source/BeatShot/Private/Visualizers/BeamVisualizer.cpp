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
	
	FVector CurrentSpawnLoc = InitialVisualizerLocation - VisualizerOffset * (static_cast<float>(AASettings.NumBandChannels - 1) / 2);
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		ASimpleBeamLight* Light = GetWorld()->SpawnActorDeferred<ASimpleBeamLight>(SimpleBeamLightClass, FTransform(VisualizerRotation, CurrentSpawnLoc), this);
		if (i != 0)
		{
			Light->InitSimpleBeamLight(BeamLightColors[BeamLightColors.Num() % i], i, BeamLightLifetimes[BeamLightLifetimes.Num() % i], bMovingLights);
		}
		else
		{
			Light->InitSimpleBeamLight(BeamLightColors[0], 0, BeamLightLifetimes[0], bMovingLights);
		}
		Visualizers.Add(Light);
		Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
		Light->FinishSpawning(FTransform(), true);
		CurrentSpawnLoc += VisualizerOffset;
	}
	
	BeamLights.Empty();
	for (const TObjectPtr<AActor> Light : GetVisualizers())
	{
		BeamLights.Add(Cast<ASimpleBeamLight>(Light));
	}
}

void ABeamVisualizer::InitializeVisualizerFromWorld()
{
	int i = 0;
	for (const TObjectPtr<AActor> Light : GetVisualizers())
	{
		if (ASimpleBeamLight* BeamLight = Cast<ASimpleBeamLight>(Light))
		{
			if (i != 0)
			{
				BeamLight->InitSimpleBeamLight(BeamLightColors[BeamLightColors.Num() % i], i, BeamLightLifetimes[BeamLightLifetimes.Num() % i], bMovingLights);
			}
			else
			{
				BeamLight->InitSimpleBeamLight(BeamLightColors[0], 0, BeamLightLifetimes[0], bMovingLights);
			}
			BeamLight->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
			BeamLights.Add(BeamLight);
		}
		i++;
	}
}

void ABeamVisualizer::ActivateVisualizer(const int32 Index)
{
	if (ActiveLightIndices.Contains(Index))
	{
		return;
	}
	GetSimpleBeamLights()[Index]->ActivateLightComponents();
	ActiveLightIndices.Add(Index);
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
	Light->InitSimpleBeamLight(Color, GetSimpleBeamLights().Num(), InLightDuration, false);
	Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
	Visualizers.Add(Light);
	BeamLights.Add(Cast<ASimpleBeamLight>(Light));
	Light->FinishSpawning(FTransform(), true);
}

void ABeamVisualizer::AddBeamLightFromWorld(const TSoftObjectPtr<ASimpleBeamLight>& InBeamLight)
{
	InBeamLight->InitSimpleBeamLight(InBeamLight->GetLightColor(), GetSimpleBeamLights().Num(), InBeamLight->GetLightDuration(), false);
	InBeamLight->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
	Visualizers.Add(InBeamLight.Get());
	BeamLights.Add(InBeamLight.Get());
}
