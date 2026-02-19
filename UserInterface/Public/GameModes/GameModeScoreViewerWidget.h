// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameModeScoreViewerWidget.generated.h"

struct FCommonScoreInfo;
struct FBS_DefiningConfig;
struct FHeatMapAxisLabelOptions;
struct FHeatMapData;
struct FLineChartSeries;
struct FAxisLabelOptions;
enum class EAxisType;
class USaveGamePlayerScore;
struct FPlayerScore;
class UTextBlock;
class UHeatMapWidget;
class ULineChartWidget;
class UComboBoxWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSelectionChanged_GameMode(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSelectionChanged_Song(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);

	void UpdateActiveScores();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* GameModeComboBoxWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UComboBoxWidget* SongComboBoxWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* ScoreVsTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* StreakVsTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AverageTargetsDestroyed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AverageReactionTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AccuracyVsTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* LocationAccuracy;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestScore;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestAccuracy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestTargetsDestroyed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestReactionTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestStreak;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_Title;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageScore;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageAccuracy;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageTargetsDestroyed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageReactionTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageStreak;

	UPROPERTY()
	TSoftObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;

	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfoMap;

	TMap<FString, TMap<FString, TArray<TSharedPtr<FPlayerScore>>>> PlayerScoreByGameModeAndSong;

	/** Times across all player scores. */
	TMap<TSharedPtr<FPlayerScore>, FDateTime> TimesByPlayerScore;

	TArray<TSharedPtr<FPlayerScore>> ActiveScores;

	TSharedPtr<TArray<FLineChartSeries>> ScoreVsTimeData;
	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> ScoreVsTimeAxisData;

	TSharedPtr<TArray<FLineChartSeries>> StreakVsTimeData;
	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> StreakVsTimeAxisData;

	TSharedPtr<TArray<FLineChartSeries>> AverageTargetsDestroyedData;
	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> AverageTargetsDestroyedAxisData;

	TSharedPtr<TArray<FLineChartSeries>> AverageReactionTimeData;
	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> AverageReactionTimeAxisData;

	TSharedPtr<TArray<FLineChartSeries>> AccuracyVsTimeData;
	TSharedPtr<TMap<EAxisType, FAxisLabelOptions>> AccuracyVsTimeAxisData;

	TSharedPtr<FHeatMapData> LocationAccuracyData;
	TSharedPtr<FHeatMapAxisLabelOptions> LocationAccuracyAxisData;
};
