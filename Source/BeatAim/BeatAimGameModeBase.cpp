// Copyright Epic Games, Inc. All Rights Reserved.

#include "BeatAimGameModeBase.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "NarrowSpreadMultiBeat.h"
#include "CustomBeat.h"
#include "NarrowSpreadSingleBeat.h"
#include "WideSpreadSingleBeat.h"
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
	// Wide Spread Multi Beat
	if (GameModeActorEnum == EGameModeActorName::WideSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadMultiBeat>(WideSpreadMultiBeatClass);
	}
	// Narrow Spread Multi Beat
	else if (GameModeActorEnum == EGameModeActorName::NarrowSpreadMultiBeat) 
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadMultiBeat>(NarrowSpreadMultiBeatClass);
	}
	// Narrow Spread Single Beat
	else if (GameModeActorEnum == EGameModeActorName::NarrowSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadSingleBeat>(NarrowSpreadSingleBeatClass);
	}
	// Wide Spread Single Beat
	else if (GameModeActorEnum == EGameModeActorName::WideSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadSingleBeat>(WideSpreadSingleBeatClass);
	}
	// Custom
	else
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	return GameModeActorBase;
}

