// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerScore.generated.h"

UCLASS()
class BEATSHOT_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:

	// Map to store all scores, accessible by GameMode
	UPROPERTY()
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;
};
