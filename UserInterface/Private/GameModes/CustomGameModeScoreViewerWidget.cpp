// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "MenuOptions/DateRangeWidget.h"
#include "Utilities/BSVerticalBox.h"
#include "Utilities/SongOptionLabels.h"

void UCustomGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	RepopulatePlayerScoreByGameModeAndSong();
}

void UCustomGameModeScoreViewerWidget::SetActiveScores(const FString& CustomGameModeName, const FSongMetadata& Song)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(CustomGameModeName);
	FilterActiveScores(CustomGameModeName, Song);
}

void UCustomGameModeScoreViewerWidget::RepopulatePlayerScoreByGameModeAndSong()
{
	PlayerScoreByGameModeAndSong.Empty();

	TMap<TSharedPtr<FPlayerScore>, FDateTime> TimesByPlayerScore;
	for (const auto& PlayerScore : SaveGamePlayerScore->GetPlayerScoresPtr())
	{
		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Custom)
		{
			PlayerScoreByGameModeAndSong.FindOrAdd(PlayerScore->DefiningConfig.CustomGameModeName).FindOrAdd(
				PlayerScore->SongMetadata).Add(PlayerScore);
			TimesByPlayerScore.Add(PlayerScore, PlayerScore->LocalDateTime);
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

	FilterVerticalBox->UpdateBrushColors();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.
	                        AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
	DateRangeWidget->OnMinMaxMenuOptionChanged.BindUObject(this, &ThisClass::OnDateChanged);
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
                                                                   const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	FilterActiveScores(ActiveSelections[0], GetSelectedSong());
}

void UCustomGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
                                                               const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const FString CurrentCustomGameModeName = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	FilterActiveScores(CurrentCustomGameModeName, GetSelectedSong());
}

void UCustomGameModeScoreViewerWidget::OnDateChanged(const FDateTime& Start, const FDateTime& End)
{
	const FString CurrentCustomGameModeName = GameModeComboBoxWidget->ComboBox->GetSelectedOptions()[0];
	const FSongMetadata CurrentSong = GetSelectedSong();
	FilterActiveScores(CurrentCustomGameModeName, CurrentSong, Start, End);
}

FSongMetadata UCustomGameModeScoreViewerWidget::GetSelectedSong() const
{
	if (const FSongMetadata* SelectedSong =
	    FindSongByOptionLabel(SongOptionEntries, SongComboBoxWidget->ComboBox->GetSelectedOption()))
	{
		return *SelectedSong;
	}
	return FSongMetadata();
}

void UCustomGameModeScoreViewerWidget::FilterActiveScores(const FString& CurrentCustomGameModeName,
                                                          const FSongMetadata& CurrentSong,
                                                          const FDateTime& StartDate,
                                                          const FDateTime& EndDate)
{
	FDateTime MinDate = StartDate;
	FDateTime MaxDate = EndDate;

	const bool SetDates = MinDate == FDateTime::MaxValue() && MaxDate == FDateTime::MinValue();

	SongOptionEntries.Reset();
	if (const TMap<FSongMetadata, TArray<TSharedPtr<FPlayerScore>>>* PlayerScoreBySong =
	    PlayerScoreByGameModeAndSong.Find(CurrentCustomGameModeName))
	{
		for (const auto& [Song, PlayerScores] : *PlayerScoreBySong)
		{
			SongOptionEntries.Add(Song);
		}
	}
	const bool HasSong = !SongOptionEntries.IsEmpty();
	FSongMetadata CurrentSongOverride;
	SongComboBoxWidget->ComboBox->ClearOptions();
	if (HasSong)
	{
		// Option labels are unique, so selecting by label is unambiguous after sorting.
		TArray<FString> SongOptionLabels = MakeSongOptionLabels(SongOptionEntries);
		const int32 RequestedSongIndex = SongOptionEntries.IndexOfByPredicate(
			[&CurrentSong](const FSongMetadata& Song)
			{
				return Song == CurrentSong;
			});
		// Keep the requested song if it is one of the options, otherwise the first alphabetically sorted option.
		SongOptionLabels.Sort();
		const FString SelectedOptionLabel = RequestedSongIndex == INDEX_NONE
	                                    ? SongOptionLabels[0]
	                                    : SongOptionLabels[RequestedSongIndex];
		if (const FSongMetadata* SelectedSong = FindSongByOptionLabel(SongOptionEntries, SelectedOptionLabel))
		{
			CurrentSongOverride = *SelectedSong;
		}
		SongComboBoxWidget->SortAndAddOptions(SongOptionLabels);
		SongComboBoxWidget->ComboBox->SetSelectedOption(SelectedOptionLabel);
	}


	if (!CurrentCustomGameModeName.IsEmpty() && CurrentSongOverride.IsValid())
	{
		if (SetDates)
		{
			for (const auto& PlayerScore : PlayerScoreByGameModeAndSong[CurrentCustomGameModeName][
				     CurrentSongOverride])
			{
				const auto LocalDateDate = PlayerScore->LocalDateTime.GetDate();
				MinDate = FMath::Min(MinDate, LocalDateDate);
				MaxDate = FMath::Max(MaxDate, LocalDateDate);
			}

			DateRangeWidget->SetValues(MinDate, MaxDate, 1.f);
			DateRangeWidget->SetValue_Min(MinDate);
			DateRangeWidget->SetValue_Max(MaxDate);
		}

		const auto Filtered = PlayerScoreByGameModeAndSong[CurrentCustomGameModeName][CurrentSongOverride].
			FilterByPredicate([&MinDate, &MaxDate](const TSharedPtr<FPlayerScore>& PlayerScore)
			{
				const auto LocalDateDate = PlayerScore->LocalDateTime.GetDate();
				return LocalDateDate >= MinDate && LocalDateDate <= MaxDate;
			});
		GameModeScoreViewerWidget->SetActiveScores(Filtered);
	}
	else
	{
		GameModeScoreViewerWidget->SetActiveScores({});
	}
}
