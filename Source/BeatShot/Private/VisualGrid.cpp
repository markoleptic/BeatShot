// Fill out your copyright notice in the Description page of Project Settings.


#include "VisualGrid.h"

#include "Components/InstancedStaticMeshComponent.h"

// Sets default values
AVisualGrid::AVisualGrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(FName("InstancedMesh"));
	RootComponent = InstancedMesh;
}

// Called when the game starts or when spawned
void AVisualGrid::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVisualGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVisualGrid::CreateGrid(const int32 X, const int32 Y)
{
	UE_LOG(LogTemp, Display, TEXT("X: %d Y: %d"), X, Y);
	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
		{
			InstancedMesh->AddInstance(FTransform(FRotator::ZeroRotator,FVector(0, i * 10 - (X*10.f/2.f), j * 10 - (Y*10.f/2.f)),  FVector(0.1)), false);
		}
	}
}

