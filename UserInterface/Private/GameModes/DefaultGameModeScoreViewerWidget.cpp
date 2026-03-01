// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"

void UDefaultGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	GameModeScoreViewerWidget->SetSaveGamePlayerScore(InSaveGamePlayerScore);
	RepopulatePlayerScoreByGameModeSongAndDifficulty();
}

void UDefaultGameModeScoreViewerWidget::SetActiveScores(const EBaseGameMode BaseGameMode,
                                                        const FString& SongTitle,
                                                        const EGameModeDifficulty Difficulty)
{
	GameModeComboBoxWidget->ComboBox->SetSelectedOption(BaseGameModeText[BaseGameMode].ToString());
	FilterActiveScores(BaseGameMode, SongTitle, Difficulty);
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
			                                       FindOrAdd(PlayerScore->SongTitle).FindOrAdd(
				                                       PlayerScore->DefiningConfig.Difficulty).Add(PlayerScore);
			TimesByPlayerScore.Add(PlayerScore, PlayerScore->LocalDateTime);
		}
	}

	TSet<FString> GameModeOptions;
	for (const auto& [GameMode, PlayerScoresBySong] : PlayerScoreByGameModeSongAndDifficulty)
	{
		GameModeOptions.Add(BaseGameModeText[GameMode].ToString());
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

void UDefaultGameModeScoreViewerWidget::SetBaseGameModeText(const TMap<EBaseGameMode, FText>& InBaseGameModeText)
{
	BaseGameModeText = InBaseGameModeText;
}

void UDefaultGameModeScoreViewerWidget::SetDifficultyText(
	const TMap<EGameModeDifficulty, FText>& InGameModeDifficultyText)
{
	DifficultyText = InGameModeDifficultyText;
}

void UDefaultGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	GameModeComboBoxWidget->ComboBox->OnSelectionChanged.
	                        AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_GameMode);
	SongComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this, &ThisClass::OnSelectionChanged_Song);
	DifficultyComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(
		this, &ThisClass::OnSelectionChanged_Difficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections,
                                                                    const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FString CurrentSongTitle = SongComboBoxWidget->ComboBox->GetSelectedOption();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSongTitle, CurrentDifficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Song(const TArray<FString>& ActiveSelections,
                                                                const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FString CurrentSongTitle = SongComboBoxWidget->ComboBox->GetSelectedOption();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSongTitle, CurrentDifficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections,
                                                                      const ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct || ActiveSelections.IsEmpty())
	{
		return;
	}
	const EBaseGameMode CurrentGameMode = FindBaseGameMode(GameModeComboBoxWidget->ComboBox->GetSelectedOption());
	const FString CurrentSongTitle = SongComboBoxWidget->ComboBox->GetSelectedOption();
	const EGameModeDifficulty CurrentDifficulty = FindGameModeDifficulty(
		DifficultyComboBoxWidget->ComboBox->GetSelectedOption());
	FilterActiveScores(CurrentGameMode, CurrentSongTitle, CurrentDifficulty);
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
	for (const auto& [Difficulty, Text] : DifficultyText)
	{
		if (Text.ToString() == InGameModeDifficulty)
		{
			return Difficulty;
		}
	}
	return EGameModeDifficulty::None;
}

void UDefaultGameModeScoreViewerWidget::FilterActiveScores(const EBaseGameMode CurrentBaseGameMode,
                                                           const FString& CurrentSongTitle,
                                                           const EGameModeDifficulty CurrentDifficulty)
{
	TSet<FString> SongOptions;
	if (PlayerScoreByGameModeSongAndDifficulty.Contains(CurrentBaseGameMode))
	{
		for (const auto& [Song, PlayerScoresForDifficulty] : PlayerScoreByGameModeSongAndDifficulty[
			     CurrentBaseGameMode])
		{
			SongOptions.Add(Song);
		}
	}
	TArray<FString> SongOptionsArray = SongOptions.Array();
	const FString CurrentSongTitleOverride = SongOptions.Contains(CurrentSongTitle)
	                                         ? CurrentSongTitle
	                                         : SongOptions.IsEmpty()
	                                         ? FString("")
	                                         : SongOptionsArray[0];
	SongComboBoxWidget->ComboBox->ClearOptions();
	SongComboBoxWidget->SortAndAddOptions(SongOptionsArray);
	const int32 SongOptionIndex = SongComboBoxWidget->ComboBox->GetIndexOfOption(CurrentSongTitleOverride);
	SongComboBoxWidget->ComboBox->SetSelectedIndex(FMath::Max(SongOptionIndex, 0));

	TSet<FString> DifficultyOptions;
	bool HasDifficulty = false;
	if (PlayerScoreByGameModeSongAndDifficulty.Contains(CurrentBaseGameMode))
	{
		const auto& PlayerScoreByGameMode = PlayerScoreByGameModeSongAndDifficulty[CurrentBaseGameMode];
		if (PlayerScoreByGameMode.Contains(CurrentSongTitleOverride))
		{
			const auto& PlayerScoreBySong = PlayerScoreByGameMode[CurrentSongTitleOverride];
			HasDifficulty = PlayerScoreBySong.Contains(CurrentDifficulty);
			for (const auto& [Difficulty, PlayerScores] : PlayerScoreBySong)
			{
				DifficultyOptions.Add(DifficultyText[Difficulty].ToString());
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
		                                    DifficultyText[CurrentDifficultyOverride].ToString());
	DifficultyComboBoxWidget->ComboBox->SetSelectedIndex(FMath::Max(DifficultyOptionIndex, 0));

	if (CurrentBaseGameMode != EBaseGameMode::None && !CurrentSongTitleOverride.IsEmpty() && CurrentDifficultyOverride
	    != EGameModeDifficulty::None)
	{
		GameModeScoreViewerWidget->SetActiveScores(
			PlayerScoreByGameModeSongAndDifficulty[CurrentBaseGameMode][CurrentSongTitleOverride][
				CurrentDifficultyOverride]);
	}
	else
	{
		GameModeScoreViewerWidget->SetActiveScores({});
	}
}
