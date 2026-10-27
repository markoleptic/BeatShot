// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "Mappings/GameModeAndDifficultyStringMap.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "MenuOptions/DateRangeWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/BSVerticalBox.h"
#include "Utilities/SongOptionLabels.h"

void UDefaultGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	RepopulatePlayerScoreByGameModeSongAndDifficulty();
}

void UDefaultGameModeScoreViewerWidget::SetActiveScores(const EBaseGameMode BaseGameMode,
                                                        const FSongMetadata& Song,
                                                        const EGameModeDifficulty Difficulty)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(
		GameModeAndDifficultyStringMap->FindTextFromEnum(BaseGameMode).ToString());
	FilterActiveScores(BaseGameMode, Song, Difficulty);
}

void UDefaultGameModeScoreViewerWidget::RepopulatePlayerScoreByGameModeSongAndDifficulty()
{
	PlayerScoreByGameModeSongAndDifficulty.Empty();

	TMap<TSharedPtr<FPlayerScore>, FDateTime> TimesByPlayerScore;
	for (const auto& PlayerScore : SaveGamePlayerScore->GetPlayerScoresPtr())
	{
		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			PlayerScoreByGameModeSongAndDifficulty.FindOrAdd(PlayerScore->DefiningConfig.BaseGameMode).
			                                       FindOrAdd(PlayerScore->SongMetadata).FindOrAdd(
				                                       PlayerScore->DefiningConfig.Difficulty).Add(PlayerScore);
			TimesByPlayerScore.Add(PlayerScore, PlayerScore->LocalDateTime);
		}
	}

	TSet<FString> GameModeOptions;
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeSongAndDifficulty)
	{
		GameModeOptions.Add(GameModeAndDifficultyStringMap->FindTextFromEnum(GameMode).ToString());
		for (const auto& [Song, PlayerScoresForSongs] : PlayerScoresBySong)
		{
			for (const auto& [Difficulty, PlayerScoresForDifficulty] : PlayerScoresForSongs)
			{
				Algo::Sort(PlayerScoresForDifficulty,
				           [&](const TSharedPtr<FPlayerScore>& Left, const TSharedPtr<FPlayerScore>& Right)
				           {
					           return TimesByPlayerScore[Left] < TimesByPlayerScore[Right];
				           });
			}
		}
	}
	GameModeScoreViewerWidget->SetTimesByPlayerScore(MoveTemp(TimesByPlayerScore));

	GameModeComboBoxWidget->ComboBox->ClearOptions();
	TArray<FString> GameModeOptionsArray = GameModeOptions.Array();
	GameModeComboBoxWidget->SortAndAddOptions(GameModeOptionsArray);

	if (!PlayerScoreByGameModeSongAndDifficulty.IsEmpty())
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

void UDefaultGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FilterVerticalBox->UpdateBrushColors();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.
	                        AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
	DifficultyComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(
		this, &ThisClass::OnSelectionChanged_Difficulty);
	DateRangeWidget->OnMinMaxMenuOptionChanged.BindUObject(this, &ThisClass::OnDateChanged);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
                                                                    const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FSongMetadata CurrentSong = GetSelectedSong();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSong, CurrentDifficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
                                                                const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FSongMetadata CurrentSong = GetSelectedSong();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSong, CurrentDifficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections,
                                                                      const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FSongMetadata CurrentSong = GetSelectedSong();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSong, CurrentDifficulty);
}

EBaseGameMode UDefaultGameModeScoreViewerWidget::FindBaseGameMode(const FString& InGameModeName)
{
	return GameModeAndDifficultyStringMap->FindEnumFromString<EBaseGameMode>(InGameModeName);
}

EGameModeDifficulty UDefaultGameModeScoreViewerWidget::FindGameModeDifficulty(const FString& InGameModeDifficulty)
{
	return GameModeAndDifficultyStringMap->FindEnumFromString<EGameModeDifficulty>(InGameModeDifficulty);
}

void UDefaultGameModeScoreViewerWidget::OnDateChanged(const FDateTime& Start, const FDateTime& End)
{
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FSongMetadata CurrentSong = GetSelectedSong();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSong, CurrentDifficulty, Start, End);
}

