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

	UPROPERTY(EditAnywhere, Category = Score)
	FVector PlayerLocation;

	UPROPERTY(EditAnywhere, Category = Score)
	float BestSpiderShotScore;

	UPROPERTY(EditAnywhere, Category = Score)
	TArray<float> SpiderShotScoreArray;

	TArray<TArray<float>> ArrayOfSpiderShotScores;

	float ArraySize;

	void InsertToArrayOfSpiderShotScores(TArray<float> ScoreArray);

	TArray<TArray<float>> GetArrayOfSpiderShotScores();
};
