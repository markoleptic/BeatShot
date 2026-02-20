// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "CustomGameModeScoreViewerWidget.generated.h"


class UGameModeScoreViewerWidget;
class USaveGamePlayerScore;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UCustomGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	void SetActiveScores(const FString& CustomGameModeName, const FString& SongTitle);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Song(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* GameModeComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* SongComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UGameModeScoreViewerWidget* GameModeScoreViewerWidget;

	TMap<FString, TMap<FString, TArray<TSharedPtr<FPlayerScore>>>> PlayerScoreByGameModeAndSong;
};
