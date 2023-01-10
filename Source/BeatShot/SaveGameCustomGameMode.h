// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameCustomGameMode.generated.h"

UCLASS()
class BEATSHOT_API USaveGameCustomGameMode : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FGameModeActorStruct> CustomGameModes;
};
