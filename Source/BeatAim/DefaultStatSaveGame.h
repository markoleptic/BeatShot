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

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings(FPlayerSettings PlayerSettingsToSave);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FPlayerSettings LoadSettings();

	// Settings

	UPROPERTY(VisibleAnywhere, Category = "Settings")
		FPlayerSettings PlayerSettings;

	// Scores

	UPROPERTY(VisibleAnywhere, Category = "Score")
		FPlayerScore PlayerScoreStruct;

	UPROPERTY(VisibleAnywhere, Category = "Score")
		TArray<FPlayerScore> ArrayOfPlayerScoreStructs;
};
