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
	LastTargetSpawnedCenter = false;
	SingleBeat = false;
	FirstSpawnLocation = BoxBounds.Origin;
	SpawnLocation = FirstSpawnLocation;
	LastSpawnLocation = FVector::ZeroVector;
	GameModeActorStruct = FGameModeActorStruct();
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

	if (TrackingTarget != nullptr)
	{
		FVector Location = TrackingTarget->GetActorLocation();

		Location += TrackingDirection * TrackingSpeed * DeltaTime;

		TrackingTarget->SetActorLocation(Location);

		CurrentDistance = (Location - StartLocation).Size();
	}
}

void ATargetSpawner::SpawnActor()
{
	if (ShouldSpawn)
	{
		// Use different function for SingleBeat game modes
		if (SingleBeat == true)
		{
			SpawnSingleActor();
		}
		else
		{
			ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
			if (SpawnTarget)
			{
				// Since scaling is done after spawn, we have access to LAST target's scale value and TargetLocation
				LastSpawnLocation = SpawnLocation;
				LastTargetScale = RandomizeScale(SpawnTarget);

				// Update reference to spawned target in Game Instance
				GI->RegisterSphereTarget(SpawnTarget);

				// Broadcast to GameModeActorBase that a target has spawned
				OnTargetSpawn.Broadcast();

				// Add Target to TArray
				GI->SphereTargetArray.Add(SpawnTarget);
				NumTargetsAddedToArray++;

				// Check for collisions for the next spawn location while randomizing
				RandomizeLocation(LastSpawnLocation, LastTargetScale);
			}
		}
	}
}

void ATargetSpawner::SpawnSingleActor()
{
	ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
	if (SpawnTarget)
	{
		// Since scaling is done after spawn, we have access to LAST target's scale value and TargetLocation
		LastSpawnLocation = SpawnLocation;
		if (LastSpawnLocation == BoxBounds.Origin)
		{
			LastTargetSpawnedCenter = true;
		}
		else
		{
			LastTargetSpawnedCenter = false;
		}

		LastTargetScale = RandomizeScale(SpawnTarget);

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(SpawnTarget);

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();

		// Add Target to TArray
		GI->SphereTargetArray.Add(SpawnTarget);
		NumTargetsAddedToArray++;

		// Check for collisions for the next spawn location while randomizing
		RandomizeLocation(LastSpawnLocation, LastTargetScale);

		// Bind the destruction of target to OnTargetDestroyed to spawn a new target
		SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::SpawnTracker()
{
	// Check for collisions for the next spawn location while randomizing
	RandomizeLocation(LastSpawnLocation, LastTargetScale);
	StartLocation = SpawnLocation;

	RandomizeLocation(LastSpawnLocation, LastTargetScale);
	FVector EndLocation = SpawnLocation;

	FVector Distance = EndLocation - StartLocation;
	TotalDistance = Distance.Size();
	CurrentDistance = 0.0f;
	TrackingDirection = Distance.GetSafeNormal();

	TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, StartLocation, SpawnBox->GetComponentRotation());
	if (TrackingTarget)
	{
		TrackingTarget->SetLifeSpan(2.5);
		// Since scaling is done after spawn, we have access to LAST target's scale value and TargetLocation
		LastSpawnLocation = SpawnLocation;
		if (LastSpawnLocation == BoxBounds.Origin)
		{
			LastTargetSpawnedCenter = true;
		}
		else
		{
			LastTargetSpawnedCenter = false;
		}

		LastTargetScale = RandomizeScale(TrackingTarget);

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(TrackingTarget);

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();

		// Add Target to TArray
		//GI->SphereTargetArray.Add(TrackingTarget);
		//NumTargetsAddedToArray++;


		// Bind the destruction of target to OnTargetDestroyed to spawn a new target
		//SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);

		SetShouldSpawn(false);
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
	SetShouldSpawn(true);
}

float ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	float RandomScaleValue = FMath::FRandRange(GameModeActorStruct.MinTargetScale, GameModeActorStruct.MaxTargetScale);
	Target->BaseMesh->SetWorldScale3D(FVector(RandomScaleValue, RandomScaleValue, RandomScaleValue));
	return RandomScaleValue;
}

void ATargetSpawner::RandomizeLocation(FVector FLastSpawnLocation, float LastTargetScaleValue)
{
	if (SingleBeat && LastTargetSpawnedCenter == false)
	{
		SpawnLocation = BoxBounds.Origin;
	}
	else
	{
		// Insert the most recent spawn location into array
		RecentSpawnLocations.Insert(LastSpawnLocation, 0);
		RecentSpawnLocations.SetNum(MaxNumberOfTargetsAtOnce);

		// Insert sphere of CheckSpawnRadius radius into sphere array
		CheckSpawnRadius = SphereTargetRadius * LastTargetScaleValue + GameModeActorStruct.MinDistanceBetweenTargets;
		RecentSpawnBounds.Insert(FSphere(LastSpawnLocation, CheckSpawnRadius), 0);
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
				if (i == RecentSpawnBounds.Num() - 1)
				{
					SphereIsInside = false;
					break;
				}
			}
		}
	}
}

void ATargetSpawner::InitializeGameModeActor(FGameModeActorStruct NewGameModeActor)
{
	// Initialize Struct passed by GameModeActorBase
	GameModeActorStruct = NewGameModeActor;

	// Only use SingleBeat if using SingleBeat gamemodes
	if (GameModeActorStruct.GameModeActorName == EGameModeActorName::NarrowSpreadSingleBeat ||
		GameModeActorStruct.GameModeActorName == EGameModeActorName::WideSpreadSingleBeat)
	{
		SingleBeat = true;
	}

	// GameMode menu uses the full width, while box bounds are only half width / half height
	GameModeActorStruct.BoxBounds.Y = GameModeActorStruct.BoxBounds.Y / 2.f;
	GameModeActorStruct.BoxBounds.Z = GameModeActorStruct.BoxBounds.Z / 2.f;

	// Change HeadshotOnly specific parameters
	if (GameModeActorStruct.HeadshotHeight == true)
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = 160.f;
		GameModeActorStruct.BoxBounds.Z = 0.f;
	}
	else
	{
		GameModeActorStruct.CenterOfSpawnBox.Z = GameModeActorStruct.BoxBounds.Z + 100.f;
	}

	// Set new location box extent for TargetSpawner
	SpawnBox->SetRelativeLocation(GameModeActorStruct.CenterOfSpawnBox);
	SpawnBox->SetBoxExtent(GameModeActorStruct.BoxBounds);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	FirstSpawnLocation = BoxBounds.Origin;

	// Setting max targets at one time for the size of RecentSpawnLocations & RecentSpawnBounds
	MaxNumberOfTargetsAtOnce = ceil(GameModeActorStruct.TargetMaxLifeSpan / GameModeActorStruct.TargetSpawnCD);
	RecentSpawnLocations.Init(BoxBounds.Origin, MaxNumberOfTargetsAtOnce);
	RecentSpawnBounds.Init(FSphere(BoxBounds.Origin, 1), MaxNumberOfTargetsAtOnce);
}

void ATargetSpawner::SetTargetSpawnCD(float NewTargetSpawnCD)
{
	GameModeActorStruct.TargetSpawnCD = NewTargetSpawnCD;
}

