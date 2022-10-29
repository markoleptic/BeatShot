// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "GameFramework/SaveGame.h"
#include "SaveGameAASettings.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API USaveGameAASettings : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGameAASettings();

	UPROPERTY(VisibleAnywhere, Category = "AA")
	FAASettingsStruct AASettings;
};
