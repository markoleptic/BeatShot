// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"
#include "SpiderShotSelector.h"
#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "DefaultPlayerController.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

void ABeatAimGameModeBase::ActorDied(AActor* DeadActor)
{
	if (ASphereTarget* DestroyedTarget = Cast<ASphereTarget>(DeadActor))
	{
		DestroyedTarget->HandleDestruction();
	}

	if (ASpiderShotSelector* DestroyedSelector = Cast<ASpiderShotSelector>(DeadActor))
	{
		StartSpiderShot();
	}
}

void ABeatAimGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	TArray<ATargetSpawner*> TargetSpawners;
	FindAllActors(GetWorld(), TargetSpawners);
	TargetSpawnerRef = TargetSpawners[0];

	//TODO: Get the game mode the player shoots from the wall
	//GameModeSelector = "SpiderShot";
	//PlayerGameModeSelected(GameModeSelector);
}

void ABeatAimGameModeBase::StartSpiderShot()
{
	TargetSpawnerRef->SetShouldSpawn(true);
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ABeatAimGameModeBase::EndSpiderShot, 10.f, false);
	TargetSpawnerRef->SpawnActor();
}

void ABeatAimGameModeBase::EndSpiderShot()
{
	TargetSpawnerRef->SetShouldSpawn(false);
	GameModeSelector = "NotSpiderShot";
}

void ABeatAimGameModeBase::PlayerGameModeSelected(FString GameMode)
{
	if (GameMode == "SpiderShot")
	{
		StartSpiderShot();
	}
}

void ABeatAimGameModeBase::FindAllActors(UWorld* World, TArray<ATargetSpawner*>& Out)
{
	for (TActorIterator<ATargetSpawner> It(World); It; ++It)
	{
		Out.Add(*It);
	}
}
