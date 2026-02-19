// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameModeScoreViewerWidget.h"
#include "DefaultGameModeScoreViewerWidget.generated.h"

UCLASS(BlueprintType)
class USERINTERFACE_API UDefaultGameModeScoreViewerWidget : public UGameModeScoreViewerWidget
{
	GENERATED_BODY()

public:
	virtual void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore) override;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_Difficulty(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UPROPERTY()
	UComboBoxWidget* DifficultyComboBoxWidget;
};
