// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "ScoreViewerWidget.generated.h"

class UScoreTable;
class UTextBlock;
struct FAxisLabelOptions;
enum class EAxisType : uint8;
struct FBarChartData;
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
	UScoreTable* ScoreTable;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_History;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_NoScores;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedDefaultGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBarChartWidget* MostPlayedCustomGameModes;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* PlayFrequency;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TotalTimeInAnyGameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultModeLabel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedDefaultMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomModeLabel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_MostPlayedCustomMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedDefaultModeLabel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedDefaultMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedCustomModeLabel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_TimePlayedForMostPlayedCustomMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UBSButton* BSButton_DeleteSelectedScores;

	UPROPERTY(EditDefaultsOnly)
	TMap<EBaseGameMode, FText> BaseGameModeText;
	UPROPERTY(EditDefaultsOnly)
	TMap<EGameModeDifficulty, FText> DifficultyText;

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

	FText FormatTime(float Seconds);

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	FDateTime StartDate;

	int32 StartDow;

	const FTextFormat PlayFrequencyDisplayFormat = FTextFormat::FromString("{0}\n{1}");

	const FTextFormat PlayFrequencyValueFormat = FTextFormat::FromString("Total for week: {0}");

	const FTextFormat SpaceSeparatedFormat = FTextFormat::FromString("{0} {1}");

	const FTextFormat TimeForFormat = FTextFormat::FromString("Time for {0}:");

	TArray<TArray<float>> PlayFrequencyData;

	TArray<TArray<float>> InitialPlayFrequencyData;

	TSharedPtr<FHeatMapData> RelativePlayFrequencyData;

	TSharedPtr<FHeatMapAxisLabelOptions> PlayFrequencyAxisData;

	TSharedPtr<FBarChartData> MostPlayedDefaultGameModesData;

	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> MostPlayedDefaultGameModesAxisData;

	TSharedPtr<FBarChartData> MostPlayedCustomGameModesData;

	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> MostPlayedCustomGameModesAxisData;

	TArray<FGameModePlayTime> DefaultGameModePlayTime;

	TArray<FGameModePlayTime> CustomGameModePlayTime;
};
