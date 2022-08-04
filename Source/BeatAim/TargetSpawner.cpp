// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "PlayerHUD.h"
#include "SphereTarget.h"
#include "TargetSubsystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
	UGameInstance* GI = UGameplayStatics::GetGameInstance(this);
	UTargetSubsystem* TargetSubsystem = GI ? GI->GetSubsystem<UTargetSubsystem>() : nullptr;
	DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	SpawnActor();
}

void ATargetSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATargetSpawner::SpawnActor()
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

void ATargetSpawner::OnTargetDestroyed(AActor* DestroyedActor)
{
	SpawnActor();
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

