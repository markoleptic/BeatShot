// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "CustomGameModeScoreViewerWidget.generated.h"

class UBSVerticalBox;
class UDateRangeWidget;
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

	void OnDateChanged(const FDateTime& Start, const FDateTime& End);

	void FilterActiveScores(const FString& CurrentCustomGameModeName,
	                        const FString& CurrentSongTitle,
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
	UComboBoxWidget* SongComboBoxWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDateRangeWidget* DateRangeWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UGameModeScoreViewerWidget* GameModeScoreViewerWidget;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	TMap<FString, TMap<FString, TArray<TSharedPtr<FPlayerScore>>>> PlayerScoreByGameModeAndSong;
};
