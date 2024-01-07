// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/GameModeBase.h"
#include "MainMenuGameMode.generated.h"

class UGameModesWidget;
class ATargetManagerPreview;
class ABSPlayerController;
class UTargetWidget;

/** Game mode used for the Main Menu level. Responsible for spawning a Target Manager for game mode preview purposes,
 *  which is bound to the GameModesWidget. */
UCLASS()
class BEATSHOT_API AMainMenuGameMode : public AGameModeBase, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	AMainMenuGameMode();

	virtual void BeginPlay() override;

	/** Enables communication between GameModesWidget and TargetManager */
	void BindGameModesWidgetToTargetManager(UGameModesWidget* GameModesWidget);

	/** Called when the GameModesWidget wants to start or stop previewing a game mode */
	void OnRequestSimulationStateChange(const bool bSimulate);

	/** Starts timers, binds timer delegates, restarts TargetManager */
	void StartSimulation();

	/** Manually calls OnAudioAnalyzerTick in TargetManager at fixed intervals */
	void OnSimulationInterval();

	/** Clears the timers and calls FinishSimulation on TargetManager */
	void FinishSimulation();

	/** Returns whether or not the SimulationTimer is active */
	bool TargetManagerIsSimulating() const;

	/** Called when the GameModesWidget contains at least one breaking game mode option, or none */
	void OnGameModeBreakingChange(const bool bIsGameModeBreakingChange);

	void FadeInMainMenuMusic(const float FadeInLength);

	void FadeOutMainMenuMusic(const float FadeOutLength);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* MainMenuMusicComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classes")
	TSubclassOf<ATargetManagerPreview> TargetManagerClass;

	/** The spawned TargetManager that provides preview logic for CustomGameModesWidget_Preview */
	UPROPERTY()
	TObjectPtr<ATargetManagerPreview> TargetManager;

	FTimerDelegate SimulationIntervalDelegate;
	FTimerHandle SimulationIntervalTimer;

	FTimerDelegate SimulationTimerDelegate;
	FTimerHandle SimulationTimer;

	/** Whether or not GameModesWidget has at least one breaking game mode option, or none. Prevents simulation if true */
	bool bGameModeBreakingChangePresent = false;
};
