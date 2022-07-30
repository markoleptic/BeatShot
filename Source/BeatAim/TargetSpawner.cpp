// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Components/BoxComponent.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	SpawnActor();
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetSpawner::SpawnActor()
{
	FVector SpawnLocation = BoxBounds.Origin;
	FRotator SpawnRotation = FRotator();
	if (LastTargetSpawnedCenter == true)
	{
		UE_LOG(LogTemp, Display, TEXT("X: %f, Y: %f, Z: %f"),BoxBounds.BoxExtent.X, BoxBounds.BoxExtent.Y, BoxBounds.BoxExtent.Z);
		SpawnLocation.X += -BoxBounds.BoxExtent.X + 2 * BoxBounds.BoxExtent.X * FMath::FRand();
		SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
		UE_LOG(LogTemp, Display, TEXT("X: %f, Y: %f, Z: %f"), SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);
		LastTargetSpawnedCenter = false;
	}
	else if (LastTargetSpawnedCenter == false) {
		LastTargetSpawnedCenter = true;
	}
	ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
	if (SpawnTarget)
	{
		// Bind the destruction of target to OnTargetDestroyed to spawn a new target
		SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
	}
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	SpawnActor();
}

