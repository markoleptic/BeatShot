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
	Cast<ADefaultGameMode>(UGameplayStatics::GetGameMode(GetWorld()))->OnAAPlayerLoaded.AddDynamic(this, &AVisualizer::OnAAManagerLoaded);
}

// Called every frame
void AVisualizer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVisualizer::OnAAManagerLoaded(UAudioAnalyzerManager* Manager)
{
	AAManager = Manager;
	InitializeCubes(LoadAASettings().NumBandChannels);
	bAAManagerInitialized = true;
}

void AVisualizer::UpdateAASettings(const FAASettingsStruct AASettingsStruct)
{
	bAAManagerInitialized = false;
	InitializeCubes(AASettingsStruct.NumBandChannels);
	AASettings = AASettingsStruct;
	bAAManagerInitialized = true;
}

