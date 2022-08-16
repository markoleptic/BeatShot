// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeActorBase.h"
#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "PlayerHUD.h"
#include "EngineUtils.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameModeActorBase::AGameModeActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//Default class specific values
	PrimaryActorTick.bCanEverTick = true;
	GameModeSelected = false;
}

// Called when the game starts or when spawned
void AGameModeActorBase::BeginPlay()
{
	Super::BeginPlay();
	// Store instance of GameModeActorBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeActorBase(this);
	}
}

// Called every frame
void AGameModeActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGameModeActorBase::SetGameModeSelected(bool IsSelected)
{
	GameModeSelected = IsSelected;
}

bool AGameModeActorBase::IsGameModeSelected()
{
	return false;
}

void AGameModeActorBase::UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned)
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

void AGameModeActorBase::ResetPlayerStats()
{
	GI->UpdateShotsFired(true);
	GI->UpdateTargetsHit(true);
	GI->UpdateTargetsSpawned(true);
	GI->UpdateScore(0, true);

	GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());

	GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());

	GI->DefaultCharacterRef->PlayerHUD->SetCurrentScore(GI->GetScore());
}

AGameModeActorBase* AGameModeActorBase::GetCurrentGameModeClass()
{
	return this;
}

void AGameModeActorBase::SetCurrentGameModeClass(AGameModeActorBase* GameModeActor)
{
	CurrentGameModeClass = GameModeActor;
}

void AGameModeActorBase::HandleGameStart()
{
	SetCurrentGameModeClass(this);
	GameModeSelected = true;
	ResetPlayerStats();
	GI->DefaultCharacterRef->LoadGame();
	GI->DefaultCharacterRef->ShowPlayerHUD(true);
	GetWorldTimerManager().SetTimer(CountDownTimer, this, &AGameModeActorBase::StartGameMode, 3.f, false);
}

void AGameModeActorBase::StartGameMode()
{
}

void AGameModeActorBase::EndGameMode()
{
}

void AGameModeActorBase::HandleGameRestart()
{
}

