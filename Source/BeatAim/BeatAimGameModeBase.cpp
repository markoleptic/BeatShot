// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "NarrowSpreadMultiBeat.h"
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
}

AGameModeActorBase* ABeatAimGameModeBase::SetGameModeActorBase(EGameModeActorName GameModeActorEnum)
{
	if (GameModeActorEnum == EGameModeActorName::WideSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadMultiBeat>(WideSpreadMultiBeatClass);
		return GameModeActorBase;
	}
	else if (GameModeActorEnum == EGameModeActorName::NarrowSpreadMultiBeat) 
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadMultiBeat>(NarrowSpreadMultiBeatClass);
		return GameModeActorBase;
	}
	// TEMPORARY so I can always play something in default start map
	GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadMultiBeat>(NarrowSpreadMultiBeatClass);
	return GameModeActorBase;
}

