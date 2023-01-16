// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameMode.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	MainMenuMusicComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MainMenuMusicComp"));
	SetRootComponent(MainMenuMusicComp);
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetBaseSoundMix(GetWorld(), GlobalSoundMix);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, LoadPlayerSettings().GlobalVolume/100, 1, 0.0f, true);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, LoadPlayerSettings().MenuVolume/100, 1, 0.0f, true);

	MainMenuMusicComp->FadeIn(2.f, 1.f, 0.f);
}
