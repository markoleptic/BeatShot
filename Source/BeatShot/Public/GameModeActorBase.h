// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveGamePlayerSettings.h"
#include "SaveGameCustomGameMode.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadInterface.h"
#include "GameModeActorBase.generated.h"

class ASphereTarget;
class AFloatingTextActor;
class ADefaultGameMode;

UCLASS()
class BEATSHOT_API AGameModeActorBase : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	AGameModeActorBase();

protected:
	virtual void BeginPlay() override;

	virtual void Destroyed() override;

	virtual void Tick(float DeltaTime) override;

public:
	/** Starts the game mode timer */
	UFUNCTION()
	void StartGameMode();

	/** Timer that spans the length of the song */
	UPROPERTY(VisibleAnywhere)
	FTimerHandle GameModeLengthTimer;

	/** The game mode defining properties */
	UPROPERTY(VisibleAnywhere)
	FGameModeActorStruct GameModeActorStruct;

	/** Function called when new settings are saved */
	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);

private:
	/** called during StartGameModeMainMenu. Loads player scores, sets the score amount for each target, and binds to TargetSpawner's
	 *  UpdateTargetSpawned and OnStreakUpdate */
	UFUNCTION()
	void InitializeGameModeActor();

	/** Reports to DefaultGameMode when the song has finished */
	UFUNCTION()
	void OnGameModeLengthTimerComplete();

	/** Function bound to DefaultGameMode's OnBeatTrackTargetSpawned delegate.
	 *  Binds the tracking target's health component's OnBeatTrackTick delegate to UpdateTrackingScore */
	UFUNCTION()
	void OnBeatTrackTargetSpawned(ASphereTarget* TrackingTarget);

	/** The floating combat text class */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AFloatingTextActor> FloatingTextActorToSpawn;

#pragma region HUDUpdate
	
	/** Function bound to DefaultGameMode's OnTargetDestroyed delegate, which is executed by TargetSpawner.
	 *  Passes the time that the target was alive for */
	UFUNCTION()
	void UpdatePlayerScores(const float TimeElapsed);

	/** Function bound to the tracking target's health component's OnBeatTrackTick delegate,
	 *  which passes the current damage taken, and the total possible damage. Executed on tick
	 *  by SphereTarget */
	UFUNCTION()
	void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);

	/** Function bound to DefaultGameMode's OnTargetSpawned delegate to keep track of number of targets spawned.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void UpdateTargetsSpawned();

	/** Function bound to Gun_AK47's FOnShotFired delegate to keep track of number of targets spawned.
	 *  Executed by Gun_AK47 */
	UFUNCTION()
	void UpdateShotsFired();

	/** Function bound to DefaultGameMode's OnStreakUpdate delegate to keep track of streak.
	 *  Executed by TargetSpawner */
	UFUNCTION()
	void OnStreakUpdate(int32 Streak, FVector Location);

	/* Called by UpdatePlayerScores since everytime that function is called, a target has been hit */
	void UpdateTargetsHit();

	/** Called by UpdatePlayerScores or UpdatingTrackingScores to recalculate the high score if needed  */
	void UpdateHighScore();

#pragma endregion

#pragma region Scoring

	/** Loads the player scores and finds the highest score that matches the GameModeActorBase, Difficulty, and song title */
	void LoadMatchingPlayerScores();

public:
	/** Prepares PlayerScores for saving in GameInstance */
	TArray<FPlayerScore> GetPlayerScores();

private:
	/** The "live" player score objects, which start fresh and import high score from SavedPlayerScores */
	UPROPERTY(VisibleAnywhere)
	FPlayerScore CurrentPlayerScore;

	/** Max score per target based on total amount of targets that could spawn */
	UPROPERTY(VisibleAnywhere)
	float MaxScorePerTarget;

#pragma endregion

	float CheckFloatNaN(const float ValueToCheck, const float ValueToRound);

	/** Whether or not to show the Streak Combat Text */
	bool bShowStreakCombatText;

	/** The frequency at which to show Streak Combat Text */
	int32 CombatTextFrequency;

	/** Reference to the GameMode because it frequently needs to be called */
	UPROPERTY()
	ADefaultGameMode* GameMode;
};
