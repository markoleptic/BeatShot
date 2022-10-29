// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerSettings.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API USaveGamePlayerSettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGamePlayerSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPlayerSettings PlayerSettings;
};
