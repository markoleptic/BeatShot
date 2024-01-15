// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "FunctionalTest.h"
#include "SaveGamePlayerSettings.h"
#include "BeatShotGameModeFunctionalTest.generated.h"

class UBSGameModeDataAsset;
class ATargetManager;

UCLASS()
class BEATSHOT_API ABeatShotGameModeFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()

public:
	ABeatShotGameModeFunctionalTest();

protected:
	// AFunctionalTest
	virtual void PrepareTest() override;
	virtual bool IsReady_Implementation() override;
	virtual void StartTest() override;
	virtual void FinishTest(EFunctionalTestResult TestResult, const FString& Message) override;
	virtual void CleanUp() override;
	// ~AFunctionalTest

	/** Fills GameModeConfigs array with configs for all difficulties. */
	void PopulateGameModeConfigs();

	/** Calls OnAudioAnalyzerBeat function of target manager. Callback function for BeatTimer. */
	void OnAudioAnalyzerBeat();

	/** Initializes target manager and starts timers. */
	void StartGameMode();

	/** Clears target manager, clears timers, calls StartGameMode if not finished.
	 *  Callback function for GameModeTimer. */
	void StopGameMode();

	/** Calls DamageSelf on targets. Callback function for DestroyTimer. */
	void DestroyTargets();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATargetManager> TargetManagerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;

	/** The game mode to test */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EBaseGameMode GameModeToTest = EBaseGameMode::None;

	/** The length of time to test each individual game mode and difficulty. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GameModeDuration = 10.f;
	
	/** How frequently to call the OnAudioAnalyzerBeat function of target manager, effectively spawning or activating
	 *  targets. Targets will be destroyed at this same frequency, offset by half. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BeatFrequency = 0.1f;

	/** Initial offset applied to the BeatTimer and DestroyTimer. DestroyTimer will get an additional offset of half
	 *  the BeatFrequency. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitialDelay = 0.1f;
	
	/** The spawned target manager. */
	UPROPERTY()
	TObjectPtr<ATargetManager> TargetManager;

	/** Game settings for the spawned targets; never changes. */
	FPlayerSettings_Game PlayerSettings_Game;

	/** Timer that spans the game mode duration. */
	FTimerHandle GameModeTimer;

	/** Looping timer that spawns or activates targets. */
	FTimerHandle BeatTimer;

	/** Looping timer slightly offset from BeatTimer that destroys activated targets. */
	FTimerHandle DestroyTimer;

	/** The current game mode config shared with the target manager. */
	TSharedPtr<FBSConfig> GameModeConfig;

	/** All difficulties for the selected GameModeToTest. */
	TArray<FBSConfig> GameModeConfigs;

	/** The current index inside GameModeConfigs. */
	int32 CurrentIndex = 0;
};
