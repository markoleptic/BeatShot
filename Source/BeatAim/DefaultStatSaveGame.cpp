// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultStatSaveGame.h"

UDefaultStatSaveGame::UDefaultStatSaveGame()
{
}

void UDefaultStatSaveGame::InsertToPlayerScoreStructArray(FPlayerScore PlayerScoreStructToAdd)
{
	ArrayOfPlayerScoreStructs.Add(PlayerScoreStructToAdd);
}

TArray<FPlayerScore> UDefaultStatSaveGame::GetArrayOfPlayerScoreStructs()
{
	return ArrayOfPlayerScoreStructs;
}


