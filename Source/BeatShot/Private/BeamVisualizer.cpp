// Fill out your copyright notice in the Description page of Project Settings.


#include "BeamVisualizer.h"
#include "SimpleBeamLight.h"

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
	SetInitialLocation(InitialBeamLocation);
	SetRotation(BeamRotation);
	SetVisualizerOffset(BeamOffset);
}

// Called every frame
void ABeamVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABeamVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();

	for (AActor* Visualizer : Visualizers)
	{
		Visualizer->Destroy();
	}

	Visualizers.Empty();
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		const FVector& CurrentSpawnLoc = i * VisualizerOffset + InitialVisualizerLocation;
		Visualizers.EmplaceAt(i, Cast<ASimpleBeamLight>(
			                      GetWorld()->SpawnActor(SimpleBeamLightClass, &CurrentSpawnLoc, &VisualizerRotation,
			                                             SpawnParameters)));
	}
}

void ABeamVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	Cast<ASimpleBeamLight>(Visualizers[Index])->UpdateNiagaraBeam(SpectrumAlpha);
}

// for (ASimpleBeamLight* BeamLight : SimpleBeamLights)
// {
// 	BeamLight->Destroy();
// }
// SimpleBeamLights.SetNum(AASettings.NumBandChannels);

//SimpleBeamLights[i] = Cast<ASimpleBeamLight>(
//	GetWorld()->SpawnActor(SimpleBeamLightClass, &BeamLightSpawnLoc, &BeamRotation, SpawnParameters));

//#include "BeamLight.h"
//#include "BeamTarget.h"
//#include "NiagaraComponent.h"
//#include "NiagaraFunctionLibrary.h"

/*for (ABeamLight* Light : BeamLights)
{
	Light->Destroy();
}

for (ABeamTarget* Target : BeamTargets)
{
	Target->Destroy();
}*/

//BeamLights.SetNum(AASettings.NumBandChannels);
//BeamTargets.SetNum(AASettings.NumBandChannels);

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

/*BeamLights[i] = Cast<ABeamLight>(
	GetWorld()->SpawnActor(BeamLightClass, &BeamLightSpawnLoc, &BeamRotation, SpawnParameters));
BeamTargets[i] = Cast<ABeamTarget>(
	GetWorld()->SpawnActor(BeamTargetClass, &BeamTargetSpawnLoc, &BeamRotation, SpawnParameters));

BeamLights[i]->InitBeamTarget(BeamTargets[i]);
BeamLights[i]->InitializeLight();
BeamTargets[i] = BeamLights[i]->BeamTarget;
BeamTargets[i]->UpdateBeamLightRefs(BeamLights[i]);*/

// for (int i = 0; i < SpectrumValues.Num(); i++)
// {
// 	if (SpectrumValues[i] > MaxSpectrumValues[i])
// 	{
// 		MaxSpectrumValues[i] = SpectrumValues[i];
// 	}
// 	if ((SpectrumValues[i] > MaxAverageValues[i]) &&
// 		(MaxAverageValues[i] <= 0) &&
// 		(SpectrumValues[i] > MaxSpectrumValues[i]/2))
// 	{
// 		MaxAverageValues[i] = SpectrumValues[i];
// 		const float ScaledValue = UKismetMathLibrary::MapRangeClamped(MaxAverageValues[i], 0, MaxSpectrumValues[i], 0, 1);
// 		SimpleBeamLights[i]->UpdateNiagaraBeam(ScaledValue);
// 	}
// }

// for (int i = 0; i < SpectrumValues.Num(); i++)
// {
// 	if (SpectrumValues[i] > 0)
// 	{
// 		SimpleBeamLights[i]->UpdateNiagaraBeam(SpectrumValues[i]);
// 	}
// }

//SimpleBeamLights[Index]->UpdateNiagaraBeam(SpectrumAlpha);

// for (float& Value : MaxAverageValues)
// {
// 	if (Value >= 0)
// 	{
// 		Value -= 0.0005;
// 	}
// }
