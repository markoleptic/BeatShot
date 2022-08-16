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

void ABeatAimGameModeBase::ActorReceivedDamage(AActor* DeadActor)
{
	if (ASphereTarget* DestroyedTarget = Cast<ASphereTarget>(DeadActor))
	{
		DestroyedTarget->HandleDestruction();
	}
	//if (ASpiderShotSelector* DestroyedSelector = Cast<ASpiderShotSelector>(DeadActor))
	//{
	//	SetGameModeSelected(true);
	//	HandleGameStart(ASpiderShotSelector::StaticClass());
	//}
}

void ABeatAimGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	// Store instance of GameModeBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
	}
	//Default class specific values
	GameModeSelected = false;
	//SpiderShotTimerHandleLength = 10.f;
}

void ABeatAimGameModeBase::HandleGameStart(TSubclassOf<AActor> GameModeSelector)
{
	SetCurrentGameModeClass(GameModeSelector);
	GameModeSelected = true;
	ResetPlayerStats();
	GI->DefaultCharacterRef->LoadGame();
	GI->DefaultCharacterRef->ShowPlayerHUD(true);
	GetWorldTimerManager().SetTimer(CountDown, this, &ABeatAimGameModeBase::StartSpiderShot, 3.f, false);
}

void ABeatAimGameModeBase::SetGameModeSelected(bool IsSelected)
{
	GameModeSelected = IsSelected;
}

void ABeatAimGameModeBase::HandleGameRestart(TSubclassOf<AActor> GameModeSelector)
{
	//if (GameModeSelector == ASpiderShotSelector::StaticClass())
	//{
	//	SpidershotGameModeBaseREF->EndSpiderShot();
	//	HandleGameStart(GameModeSelector);
	//}
	if (GameModeSelector == ASpiderShotSelector::StaticClass())
	{
		EndSpiderShot();
		HandleGameStart(GameModeSelector);
	}
}

TSubclassOf<AActor> ABeatAimGameModeBase::GetCurrentGameModeClass()
{
	return CurrentGameModeClass;
}

void ABeatAimGameModeBase::SetCurrentGameModeClass(TSubclassOf<AActor> GameModeStaticClass)
{
	CurrentGameModeClass = GameModeStaticClass;
}

bool ABeatAimGameModeBase::IsGameModeSelected()
{
	return GameModeSelected;
}

/* Called by DefaultCharacter to update shots fired.
 * Called by Projectile to update targets hit.
 * Called by TargetSpawner to update targets spawned.
 */
void ABeatAimGameModeBase::UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned)
{
	if (ShotFired == true)
	{
		GI->UpdateShotsFired();
		GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	}
	if (TargetHit == true)
	{
		GI->UpdateTargetsHit();
		GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	}
	if (TargetSpawned == true)
	{
		GI->UpdateTargetsSpawned();
		GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());
	}
	GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());
}

void ABeatAimGameModeBase::ResetPlayerStats()
{
	GI->UpdateShotsFired(true);
	GI->UpdateTargetsHit(true);
	GI->UpdateTargetsSpawned(true);
	GI->UpdateScore(0,true);

	GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());

	GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());

	GI->DefaultCharacterRef->PlayerHUD->SetCurrentScore(GI->GetScore());
}

void ABeatAimGameModeBase::StartSpiderShot()
{
	if (IsGameModeSelected() == true)
	{
		GI->TargetSpawnerRef->SetShouldSpawn(true);
		GetWorldTimerManager().SetTimer(SpiderShotGameLength, this, &ABeatAimGameModeBase::EndSpiderShot, SpiderShotTimerHandleLength, false);
	}
}

void ABeatAimGameModeBase::EndSpiderShot()
{
	if (GI->GetScore() > GI->GetHighScore())
	{
		GI->UpdateHighScore(GI->GetScore());
	}
	if (GI->SphereTargetArray.Num() > 0)
	{
		for (ASphereTarget* Target : GI->SphereTargetArray)
		{
			if (Target)
			{
				Target->HandleDestruction();
			}
		}
	}

	GI->DefaultCharacterRef->SaveGame();
	SetGameModeSelected(false);
	GI->TargetSpawnerRef->SetShouldSpawn(false);
	StopAAPlayerAndTracker();
	GetWorldTimerManager().ClearAllTimersForObject(this);
	//CountDown.Invalidate();
	//SpiderShotGameLength.Invalidate();
	if (GI->SphereTargetRef)
	{
		GI->SphereTargetRef->HandleDestruction();
	}
	GI->DefaultCharacterRef->ShowPlayerHUD(false);
}