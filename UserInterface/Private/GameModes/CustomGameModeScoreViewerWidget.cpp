// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"

void UCustomGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	PlayerScoreByGameModeAndSong.Empty();

	for (const auto& PlayerScore : InSaveGamePlayerScore->GetPlayerScoresRefPtr())
	{
		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Custom)
		{
			PlayerScoreByGameModeAndSong[PlayerScore->DefiningConfig.CustomGameModeName][PlayerScore->SongTitle].Add(
				PlayerScore);
			FDateTime DateTime;
			FDateTime::ParseIso8601(*PlayerScore->Time, DateTime);
			GameModeScoreViewerWidget->TimesByPlayerScore.Add(PlayerScore, DateTime);
		}
	}
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeAndSong)
	{
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
		{
			Algo::Sort(PlayerScoresForSongs,
				[&](const TSharedPtr<FPlayerScore>& Left, const TSharedPtr<FPlayerScore>& Right)
				{
					return GameModeScoreViewerWidget->TimesByPlayerScore[Left] < GameModeScoreViewerWidget->
						TimesByPlayerScore[Right];
				});
		}
	}
}

void UCustomGameModeScoreViewerWidget::SetActiveScores(const FString& CustomGameModeName, const FString& SongTitle)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(CustomGameModeName);
	SongComboBoxWidget->ComboBox->SetSelectedOption(SongTitle);
	FilterActiveScores();
}

void UCustomGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString SongTitle = SongComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeAndSong[ActiveSelections[0]][SongTitle];
	GameModeScoreViewerWidget->UpdateActiveScores();
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString GameMode = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeAndSong[GameMode][ActiveSelections[0]];
	GameModeScoreViewerWidget->UpdateActiveScores();
}

void UCustomGameModeScoreViewerWidget::FilterActiveScores()
{
	const FString CurrentGameMode = GameModeComboBoxWidget->ComboBox->GetSelectedOption();
	FString CurrentSongTitle = SongComboBoxWidget->ComboBox->GetSelectedOption();
	TSet<FString> SongOptions;
	bool HasSongTitle = false;

	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeAndSong)
	{
		if (CurrentGameMode == GameMode)
		{
			for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
			{
				if (Song == CurrentSongTitle)
				{
					SongOptions.Add(Song);
					HasSongTitle = true;
				}
			}
		}
	}

	TArray<FString> SongOptionsArray = SongOptions.Array();
	SongComboBoxWidget->ComboBox->ClearOptions();
	SongComboBoxWidget->SortAndAddOptions(SongOptionsArray);
	if (!HasSongTitle)
	{
		if (SongOptionsArray.IsEmpty())
		{
			CurrentSongTitle.Reset();
		}
		else
		{
			CurrentSongTitle = SongOptionsArray[0];
		}
	}
	SongComboBoxWidget->ComboBox->SetSelectedIndex(
		FMath::Max(SongComboBoxWidget->ComboBox->GetIndexOfOption(CurrentSongTitle), 0));

	if (!CurrentSongTitle.IsEmpty())
	{
		GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeAndSong[CurrentGameMode][CurrentSongTitle];
	}
	else
	{
		GameModeScoreViewerWidget->ActiveScores = {};
	}
	GameModeScoreViewerWidget->UpdateActiveScores();
}
