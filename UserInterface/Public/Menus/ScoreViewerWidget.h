// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ScoreViewerWidget.generated.h"


class UBSButton;
class UCustomGameModeScoreViewerWidget;
class UBarChartWidget;
class UDefaultGameModeScoreViewerWidget;
class UGameModeScoreViewerWidget;
class UVerticalBox;
class UWidgetSwitcher;
class UMenuButton;
struct FHeatMapAxisLabelOptions;
struct FHeatMapData;
class UHeatMapWidget;
class USaveGamePlayerScore;

UCLASS()
class USERINTERFACE_API UScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void LoadScores(USaveGamePlayerScore* SaveGamePlayerScore, bool SwitchToMostRecent);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* Switcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Overview;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_History;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_DefaultModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_CustomModes;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Overview;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UDefaultGameModeScoreViewerWidget* DefaultGameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModeScoreViewerWidget* CustomGameModeScoreViewerWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_History;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedDefaultGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedCustomGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* PlayFrequency;

	void OnButtonClicked_BSButton(const UBSButton* Button);

private:
	TSharedPtr<FHeatMapData> PlayFrequencyData;

	TSharedPtr<FHeatMapAxisLabelOptions> PlayFrequencyAxisData;
};
