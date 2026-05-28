// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "DefaultGameModeScoreViewerWidget.generated.h"

class UGameModeAndDifficultyStringMap;
class UBSVerticalBox;
class UDateRangeWidget;
class UTextBlock;
class UVerticalBox;
class UGameModeScoreViewerWidget;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UDefaultGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Adds a reference to the save game and calls RepopulatePlayerScoreByGameModeSongAndDifficulty. */
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	/** Filters the active scores based on BaseGameMode, SongTitle, and Difficulty. Always updates the
	 *  song and difficulty combo boxes and all data visualizations. */
	void SetActiveScores(EBaseGameMode BaseGameMode, const FString& SongTitle, EGameModeDifficulty Difficulty);

	/** Repopulates all default game mode player scores from the save game. Updates the time for each player score and
	 *  repopulates the game mode combo box. If there are no scores, the main box is hidden and the no scores message
	 *  is shown. */
	void RepopulatePlayerScoreByGameModeSongAndDifficulty();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Song(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	EBaseGameMode FindBaseGameMode(const FString& InGameModeName);

	EGameModeDifficulty FindGameModeDifficulty(const FString& InGameModeDifficulty);

	void OnDateChanged(const FDateTime& Start, const FDateTime& End);

	void FilterActiveScores(EBaseGameMode CurrentBaseGameMode,
	                        const FString& CurrentSongTitle,
	                        EGameModeDifficulty CurrentDifficulty,
	                        const FDateTime& StartDate = FDateTime::MaxValue(),
	                        const FDateTime& EndDate = FDateTime::MinValue());

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* MainBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* FilterVerticalBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_NoScores;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* GameModeComboBoxWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* DifficultyComboBoxWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* SongComboBoxWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDateRangeWidget* DateRangeWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameModeScoreViewerWidget* GameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGameModeAndDifficultyStringMap> GameModeAndDifficultyStringMap;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	TMap<EBaseGameMode, TMap<FString, TMap<EGameModeDifficulty, TArray<TSharedPtr<FPlayerScore>>>>>
	PlayerScoreByGameModeSongAndDifficulty;
};
