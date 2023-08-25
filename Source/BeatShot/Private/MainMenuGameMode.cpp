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

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::SetBaseSoundMix(GetWorld(), GlobalSoundMix);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, LoadPlayerSettings().VideoAndSound.GlobalVolume / 100, 1, 0.0f, true);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, LoadPlayerSettings().VideoAndSound.MenuVolume / 100, 1, 0.0f, true);

	MainMenuMusicComp->FadeIn(2.f, 1.f, 0.f);
}

void AMainMenuGameMode::BindGameModesWidgetToTargetManager(UGameModesWidget* GameModesWidget)
{
	TargetManager = GetWorld()->SpawnActor<ATargetManagerPreview>(TargetManagerClass, FVector::Zero(), FRotator::ZeroRotator);
	TargetManager->InitBoxBoundsWidget(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->BoxBounds);
	TargetManager->Init(GameModesWidget->GetConfigPointer(), LoadPlayerSettings().Game);
	TargetManager->CreateTargetWidget.BindUObject(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview, &UCustomGameModesWidget_Preview::ConstructTargetWidget);

	GameModesWidget->RequestSimulateTargetManager.AddUObject(this, &ThisClass::StartSimulation);
	GameModesWidget->OnPopulateGameModeOptions.AddUObject(this, &ThisClass::OnGameModesWidgetPopulateGameModeOptions);
	GameModesWidget->OnCreatorViewVisibilityChanged.AddUObject(this, &ThisClass::OnCreatorViewVisibilityChanged);
}

void AMainMenuGameMode::OnGameModesWidgetPopulateGameModeOptions()
{
	StartSimulation();
}

void AMainMenuGameMode::OnCreatorViewVisibilityChanged(const bool bVisible)
{
	if (bVisible)
	{
		StartSimulation();
	}
	else
	{
		FinishSimulation();
	}
}

void AMainMenuGameMode::StartSimulation()
{
	if (!TargetManager)
	{
		return;
	}

	if (TargetManagerIsSimulating())
	{
		FinishSimulation();
	}
	
	TargetManager->RestartSimulation();
	TargetManager->SetSimulatePlayerDestroyingTargets(true, 1.1f);
	TargetManager->SetShouldSpawn(true);
	
	// Bind the simulation timer
	SimulationTimerDelegate.BindUObject(this, &ThisClass::FinishSimulation);
	SimulationIntervalDelegate.BindUObject(this, &ThisClass::OnSimulationInterval);

	// Start timers
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(SimulationTimer, SimulationTimerDelegate, 15.f, false);
	TimerManager.SetTimer(SimulationIntervalTimer, SimulationIntervalDelegate, TargetManager->GetSimulation_TargetSpawnCD(), true, 1.f);
}

void AMainMenuGameMode::OnSimulationInterval()
{
	if (TargetManager)
	{
		TargetManager->OnAudioAnalyzerBeat();
	}
}

void AMainMenuGameMode::FinishSimulation()
{
	// Unbind delegates
	if (SimulationTimerDelegate.IsBound())
	{
		SimulationTimerDelegate.Unbind();
	}
	if (SimulationIntervalDelegate.IsBound())
	{
		SimulationIntervalDelegate.Unbind();
	}
	
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();

	// Clear Timers
	TimerManager.ClearTimer(SimulationIntervalTimer);
	TimerManager.ClearTimer(SimulationTimer);

	if (TargetManager)
	{
		TargetManager->SetShouldSpawn(false);
		TargetManager->FinishSimulation();
	}
}

bool AMainMenuGameMode::TargetManagerIsSimulating() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(SimulationTimer);
}