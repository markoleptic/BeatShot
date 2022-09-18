// Fill out your copyright notice in the Description page of Project Settings.


#include "ScoreAnalysisWidget.h"
#include "DefaultGameInstance.h"
#include "Components/ComboBoxString.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "KantanChartsUMGClasses.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetTextLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void UScoreAnalysisWidget::NativeConstruct()
{
	GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(this));

	LoadPlayerScores();

	// update the song selector and clear the scoreboxes when game mode selection changes
	GameModeSelectComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::ClearScoreBoxes);
	GameModeSelectComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateSongSelectorComboBox);
	// repopulate recent and average score boxes when song selection changes
	SongSelectorComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateRecentScores);
	SongSelectorComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateAvgScoresBySong);
}

void UScoreAnalysisWidget::PopulateRecentScores(FString SelectedSong, ESelectInfo::Type SelectionType)
{
	// clear all text
	TargetBar->SetPercent(0.f);
	TargetsHitText->SetText(FText::AsNumber(0.f));
	Accuracy->SetText(FText::AsPercent(0.f));
	ShotsFiredText->SetText(FText::AsNumber(0.f));
	TargetsSpawnedText->SetText(FText::AsNumber(0.f));
	CurrentScoreText->SetText(FText::AsNumber(0.f));
	TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(0.f), 3)));

	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// find matching Elem.Key for SelectedGameMode
		if (GameModeSelectComboBox->GetSelectedOption() == UKismetStringLibrary::RightChop(UEnum::GetValueAsString<EGameModeActorName>(Elem.Key.GameModeActorName), 20) ||
			GameModeSelectComboBox->GetSelectedOption() == Elem.Key.CustomGameModeName)
		{
			// find matching Elem.Key for SelectedSong
			if (SelectedSong == Elem.Key.SongTitle)
			{
				FPlayerScore MostRecentScoreObject;
				FDateTime MostRecentTime = FDateTime::MinValue();

				// find the most recent time in the Score Array
				for (FPlayerScore CurrentPlayerScoreObject : Elem.Value.PlayerScoreArray)
				{
					if (CurrentPlayerScoreObject.Time > MostRecentTime)
					{
						MostRecentScoreObject = CurrentPlayerScoreObject;
						MostRecentTime = CurrentPlayerScoreObject.Time;
					}
				}

				const float TargetsHit = MostRecentScoreObject.TargetsHit;
				const float ShotsFired = MostRecentScoreObject.ShotsFired;
				const float TargetsSpawned = MostRecentScoreObject.TargetsSpawned;
				const float Score = MostRecentScoreObject.Score;

				// update values
				TargetBar->SetPercent(TargetsHit / ShotsFired);
				TargetsHitText->SetText(FText::AsNumber(TargetsHit));
				Accuracy->SetText(FText::AsPercent(TargetsHit / ShotsFired));
				ShotsFiredText->SetText(FText::AsNumber(ShotsFired));
				TargetsSpawnedText->SetText(FText::AsNumber(TargetsSpawned));
				CurrentScoreText->SetText(FText::AsNumber(Score));
				TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(MostRecentScoreObject.SongLength), 3)));
			}
		}
	}
}

void UScoreAnalysisWidget::PopulateSongSelectorComboBox(FString SelectedGameMode, ESelectInfo::Type SelectionType)
{
	SongSelectorComboBox->ClearOptions();

	TArray<FGameModeActorStruct> Keys;
	PlayerScoreMap.GenerateKeyArray(Keys);

	// looping through all game modes in PlayerScoreMap
	for (FGameModeActorStruct& GameModeActorStruct : Keys)
	{
		// find matching key for SelectedGameMode
		if (SelectedGameMode == UKismetStringLibrary::RightChop(UEnum::GetValueAsString(GameModeActorStruct.GameModeActorName),20) ||
			SelectedGameMode == GameModeActorStruct.CustomGameModeName)
		{
			if (SongSelectorComboBox->FindOptionIndex(GameModeActorStruct.SongTitle) == -1)
			{
				SongSelectorComboBox->AddOption(GameModeActorStruct.SongTitle);
			}
		}
	}
}

