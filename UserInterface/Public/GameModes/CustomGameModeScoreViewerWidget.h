// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "CustomGameModeScoreViewerWidget.generated.h"


class UTextBlock;
class UVerticalBox;
class UGameModeScoreViewerWidget;
class USaveGamePlayerScore;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UCustomGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Adds a reference to the save game and calls RepopulatePlayerScoreByGameModeAndSong. */
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	/** Filters the active scores based on CustomGameModeName and SongTitle. Always updates the song
	 *  combo box and all data visualizations. */
	void SetActiveScores(const FString& CustomGameModeName, const FString& SongTitle);

	/** Repopulates all custom game mode player scores from the save game. Updates the time for each player score and 
	 *  repopulates the game mode combo box. If there are no scores, the main box is hidden and the no scores 
	 *  message is shown. */
	void RepopulatePlayerScoreByGameModeAndSong();

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Song(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	void FilterActiveScores(const FString& CurrentCustomGameModeName, const FString& CurrentSongTitle);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* MainBox;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_NoScores;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* GameModeComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* SongComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModeScoreViewerWidget* GameModeScoreViewerWidget;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	TMap<FString, TMap<FString, TArray<TSharedPtr<FPlayerScore>>>> PlayerScoreByGameModeAndSong;
};
