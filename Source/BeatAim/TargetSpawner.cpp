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
	SpawnLocation = BoxBounds.Origin;
	LastSpawnLocation = FVector::ZeroVector;
	RecentSpawnLocations.Init(BoxBounds.Origin, 3);
	RecentSpawnBounds.Init(FBox(BoxBounds.Origin, BoxBounds.Origin), 3);
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
		//if (LastTargetSpawnedCenter == true)
		//{
		//}
		//else if (LastTargetSpawnedCenter == false) {
		//TEMP
		//LastTargetSpawnedCenter = true;
		//}
		// Spawn the target
		//SpawnLocation = BoxBounds.Origin;
		//RandomizeLocation(LastSpawnLocation);
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		if (SpawnTarget)
		{
			LastSpawnLocation = SpawnLocation;
			RandomizeScale(SpawnTarget);

			// Update reference to spawned target in Game Instance
			GI->RegisterSphereTarget(SpawnTarget);

			// Add Target to TArray
			GI->SphereTargetArray.Add(SpawnTarget);
			NumTargetsAddedToArray++;

			// Broadcast to GameModeActorBase that a target has spawned
			OnTargetSpawn.Broadcast();
			RandomizeLocation(LastSpawnLocation);

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
	// Insert the most recent spawn location into array
	RecentSpawnLocations.Insert(LastSpawnLocation, 0);
	RecentSpawnLocations.SetNum(4);

	//FVector Origin = { LastSpawnLocation.X -30, LastSpawnLocation.Y - 30, LastSpawnLocation.Z = -30 };
	//FVector Extent = { LastSpawnLocation.X +30, LastSpawnLocation.Y + 30, LastSpawnLocation.Z = +30 };

	FVector NewExtent = { 1, 200, 200 };

	// Insert box around spawn location into box array
	RecentSpawnBounds.Insert(FBox::BuildAABB(LastSpawnLocation, NewExtent),0);
	RecentSpawnBounds.SetNum(4);

	// new spawn location
	// SpawnLocation is always initially at BoxBounds.Origin
	SpawnLocation = BoxBounds.Origin;
	SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
	SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

	while (RecentSpawnBounds[0].IsInside(SpawnLocation) ||
		RecentSpawnBounds[1].IsInside(SpawnLocation) ||
		RecentSpawnBounds[2].IsInside(SpawnLocation) ||
		RecentSpawnBounds[3].IsInside(SpawnLocation)) 
	{
		UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *SpawnLocation.ToString());
		SpawnLocation = BoxBounds.Origin;
		SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
	}
//LastTargetSpawnedCenter = false;
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	TargetSpawnCD = NewTargetSpawnCD;
}

