// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "MenuOptions/ComboBoxWidget.h"

void UDefaultGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	Super::SetSaveGamePlayerScore(InSaveGamePlayerScore);
}

void UDefaultGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	DifficultyComboBoxWidget->ComboBox->OnSelectionChanged.AddUniqueDynamic(this,
		&ThisClass::OnSelectionChanged_Difficulty);
}

void UDefaultGameModeScoreViewerWidget::OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections,
	ESelectInfo::Type SelectionType)
{
}
