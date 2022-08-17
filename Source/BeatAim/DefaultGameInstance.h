// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatAimGameModeBase.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class AGameModeBase;
class ASphereTarget;
/**
 * 
 */
UCLASS()
class BEATAIM_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	//References

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ADefaultCharacter* DefaultCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ATargetSpawner* TargetSpawnerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ASphereTarget* SphereTargetRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TArray<ASphereTarget*> SphereTargetArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	//class ABeatAimGameModeBase* GameModeBaseRef;
	ABeatAimGameModeBase* GameModeBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class AGameModeActorBase* GameModeActorBaseRef;

	// Register Functions

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterTargetSpawner(ATargetSpawner* TargetSpawner);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterSphereTarget(ASphereTarget* SphereTarget);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeBase(ABeatAimGameModeBase* GameModeBase);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeActorBase(AGameModeActorBase* GameModeActorBase);

	// Modifying Game Mode Variables

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdateTargetsSpawned(bool doReset = false);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdateShotsFired(bool doReset = false);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdateTargetsHit(bool doReset = false);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdateScore(float ScoreToAdd, bool doReset = false);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdateHighScore(float HighScoreToUpdate);

	// Getter functions for Game Mode Variables

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetTargetsHit();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetShotsFired();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetTargetsSpawned();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetScore();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetHighScore();

	// Setter functions for Game Mode Variables, used to load saved scores

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void SetTargetsHit(float SavedTargetsHit);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void SetShotsFired(float SavedShotsFired);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void SetTargetsSpawned(float SavedTargetsSpawned);

	// Settings Menu setter and getter functions

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float InputSensitivity);

	UFUNCTION(BlueprintCallable)
	float GetSensitivity();

	UFUNCTION(BlueprintCallable)
	void SetTargetSpawnCD(float NewTargetSpawnCD);

	UFUNCTION(BlueprintCallable)
	float GetTargetSpawnCD();

private:
	// Game Mode Variables Associated with Scoring

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float TargetsHit = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float ShotsFired = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float TargetsSpawned = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float Score = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float HighScore = 0;

	// Settings Menu options so that options are saved even if character hasn't spawned

	UPROPERTY(VisibleAnywhere, Category = "Settings Menu")
		float Sensitivity = 12.59;

	UPROPERTY(VisibleAnywhere, Category = "Settings Menu")
		float TargetSpawnCD = 0.35f;

};
