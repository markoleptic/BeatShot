// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

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
	class ABeatAimGameModeBase* GameModeBaseRef;

	// Register Functions

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterTargetSpawner(ATargetSpawner* TargetSpawner);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterSphereTarget(ASphereTarget* SphereTarget);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeBase(ABeatAimGameModeBase* GameModeBase);

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
};
