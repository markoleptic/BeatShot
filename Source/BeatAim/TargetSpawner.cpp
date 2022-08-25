// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;

	SetShouldSpawn(false);

	FirstSpawnLocation = BoxBounds.Origin;
	SpawnLocation = FirstSpawnLocation;
	LastSpawnLocation = FVector::ZeroVector;

	GameModeActorStruct = FGameModeActorStruct();

	// TODO: base CheckSpawnRadius on MaxTargetScale
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
	float RandomScaleValue = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	Target->BaseMesh->SetWorldScale3D(FVector(RandomScaleValue, RandomScaleValue, RandomScaleValue));
	return RandomScaleValue;
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	// Insert the most recent spawn location into array
	RecentSpawnLocations.Insert(LastSpawnLocation, 0);
	RecentSpawnLocations.SetNum(MaxNumberOfTargetsAtOnce);
	UE_LOG(LogTemp, Display, TEXT("size of spawnlocations: %f ") , RecentSpawnLocations.Num());

	// Insert sphere of CheckSpawnRadius radius into sphere array
	CheckSpawnRadius = SphereTargetSize * LastTargetScaleValue + GameModeActorStruct.MinDistanceBetweenTargets;
	RecentSpawnBounds.Insert(FSphere(LastSpawnLocation, CheckSpawnRadius),0);
	RecentSpawnBounds.SetNum(MaxNumberOfTargetsAtOnce);

	// SpawnLocation initially centered inside SpawnBox
	SpawnLocation = BoxBounds.Origin;
	SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
	SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

	// bool variable to track if the SpawnLocation is inside of any of the active targets
	bool SphereIsInside = true;

	// while loop that spans the size of RecentSpawnBounds and only finishes if
	// SpawnLocation is not inside any of the RecentSpawnBounds spheres
	while (SphereIsInside)
	{
		for (int i = 0; i < RecentSpawnBounds.Num(); i++)
		{
			if (RecentSpawnBounds[i].IsInside(SpawnLocation))
			{
				UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *SpawnLocation.ToString());
				SpawnLocation = BoxBounds.Origin;
				SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
				SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
				break;
			}
			if (i== RecentSpawnBounds.Num()-1)
			{
				SphereIsInside = false;
				break;
			}
		}
	}
}

void ATargetSpawner::InitializeGameModeActor(FGameModeActorStruct NewGameModeActor)
{
	GameModeActorStruct = NewGameModeActor;
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnLocations.Init(BoxBounds.Origin, MaxNumberOfTargetsAtOnce);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), MaxNumberOfTargetsAtOnce);
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	GameModeActorStruct.TargetSpawnCD = NewTargetSpawnCD;
}

