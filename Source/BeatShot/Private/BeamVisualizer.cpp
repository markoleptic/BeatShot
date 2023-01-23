// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamVisualizer.h"
#include "BeamLight.h"
#include "BeamTarget.h"
#include "AudioAnalyzerManager.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "SimpleBeamLight.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABeamVisualizer::ABeamVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABeamVisualizer::BeginPlay()
{
	Super::BeginPlay();
	AASettings = LoadAASettings();
}

// Called every frame
void ABeamVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bAAManagerInitialized || !AAManager)
	{
		return;
	}

	TArray<bool> Beats;
	TArray<float> SpectrumValues;
	TArray<int32> BPMCurrent;
	TArray<int32> BPMTotal;
	AAManager->GetBeatTrackingWLimitsWThreshold(Beats, SpectrumValues, BPMCurrent, BPMTotal,
	                                            AASettings.BandLimitsThreshold);
	AAManager->GetBeatTrackingAverage(AvgSpectrumValues);
	if (SpectrumValues.Num() > 0)
	{
		UpdateLightBeams(SpectrumValues, DeltaTime);
	}
}

void ABeamVisualizer::OnAAPlayerLoaded(UAudioAnalyzerManager* Manager)
{
	AAManager = Manager;
	InitializeLightBeams(AASettings.NumBandChannels);
	bAAManagerInitialized = true;
}

void ABeamVisualizer::InitializeLightBeams(int32 NewNumBandChannels)
{
	MaxAverageValues.SetNum(NewNumBandChannels);
	MaxSpectrumValues.SetNum(NewNumBandChannels);
	/*for (ABeamLight* Light : BeamLights)
	{
		Light->Destroy();
	}

	for (ABeamTarget* Target : BeamTargets)
	{
		Target->Destroy();
	}*/

	for (ASimpleBeamLight* BeamLight : SimpleBeamLights)
	{
		BeamLight->Destroy();
	}

	BeamLights.SetNum(NewNumBandChannels);
	BeamTargets.SetNum(NewNumBandChannels);
	SimpleBeamLights.SetNum(NewNumBandChannels);
	
	FVector Offset = FVector::ZeroVector;
	
	for (int i = 0; i < NewNumBandChannels; i++)
	{
		const FVector& BeamLightSpawnLoc = Offset + InitialBeamLocation;
		const FVector& BeamTargetSpawnLoc = Offset + InitialTargetLocation;

		SimpleBeamLights[i] = Cast<ASimpleBeamLight>(
			GetWorld()->SpawnActor(SimpleBeamLightClass, &BeamLightSpawnLoc, &BeamRotation, SpawnParameters));
		SimpleBeamLights[i]->InitializeBeam();
		/*BeamLights[i] = Cast<ABeamLight>(
			GetWorld()->SpawnActor(BeamLightClass, &BeamLightSpawnLoc, &BeamRotation, SpawnParameters));
		BeamTargets[i] = Cast<ABeamTarget>(
			GetWorld()->SpawnActor(BeamTargetClass, &BeamTargetSpawnLoc, &BeamRotation, SpawnParameters));
		
		BeamLights[i]->InitBeamTarget(BeamTargets[i]);
		BeamLights[i]->InitializeLight();
		BeamTargets[i] = BeamLights[i]->BeamTarget;
		BeamTargets[i]->UpdateBeamLightRefs(BeamLights[i]);*/
		Offset += FVector(100, 0, 0);
	}
}

void ABeamVisualizer::UpdateLightBeams(TArray<float> SpectrumValues, float DeltaTime)
{
	/*bool UpdateValues = false;
	for (int i = 0; i < MaxAverageValues.Num(); i++)
	{
		if (MaxAverageValues[i] < 0 )
		{
			
		}
	}w
	for (const float Value : MaxAverageValues)
	{
		if (Value <= 0)
		{
			UpdateValues = true;
			break;
		}
	}*/
	
	for (int i = 0; i < SpectrumValues.Num(); i++)
	{
		if (SpectrumValues[i] > MaxSpectrumValues[i])
		{
			MaxSpectrumValues[i] = SpectrumValues[i];
		}
		if ((SpectrumValues[i] > MaxAverageValues[i]) &&
			(MaxAverageValues[i] <= 0) &&
			(SpectrumValues[i] > MaxSpectrumValues[i]/2))
		{
			MaxAverageValues[i] = SpectrumValues[i];
			const float ScaledValue = UKismetMathLibrary::MapRangeClamped(MaxAverageValues[i], 0, MaxSpectrumValues[i], 0, 1);
			SimpleBeamLights[i]->UpdateNiagaraBeam(ScaledValue);
		}
	}
	
	/*for (int i = 0; i < MaxAverageValues.Num(); i++)
	{
		
		const FVector CurrentLoc = BeamTargets[i]->GetActorLocation();
		const float ScaledValue = UKismetMathLibrary::MapRangeClamped(MaxAverageValues[i], 0, MaxSpectrumValues[i], 0, 2);
		FVector NewLoc;
		FVector UpdatedRotation;
		if (MaxAverageValues[i] < 0)
		{
			NewLoc = CurrentLoc + FVector(0, 0, -MaxAverageValues[i] * 100);
		}
		else
		{
			NewLoc = FVector(100 * i, 0, 0) + InitialTargetLocation;
		}
		BeamLights[i]->LightIntensity = ScaledValue;
		BeamLights[i]->LightMultiplier = FLinearColor(1, 1, 1, MaxAverageValues[i]);
		UpdatedRotation = UKismetMathLibrary::VInterpTo(CurrentLoc, NewLoc, DeltaTime, 1);
		BeamTargets[i]->SetActorLocation(UpdatedRotation);
	}*/

	for (float& Value : MaxAverageValues)
	{
		if (Value >= 0)
		{
			Value -= 0.0005;
		}
	}
}

void ABeamVisualizer::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	bAAManagerInitialized = false;
	InitializeLightBeams(AASettingsStruct.NumBandChannels);
	AASettings = AASettingsStruct;
	bAAManagerInitialized = true;
}


