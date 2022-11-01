// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefaultCharacter.h"
#include "GameModeActorStruct.h"
#include "GameModeActorBase.generated.h"

class ASphereTarget;
class UAudioAnalyzerManager;
class UDefaultGameInstance;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore, NewPlayerScoreStruct);

UCLASS()
class BEATSHOT_API AGameModeActorBase : public AActor
{
	GENERATED_BODY()

public:

	AGameModeActorBase();

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void SavePlayerScores();

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void LoadPlayerScores();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// called by WBP_Countdown when player clicks anywhere on screen, including after game restart
	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void InitializeGameModeActor();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void StartGameMode();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void EndGameMode(bool ShouldSavePlayerScores = false);

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void StartCountDownTimer();

	UFUNCTION(BlueprintCallable, Category = "Game Start/End")
		void OnGameModeLengthTimerComplete();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Game Properties")
		FUpdateScoresToHUD UpdateScoresToHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
		FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
		FTimerHandle CountDownTimer;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		UDefaultGameInstance* GI;

	// Scoring

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;

	// the saved score object, with accuracy, etc.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScore SavedPlayerScores;

	// the wrapper struct that contains the array of saved player score objects
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScoreArrayWrapper PlayerScoreArrayWrapper;

	// the "live" player score objects, which start fresh and import high score from SavedPlayerScores
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		FPlayerScore PlayerScores;

	// max score per target based on total amount of targets that could spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
		float MaxScorePerTarget;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateHighScore();

	// Called by SphereTarget when it takes damage
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdatePlayerScores(float TimeElapsed);

	// Called by TargetSpawner when a SphereTarget is spawned
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTargetsSpawned();

	// Called by DefaultCharacter when player shoots during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateShotsFired();

	// Called by Projectile when a Player's projectile hits a SphereTarget during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTargetsHit();

	// Called when IsTrackingGameMode == true
	UFUNCTION(BlueprintCallable, Category = "Scoring")
		void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);
};
