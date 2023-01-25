// Fill out your copyright notice in the Description page of Project Settings.


#include "StaticCubeVisualizer.h"
#include "DefaultGameMode.h"
#include "Kismet/KismetMathLibrary.h"

AStaticCubeVisualizer::AStaticCubeVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AStaticCubeVisualizer::BeginPlay()
{
	Super::BeginPlay();
	SetVisualizerOffset(FVector(0,-110, 0));
}

void AStaticCubeVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AStaticCubeVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();

	for (AActor* Visualizer : Visualizers)
	{
		if (Cast<USceneComponent>(Visualizer))
		{
			Cast<USceneComponent>(Visualizer)->DestroyComponent();
		}
	}

	for (UStaticMeshComponent* Cube : Cubes)
	{
		Cube->DestroyComponent();
	}
	
	Cubes.Empty();
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		Cubes.EmplaceAt(i, NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), TEXT("Cube " + i)));
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
	Cubes[Index]->SetRelativeScale3D(FVector(1,1,GetScaledHeight(SpectrumAlpha)));
	Cubes[Index]->SetScalarParameterValueOnMaterials("RedGreenAlpha", SpectrumAlpha);
}

float AStaticCubeVisualizer::GetScaledHeight(const float SpectrumValue)
{
	return UKismetMathLibrary::MapRangeClamped(SpectrumValue, 0, 1, 1, 5);
}

// for (int i = 0; i < SpectrumValues.Num(); i++)
// {
// 	if (SpectrumValues[i] > MaxAverageValues[i])
// 	{
// 		MaxAverageValues[i] = SpectrumValues[i];
// 	}
// }
	
// for (int i = 0; i < MaxAverageValues.Num(); i++)
// {
// 	const FVector CurrentLoc = Cubes[i]->GetRelativeLocation();
// 	Cubes[i]->SetRelativeScale3D(FVector(1,1,MaxAverageValues[i] * 20 + 1));
// 	Cubes[i]->SetRelativeLocation(FVector(CurrentLoc.X, CurrentLoc.Y, MaxAverageValues[i] * 100));
// 	Cubes[i]->SetScalarParameterValueOnMaterials("value", MaxAverageValues[i] * 100);
// }
// for (float& Value : MaxAverageValues)
// {
// 	Value -= 0.01;
// }

// for (int i = 0; i < SpectrumValues.Num(); i++)
// {
// 	if (SpectrumValues[i] > LocalSpectrumValues[i])
// 	{
// 		LocalSpectrumValues[i] = SpectrumValues[i];
// 	}
// 	Cubes[i]->SetRelativeScale3D(FVector(1,1,GetScaledHeight(LocalSpectrumValues[i])));
// 	Cubes[i]->SetScalarParameterValueOnMaterials("RedGreenAlpha", LocalSpectrumValues[i]);
// 	if (LocalSpectrumValues[i] >= 0)
// 	{
// 		LocalSpectrumValues[i]-= 0.005;
// 	}
// }

