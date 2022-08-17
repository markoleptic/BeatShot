// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "SphereTarget.h"
#include "TargetSpawner.h"
#include "DefaultCharacter.h"
#include "DefaultGameInstance.h"
#include "PlayerHUD.h"
#include "EngineUtils.h"
#include "GameModeActorBase.h"
#include "WideSpreadMultiBeat.h"
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
	//UE_LOG(LogTemp, Warning, TEXT("here"));
	//GameModeSelected = true;
	//FVector Location = { 0, 0, 0 };
	//FRotator Rotation = { 0, 0, 0 };
	//AWideSpreadMultiBeat* WideSpreadMultiBeat = GetWorld()->SpawnActor<AWideSpreadMultiBeat>(WideSpreadMultiBeatClass, Location, Rotation);
}

void ABeatAimGameModeBase::SetGameModeSelected(bool IsSelected)
{
	GameModeSelected = IsSelected;
}

//TSubclassOf<AActor> ABeatAimGameModeBase::GetCurrentGameModeClass()
//{
//	return CurrentGameModeClass;
//}
//
//void ABeatAimGameModeBase::SetCurrentGameModeClass(TSubclassOf<AActor> GameModeStaticClass)
//{
//	CurrentGameModeClass = GameModeStaticClass;
//}

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
	//if (ShotFired == true)
	//{
	//	GI->UpdateShotsFired();
	//	GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	//}
	//if (TargetHit == true)
	//{
	//	GI->UpdateTargetsHit();
	//	GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	//}
	//if (TargetSpawned == true)
	//{
	//	GI->UpdateTargetsSpawned();
	//	GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());
	//}
	//GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	//GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());
}

void ABeatAimGameModeBase::ResetPlayerStats()
{
	GI->UpdateShotsFired(true);
	GI->UpdateTargetsHit(true);
	GI->UpdateTargetsSpawned(true);
	GI->UpdateScore(0,true);

	//GI->DefaultCharacterRef->PlayerHUD->SetShotsFired(GI->GetShotsFired());
	//GI->DefaultCharacterRef->PlayerHUD->SetTargetsHit(GI->GetTargetsHit());
	//GI->DefaultCharacterRef->PlayerHUD->SetTargetsSpawned(GI->GetTargetsSpawned());

	//GI->DefaultCharacterRef->PlayerHUD->SetAccuracy(GI->GetTargetsHit(), GI->GetShotsFired());
	//GI->DefaultCharacterRef->PlayerHUD->SetTargetBar(GI->GetTargetsHit(), GI->GetShotsFired());

	//GI->DefaultCharacterRef->PlayerHUD->SetCurrentScore(GI->GetScore());
}