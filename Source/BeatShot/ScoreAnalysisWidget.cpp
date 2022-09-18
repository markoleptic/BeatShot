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
	GameModeSelectComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateSongSelectorComboBox);
	GameModeSelectComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::ClearScoreBoxes);
	SongSelectorComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateRecentScores);
	SongSelectorComboBox->OnSelectionChanged.AddDynamic(this, &UScoreAnalysisWidget::PopulateAvgScoresBySong);
}

void UScoreAnalysisWidget::PopulateRecentScores(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	TargetBar->SetPercent(0.f);
	TargetsHitText->SetText(FText::AsNumber(0.f));
	Accuracy->SetText(FText::AsPercent(0.f));
	ShotsFiredText->SetText(FText::AsNumber(0.f));
	TargetsSpawnedText->SetText(FText::AsNumber(0.f));
	CurrentScoreText->SetText(FText::AsNumber(0.f));
	TotalSongLength->SetText(UKismetTextLibrary::Conv_StringToText(UKismetStringLibrary::LeftChop(UKismetStringLibrary::TimeSecondsToString(0.f), 3)));

	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		// get array of player scores from current key value
		if (GameModeSelectComboBox->GetSelectedOption() == UKismetStringLibrary::RightChop(UEnum::GetValueAsString<EGameModeActorName>(Elem.Key.GameModeActorName), 20) ||
			GameModeSelectComboBox->GetSelectedOption() == Elem.Key.CustomGameModeName)
		{
			if (SelectedItem == Elem.Key.SongTitle)
			{
				FPlayerScore MostRecentScoreObject;
				FDateTime MostRecentTime = FDateTime::MinValue();

				//FPlayerScoreArrayWrapper ScoreArrayWrapper = PlayerScoreMap.FindRef(MatchingGameMode);

				// find the most recent time in the Score Array
				for (FPlayerScore CurrentPlayerScoreObject : Elem.Value.PlayerScoreArray)
				{
					UE_LOG(LogTemp, Display, TEXT("OuterTime: %s"), *CurrentPlayerScoreObject.Time.ToString());
					if (CurrentPlayerScoreObject.Time > MostRecentTime)
					{
						MostRecentScoreObject = CurrentPlayerScoreObject;
						MostRecentTime = CurrentPlayerScoreObject.Time;
						UE_LOG(LogTemp, Display, TEXT("InnerTime: %s"), *CurrentPlayerScoreObject.Time.ToString());
					}
				}

				const float TargetsHit = MostRecentScoreObject.TargetsHit;
				const float ShotsFired = MostRecentScoreObject.ShotsFired;
				const float TargetsSpawned = MostRecentScoreObject.TargetsSpawned;
				const float Score = MostRecentScoreObject.Score;

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

void UScoreAnalysisWidget::PopulateSongSelectorComboBox(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	SongSelectorComboBox->ClearOptions();

	TArray<FGameModeActorStruct> Keys;
	PlayerScoreMap.GenerateKeyArray(Keys);

	// looping through all game modes in PlayerScoreMap
	for (FGameModeActorStruct& GameModeActorStruct : Keys)
	{
		// find matching key for SelectedItem
		if (SelectedItem == UKismetStringLibrary::RightChop(UEnum::GetValueAsString(GameModeActorStruct.GameModeActorName),20) ||
			SelectedItem == GameModeActorStruct.CustomGameModeName)
		{
			if (SongSelectorComboBox->FindOptionIndex(GameModeActorStruct.SongTitle) == -1)
			{
				SongSelectorComboBox->AddOption(GameModeActorStruct.SongTitle);
			}
		}
	}
}

void UScoreAnalysisWidget::PopulateAllMatchingSavedScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToLoad)
{

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

void UScoreAnalysisWidget::PopulateAvgScoresBySong(FString SelectedItem, ESelectInfo::Type SelectionType)
{
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

	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : PlayerScoreMap)
	{
		UE_LOG(LogTemp, Display, TEXT("CustomGameModeNameInAvg: %s"), *Elem.Key.CustomGameModeName);
		UE_LOG(LogTemp, Display, TEXT("SelectedGameModeInAvg: %s"), *GameModeSelectComboBox->GetSelectedOption());
		// find matching key for SelectedItem
		if (GameModeSelectComboBox->GetSelectedOption() == UKismetStringLibrary::RightChop(UEnum::GetValueAsString(Elem.Key.GameModeActorName),20) ||
			GameModeSelectComboBox->GetSelectedOption() == Elem.Key.CustomGameModeName)
		{
			if (Elem.Key.SongTitle == SelectedItem)
			{
				int ArraySize = Elem.Value.PlayerScoreArray.Num();
				UE_LOG(LogTemp, Display, TEXT("ArraySize: %f"), Elem.Value.PlayerScoreArray.Num());
				for (FPlayerScore CurrentPlayerScoreObject : Elem.Value.PlayerScoreArray)
				{
					UE_LOG(LogTemp, Display, TEXT("Inside CurrentPlayerScoreObject"));
					SumTargetsHit += CurrentPlayerScoreObject.TargetsHit;
					SumShotsFired += CurrentPlayerScoreObject.ShotsFired;
					SumTargetsSpawned += CurrentPlayerScoreObject.TargetsSpawned;
					SumScore += CurrentPlayerScoreObject.Score;

					if (CurrentPlayerScoreObject.HighScore > HighScore)
					{
						HighScore = CurrentPlayerScoreObject.HighScore;
					}
				}
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
