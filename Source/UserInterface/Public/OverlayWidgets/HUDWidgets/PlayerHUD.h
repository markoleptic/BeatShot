// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class UHitTimingWidget;
class UImage;
class UHorizontalBox;
class ABeatAimGameModeBase;
class ABSCharacter;
class UProgressBar;
class UTextBlock;
class UMaterialInstanceDynamic;

/** The widget displayed during a game mode that shows current stats */
UCLASS()
class USERINTERFACE_API UPlayerHUD : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void Init(const FBSConfig& InConfig);

	FOnPlayerSettingsChanged_Game& GetGameDelegate() { return OnPlayerSettingsChangedDelegate_Game; }

	/** Called when another class saves Game settings */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	
	/** Takes in a PlayerScore struct and updates all elements of the PlayerHUD */
	UFUNCTION()
	void UpdateAllElements(const FPlayerScore& NewPlayerScoreStruct, const float NormalizedHitTimingError, const float HitTimingError);
	
	/** Callback function for OnSecondPassed to update the current song progress. Called every second by DefaultGameMode */
	UFUNCTION()
	void UpdateSongProgress(const float PlaybackTime);

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHitTimingWidget* HitTimingWidget;

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
};
