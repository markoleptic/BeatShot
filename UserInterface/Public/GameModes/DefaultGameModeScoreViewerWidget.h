// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "DefaultGameModeScoreViewerWidget.generated.h"

class UGameModeScoreViewerWidget;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UDefaultGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	void SetActiveScores(EBaseGameMode BaseGameMode, const FString& SongTitle, EGameModeDifficulty Difficulty);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Song(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

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

	EBaseGameMode FindBaseGameMode(const FString& InGameModeName);

	EGameModeDifficulty FindGameModeDifficulty(const FString& InGameModeDifficulty);

	void FilterActiveScores();

	TMap<EBaseGameMode, TMap<FString, TMap<EGameModeDifficulty, TArray<TSharedPtr<FPlayerScore>>>>>
	PlayerScoreByGameModeSongAndDifficulty;
};
