// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"

void UDefaultGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	PlayerScoreByGameModeSongAndDifficulty.Empty();

	for (const auto& PlayerScore : InSaveGamePlayerScore->GetPlayerScoresRefPtr())
	{
		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			PlayerScoreByGameModeSongAndDifficulty.FindOrAdd(PlayerScore->DefiningConfig.BaseGameMode).
			                                       FindOrAdd(PlayerScore->SongTitle).FindOrAdd(
				                                       PlayerScore->DefiningConfig.Difficulty).Add(PlayerScore);
			FDateTime DateTime;
			FDateTime::ParseIso8601(*PlayerScore->Time, DateTime);
			GameModeScoreViewerWidget->TimesByPlayerScore.Add(PlayerScore, DateTime);
		}
	}
	TSet<FString> GameModeOptions;
	TSet<FString> SongOptions;
	TSet<FString> DifficultyOptions;
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeSongAndDifficulty)
	{
		GameModeOptions.Add(BaseGameModeText[GameMode].ToString());
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
		{
			SongOptions.Add(Song);
			for (const auto& [Difficulty, PlayerScoresForDifficulty] : PlayerScoresForSongs)
			{
				DifficultyOptions.Add(GameModeDifficultyText[Difficulty].ToString());
				Algo::Sort(PlayerScoresForDifficulty,
					[&](const TSharedPtr<FPlayerScore>& Left, const TSharedPtr<FPlayerScore>& Right)
					{
						return GameModeScoreViewerWidget->TimesByPlayerScore[Left] < GameModeScoreViewerWidget->
							TimesByPlayerScore[Right];
					});
			}
		}
	}
	TArray<FString> GameModeOptionsArray = GameModeOptions.Array();
	GameModeComboBoxWidget->ComboBox->ClearOptions();
	GameModeComboBoxWidget->SortAndAddOptions(GameModeOptionsArray);
	TArray<FString> SongOptionsArray = SongOptions.Array();
	SongComboBoxWidget->ComboBox->ClearOptions();
	SongComboBoxWidget->SortAndAddOptions(SongOptionsArray);
	TArray<FString> DifficultyOptionsArray = DifficultyOptions.Array();
	DifficultyComboBoxWidget->ComboBox->ClearOptions();
	DifficultyComboBoxWidget->SortAndAddOptions(DifficultyOptionsArray);
}

void UDefaultGameModeScoreViewerWidget::SetActiveScores(const EBaseGameMode BaseGameMode, const FString& SongTitle,
	const EGameModeDifficulty Difficulty)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(BaseGameModeText[BaseGameMode].ToString());
	SongComboBoxWidget->ComboBox->SetSelectedOption(SongTitle);
	DifficultyComboBoxWidget->ComboBox->SetSelectedOption(GameModeDifficultyText[Difficulty].ToString());
	FilterActiveScores();
}

void UDefaultGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
	DifficultyComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_Difficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}

	FilterActiveScores();
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}

	FilterActiveScores();
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}

	FilterActiveScores();
}

EBaseGameMode UDefaultGameModeScoreViewerWidget::FindBaseGameMode(const FString& InGameModeName)
{
	for (const auto& [BaseGameMode, Text] : BaseGameModeText)
	{
		if (Text.ToString() == InGameModeName)
		{
			return BaseGameMode;
		}
	}
	return EBaseGameMode::None;
}

EGameModeDifficulty UDefaultGameModeScoreViewerWidget::FindGameModeDifficulty(const FString& InGameModeDifficulty)
{
	for (const auto& [Difficulty, Text] : GameModeDifficultyText)
	{
		if (Text.ToString() == InGameModeDifficulty)
		{
			return Difficulty;
		}
	}
	return EGameModeDifficulty::None;
}

void UDefaultGameModeScoreViewerWidget::FilterActiveScores()
{
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	FString CurrentSongTitle = SongComboBoxWidget->ComboBox->GetSelectedOption();
	EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	TSet<FString> SongOptions;
	TSet<FString> DifficultyOptions;

	bool HasSongTitle = false;
	bool HasDifficulty = false;

	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeSongAndDifficulty)
	{
		if (GameMode == CurrentGameMode)
		{
			for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
			{
				SongOptions.Add(Song);
				if (CurrentSongTitle == Song)
				{
					HasSongTitle = true;
					for (const auto& [Difficulty, PlayerScoresForDifficulty] : PlayerScoresForSongs)
					{
						if (CurrentDifficulty == Difficulty)
						{
							HasDifficulty = true;
						}
						DifficultyOptions.Add(GameModeDifficultyText[Difficulty].ToString());
					}
				}
			}
			break;
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

	TArray<FString> DifficultyOptionsArray = DifficultyOptions.Array();
	DifficultyComboBoxWidget->ComboBox->ClearOptions();
	DifficultyComboBoxWidget->SortAndAddOptions(DifficultyOptionsArray);
	DifficultyComboBoxWidget->ComboBox->SetSelectedIndex(FMath::Max(
		DifficultyComboBoxWidget->ComboBox->GetIndexOfOption(GameModeDifficultyText[CurrentDifficulty].ToString()), 0));
	if (!HasDifficulty)
	{
		if (DifficultyOptionsArray.IsEmpty())
		{
			CurrentDifficulty = EGameModeDifficulty::None;
		}
		else
		{
			CurrentDifficulty = FindGameModeDifficulty(DifficultyOptionsArray[0]);
		}
	}

	if (!CurrentSongTitle.IsEmpty() && CurrentDifficulty != EGameModeDifficulty::None)
	{
		GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeSongAndDifficulty[CurrentGameMode][
			CurrentSongTitle][CurrentDifficulty];
	}
	else
	{
		GameModeScoreViewerWidget->ActiveScores = {};
	}
	GameModeScoreViewerWidget->UpdateActiveScores();
}
