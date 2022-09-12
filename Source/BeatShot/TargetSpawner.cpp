// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;

	SetShouldSpawn(false);
	LastTargetSpawnedCenter = false;
	SingleBeat = false;
	LastSpawnLocation = FVector::ZeroVector;
	LastTargetScale = 1.f;
	CurrentDistance = 0.f;
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
	FirstSpawnLocation = BoxBounds.Origin;
	SpawnLocation = FirstSpawnLocation;
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsValid(TrackingTarget))
	{
		CurrentTrackerLocation = TrackingTarget->GetActorLocation();
		CurrentTrackerLocation += TrackingDirection * TrackingSpeed * DeltaTime;

		TrackingTarget->SetActorLocation(CurrentTrackerLocation);
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

		// Check for collisions for the next spawn location while randomizing
		RandomizeLocation(LastSpawnLocation, LastTargetScale);

		// Bind the destruction of target to OnTargetDestroyed to spawn a new target
		SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		SetShouldSpawn(false);
	}
}

void ATargetSpawner::SpawnTracker()
{
	// Gets more complicated if used with regular targets
	// Move at (maybe random / based on BPM?) speed until another beat

	if (TrackingTarget)
	{
		LocationBeforeDirectionChange = TrackingTarget->GetActorLocation();
	}

	if (IsValid(TrackingTarget) == false)
	{
		// Only spawn tracker once in the center if Tracking GameMode
		TrackingTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, FirstSpawnLocation, SpawnBox->GetComponentRotation());
		TrackingTarget->SetMaxHealth(100000000.f);
		TrackingTarget->SetLifeSpan(GI->GameModeActorStruct.GameModeLength);
		LocationBeforeDirectionChange = FirstSpawnLocation;

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(TrackingTarget);

		// Broadcast to GameModeActorBase that a target has spawned
		OnTargetSpawn.Broadcast();
	}

	if (TrackingTarget)
	{
		RandomizeScale(TrackingTarget);
		EndLocation = RandomizeTrackerLocation(LocationBeforeDirectionChange);
		TrackingDirection = UKismetMathLibrary::GetDirectionUnitVector(LocationBeforeDirectionChange, EndLocation);

		//SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
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
		//SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		//SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

		// bool variable to track if the SpawnLocation is inside of any of the active targets
		bool SphereIsInside = true;

		int OverloadProtect = 0;

		// while loop that spans the size of RecentSpawnBounds and only finishes if
		// SpawnLocation is not inside any of the RecentSpawnBounds spheres
		while (SphereIsInside)
		{
			for (int i = 0; i < RecentSpawnBounds.Num(); i++)
			{
				if (RecentSpawnBounds[i].IsInside(SpawnLocation))
				{
					if (OverloadProtect > 20)
					{
						ShouldSpawn = false;
						exit(0);
					}
					UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *SpawnLocation.ToString());
					SpawnLocation = BoxBounds.Origin;
					SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
					SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
					OverloadProtect++;
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

FVector ATargetSpawner::RandomizeTrackerLocation(FVector LocationBeforeChange)
{
	// if just doing tracking only, we don't really care about recent spawn locations
	// **Not currently using LastTargetScaleValue**
	CheckSpawnRadius = SphereTargetRadius * 2 + GameModeActorStruct.MinDistanceBetweenTargets;
	FSphere LastSpawnSphere = FSphere(LocationBeforeChange, CheckSpawnRadius);

	//try to spawn at origin if available
	FVector LocationToReturn = BoxBounds.Origin;

	//So we don't get stuck in infinite loop
	int OverloadProtect = 0;

	while (LastSpawnSphere.IsInside(LocationToReturn))
	{
		if (OverloadProtect > 20)
		{
			ShouldSpawn = false;
			exit(0);
		}

		UE_LOG(LogTemp, Display, TEXT("Iterating %s"), *LocationToReturn.ToString());
		LocationToReturn = BoxBounds.Origin;
		LocationToReturn.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
		LocationToReturn.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();

		OverloadProtect++;
	}
	return LocationToReturn;
}

void ATargetSpawner::InitializeGameModeActor(FGameModeActorStruct NewGameModeActor)
{
	// Initialize Struct passed by GameModeActorBase
	GameModeActorStruct = NewGameModeActor;

	// Only use SingleBeat if using a SingleBeat GameMode
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