FSongMetadata UDefaultGameModeScoreViewerWidget::GetSelectedSong() const
{
	if (const FSongMetadata* SelectedSong =
	    FindSongByOptionLabel(SongOptionEntries, SongComboBoxWidget->ComboBox->GetSelectedOption()))
	{
		return *SelectedSong;
	}
	return FSongMetadata();
}

void UDefaultGameModeScoreViewerWidget::FilterActiveScores(const EBaseGameMode CurrentBaseGameMode,
                                                           const FSongMetadata& CurrentSong,
                                                           const EGameModeDifficulty CurrentDifficulty,
                                                           const FDateTime& StartDate,
                                                           const FDateTime& EndDate)
{
	FDateTime MinDate = StartDate;
	FDateTime MaxDate = EndDate;

	const bool SetDates = MinDate == FDateTime::MaxValue() && MaxDate == FDateTime::MinValue();

	SongOptionEntries.Reset();
	if (const TMap<FSongMetadata, TMap<EGameModeDifficulty, TArray<TSharedPtr<FPlayerScore>>>>* PlayerScoreBySong =
	    PlayerScoreByGameModeSongAndDifficulty.Find(CurrentBaseGameMode))
	{
		for (const auto& [Song, PlayerScoresByDifficulty] : *PlayerScoreBySong)
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

	TSet<FString> DifficultyOptions;
	bool HasDifficulty = false;
	if (PlayerScoreByGameModeSongAndDifficulty.Contains(CurrentBaseGameMode))
	{
		const auto& PlayerScoreByGameMode = PlayerScoreByGameModeSongAndDifficulty[CurrentBaseGameMode];
		if (PlayerScoreByGameMode.Contains(CurrentSongOverride))
		{
			const auto& PlayerScoreBySong = PlayerScoreByGameMode[CurrentSongOverride];
			HasDifficulty = PlayerScoreBySong.Contains(CurrentDifficulty);
			for (const auto& [Difficulty, PlayerScores] : PlayerScoreBySong)
			{
				DifficultyOptions.Add(GameModeAndDifficultyStringMap->FindTextFromEnum(Difficulty).ToString());
			}
		}
	}
	TArray<FString> DifficultyOptionsArray = DifficultyOptions.Array();
	const EGameModeDifficulty CurrentDifficultyOverride = HasDifficulty
	                                                      ? CurrentDifficulty
	                                                      : DifficultyOptions.IsEmpty()
	                                                      ? EGameModeDifficulty::None
	                                                      : FindGameModeDifficulty(DifficultyOptionsArray[0]);
	DifficultyComboBoxWidget->ComboBox->ClearOptions();
	DifficultyComboBoxWidget->SortAndAddOptions(DifficultyOptionsArray);
	const int32 DifficultyOptionIndex = CurrentDifficultyOverride == EGameModeDifficulty::None
	                                    ? 0
	                                    : DifficultyComboBoxWidget->ComboBox->GetIndexOfOption(
		                                    GameModeAndDifficultyStringMap->FindTextFromEnum(CurrentDifficultyOverride).
		                                                                    ToString());
	DifficultyComboBoxWidget->ComboBox->SetSelectedIndex(FMath::Max(DifficultyOptionIndex, 0));

	if (CurrentBaseGameMode != EBaseGameMode::None && CurrentSongOverride.IsValid() && CurrentDifficultyOverride
	    != EGameModeDifficulty::None)
	{
		if (SetDates)
		{
			for (const auto& PlayerScore : PlayerScoreByGameModeSongAndDifficulty[CurrentBaseGameMode][
				     CurrentSongOverride][CurrentDifficultyOverride])
			{
				const auto LocalDateDate = PlayerScore->LocalDateTime.GetDate();
				MinDate = FMath::Min(MinDate, LocalDateDate);
				MaxDate = FMath::Max(MaxDate, LocalDateDate);
			}
			DateRangeWidget->SetValues(MinDate, MaxDate, 1.f);
			DateRangeWidget->SetValue_Min(MinDate);
			DateRangeWidget->SetValue_Max(MaxDate);
		}

		const auto Filtered = PlayerScoreByGameModeSongAndDifficulty[CurrentBaseGameMode][CurrentSongOverride][
			CurrentDifficultyOverride].FilterByPredicate(
			[&MinDate, &MaxDate](const TSharedPtr<FPlayerScore>& PlayerScore)
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