void UScoreAnalysisWidget::PopulateGameModeSelectComboBox()
{
	SongSelectorComboBox->ClearOptions();

	TArray<FGameModeActorStruct> Keys;
	PlayerScoreMap.GenerateKeyArray(Keys);

	// looping through all game modes in PlayerScoreMap
	for (FGameModeActorStruct GameModeActorStruct: Keys)
	{
		// first check if it is a base game mode
		for (EGameModeActorName BaseGameModeActorName : AllGameModeActorNames.AllGameModeActorNames)
		{
			if (GameModeActorStruct.GameModeActorName == BaseGameModeActorName)
			{
				if (GameModeSelectComboBox->FindOptionIndex(UKismetStringLibrary::RightChop(UEnum::GetValueAsString(GameModeActorStruct.GameModeActorName),20)) == -1)
				{
					GameModeSelectComboBox->AddOption(UKismetStringLibrary::RightChop(UEnum::GetValueAsString(GameModeActorStruct.GameModeActorName) ,20));
				}
			}
		}
		// if it isn't a base game mode, add the custom game mode name instead
		if (GameModeActorStruct.CustomGameModeName.IsEmpty() == false && GameModeSelectComboBox->FindOptionIndex(GameModeActorStruct.CustomGameModeName) == -1)
		{
			GameModeSelectComboBox->AddOption(GameModeActorStruct.CustomGameModeName);
		}
	}
}

void UScoreAnalysisWidget::PopulateAvgScoresBySong(FString SelectedSong, ESelectInfo::Type SelectionType)
{
	// clear all text
	AvgScoreText->SetText(FText::AsNumber(0.f));
	AvgTargetBar->SetPercent(0.f);
	AvgTargetsHitText->SetText(FText::AsNumber(0.f));
	AvgAccuracy->SetText(FText::AsPercent(0.f));
	AvgShotsFiredText->SetText(FText::AsNumber(0.f));
	AvgTargetsSpawnedText->SetText(FText::AsNumber(0.f));
	HighScoreText->SetText(FText::AsNumber(0.f));

	float SumTargetsHit = 0;
	float SumShotsFired = 0;
	float SumTargetsSpawned = 0;
	float SumScore = 0;
	float HighScore = 0;
	int ArraySize = 0;

	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// find matching Elem.Key for SelectedGameMode
		if (GameModeSelectComboBox->GetSelectedOption() == UKismetStringLibrary::RightChop(UEnum::GetValueAsString(Elem.Key.GameModeActorName),20) ||
			GameModeSelectComboBox->GetSelectedOption() == Elem.Key.CustomGameModeName)
		{
			// find matching Elem.Key for SelectedSong
			if (Elem.Key.SongTitle == SelectedSong)
			{
				// if Song Matches, this is the PlayerScoreArray for selected Game Mode and Song
				for (FPlayerScore CurrentPlayerScoreObject : Elem.Value.PlayerScoreArray)
				{
					SumTargetsHit += CurrentPlayerScoreObject.TargetsHit;
					SumShotsFired += CurrentPlayerScoreObject.ShotsFired;
					SumTargetsSpawned += CurrentPlayerScoreObject.TargetsSpawned;
					SumScore += CurrentPlayerScoreObject.Score;
					ArraySize++;

					// only update high score if greater than the last high score in PlayerScoreArray
					if (CurrentPlayerScoreObject.HighScore > HighScore)
					{
						HighScore = CurrentPlayerScoreObject.HighScore;
					}
				}
				// update values
				AvgScoreText->SetText(FText::AsNumber(SumScore / ArraySize));
				AvgTargetBar->SetPercent((SumTargetsHit / ArraySize) / (SumShotsFired / ArraySize));
				AvgTargetsHitText->SetText(FText::AsNumber(SumTargetsHit / ArraySize));
				AvgAccuracy->SetText(FText::AsPercent((SumTargetsHit / ArraySize) / (SumShotsFired / ArraySize)));
				AvgShotsFiredText->SetText(FText::AsNumber(SumShotsFired / ArraySize));
				AvgTargetsSpawnedText->SetText(FText::AsNumber(SumTargetsSpawned / ArraySize));
				HighScoreText->SetText(FText::AsNumber(HighScore));
			}
		}
	}
}

void UScoreAnalysisWidget::LoadPlayerScores()
{
	PlayerScoreMap = GI->LoadPlayerScores();
}

void UScoreAnalysisWidget::ClearScoreBoxes(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TargetBar->SetPercent(0.f);
	TargetsHitText->SetText(FText::AsNumber(0.f));
	Accuracy->SetText(FText::AsPercent(0.f));
	ShotsFiredText->SetText(FText::AsNumber(0.f));
	TargetsSpawnedText->SetText(FText::AsNumber(0.f));
	CurrentScoreText->SetText(FText::AsNumber(0.f));
	TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(0.f), 3)));

	AvgScoreText->SetText(FText::AsNumber(0.f));
	AvgTargetBar->SetPercent(0.f);
	AvgTargetsHitText->SetText(FText::AsNumber(0.f));
	AvgAccuracy->SetText(FText::AsPercent(0.f));
	AvgShotsFiredText->SetText(FText::AsNumber(0.f));
	AvgTargetsSpawnedText->SetText(FText::AsNumber(0.f));
	HighScoreText->SetText(FText::AsNumber(0.f));
}

void UScoreAnalysisWidget::PopulateAllMatchingSavedScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToLoad)
{

}