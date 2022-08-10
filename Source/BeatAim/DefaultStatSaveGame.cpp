// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStatSaveGame.h"

UDefaultStatSaveGame::UDefaultStatSaveGame()
{
	PlayerLocation = FVector(0.f, 0.f, 0.f);
	BestSpiderShotScore = 0.f;
	//SpiderShotScoreArray.Init(0.f, 3.f);
	//ArrayOfSpiderShotScores.Add(SpiderShotScoreArray);
}

float UDefaultStatSaveGame::GetBestSpiderShotScore()
{
	return BestSpiderShotScore;
}

FVector UDefaultStatSaveGame::GetPlayerLocation()
{
	return PlayerLocation;
}

void UDefaultStatSaveGame::SavePlayerLocation(FVector PlayerLocationToSave)
{
	PlayerLocation = PlayerLocationToSave;
}

void UDefaultStatSaveGame::SaveHighScore(float HighScoreToSave)
{
	BestSpiderShotScore = HighScoreToSave;
}

void UDefaultStatSaveGame::InsertToArrayOfSpiderShotScores(TArray<float> ScoreArray)
{
	ArrayOfSpiderShotScores.Add(ScoreArray);
}

TArray<TArray<float>> UDefaultStatSaveGame::GetArrayOfSpiderShotScores()
{
	return ArrayOfSpiderShotScores;
}

