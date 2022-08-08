// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "DefaultGameInstance.h"
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
	GI->RegisterTargetSpawner(this);
	BoxBounds = SpawnBox->CalcBounds(GetActorTransform());
	//UTargetSubsystem* TargetSubsystem = GI ? GI->GetSubsystem<UTargetSubsystem>() : nullptr;
	DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
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
		ASphereTarget* SpawnTarget = GetWorld()->SpawnActor<ASphereTarget>(ActorToSpawn, SpawnLocation, SpawnBox->GetComponentRotation());
		RandomizeScale(SpawnTarget);
		GI->RegisterSphereTarget(SpawnTarget);
		if (SpawnTarget)
		{
			TargetsSpawned++;
			if (DefaultCharacter->PlayerHUD)
			{
				DefaultCharacter->PlayerHUD->SetTargetsSpawned(TargetsSpawned);
			}
			// Bind the destruction of target to OnTargetDestroyed to spawn a new target
			SpawnTarget->OnDestroyed.AddDynamic(this, &ATargetSpawner::OnTargetDestroyed);
		}
	}
}

void ATargetSpawner::SetShouldSpawn(bool bShouldSpawn)
{
	LastTargetSpawnedCenter = false;
	ShouldSpawn = bShouldSpawn;
}

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
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

