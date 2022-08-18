// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameModeActorStruct.h"
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

	void InsertToPlayerScoreStructArray(FPlayerScore PlayerScoreStructToAdd);

	TArray<FPlayerScore> GetArrayOfPlayerScoreStructs();

private:

	FPlayerScore PlayerScoreStruct;

	UPROPERTY(EditAnywhere, Category = Score)
	TArray<FPlayerScore> ArrayOfPlayerScoreStructs;
};
