// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameModeActorStruct.h"
#include "SimpleBarChart.h"
#include "SimpleCartesianPlot.h"
#include "CartesianPlot.h"
#include "ScoreAnalysisWidget.generated.h"

struct FPlayerScore;
class ABeatAimGameModeBase;
class UDefaultGameInstance;
class ADefaultCharacter;
class UTargetSubsystem;
class UProgressBar;
class UTextBlock;
class UComboBoxString;
class UWidgetSwitcher;

/**
 * 
 */
UCLASS()
class BEATSHOT_API UScoreAnalysisWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:

	// UMG widgets

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UComboBoxString* GameModeSelectComboBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UComboBoxString* SongSelectorComboBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UWidgetSwitcher* GameModeSpecificGraphSwitcher;


	// recent score widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UProgressBar* TargetBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* Accuracy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* TargetsHitText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* ShotsFiredText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* TargetsSpawnedText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* CurrentScoreText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* HighScoreText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* TotalSongLength;

	// average score widgets
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UProgressBar* AvgTargetBar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* AvgAccuracy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* AvgTargetsHitText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* AvgShotsFiredText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* AvgTargetsSpawnedText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UTextBlock* AvgScoreText;

	// graphs

	// game mode specific player scores line graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleCartesianPlot* PlayerScoresChart;
	// game mode specific player accuracy line graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleCartesianPlot* PlayerAccuracyChart;
	// all game modes player scores bar graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleBarChart* AllPlayerScoresChart;
	// all game modes player accuracy bar graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleBarChart* AllPlayerAccuracyChart;

	/* ----------------- */
	/* Scoring Variables */
	/* ----------------- */

	// the player score map obtained from Game Instance
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;

	/* ---------------------------- */
	/* Game Mode Specific functions */
	/* ---------------------------- */

	// game modes to display based on saved scores in PlayerScoreMap
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateGameModeSelectComboBox();
	// songs to display depend on the selected game mode (SelectedGameMode)
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateSongSelectorComboBox(FString SelectedGameMode, ESelectInfo::Type SelectionType);
	// only populates the Last Session Box based on SelectedSong
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateRecentScores(FString SelectedSong, ESelectInfo::Type SelectionType);
	// similar to Populate Recent Scores,  but instead takes an average based on SelectedSong
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateAvgScoresBySong(FString SelectedSong, ESelectInfo::Type SelectionType);
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateAbsoluteMostRecentScores();


	// All Game Modes functions

	// NYI
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateAllMatchingSavedScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToLoad);

	/* ----------------- */
	/* Utility functions */
	/* ----------------- */

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void LoadPlayerScores();
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void ClearScoreBoxes(FString SelectedItem, ESelectInfo::Type SelectionType);

	/* ---------- */
	/* References */
	/* ---------- */

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		class UDefaultGameInstance* GI;
};
