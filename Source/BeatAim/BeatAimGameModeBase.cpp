// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "PlayerHUD.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
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
	// Store reference of GameModeBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
	}

	//Default class specific values
	GameModeSelected = false;
}

void ABeatAimGameModeBase::UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned)
{
	if (ShotFired == true)
	{
		GI->IncrementShotsFired();
		GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	}
	if (TargetHit == true)
	{
		GI->IncrementTargetsHit();
		GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	}
	if (TargetSpawned == true)
	{
		GI->IncrementTargetsSpawned();
		GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());
	}
	GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());
}

void ABeatAimGameModeBase::ShowPlayerHUD()
{
	GI->DefaultCharacterRef->ShowPlayerHUD(true);
}

void ABeatAimGameModeBase::StartSpiderShot()
{
	if (GameModeSelected == true)
	{
		GI->TargetSpawnerRef->SetShouldSpawn(true);
		GetWorldTimerManager().SetTimer(SpiderShotGameLength, this, &ABeatAimGameModeBase::EndSpiderShot, 5.f, false);
		if (SpiderShotGameLength.IsValid())
		{
			GI->TargetSpawnerRef->SpawnActor();
		}
	}
}

void ABeatAimGameModeBase::EndSpiderShot()
{
	GameModeSelected = false;
	GI->TargetSpawnerRef->SetShouldSpawn(false);
	SpiderShotGameLength.Invalidate();
	if (GI->SphereTargetRef)
	{
		GI->SphereTargetRef->HandleDestruction();
	}
	GI->DefaultCharacterRef->ShowPlayerHUD(false);
}