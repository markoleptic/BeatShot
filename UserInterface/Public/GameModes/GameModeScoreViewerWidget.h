// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BSGameModeConfig/DefiningConfig.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Slate/LineChart.h"
#include "GameModeScoreViewerWidget.generated.h"

class UTextBlock;
class UHeatMapWidget;
class ULineChartWidget;

UCLASS(BlueprintType)
class USERINTERFACE_API UGameModeScoreViewerWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	/** Updates the */
	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	void UpdateDataVisualization();

	void SetTimesByPlayerScore(TMap<TSharedPtr<FPlayerScore>, FDateTime>&& InTimesByPlayerScore);

	void SetActiveScores(const TArray<TSharedPtr<FPlayerScore>>& InActiveScores);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* ScoreVsTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* StreakVsTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AverageTargetsDestroyed;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AverageReactionTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	ULineChartWidget* AccuracyVsTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHeatMapWidget* LocationAccuracy;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestScore;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestAccuracy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestTargetsDestroyed;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestReactionTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_BestStreak;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageScore;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageAccuracy;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageTargetsDestroyed;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageReactionTime;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TextBlock_AverageStreak;

private:
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfoMap;

	/** Times across all player scores. */
	TMap<TSharedPtr<FPlayerScore>, FDateTime> TimesByPlayerScore;

	TArray<TSharedPtr<FPlayerScore>> ActiveScores;

	TSharedPtr<TArray<FLineChartSeries>> ScoreVsTimeData;

	TSharedPtr<TArray<FLineChartSeries>> StreakVsTimeData;

	TSharedPtr<TArray<FLineChartSeries>> AverageTargetsDestroyedData;

	TSharedPtr<TArray<FLineChartSeries>> AverageReactionTimeData;

	TSharedPtr<TArray<FLineChartSeries>> AccuracyVsTimeData;

	TSharedPtr<TArray<TArray<float>>> LocationAccuracyData;

	FText HandleGenericDateValueText(int32, int32);

	FText HandleLocationAccuracyDisplayText(int32, int32);

	FText HandleShortDateXAxisFormatter(int32, float);
};
