// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticCubeVisualizer.h"
#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"

// Sets default values
AStaticCubeVisualizer::AStaticCubeVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AStaticCubeVisualizer::BeginPlay()
{
	Super::BeginPlay();
	AASettings = LoadAASettings();
}

// Called every frame
void AStaticCubeVisualizer::Tick(float DeltaTime)
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
	UpdateCubes(SpectrumValues);
}

void AStaticCubeVisualizer::OnAAPlayerLoaded(UAudioAnalyzerManager* Manager)
{
	AAManager = Manager;
	InitializeCubes(AASettings.NumBandChannels);
	bAAManagerInitialized = true;
}

void AStaticCubeVisualizer::InitializeCubes(const int32 NewNumBandChannels)
{
	MaxAverageValues.SetNum(NewNumBandChannels);
	for (UStaticMeshComponent* Comp : Cubes)
	{
		Comp->DestroyComponent();
	}
	Cubes.SetNum(NewNumBandChannels);
	for (int i = 0; i < NewNumBandChannels; i++)
	{
		Cubes[i] = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("Cube " + i));
		Cubes[i]->RegisterComponent();
		Cubes[i]->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		Cubes[i]->SetRelativeLocation(FVector(0,i * -110, 0));
		Cubes[i]->SetStaticMesh(CubeMesh);
		Cubes[i]->SetRenderCustomDepth(true);
		Cubes[i]->SetCustomDepthStencilValue(0);
		Cubes[i]->SetMaterial(0, CubeMaterial);
	}
}

void AStaticCubeVisualizer::UpdateCubes(TArray<float> SpectrumValues)
{
	for (int i = 0; i < SpectrumValues.Num(); i++)
	{
		if (SpectrumValues[i] > MaxAverageValues[i])
		{
			MaxAverageValues[i] = SpectrumValues[i];
		}
	}
	for (int i = 0; i < MaxAverageValues.Num(); i++)
	{
		const FVector CurrentLoc = Cubes[i]->GetRelativeLocation();
		Cubes[i]->SetRelativeScale3D(FVector(1,1,MaxAverageValues[i] * 20 + 1));
		Cubes[i]->SetRelativeLocation(FVector(CurrentLoc.X, CurrentLoc.Y, MaxAverageValues[i] * 100));
		Cubes[i]->SetScalarParameterValueOnMaterials("value", MaxAverageValues[i] * 100);
	}
	for (float& Value : MaxAverageValues)
	{
		Value -= 0.01;
	}
}

void AStaticCubeVisualizer::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	 bAAManagerInitialized = false;
	 InitializeCubes(AASettingsStruct.NumBandChannels);
	 AASettings = AASettingsStruct;
	 bAAManagerInitialized = true;
}

