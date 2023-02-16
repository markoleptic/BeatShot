// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


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
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, LoadPlayerSettings().VideoAndSound.GlobalVolume/100, 1, 0.0f, true);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, LoadPlayerSettings().VideoAndSound.MenuVolume/100, 1, 0.0f, true);

	MainMenuMusicComp->FadeIn(2.f, 1.f, 0.f);
}
