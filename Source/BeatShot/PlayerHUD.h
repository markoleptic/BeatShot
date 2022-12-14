// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PlayerHUD.generated.h"

class UHorizontalBox;
class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class UProgressBar;
class UTextBlock;

UCLASS()
class BEATSHOT_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
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

private:
	
	/** Takes in a PlayerScore struct and updates all elements of the PlayerHUD. Called from GameModeActorBase */
	UFUNCTION()
	void UpdateAllElements(FPlayerScore NewPlayerScoreStruct);
	/** Callback function for OnAAManagerSecondPassed to update the current song progress. Called every second by DefaultGameMode */
	UFUNCTION()
	void UpdateSongProgress(const float PlaybackTime);
};
