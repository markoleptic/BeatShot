// Copyright Epic Games, Inc. All Rights Reserved.

#include "DefaultGameMode.h"
#include "BeatTrack.h"
#include "DefaultGameInstance.h"
#include "GameModeActorStruct.h"
#include "NarrowSpreadMultiBeat.h"
#include "CustomBeat.h"
#include "DefaultPlayerController.h"
#include "NarrowSpreadSingleBeat.h"
#include "WideSpreadSingleBeat.h"
#include "WideSpreadMultiBeat.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ADefaultGameMode::BeginPlay()
{
	Super::BeginPlay();

	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));
	if (GI)
	{
		GI->RegisterGameModeBase(this);
		// listen to changes that are made to Audio Analyzer settings in case user changes during a game
		GI->OnAASettingsChange.AddDynamic(this, &ADefaultGameMode::RefreshAASettings);
	}
	GameModeActorAlive = false;
	InitializeGameMode(GI->GameModeActorStruct.GameModeActorName);
}

void ADefaultGameMode::InitializeGameMode(EGameModeActorName GameModeActorName)
{
	// Wide Spread Multi Beat
	if (GameModeActorName == EGameModeActorName::WideSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadMultiBeat>(WideSpreadMultiBeatClass);
	}
	// Narrow Spread Multi Beat
	else if (GameModeActorName == EGameModeActorName::NarrowSpreadMultiBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadMultiBeat>(NarrowSpreadMultiBeatClass);
	}
	// Narrow Spread Single Beat
	else if (GameModeActorName == EGameModeActorName::NarrowSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ANarrowSpreadSingleBeat>(NarrowSpreadSingleBeatClass);
	}
	// Wide Spread Single Beat
	else if (GameModeActorName == EGameModeActorName::WideSpreadSingleBeat)
	{
		GameModeActorBase = GetWorld()->SpawnActor<AWideSpreadSingleBeat>(WideSpreadSingleBeatClass);
	}
	else if (GameModeActorName == EGameModeActorName::BeatTrack)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ABeatTrack>(BeatTrackClass);
	}
	else if (GameModeActorName == EGameModeActorName::BeatGrid)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	else if (GameModeActorName == EGameModeActorName::Custom)
	{
		GameModeActorBase = GetWorld()->SpawnActor<ACustomBeat>(CustomBeatClass);
	}
	else
	{
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("GameMode Init Error"));
		}
	}

	// initialize AA settings
	RefreshAASettings();

	// spawn visualizer
	const FVector VisualizerLocation = { 100,0,60 };
	const FRotator Rotation = FRotator::ZeroRotator;
	const FActorSpawnParameters SpawnParameters;
	Visualizer = GetWorld()->SpawnActor(VisualizerClass, &VisualizerLocation, &Rotation, SpawnParameters);

	// call blueprint function
	InitializeAudioManagers();

	// Call blueprint function
	PauseAAManagers();

	// Show crosshair and countdown
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCrosshair();
	Cast<ADefaultPlayerController>(GetWorld()->GetFirstPlayerController())->ShowCountdown();
}

void ADefaultGameMode::RefreshAASettings()
{
	AASettings = GI->LoadAASettings();
}


