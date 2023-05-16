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
	
	BeamVisualizerDefinition = Cast<UBeamVisualizerDefinition>(VisualizerDefinition.GetDefaultObject());
	
	if (GetSimpleBeamLights().Num() >= GetFastDef().NumVisualizerLightsToSpawn)
	{
		return;
	}

	TArray<FTransform> SpawnTransforms;
	
	switch(BeamVisualizerDefinition->VisualizerLightSpawningMethod)
	{
	case EVisualizerLightSpawningMethod::SpawnUsingPositionOffsets:
		{
			// Default Start Location
			FTransform CurrentOffsetTransform(GetFastDef().Rotation, GetFastDef().Location, GetFastDef().Scale);

			// If growing from center, start location will be offset by half the total length, growing in the positive direction
			if (GetFastDef().bGrowFromCenter)
			{
				FVector LocationOffset = GetFastDef().Location - GetFastDef().OffsetLocation * (static_cast<float>(GetFastDef().NumVisualizerLightsToSpawn - 1) / 2);
				FRotator RotationOffset = GetFastDef().Rotation - GetFastDef().OffsetRotation * (static_cast<float>(GetFastDef().NumVisualizerLightsToSpawn - 1) / 2);
				float Exponent = static_cast<float>(GetFastDef().NumVisualizerLightsToSpawn - 1) / 2;
				FVector ScaleOffset(FMath::Pow(GetFastDef().OffsetScale.X, Exponent), FMath::Pow(GetFastDef().OffsetScale.Y, Exponent), FMath::Pow(GetFastDef().OffsetScale.Z, Exponent));
				CurrentOffsetTransform = FTransform(RotationOffset, LocationOffset, ScaleOffset);
			}
		
			SpawnTransforms.Add(CurrentOffsetTransform);
		
			for (int i = 0; i < GetFastDef().NumVisualizerLightsToSpawn - 1; i++)
			{
				CurrentOffsetTransform = FTransform(CurrentOffsetTransform.Rotator() + GetFastDef().OffsetRotation,
				CurrentOffsetTransform.GetLocation() + GetFastDef().OffsetLocation, 
				CurrentOffsetTransform.GetScale3D() * GetFastDef().OffsetScale);
				SpawnTransforms.Add(CurrentOffsetTransform);
			}
			break;
		}
	case EVisualizerLightSpawningMethod::SpawnUsingSpline:
		{
			const TArray<FVector> SplineLocations = GetSplinePointLocations();
			for (int i = 0; i < SplineLocations.Num(); i++)
			{
				SpawnTransforms.Emplace(GetFastDef().SplineActorRotationOffset, SplineLocations[i], GetFastDef().Scale);
			}
			break;
		}
	case EVisualizerLightSpawningMethod::AddExistingLightsFromLevel:
		return;
	}
	
	for (int i = 0; i < SpawnTransforms.Num(); i++)
	{
		UE_LOG(LogTemp, Display, TEXT("Transfrom: %s"), *SpawnTransforms[i].ToString());
		ASimpleBeamLight* Light = GetWorld()->SpawnActorDeferred<ASimpleBeamLight>(GetFastDef().VisualizerLightClass, SpawnTransforms[i], this);
		FSimpleBeamLightConfig Config;
		Config.bUseSpotlight = GetFastDef().bUseSpotlight;
		Config.LightDuration = GetFastDef().LightDuration;
		Config.NiagaraSystem = GetFastDef().NiagaraSystem;
		Config.InnerConeAngle = GetFastDef().InnerConeAngle;
		Config.LightMovementCurve = GetFastDef().LightMovementCurve;
		Config.MaxSpotlightIntensity = GetFastDef().MaxSpotlightIntensity;
		Config.NiagaraColorParameters = GetFastDef().NiagaraColorParameters;
		Config.NiagaraFloatParameters = GetFastDef().NiagaraFloatParameters;
		Config.OuterConeAngle = GetFastDef().OuterConeAngle;
		Config.bAutoCalculateBeamLength = GetFastDef().bAutoCalculateBeamLength;
		Config.bUseBeamEndLight = GetFastDef().bUseBeamEndLight;
		Config.MaxBeamEndLightIntensity = GetFastDef().MaxBeamEndLightIntensity;
		Config.MaxEmissiveLightBulbLightIntensity = GetFastDef().MaxEmissiveLightBulbLightIntensity;
		Config.bIsMovingLight = GetFastDef().bIsMovingLight;
		Config.Index = i;
		
		if (GetFastDef().bOverrideLightColors)
		{
			Config.LightColor = GetFastDef().BeamLightColors[Config.Index % GetFastDef().BeamLightColors.Num()];
		}
		
		Light->InitSimpleBeamLight(Config);
		BeamLights.AddUnique(Light);
		Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
		Light->FinishSpawning(SpawnTransforms[i], true);
	}
}

void ABeamVisualizer::InitializeVisualizerFromWorld(const FPlayerSettings_AudioAnalyzer& InAASettings, const int32 NumSpawnedVisualizers)
{
	BeamVisualizerDefinition = Cast<UBeamVisualizerDefinition>(VisualizerDefinition.GetDefaultObject());
	
	Super::InitializeVisualizerFromWorld(InAASettings, BeamVisualizerDefinition->VisualizerLightsFromLevel.Num());
	
	int i = 0;
	for (const TSoftObjectPtr<AActor>& Actor : GetFastDef().VisualizerLightsFromLevel)
	{
		if (ASimpleBeamLight* Light = Cast<ASimpleBeamLight>(Actor.Get()))
		{
			FSimpleBeamLightConfig Config = Light->GetSimpleBeamLightConfig();
			if (GetFastDef().bOverrideChildLightConfig)
			{
				Config.bUseSpotlight = GetFastDef().bUseSpotlight;
				Config.LightDuration = GetFastDef().LightDuration;
				Config.NiagaraSystem = GetFastDef().NiagaraSystem;
				Config.InnerConeAngle = GetFastDef().InnerConeAngle;
				Config.LightMovementCurve = GetFastDef().LightMovementCurve;
				Config.MaxSpotlightIntensity = GetFastDef().MaxSpotlightIntensity;
				Config.NiagaraColorParameters = GetFastDef().NiagaraColorParameters;
				Config.NiagaraFloatParameters = GetFastDef().NiagaraFloatParameters;
				Config.OuterConeAngle = GetFastDef().OuterConeAngle;
				Config.bAutoCalculateBeamLength = GetFastDef().bAutoCalculateBeamLength;
				Config.bUseBeamEndLight = GetFastDef().bUseBeamEndLight;
				Config.MaxBeamEndLightIntensity = GetFastDef().MaxBeamEndLightIntensity;
				Config.MaxEmissiveLightBulbLightIntensity = GetFastDef().MaxEmissiveLightBulbLightIntensity;
				Config.bIsMovingLight = GetFastDef().bIsMovingLight;
			}
		
			Config.Index = i;
			
			if (GetFastDef().bOverrideLightColors)
			{
				Config.LightColor = GetFastDef().BeamLightColors[Config.Index % GetFastDef().BeamLightColors.Num()];
			}
		
			Light->InitSimpleBeamLight(Config);
			Light->OnBeamLightLifetimeCompleted.AddUObject(this, &ABeamVisualizer::OnBeamLightLifetimeCompleted);
			BeamLights.AddUnique(Light);
			
			i++;
		}
	}
	GetFastDef().VisualizerLightsFromLevel.Empty();
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
