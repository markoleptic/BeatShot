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
			PlayerScoreByGameModeSongAndDifficulty[PlayerScore->DefiningConfig.CustomGameModeName][PlayerScore->
				SongTitle][PlayerScore->DefiningConfig.Difficulty].Add(PlayerScore);
			FDateTime DateTime;
			FDateTime::ParseIso8601(*PlayerScore->Time, DateTime);
			GameModeScoreViewerWidget->TimesByPlayerScore.Add(PlayerScore, DateTime);
		}
	}
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeSongAndDifficulty)
	{
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
		{
			for (const auto& [Difficulty, PlayerScoresForDifficulty] : PlayerScoresForSongs)
			{
				Algo::Sort(PlayerScoresForDifficulty,
					[&](const TSharedPtr<FPlayerScore>& Left, const TSharedPtr<FPlayerScore>& Right)
					{
						return GameModeScoreViewerWidget->TimesByPlayerScore[Left] < GameModeScoreViewerWidget->
							TimesByPlayerScore[Right];
					});
			}
		}
	}
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
	const FString SongTitle = SongComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	const FString Difficulty = DifficultyComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	/*GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeSongAndDifficulty[ActiveSelections[0]][SongTitle][
		Difficulty];*/
	GameModeScoreViewerWidget->UpdateActiveScores();
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString GameMode = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	const FString Difficulty = DifficultyComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	/*GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeSongAndDifficulty[GameMode][ActiveSelections[0]][
		Difficulty];*/
	GameModeScoreViewerWidget->UpdateActiveScores();
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString GameMode = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	const FString SongTitle = SongComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	/*GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeSongAndDifficulty[GameMode][SongTitle][
		ActiveSelections[0]];*/
	GameModeScoreViewerWidget->UpdateActiveScores();
}
