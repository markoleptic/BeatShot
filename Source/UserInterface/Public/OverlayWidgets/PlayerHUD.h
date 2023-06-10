// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerScore.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UHorizontalBox;
class ABeatAimGameModeBase;
class ABSCharacter;
class UProgressBar;
class UTextBlock;

/** The widget displayed during a game mode that shows current stats */
UCLASS()
class USERINTERFACE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void InitHUD(const FBSConfig& InConfig);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* ProgressBar_Accuracy;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Accuracy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TargetsHit;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_ShotsFired;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TargetsSpawned;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_CurrentScore;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_HighScore;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_GameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SongTitle;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_SongTimeElapsed;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TotalSongLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_CurrentStreakBest;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_TargetsSpawned;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Streak;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_TargetsHit;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_ShotsFired;

	FBSConfig Config;

	/** Takes in a PlayerScore struct and updates all elements of the PlayerHUD */
	UFUNCTION()
	void UpdateAllElements(const FPlayerScore& NewPlayerScoreStruct);
	/** Callback function for OnSecondPassed to update the current song progress. Called every second by DefaultGameMode */
	UFUNCTION()
	void UpdateSongProgress(const float PlaybackTime);
};
