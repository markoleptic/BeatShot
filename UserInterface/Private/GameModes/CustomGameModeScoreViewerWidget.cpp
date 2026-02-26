// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"

void UCustomGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	RepopulatePlayerScoreByGameModeAndSong();
}

void UCustomGameModeScoreViewerWidget::SetActiveScores(const FString& CustomGameModeName, const FString& SongTitle)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(CustomGameModeName);
	FilterActiveScores(CustomGameModeName, SongTitle);
}

void UCustomGameModeScoreViewerWidget::RepopulatePlayerScoreByGameModeAndSong()
{
	PlayerScoreByGameModeAndSong.Empty();

	TMap<TSharedPtr<FPlayerScore>, FDateTime> TimesByPlayerScore;
	for (const auto& PlayerScore : SaveGamePlayerScore->GetPlayerScoresPtr())
	{
		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Custom)
		{
			PlayerScoreByGameModeAndSong[PlayerScore->DefiningConfig.CustomGameModeName][PlayerScore->SongTitle].Add(
				PlayerScore);
			FDateTime DateTime;
			FDateTime::ParseIso8601(*PlayerScore->Time, DateTime);
			TimesByPlayerScore.Add(PlayerScore, DateTime);
		}
	}

	TSet<FString> GameModeOptions;
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeAndSong)
	{
		GameModeOptions.Add(GameMode);
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
		{
			Algo::Sort(PlayerScoresForSongs,
			           [&](const TSharedPtr<FPlayerScore>& Left, const TSharedPtr<FPlayerScore>& Right)
			           {
				           return TimesByPlayerScore[Left] < TimesByPlayerScore[Right];
			           });
		}
	}
	GameModeScoreViewerWidget->SetTimesByPlayerScore(MoveTemp(TimesByPlayerScore));

	GameModeComboBoxWidget->ComboBox->ClearOptions();
	TArray<FString> GameModeOptionsArray = GameModeOptions.Array();
	GameModeComboBoxWidget->SortAndAddOptions(GameModeOptionsArray);

	if (!PlayerScoreByGameModeAndSong.IsEmpty())
	{
		MainBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_NoScores->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		MainBox->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_NoScores->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

void UCustomGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
	                                                                      &ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
                                                                   const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	FilterActiveScores(ActiveSelections[0], SongComboBoxWidget->ComboBox->GetSelectedOption());
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
                                                               const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString CurrentCustomGameModeName = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	GameModeScoreViewerWidget->SetActiveScores(
		PlayerScoreByGameModeAndSong[CurrentCustomGameModeName][ActiveSelections[0]]);
}

void UCustomGameModeScoreViewerWidget::FilterActiveScores(const FString& CurrentCustomGameModeName,
                                                          const FString& CurrentSongTitle)
{
	TSet<FString> SongOptions;
	bool HasSongTitle = false;

	FString CurrentSongTitleOverride = CurrentSongTitle;

	if (PlayerScoreByGameModeAndSong.Contains(CurrentCustomGameModeName))
	{
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoreByGameModeAndSong[CurrentSongTitle])
		{
			if (Song == CurrentSongTitle)
			{
				HasSongTitle = true;
			}
			SongOptions.Add(Song);
		}
	}

	SongComboBoxWidget->ComboBox->ClearOptions();
	TArray<FString> SongOptionsArray = SongOptions.Array();
	SongComboBoxWidget->SortAndAddOptions(SongOptionsArray);
	if (!HasSongTitle)
	{
		if (SongOptionsArray.IsEmpty())
		{
			CurrentSongTitleOverride.Reset();
		}
		else
		{
			CurrentSongTitleOverride = SongOptionsArray[0];
		}
	}
	SongComboBoxWidget->ComboBox->SetSelectedIndex(
		FMath::Max(SongComboBoxWidget->ComboBox->GetIndexOfOption(CurrentSongTitleOverride), 0));

	if (!CurrentCustomGameModeName.IsEmpty() && !CurrentSongTitleOverride.IsEmpty())
	{
		GameModeScoreViewerWidget->SetActiveScores(
			PlayerScoreByGameModeAndSong[CurrentCustomGameModeName][CurrentSongTitleOverride]);
	}
	else
	{
		GameModeScoreViewerWidget->SetActiveScores({});
	}
}
