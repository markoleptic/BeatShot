// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "MainMenuGameMode.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SubMenuWidgets/GameModesWidget.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidget_CreatorView.h"
#include "Target/TargetManagerPreview.h"

AMainMenuGameMode::AMainMenuGameMode()
{
	MainMenuMusicComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MainMenuMusicComp"));
	SetRootComponent(MainMenuMusicComp);
}

void AMainMenuGameMode::BindControllerToTargetManager(ABSPlayerController* InController, UGameModesWidget* GameModesWidget)
{
	TargetManager = GetWorld()->SpawnActor<ATargetManagerPreview>(TargetManagerClass, FVector::Zero(), FRotator::ZeroRotator);
	TargetManager->Init(*GameModesWidget->GetConfigPointer(), LoadPlayerSettings().Game);
	TargetManager->InitTargetManagerPreview(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->BoxBounds, GameModesWidget->GetConfigPointer());
	TargetManager->CreateTargetWidget.BindUObject(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview, &UCustomGameModesWidget_Preview::ConstructTargetWidget);
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetBaseSoundMix(GetWorld(), GlobalSoundMix);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, LoadPlayerSettings().VideoAndSound.GlobalVolume / 100, 1, 0.0f, true);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, LoadPlayerSettings().VideoAndSound.MenuVolume / 100, 1, 0.0f, true);

	MainMenuMusicComp->FadeIn(2.f, 1.f, 0.f);
}

void AMainMenuGameMode::SimulateTargetManager()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(TotalSimulationTimer))
	{
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(TotalSimulationTimer, 10.f, false);
	TargetManager->SetShouldSpawn(true);
	
	SimulateTargetManagerDelegate.BindLambda([this]
	{
		if (!GetWorld()->GetTimerManager().IsTimerActive(TotalSimulationTimer))
		{
			GetWorld()->GetTimerManager().ClearTimer(SimulateTargetManagerTimer);
			TargetManager->SetShouldSpawn(false);
			return;
		}
		if (TargetManager)
		{
			
			TargetManager->OnAudioAnalyzerBeat();
		}
	});
	
	GetWorld()->GetTimerManager().SetTimer(SimulateTargetManagerTimer, SimulateTargetManagerDelegate, 0.5f, true, 5.f);
}
