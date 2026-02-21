// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "DefaultGameModeScoreViewerWidget.generated.h"

class UTextBlock;
class UVerticalBox;
class UGameModeScoreViewerWidget;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UDefaultGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Adds a reference to the save game and calls @link RepopulatePlayerScoreByGameModeSongAndDifficulty. */
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	/** Filters the active scores based on @param BaseGameMode @param SongTitle @param Difficulty . Always updates the
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

	void FilterActiveScores(EBaseGameMode CurrentBaseGameMode, const FString& CurrentSongTitle,
		EGameModeDifficulty CurrentDifficulty);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* MainBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_NoScores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* GameModeComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* DifficultyComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* SongComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModeScoreViewerWidget* GameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly)
	TMap<EBaseGameMode, FText> BaseGameModeText;
	UPROPERTY(EditDefaultsOnly)
	TMap<EGameModeDifficulty, FText> GameModeDifficultyText;

	UPROPERTY()
	USaveGamePlayerScore* SaveGamePlayerScore;

	TMap<EBaseGameMode, TMap<FString, TMap<EGameModeDifficulty, TArray<TSharedPtr<FPlayerScore>>>>>
	PlayerScoreByGameModeSongAndDifficulty;
};
