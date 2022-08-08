// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"
#include "SpiderShotSelector.h"
#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
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
		HandleGameStart(DestroyedSelector);
	}
}

void ABeatAimGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	//DefaultCharacter = Cast<ADefaultCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	GI->RegisterGameModeBase(this);
	DefaultCharacter = GI->DefaultCharacterRef;
	//TArray<ATargetSpawner*> TargetSpawners;
	//FindAllActors(GetWorld(), TargetSpawners);
	//TargetSpawnerRef = TargetSpawners[0];
	TargetSpawner = GI->TargetSpawnerRef;
	SphereTarget = GI->SphereTargetRef;
	GameModeSelected = false;
}

void ABeatAimGameModeBase::StartSpiderShot()
{
	if (GameModeSelected == true)
	{
		TargetSpawner->SetShouldSpawn(true);
		GetWorldTimerManager().SetTimer(SpiderShotGameLength, this, &ABeatAimGameModeBase::EndSpiderShot, 5.f, false);
		if (SpiderShotGameLength.IsValid())
		{
			TargetSpawner->SpawnActor();
		}
	}
}

void ABeatAimGameModeBase::EndSpiderShot()
{
	SpiderShotGameLength.Invalidate();
	TargetSpawner->SetShouldSpawn(false);
	GameModeSelected = false;
	if (SphereTarget)
	{
		SphereTarget->HandleDestruction();
	}
}

//void ABeatAimGameModeBase::FindAllActors(UWorld* World, TArray<ATargetSpawner*>& Out)
//{
//	for (TActorIterator<ATargetSpawner> It(World); It; ++It)
//	{
//		Out.Add(*It);
//	}
//}