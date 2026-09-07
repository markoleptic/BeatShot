// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "ScoreViewerWidget.generated.h"

class UGameModeAndDifficultyStringMap;
class UScoreTable;
class UTextBlock;
class UBSButton;
class UCustomGameModeScoreViewerWidget;
class UBarChartWidget;
class UDefaultGameModeScoreViewerWidget;
class UVerticalBox;
class UWidgetSwitcher;
class UMenuButton;
class UHeatMapWidget;

enum class EGameModePlayTimeUnits
{
	Minutes,
	Hours
};

struct FGameModePlayTime
{
	float PlayTime;
	EGameModeType GameModeType;
	FString CustomGameModeName;
	EBaseGameMode BaseGameMode;
};

struct FCalculateTimeStatisticsResult
{
	TSharedPtr<FPlayerScore> MostRecentPlayerScore;
	TSharedPtr<FPlayerScore> MostRecentDefaultPlayerScore;
	TSharedPtr<FPlayerScore> MostRecentCustomPlayerScore;
	float TotalSecondsInAnyGameMode = 0.f;
	EGameModePlayTimeUnits DefaultGameModePlayTimeUnits;
	EGameModePlayTimeUnits CustomGameModePlayTimeUnits;
};

UCLASS()
class USERINTERFACE_API UScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void LoadScores(USaveGamePlayerScore* InSaveGamePlayerScore, bool SwitchToMostRecent);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* Switcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_Overview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_History;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_DefaultModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_CustomModes;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_Overview;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UDefaultGameModeScoreViewerWidget* DefaultGameModeScoreViewerWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCustomGameModeScoreViewerWidget* CustomGameModeScoreViewerWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UScoreTable* ScoreTable;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_History;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_NoScores;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedDefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedCustomGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* PlayFrequency;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TotalTimeInAnyGameMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultModeLabel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomModeLabel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedDefaultModeLabel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedDefaultMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedCustomModeLabel;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedCustomMode;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* BSButton_DeleteSelectedScores;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UGameModeAndDifficultyStringMap> GameModeAndDifficultyStringMap;

	/** Maximum bars to show for Default Game Modes bar chart on the Overview page. */
	UPROPERTY(EditDefaultsOnly)
	int MaxDefaultGameModesBarChart = 8;

	/** Maximum bars to show for Custom Game Modes bar chart on the Overview page. */
	UPROPERTY(EditDefaultsOnly)
	int MaxCustomGameModesBarChart = 8;

private:
	void OnButtonClicked_MenuButton(const UBSButton* Button);

	void OnButtonClicked_DeleteSelectedScoresButton(const UBSButton* Button);

	/** Calculates and updates the data for most played default modes, most played custom modes, and play frequency. */
	FCalculateTimeStatisticsResult CalculateTimeStatistics(const TArray<TSharedPtr<FPlayerScore>>& PlayerScoresPtr,
	                                                       TMap<EBaseGameMode, FGameModePlayTime>&
	                                                       PlayTimeByBaseGameMode,
	                                                       TMap<FString, FGameModePlayTime>&
	                                                       PlayTimeByCustomGameModeName);

	/** Updates relative play frequency data using absolute play frequency data. */
	void UpdateRelativePlayFrequency();

	void UpdateUserFacingTimeStatisticsLabels(const TMap<EBaseGameMode, FGameModePlayTime>& PlayTimeByBaseGameMode,
	                                          const TMap<FString, FGameModePlayTime>& PlayTimeByCustomGameModeName,
	                                          float TotalTimeInAnyGameMode);

	void OnSelectionChanged_ScoreTable(bool HasSelection);

	FText HandlePlayFrequencyDisplayText(int32, int32);

	FText HandlePlayFrequencyValueText(int32, int32, float);

	FText HandleMostPlayedDefaultGameModesDisplayText(int32);

	FText HandleMostPlayedDefaultGameModesValueText(int32, float);

	FText HandleMostPlayedCustomGameModesDisplayText(int32);

	FText HandleMostPlayedCustomGameModesValueText(int32, float);

	FText HandleMostPlayedDefaultGameModesXAxisFormatter(int32, float);

	FText HandleMostPlayedDefaultGameModesYAxisFormatter(int32, float);

	FText HandleMostPlayedCustomGameModesXAxisFormatter(int32, float);

	FText HandleMostPlayedCustomGameModesYAxisFormatter(int32, float);

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	FDateTime StartDate;

	int32 StartDow;

	/** Initialized with zeros for the past year. */
	TArray<TArray<float>> InitialPlayFrequencyData;

	/** Absolute (total seconds per day) play frequency data ([WeekIndex][DayIndex]). Only for the past 365 days. */
	TArray<TArray<float>> PlayFrequencyData;

	/** Normalized play frequency data shared with the PlayFrequency widget. */
	TSharedPtr<TArray<TArray<float>>> NormalizedPlayFrequencyData;

	TSharedPtr<TArray<float>> MostPlayedDefaultGameModesData;

	TSharedPtr<TArray<float>> MostPlayedCustomGameModesData;

	TArray<FGameModePlayTime> DefaultGameModePlayTime;

	TArray<FGameModePlayTime> CustomGameModePlayTime;

	int32 DefaultGameModePrecision = 0;
	int32 CustomGameModePrecision = 0;

	EGameModePlayTimeUnits DefaultGameModePlayTimeUnits = EGameModePlayTimeUnits::Hours;
	EGameModePlayTimeUnits CustomGameModePlayTimeUnits = EGameModePlayTimeUnits::Hours;
};
