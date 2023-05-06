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

void AStaticCubeVisualizer::InitializeVisualizer(const FPlayerSettings_AudioAnalyzer& InAASettings)
{
	Super::InitializeVisualizer(InAASettings);

	InstancedBaseMesh->ClearInstances();
	InstancedVerticalOutlineMesh->ClearInstances();
	InstancedTopMesh->ClearInstances();

	const FBaseVisualizerConfig VisualizerConfig = GetConfig();
	
	for (int i = 0; i < VisualizerConfig.NumVisualizerLightsToSpawn; i++)
	{
		AddInstancedCubeMesh(FTransform(i * VisualizerConfig.OffsetRotation,
			i * VisualizerConfig.OffsetLocation,
			VisualizerConfig.OffsetScale * CubeVisualizerConfig.MeshScale));
	}
}

void AStaticCubeVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	for (const int32 LightIndex : GetLightIndices(Index))
	{
		const float CustomDataValue = (GetScaledHeight(SpectrumAlpha) - CubeVisualizerConfig.MinCubeVisualizerHeightScale) /
			(CubeVisualizerConfig.MaxCubeVisualizerHeightScale - CubeVisualizerConfig.MinCubeVisualizerHeightScale);
	
		InstancedBaseMesh->UpdateInstanceTransform(LightIndex, GetSideAndBaseTransform(LightIndex, SpectrumAlpha));
		InstancedVerticalOutlineMesh->UpdateInstanceTransform(LightIndex, GetSideAndBaseTransform(LightIndex, SpectrumAlpha));
		InstancedTopMesh->UpdateInstanceTransform(LightIndex, GetTopMeshTransform(LightIndex, SpectrumAlpha));
		
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
	return UKismetMathLibrary::MapRangeClamped(SpectrumValue, 0, 1, CubeVisualizerConfig.MinCubeVisualizerHeightScale, CubeVisualizerConfig.MaxCubeVisualizerHeightScale);
}

FVector AStaticCubeVisualizer::GetScale3D(const float ScaledHeight)
{
	return FVector(CubeVisualizerConfig.MeshScale * GetConfig().OffsetScale.X,
		CubeVisualizerConfig.MeshScale * GetConfig().OffsetScale.Y,
		CubeVisualizerConfig.MeshScale * GetConfig().OffsetScale.Z * ScaledHeight);
}

FTransform AStaticCubeVisualizer::GetTopMeshTransform(const int32 Index, const float SpectrumValue)
{
	return FTransform(FRotator(),
		Index * GetConfig().OffsetLocation + FVector(0, 0, CubeVisualizerConfig.CubeHeight * (GetScaledHeight(SpectrumValue) - 1)),
		GetScale3D(1.f));
}

FTransform AStaticCubeVisualizer::GetSideAndBaseTransform(const int32 Index, const float SpectrumValue)
{
	return FTransform(FRotator(),
		Index * GetConfig().OffsetLocation,
		GetScale3D(GetScaledHeight(SpectrumValue)));
}

void AStaticCubeVisualizer::AddInstancedCubeMesh(const FTransform& RelativeTransform)
{
	const int32 BaseMeshIndex = InstancedBaseMesh->AddInstance(RelativeTransform, false);
	InstancedBaseMesh->SetMaterial(BaseMeshIndex, BaseCubeMaterial);
	const int32 OutlineMeshIndex = InstancedVerticalOutlineMesh->AddInstance(RelativeTransform, false);
	InstancedVerticalOutlineMesh->SetMaterial(OutlineMeshIndex, OutlineMaterial);
	const int32 TopMeshIndex = InstancedTopMesh->AddInstance(RelativeTransform, false);
	InstancedTopMesh->SetMaterial(TopMeshIndex, OutlineMaterial);
}
