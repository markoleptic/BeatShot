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
	for (int i = 0; i < Y; i++)
	{
		for (int j = 0; j < X; j++)
		{
			InstancedMesh->AddInstance(
				FTransform(
					FRotator::ZeroRotator,
					FVector(0, j * 10 - (X * 10.f / 2.f), i * 10 - (Y * 10.f / 2.f)),
					FVector(0.1)), false);
		}
	}
}
