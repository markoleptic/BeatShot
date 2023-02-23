// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "VisualGrid.h"

#include "Components/InstancedStaticMeshComponent.h"

AVisualGrid::AVisualGrid()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("InstancedMesh"));
	RootComponent = InstancedMesh;
}

void AVisualGrid::BeginPlay()
{
	Super::BeginPlay();
}

void AVisualGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualGrid::CreateGrid(const int32 X, const int32 Y)
{
	int Count = 0;
	NumRowsGrid = X;
	NumColsGrid = Y;
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			Count++;
			InstancedMesh->AddInstance(
				FTransform(
					FRotator::ZeroRotator,
					FVector(0, j * 10 - (X * 10.f / 2.f), i * 10 - (Y * 10.f / 2.f)),
					FVector(0.1)), false);
		}
	}
	UE_LOG(LogTemp, Display, TEXT("CountfromCreateGrid: %d"), Count);
}

void AVisualGrid::SetCustomDataValues(const TArray<FVector> Points, const FVector Center, const FVector Extent, const float scale, 
	const int32 Value)
{
	const FTransform SpawnMemoryTransform =  FTransform(FRotator::ZeroRotator, FVector::ZeroVector, FVector(scale));
	const FBox Box = FBox::BuildAABB(Center, Extent).TransformBy(SpawnMemoryTransform);
	FVector BoxCenter;
	FVector Extents;
	Box.GetCenterAndExtents(BoxCenter, Extents);
				
	for (const FVector Vector : Points)
	{
		const float FloatValue = roundf((Vector.Y + Extents.Y) + (((NumRowsGrid)* (NumColsGrid-1)) - (roundf(BoxCenter.Z + Extents.Z - Vector.Z) * NumRowsGrid)));
		InstancedMesh->SetCustomDataValue(
			FloatValue, 0,
			Value, true);
	}
}
