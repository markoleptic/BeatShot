// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefaultCharacter.h"
#include "GameModeActorStruct.h"
#include "GameModeActorBase.generated.h"

class ASphereTarget;
class UAudioAnalyzerManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore, NewPlayerScoreStruct);

UCLASS()
class BEATSHOT_API AGameModeActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameModeActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void SavePlayerScores();

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void LoadPlayerScores();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// called by WBP_Countdown when player clicks anywhere on screen, including after game restart
	UFUNCTION(BlueprintCallable)
	virtual void HandleGameStart();

	UFUNCTION(BlueprintCallable)
	virtual void HandleGameRestart();

	UFUNCTION(BlueprintCallable)
	virtual void StartGameMode();

	UFUNCTION(BlueprintCallable)
	virtual void EndGameMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Game Properties")
	FUpdateScoresToHUD UpdateScoresToHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float CountdownTimerLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float GameModeLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
	FTimerHandle CountDownTimer;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Scoring")
	float MaxScorePerTarget;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateHighScore();

	// Called by SphereTarget when it takes damage
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	virtual void UpdatePlayerScores(float TimeElapsed);

	// Called by TargetSpawner when a SphereTarget is spawned
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTargetsSpawned();

	// Called by DefaultCharacter when player shoots during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateShotsFired();

	// Called by Projectile when a Player's projectile hits a SphereTarget during an active game that is not a BeatTracking game modes
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTargetsHit();

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void UpdateTrackingScore(float DamageTaken, float TotalPossibleDamage);
};
