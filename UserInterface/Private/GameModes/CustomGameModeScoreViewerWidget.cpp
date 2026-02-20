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
	GameModeScoreViewerWidget->ActiveScores = PlayerScoreByGameModeAndSong[CustomGameModeName][SongTitle];
	GameModeScoreViewerWidget->UpdateActiveScores();
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
