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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UComboBoxString* GameModeSelectComboBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		UComboBoxString* SongSelectorComboBox;

	// accuracy bar
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

	// averages

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

	// game mode specific player scores chart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleCartesianPlot* PlayerScoresChart;

	// game mode specific player accuracy chart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleCartesianPlot* PlayerAccuracyChart;

	// all game modes player scores chart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleBarChart* AllPlayerScoresChart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Stats")
		USimpleBarChart* AllPlayerAccuracyChart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		FAllGameModeActorNames AllGameModeActorNames;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMap;

	// only populates the Last Session Box
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateRecentScores(FString SelectedItem, ESelectInfo::Type SelectionType);

	// songs to display depend on the selected game mode (SelectedItem)
	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateSongSelectorComboBox(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateAllMatchingSavedScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToLoad);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateGameModeSelectComboBox();

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void PopulateAvgScoresBySong(FString SelectedItem, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void LoadPlayerScores();

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
		void ClearScoreBoxes(FString SelectedItem, ESelectInfo::Type SelectionType);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
		class UDefaultGameInstance* GI;
};
