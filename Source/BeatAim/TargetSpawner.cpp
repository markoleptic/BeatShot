// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
#include "BeatAimGameModeBase.h"
#include "TargetSubsystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ATargetSpawner::ATargetSpawner()
{
	PrimaryActorTick.bCanEverTick = true;
	SpawnBox = CreateDefaultSubobject<UBoxComponent>("SpawnBox");
	RootComponent = SpawnBox;
	SetShouldSpawn(false);
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
	//UTargetSubsystem* TargetSubsystem = GI ? GI->GetSubsystem<UTargetSubsystem>() : nullptr;
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetSpawner::SpawnActor()
{
	if (ShouldSpawn)
	{
		SpawnLocation = BoxBounds.Origin;
		if (LastTargetSpawnedCenter == true)
		{
			RandomizeLocation();
		}
		else if (LastTargetSpawnedCenter == false) {
			LastTargetSpawnedCenter = true;
		}
		// Spawn the target
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		RandomizeScale(SpawnTarget);

		// Update reference to spawned target in Game Instance
		GI->RegisterSphereTarget(SpawnTarget);
		if (SpawnTarget)
		{
			// Bind the spawning of Target to GetTimeBasedScore
			GetWorldTimerManager().SetTimer(TimeSinceSpawn, SpawnTarget->MaxLifeSpan, false);
			// Bind the destruction of target to OnTargetDestroyed to spawn a new target
			SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		}
		if (GI->DefaultCharacterRef->HUDActive)
		{
			// Only update Targets Spawned
			GI->GameModeBaseRef->UpdatePlayerStats(false, false, true);
		}
	}
}

void ATargetSpawner::SetShouldSpawn(bool bShouldSpawn)
{
	// Whenever this function is called, we want the target to always spawn in center
	LastTargetSpawnedCenter = false;
	ShouldSpawn = bShouldSpawn;
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	//GI->UpdateScore(GetTimeBasedScore(GetTimeSinceSpawn(TimeSinceSpawn), 50));
	//GI->DefaultCharacterRef->PlayerHUD->SetCurrentScore(GI->GetScore());
	TimeSinceSpawn.Invalidate();
	if (ShouldSpawn)
	{
		SpawnActor();
	}
}

void ATargetSpawner::RandomizeScale(ASphereTarget* Target)
{
	float RandomScaleValue = FMath::FRandRange(0.2f, 2.f);
	FVector RandomScaleVector = { RandomScaleValue, RandomScaleValue, RandomScaleValue };
	Target->BaseMesh->SetWorldScale3D(RandomScaleVector);
}

void ATargetSpawner::RandomizeLocation()
{
	SpawnLocation.X += -BoxBounds.BoxExtent.X + 2 * BoxBounds.BoxExtent.X * FMath::FRand();
	SpawnLocation.Y += -BoxBounds.BoxExtent.Y + 2 * BoxBounds.BoxExtent.Y * FMath::FRand();
	SpawnLocation.Z += -BoxBounds.BoxExtent.Z + 2 * BoxBounds.BoxExtent.Z * FMath::FRand();
	LastTargetSpawnedCenter = false;
}

float ATargetSpawner::GetTimeBasedScore(float TimeElapsed, float ScoreMultiplier)
{
	UE_LOG(LogTemp, Display, TEXT("Time Elapsed: %f"), TimeElapsed);
	return ScoreMultiplier / TimeElapsed;
}

float ATargetSpawner::GetTimeSinceSpawn(FTimerHandle TimerHandle)
{
	return GetWorldTimerManager().GetTimerElapsed(TimerHandle);
}

