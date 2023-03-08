// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerScore.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"

class UHorizontalBox;
class ABeatAimGameModeBase;
class ABSCharacter;
class UProgressBar;
class UTextBlock;

UCLASS()
class USERINTERFACE_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* TargetBar;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* Accuracy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TargetsHitText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ShotsFiredText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TargetsSpawnedText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentScoreText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HighScoreText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* GameModeNameText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SongTitle;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* SongTimeElapsed;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TotalSongLength;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentStreakBestText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TargetsSpawnedBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* StreakBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* TargetsHitBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* ShotsFiredBox;

	/** Takes in a PlayerScore struct and updates all elements of the PlayerHUD. Called from GameModeActorBase */
	UFUNCTION()
	void UpdateAllElements(FPlayerScore NewPlayerScoreStruct);
	/** Callback function for OnSecondPassed to update the current song progress. Called every second by DefaultGameMode */
	UFUNCTION()
	void UpdateSongProgress(const float PlaybackTime);

private:
};
