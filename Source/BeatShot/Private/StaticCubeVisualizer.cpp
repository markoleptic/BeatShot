// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticCubeVisualizer.h"
#include "Kismet/KismetMathLibrary.h"

AStaticCubeVisualizer::AStaticCubeVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AStaticCubeVisualizer::BeginPlay()
{
	Super::BeginPlay();
	SetVisualizerOffset(CubeOffset);
}

void AStaticCubeVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStaticCubeVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();
	for (UStaticMeshComponent* Cube : Cubes)
	{
		Cube->DestroyComponent();
	}

	Cubes.Empty();

	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		Cubes.EmplaceAt(
			i, NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("Cube " + i)));
		UStaticMeshComponent* Visualizer = Cubes[i];
		Visualizer->RegisterComponent();
		Visualizer->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		Visualizer->SetRelativeLocation(i * VisualizerOffset);
		Visualizer->SetStaticMesh(CubeMesh);
		Visualizer->SetRenderCustomDepth(true);
		Visualizer->SetCustomDepthStencilValue(0);
		Visualizer->SetMaterial(0, CubeMaterial);
	}
}

void AStaticCubeVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	Cubes[Index]->SetRelativeScale3D(FVector(1, 1, GetScaledHeight(SpectrumAlpha)));
	Cubes[Index]->SetScalarParameterValueOnMaterials("RedGreenAlpha", SpectrumAlpha);
}

float AStaticCubeVisualizer::GetScaledHeight(const float SpectrumValue) const
{
	return UKismetMathLibrary::MapRangeClamped(SpectrumValue, 0, 1, MinCubeHeightScale, MaxCubeHeightScale);
}
