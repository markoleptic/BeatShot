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
class BEATSHOT_API UDefaultStatSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UDefaultStatSaveGame();

	// Settings

	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SaveSettings(FPlayerSettings PlayerSettingsToSave);

	UFUNCTION(BlueprintCallable, Category = "Settings")
	FPlayerSettings LoadSettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	FPlayerSettings SavedPlayerSettings;

	// Scoring

	UFUNCTION(BlueprintCallable, Category = "Score")
	void InsertToPlayerScoreStructArray(FPlayerScore PlayerScoreStructToAdd);

	UFUNCTION(BlueprintCallable, Category = "Score")
	TArray<FPlayerScore> GetArrayOfPlayerScoreStructs();

	UPROPERTY(VisibleAnywhere, Category = "Score")
	FPlayerScore PlayerScoreStruct;

	UPROPERTY(VisibleAnywhere, Category = "Score")
	TArray<FPlayerScore> ArrayOfPlayerScoreStructs;
};
