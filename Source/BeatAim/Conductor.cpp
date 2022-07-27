// Fill out your copyright notice in the Description page of Project Settings.


#include "Conductor.h"
#include "AudioAnalyzerManager.h"
#include "Runtime/Core/Public/Misc/Paths.h"

// Sets default values
AConductor::AConductor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AConductor::BeginPlay()
{
	Super::BeginPlay();

	//Construct AudioAnalyzerManager BP and Set
	Manager = NewObject<UAudioAnalyzerManager>(this, TEXT("AAManager"));
	//Init Player Audio and Play
	FString AudioFileName = FPaths::ProjectContentDir() + "NonAssets/AShotInTheDark.mp3";
	if (Manager->InitPlayerAudio(AudioFileName))
	{
		// Type: Linear
		// Channel Mode: All in one
		// Channel: 0
		// Num Freq Bands: 1
		// Time Window: 0.02s
		// History Size: 30
		// Use Peak Values: true
		// Num Peaks: 1
		Manager->InitSpectrumConfig(ESpectrumType::ST_Linear,
			EChannelSelectionMode::All_in_one,
			0,
			1,
			0.02,
			30,
			true,
			1);
		Manager->Play();
	}
}

// Called every frame
void AConductor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Manager && Manager->IsPlaying())
	{
		TArray<float> Frequencies;
		TArray<float> AverageFrequencies;
		Manager->GetSpectrum(Frequencies, AverageFrequencies, false);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 
				5.f, 
				FColor::Cyan, 
				FString::Printf(TEXT("%f"), 
				Frequencies[0]));
	}
}
