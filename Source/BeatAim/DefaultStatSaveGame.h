// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "DefaultStatSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class BEATAIM_API UDefaultStatSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	UDefaultStatSaveGame();

	float GetBestSpiderShotScore();

	FVector GetPlayerLocation();

	void SavePlayerLocation(FVector PlayerLocationToSave);

	void SaveHighScore(float HighScoreToSave);

private:
	UPROPERTY(EditAnywhere, Category = Score)
	FVector PlayerLocation;

	UPROPERTY(EditAnywhere, Category = Score)
	float BestSpiderShotScore;

	UPROPERTY(EditAnywhere, Category = Score)
	TArray<float> SpiderShotScoreArray;

	TArray<TArray<float>> ArrayOfSpiderShotScores;

	void InsertToArrayOfSpiderShotScores(TArray<float> ScoreArray);

	TArray<TArray<float>> GetArrayOfSpiderShotScores();
};
