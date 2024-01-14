// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "FunctionalTest.h"
#include "SaveGamePlayerSettings.h"
#include "TargetManagerFunctionalTest.generated.h"

class UBSGameModeDataAsset;
class ATargetManager;

UCLASS()
class BEATSHOT_API ATargetManagerFunctionalTest : public AFunctionalTest
{
	GENERATED_BODY()

public:
	ATargetManagerFunctionalTest();

protected:
	// AFunctionalTest
	virtual void PrepareTest() override;
	virtual bool IsReady_Implementation() override;
	virtual void StartTest() override;
	virtual void FinishTest(EFunctionalTestResult TestResult, const FString& Message) override;
	virtual void CleanUp() override;
	// ~AFunctionalTest

	/** Calls OnAudioAnalyzerBeat function of Target Manager. Callback function for BeatTimer. */
	void OnAudioAnalyzerBeat();

	/** Initializes Target Manager and starts timers. */
	void StartGameMode(const FBSConfig& InConfig);

	/** Clears Target Manager, clears timers, calls StartGameMode if not finished.
	 *  Callback function for GameModeTimer. */
	void StopGameMode();

	/** Calls DamageSelf on targets. Callback function for DestroyTimer. */
	void DestroyTargets();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ATargetManager> TargetManagerClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;

	/** The length of time to test each individual game mode and difficulty. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float GameModeDuration = 10.f;
	
	/** How frequently to call the OnAudioAnalyzerBeat function of Target Manager, effectively spawning or activating
	 *  targets. Targets will be destroyed at this same frequency, offset by half. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BeatFrequency = 0.1f;
	
	/** The spawned Target Manager */
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

	/** The game modes to test, obtained from GameModeDataAsset. */
	TArray<FBSConfig> DefaultGameModes;

	/** The current index of the DefaultGameModes array. */
	int32 CurrentIndex = 0;
};
