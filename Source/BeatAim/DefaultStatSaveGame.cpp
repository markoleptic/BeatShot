// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStatSaveGame.h"

UDefaultStatSaveGame::UDefaultStatSaveGame()
{
	PlayerLocation = FVector(0.f, 0.f, 0.f);
	SpiderShotScoreArray.Init(0.f, 3.f);
	ArrayOfSpiderShotScores.Add(SpiderShotScoreArray);
}

void UDefaultStatSaveGame::InsertToArrayOfSpiderShotScores(TArray<float> ScoreArray)
{
	ArrayOfSpiderShotScores.Add(ScoreArray);
}

TArray<TArray<float>> UDefaultStatSaveGame::GetArrayOfSpiderShotScores()
{
	return ArrayOfSpiderShotScores;
}

