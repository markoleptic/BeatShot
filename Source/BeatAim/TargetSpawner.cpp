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
	//TODO: Set SpawnBox Extent here

	SetShouldSpawn(false);

	FirstSpawnLocation = BoxBounds.Origin;
	SpawnLocation = FirstSpawnLocation;
	LastSpawnLocation = FVector::ZeroVector;

	RecentSpawnLocations.Init(BoxBounds.Origin, 4);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), 4);

	MinTargetScale = 0.8f;
	MaxTargetScale = 1.8f;

	// TODO: base CheckSpawnRadius on MaxTargetScale

	MinDistanceBetweenTargets = 100.f;
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

	//TODO: Set Max Number of targets up at once using GetTargetCD here
	//TODO: Set Scale targets
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetSpawner::SpawnActor()
{
	if (ShouldSpawn)
	{
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		if (SpawnTarget)
		{
			// Since scaling is done after spawn, we have access to LAST scale value and TargetLocation
			LastSpawnLocation = SpawnLocation;
			LastTargetScale = RandomizeScale(SpawnTarget);

			// Update reference to spawned target in Game Instance
			GI->RegisterSphereTarget(SpawnTarget);

			// Add Target to TArray
			GI->SphereTargetArray.Add(SpawnTarget);
			NumTargetsAddedToArray++;

			// Broadcast to GameModeActorBase that a target has spawned
			OnTargetSpawn.Broadcast();

			// Check for collisions for the next spawn location while randomizing
			RandomizeLocation(LastSpawnLocation, LastTargetScale);

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

float ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	float RandomScaleValue = FMath::FRandRange(MinTargetScale, MaxTargetScale);
	Target->BaseMesh->SetWorldScale3D(FVector(RandomScaleValue, RandomScaleValue, RandomScaleValue));
	return RandomScaleValue;
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	// Insert the most recent spawn location into array
	RecentSpawnLocations.Insert(LastSpawnLocation, 0);
	RecentSpawnLocations.SetNum(4);

	// Insert sphere of CheckSpawnRadius radius into sphere array
	CheckSpawnRadius = SphereTargetSize * LastTargetScaleValue + MinDistanceBetweenTargets;
	RecentSpawnBounds.Insert(FSphere(LastSpawnLocation, CheckSpawnRadius),0);
	RecentSpawnBounds.SetNum(4);

	// SpawnLocation initially centered inside SpawnBox
	SpawnLocation = BoxBounds.Origin;
	SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
	SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

	// Keep generating random locations until location is not inside the radius of any spheres
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
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	TargetSpawnCD = NewTargetSpawnCD;
}

