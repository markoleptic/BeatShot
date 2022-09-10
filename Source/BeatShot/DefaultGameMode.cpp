// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"

#include "BeatTrack.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "NarrowSpreadMultiBeat.h"
#include "CustomBeat.h"
#include "NarrowSpreadSingleBeat.h"
#include "WideSpreadSingleBeat.h"
#include "WideSpreadMultiBeat.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();
	// Store instance of GameModeBase in Game Instance
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
	}

	BandLimitsThreshold = { 2.f,2.5f,3.f,3.f };
	BandLimits = { FVector2D(0.f, 50.f),FVector2D(51.f, 100.f),FVector2D(101.f, 2000.f),FVector2D(2001.f, 10000.f) };
	TimeWindow = 0.02f;
	HistorySize = 30.f;
}

AGameModeActorBase* ADefaultGameMode::SetGameModeActorBase(EGameModeActorName GameModeActorEnum)
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
	else if (GameModeActorEnum == EGameModeActorName::BeatTrack)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ABeatTrack>(BeatTrackClass);
	}
	else if (GameModeActorEnum == EGameModeActorName::Custom)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	else
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GameMode Init Error"));
		}
	}
	return GameModeActorBase;
}

