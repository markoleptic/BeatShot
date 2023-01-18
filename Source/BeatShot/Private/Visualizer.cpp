// Fill out your copyright notice in the Description page of Project Settings.


#include "Visualizer.h"

#include "DefaultGameMode.h"
#include "AudioAnalyzerManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVisualizer::AVisualizer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AVisualizer::BeginPlay()
{
	Super::BeginPlay();
	AASettings = LoadAASettings();
}

// Called every frame
void AVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualizer::OnAAPlayerLoaded(UAudioAnalyzerManager* Manager)
{
	AAManager = Manager;
	InitializeCubes(AASettings.NumBandChannels);
	bAAManagerInitialized = true;
}

void AVisualizer::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	 bAAManagerInitialized = false;
	 InitializeCubes(AASettingsStruct.NumBandChannels);
	 AASettings = AASettingsStruct;
	 bAAManagerInitialized = true;
}

