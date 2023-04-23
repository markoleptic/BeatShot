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
	SetVisualizerOffset(Constants::CubeOffset);
}

void AStaticCubeVisualizer::InitializeVisualizer()
{
	Super::InitializeVisualizer();

	InstancedBaseMesh->ClearInstances();
	InstancedVerticalOutlineMesh->ClearInstances();
	InstancedTopMesh->ClearInstances();
	
	for (int i = 0; i < AASettings.NumBandChannels; i++)
	{
		AddInstancedCubeMesh(i * VisualizerOffset);
	}
}

void AStaticCubeVisualizer::UpdateVisualizer(const int32 Index, const float SpectrumAlpha)
{
	const float NewHeightScale = GetScaledHeight(SpectrumAlpha);
	const FTransform BaseAndSideTransform(FRotator(), Index * VisualizerOffset, FVector(MeshScale, MeshScale, MeshScale * NewHeightScale));

	const float CustomDataValue = (NewHeightScale - Constants::MinCubeHeightScale) / (Constants::MaxCubeHeightScale - Constants::MinCubeHeightScale);
	
	InstancedBaseMesh->UpdateInstanceTransform(Index, BaseAndSideTransform);
	InstancedVerticalOutlineMesh->UpdateInstanceTransform(Index, BaseAndSideTransform);
	InstancedTopMesh->UpdateInstanceTransform(Index, FTransform(FRotator(), Index * VisualizerOffset + FVector(0, 0, CubeHeight * (NewHeightScale - 1)), FVector(MeshScale)));

	InstancedVerticalOutlineMesh->SetCustomDataValue(Index, 0, CustomDataValue, false);
	InstancedTopMesh->SetCustomDataValue(Index, 0, CustomDataValue, false);
}

void AStaticCubeVisualizer::MarkRenderStateDirty()
{
	InstancedBaseMesh->MarkRenderStateDirty();
	InstancedVerticalOutlineMesh->MarkRenderStateDirty();
	InstancedTopMesh->MarkRenderStateDirty();
}

float AStaticCubeVisualizer::GetScaledHeight(const float SpectrumValue) const
{
	return UKismetMathLibrary::MapRangeClamped(SpectrumValue, 0, 1, Constants::MinCubeHeightScale, Constants::MaxCubeHeightScale);
}

void AStaticCubeVisualizer::AddInstancedCubeMesh(const FVector& RelativePosition)
{
	const int32 BaseMeshIndex = InstancedBaseMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(MeshScale)), false);
	InstancedBaseMesh->SetMaterial(BaseMeshIndex, BaseCubeMaterial);
	const int32 OutlineMeshIndex = InstancedVerticalOutlineMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(MeshScale)), false);
	InstancedVerticalOutlineMesh->SetMaterial(OutlineMeshIndex, OutlineMaterial);
	const int32 TopMeshIndex = InstancedTopMesh->AddInstance(FTransform(FRotator(), RelativePosition, FVector(MeshScale)), false);
	InstancedTopMesh->SetMaterial(TopMeshIndex, OutlineMaterial);
}
