// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "BeatAimGameModeBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	SetShouldSpawn(false);
	LastSpawnLocation = FVector::ZeroVector;
}

void ATargetSpawner::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterTargetSpawner(this);
	}
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	SetTargetSpawnCD(GI->GetTargetSpawnCD());
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetSpawner::SpawnActor()
{
	if (ShouldSpawn)
	{
		//TEMP
		//LastTargetSpawnedCenter = true;
		SpawnLocation = BoxBounds.Origin;
		RandomizeLocation(LastSpawnLocation);
		//if (LastTargetSpawnedCenter == true)
		//{
		//}
		//else if (LastTargetSpawnedCenter == false) {
		//TEMP
		//LastTargetSpawnedCenter = true;
		//}
		// Spawn the target
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		if (SpawnTarget)
		{
			RandomizeScale(SpawnTarget);
			LastSpawnLocation = SpawnLocation;

			// Update reference to spawned target in Game Instance
			GI->RegisterSphereTarget(SpawnTarget);

			// Add Target to TArray
			GI->SphereTargetArray.Add(SpawnTarget);
			NumTargetsAddedToArray++;

			// Broadcast to GameModeActorBase that a target has spawned
			OnTargetSpawn.Broadcast();

			// Bind the destruction of target to OnTargetDestroyed to spawn a new target
			//SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		}
	}
}

bool ATargetSpawner::GetShouldSpawn()
{
	return ShouldSpawn;
}

void ATargetSpawner::SetShouldSpawn(bool bShouldSpawn)
{
	// Whenever this function is called, we want the target to always spawn in center
	//LastTargetSpawnedCenter = false;
	ShouldSpawn = bShouldSpawn;
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	//TimeSinceSpawn.Invalidate();
}

void ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	float RandomScaleValue = FMath::FRandRange(0.8f, 2.f);
	FVector RandomScaleVector = { RandomScaleValue, RandomScaleValue, RandomScaleValue };
	Target->BaseMesh->SetWorldScale3D(RandomScaleVector);
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation)
{
	if (FLastSpawnLocation.Equals(FVector::ZeroVector))
	{
		SpawnLocation.X += BoxBounds.BoxExtent.X;
		SpawnLocation.Y += BoxBounds.BoxExtent.Y;
		SpawnLocation.Z += BoxBounds.BoxExtent.Z;
		LastSpawnLocation.X += -BoxBounds.BoxExtent.X + 2 * BoxBounds.BoxExtent.X * FMath::FRand();
		LastSpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		LastSpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
	}
	else
	{
		SpawnLocation.X += -BoxBounds.BoxExtent.X + 2 * BoxBounds.BoxExtent.X * FMath::FRand();
		SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
	}
	//LastTargetSpawnedCenter = false;
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	TargetSpawnCD = NewTargetSpawnCD;
}

