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
	TargetManager->InitBoxBoundsWidget(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->BoxBounds_Current,
		GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->BoxBounds_Min,
		GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->BoxBounds_Max,
		GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview->FloorDistance);
	TargetManager->Init(GameModesWidget->GetConfigPointer(), LoadPlayerSettings().Game);
	TargetManager->CreateTargetWidget.BindUObject(GameModesWidget->CustomGameModesWidget_CreatorView->Widget_Preview, &UCustomGameModesWidget_Preview::ConstructTargetWidget);
	
	GameModesWidget->RequestSimulateTargetManagerStateChange.AddUObject(this, &ThisClass::OnRequestSimulationStateChange);
	GameModesWidget->OnGameModeBreakingChange.AddUObject(this, &ThisClass::OnGameModeBreakingChange);
}

void AMainMenuGameMode::OnRequestSimulationStateChange(const bool bSimulate)
{
	if (bSimulate)
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

	if (bGameModeBreakingChangePresent)
	{
		return;
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
	if (TargetManager && !bGameModeBreakingChangePresent)
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

void AMainMenuGameMode::OnGameModeBreakingChange(const bool bIsGameModeBreakingChange)
{
	if (bIsGameModeBreakingChange == bGameModeBreakingChangePresent)
	{
		return;
	}
	bGameModeBreakingChangePresent = bIsGameModeBreakingChange;
	if (bIsGameModeBreakingChange && TargetManagerIsSimulating())
	{
		FinishSimulation();
	}
}
