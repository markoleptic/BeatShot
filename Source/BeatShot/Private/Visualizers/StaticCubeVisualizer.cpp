// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Visualizers/StaticCubeVisualizer.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

AStaticCubeVisualizer::AStaticCubeVisualizer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InstancedBaseMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("InstancedBaseMesh"));
	InstancedVerticalOutlineMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("InstancedVerticalOutlineMesh"));
	InstancedTopMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("InstancedTopMesh"));
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(FName("RootSceneComponent"));
	RootComponent = RootSceneComponent;
	InstancedBaseMesh->SetupAttachment(RootSceneComponent);
	InstancedVerticalOutlineMesh->SetupAttachment(RootSceneComponent);
	InstancedTopMesh->SetupAttachment(RootSceneComponent);
}

void AStaticCubeVisualizer::BeginPlay()
{
	Super::BeginPlay();
	SetVisualizerOffset(DefaultCubeVisualizerOffset);
}

void AStaticCubeVisualizer::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	Super::InitializeVisualizer(InAASettings);

	InstancedBaseMesh->ClearInstances();
	InstancedVerticalOutlineMesh->ClearInstances();
	InstancedTopMesh->ClearInstances();
	
	for (int i = 0; i < BaseConfig.NumVisualizerLightsToSpawn; i++)
	{
		AddInstancedCubeMesh(i * CubeVisualizerConfig.CubeVisualizerOffset);
	}
}

void AStaticCubeVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	for (const int32 LightIndex : GetLightIndices(Index))
	{
		const float NewHeightScale = GetScaledHeight(SpectrumAlpha);
		const FTransform BaseAndSideTransform(FRotator(), LightIndex * CubeVisualizerConfig.CubeVisualizerOffset,
			FVector(CubeVisualizerConfig.MeshScale, CubeVisualizerConfig.MeshScale, CubeVisualizerConfig.MeshScale * NewHeightScale));

		const float CustomDataValue = (NewHeightScale - CubeVisualizerConfig.MinCubeVisualizerHeightScale) /
			(CubeVisualizerConfig.MaxCubeVisualizerHeightScale - CubeVisualizerConfig.MinCubeVisualizerHeightScale);
	
		InstancedBaseMesh->UpdateInstanceTransform(LightIndex, BaseAndSideTransform);
		InstancedVerticalOutlineMesh->UpdateInstanceTransform(LightIndex, BaseAndSideTransform);
		InstancedTopMesh->UpdateInstanceTransform(LightIndex, FTransform(FRotator(), LightIndex * CubeVisualizerConfig.CubeVisualizerOffset
			+ FVector(0, 0, CubeVisualizerConfig.CubeHeight * (NewHeightScale - 1)), FVector(CubeVisualizerConfig.MeshScale)));

		InstancedBaseMesh->SetCustomDataValue(LightIndex, 0, CustomDataValue, false);
		InstancedVerticalOutlineMesh->SetCustomDataValue(LightIndex, 0, CustomDataValue, false);
		InstancedTopMesh->SetCustomDataValue(LightIndex, 0, CustomDataValue, false);
	}
}

void AStaticCubeVisualizer::MarkRenderStateDirty()
{
	InstancedBaseMesh->MarkRenderStateDirty();
	InstancedVerticalOutlineMesh->MarkRenderStateDirty();
	InstancedTopMesh->MarkRenderStateDirty();
}

float AStaticCubeVisualizer::GetScaledHeight(const float SpectrumValue) const
{
	return UKismetMathLibrary::MapRangeClamped(SpectrumValue, 0, 1, DefaultMinCubeVisualizerHeightScale, DefaultMaxCubeVisualizerHeightScale);
}

void AStaticCubeVisualizer::AddInstancedCubeMesh(const FVector& RelativePosition)
{
	const int32 BaseMeshIndex = InstancedBaseMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(CubeVisualizerConfig.MeshScale)), false);
	InstancedBaseMesh->SetMaterial(BaseMeshIndex, BaseCubeMaterial);
	const int32 OutlineMeshIndex = InstancedVerticalOutlineMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(CubeVisualizerConfig.MeshScale)), false);
	InstancedVerticalOutlineMesh->SetMaterial(OutlineMeshIndex, OutlineMaterial);
	const int32 TopMeshIndex = InstancedTopMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(CubeVisualizerConfig.MeshScale)), false);
	InstancedTopMesh->SetMaterial(TopMeshIndex, OutlineMaterial);
}
