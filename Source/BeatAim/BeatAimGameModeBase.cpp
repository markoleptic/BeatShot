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
