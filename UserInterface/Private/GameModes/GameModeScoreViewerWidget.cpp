// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/GameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/BSWidgetInterface.h"
#include "Widgets/HeatMapWidget.h"
#include "Widgets/LineChartWidget.h"


void UGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	CommonScoreInfoMap = InSaveGamePlayerScore->GetCommonScoreInfo();
}

void UGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	constexpr int MaxTickCount = 16;

	const auto ShortDateXAxisFormatter = TDelegate<FText(int32, float)>::CreateUObject(
		this, &ThisClass::HandleShortDateXAxisFormatter);
	const auto ZeroDecimalFormatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
	{
		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MaximumFractionalDigits = 0;
		NumberFormattingOptions.MinimumFractionalDigits = 0;
		return FText::AsNumber(Value, &NumberFormattingOptions);
	});

	ScoreVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	{
		auto& XAxisOptions = ScoreVsTime->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = ShortDateXAxisFormatter;
		XAxisOptions.AxisGridOptions.MaxGridLineCount = MaxTickCount;

		auto& YAxisOptions = ScoreVsTime->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisGridOptions.MinGridStep = 1;
		YAxisOptions.AxisGridOptions.MinGridLineCount = 3;
		YAxisOptions.AxisLabelGenerator = ZeroDecimalFormatter;
	}

	StreakVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	{
		auto& XAxisOptions = StreakVsTime->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = ShortDateXAxisFormatter;
		XAxisOptions.AxisGridOptions.MaxGridLineCount = MaxTickCount;

		auto& YAxisOptions = StreakVsTime->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisGridOptions.MinGridStep = 1;
		YAxisOptions.AxisGridOptions.MinGridLineCount = 3;
		YAxisOptions.AxisLabelGenerator = ZeroDecimalFormatter;
	}

	AverageTargetsDestroyedData = MakeShared<TArray<FLineChartSeries>>();
	{
		auto& XAxisOptions = AverageTargetsDestroyed->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = ShortDateXAxisFormatter;
		XAxisOptions.AxisGridOptions.MaxGridLineCount = MaxTickCount;

		auto& YAxisOptions = AverageTargetsDestroyed->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisLabelGenerator = ZeroDecimalFormatter;
		YAxisOptions.AxisGridOptions.MinRangeOverride = 0.f;
		YAxisOptions.AxisGridOptions.MaxRangeOverride = 100.f;
		YAxisOptions.AxisGridOptions.GridStepOverride = 10.0f;
	}

	AverageReactionTimeData = MakeShared<TArray<FLineChartSeries>>();
	{
		auto& XAxisOptions = AverageReactionTime->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = ShortDateXAxisFormatter;
		XAxisOptions.AxisGridOptions.MaxGridLineCount = MaxTickCount;

		auto& YAxisOptions = AverageReactionTime->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisGridOptions.MinGridStep = 1.f;
		YAxisOptions.AxisGridOptions.MaxGridLineCount = 3;
		YAxisOptions.AxisLabelGenerator = ZeroDecimalFormatter;
	}

	AccuracyVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	{
		auto& XAxisOptions = AccuracyVsTime->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = ShortDateXAxisFormatter;
		XAxisOptions.AxisGridOptions.MaxGridLineCount = MaxTickCount;

		auto& YAxisOptions = AccuracyVsTime->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisLabelGenerator = ZeroDecimalFormatter;
		YAxisOptions.AxisGridOptions.MinRangeOverride = 0.f;
		YAxisOptions.AxisGridOptions.MaxRangeOverride = 100.f;
		YAxisOptions.AxisGridOptions.GridStepOverride = 10.0f;
	}

	LocationAccuracyData = MakeShared<TArray<TArray<float>>>();
	LocationAccuracy->GetOptions()->DrawSectionIfValueLessThanZero = true;
	LocationAccuracy->GetOptions()->AxisOptions[EAxisType::X].DrawIndices = TSet{0, 1, 2, 3, 4};
	LocationAccuracy->GetOptions()->AxisOptions[EAxisType::Y].DrawIndices = TSet{0, 1, 2, 3, 4};
	const auto LocationAccuracyValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, float)
		{
			return FText();
		});

	const auto ScoreVsTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(0);
			return FText::AsNumber(Value * 1000.f, &NumberFormattingOptions);
		});

	const auto StreakVsTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(0);
			return FText::AsNumber(Value, &NumberFormattingOptions);
		});

	const auto AverageReactionTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 0;
			NumberFormattingOptions.MinimumFractionalDigits = 0;
			static const FTextFormat MillisecondFormat = FTextFormat(
				IBSWidgetInterface::GetWidgetTextFromKey("Scores_MillisecondFormat"));
			return FText::Format(MillisecondFormat, FText::AsNumber(Value * 1000.f, &NumberFormattingOptions));
		});

	const auto GenericPercentValueTextDelegate = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			static const FTextFormat PercentFormat = FTextFormat::FromString("{0}%");
			return FText::Format(PercentFormat, FText::AsNumber(Value, &NumberFormattingOptions));
		});


	const auto GenericDateValueTextDelegate = TDelegate<FText(int32, int32)>::CreateUObject(
		this, &ThisClass::HandleGenericDateValueText);

	ScoreVsTime->GetOptions()->TooltipTitleTextGenerator = GenericDateValueTextDelegate;
	ScoreVsTime->GetOptions()->TooltipTextGenerator = ScoreVsTimeValueTextGetter;
	ScoreVsTime->SetData(ScoreVsTimeData);

	StreakVsTime->GetOptions()->TooltipTitleTextGenerator = GenericDateValueTextDelegate;
	StreakVsTime->GetOptions()->TooltipTextGenerator = StreakVsTimeValueTextGetter;
	StreakVsTime->SetData(StreakVsTimeData);

	AverageReactionTime->GetOptions()->TooltipTitleTextGenerator = GenericDateValueTextDelegate;
	AverageReactionTime->GetOptions()->TooltipTextGenerator = AverageReactionTimeValueTextGetter;
	AverageReactionTime->SetData(AverageReactionTimeData);

	AverageTargetsDestroyed->GetOptions()->TooltipTitleTextGenerator = GenericDateValueTextDelegate;
	AverageTargetsDestroyed->GetOptions()->TooltipTextGenerator = GenericPercentValueTextDelegate;
	AverageTargetsDestroyed->SetData(AverageTargetsDestroyedData);

	AccuracyVsTime->GetOptions()->TooltipTitleTextGenerator = GenericDateValueTextDelegate;
	AccuracyVsTime->GetOptions()->TooltipTextGenerator = GenericPercentValueTextDelegate;
	AccuracyVsTime->SetData(AccuracyVsTimeData);

	LocationAccuracy->GetOptions()->TooltipTitleTextGenerator = TDelegate<FText(int32, int32)>::CreateUObject(
		this, &ThisClass::HandleLocationAccuracyDisplayText);
	LocationAccuracy->GetOptions()->TooltipTextGenerator = LocationAccuracyValueTextGetter;
	LocationAccuracy->SetData(LocationAccuracyData);
}

void UGameModeScoreViewerWidget::UpdateDataVisualization()
{
	if (ActiveScores.IsEmpty())
	{
		TextBlock_BestScore->SetText(FText());
		TextBlock_BestReactionTime->SetText(FText());
		TextBlock_BestStreak->SetText(FText());
		TextBlock_BestAccuracy->SetText(FText());
		TextBlock_BestTargetsDestroyed->SetText(FText());

		TextBlock_AverageScore->SetText(FText());
		TextBlock_AverageReactionTime->SetText(FText());
		TextBlock_AverageStreak->SetText(FText());
		TextBlock_AverageAccuracy->SetText(FText());
		TextBlock_AverageTargetsDestroyed->SetText(FText());

		ScoreVsTimeData->Empty();
		ScoreVsTime->Redraw();

		StreakVsTimeData->Empty();
		StreakVsTime->Redraw();

		AverageReactionTimeData->Empty();
		AverageReactionTime->Redraw();

		AverageTargetsDestroyedData->Empty();
		AverageTargetsDestroyed->Redraw();

		AccuracyVsTimeData->Empty();
		AccuracyVsTime->Redraw();

		LocationAccuracyData->Empty();
		LocationAccuracy->Redraw();
	}
	else
	{
		float BestScore = 0.0f;
		float BestAccuracy = 0.0f;
		int BestStreak = 0;
		float BestReactionTime = TNumericLimits<float>::Max();
		float BestCompletion = 0.f;

		float TotalScore = 0.0f;
		float TotalAccuracy = 0.0f;
		int TotalStreak = 0;
		float TotalReactionTime = 0.0f;
		float TotalCompletion = 0.0f;
		for (const auto& PlayerScore : ActiveScores)
		{
			BestScore = FMath::Max(BestScore, PlayerScore->Score);
			TotalScore += PlayerScore->Score;
			BestAccuracy = FMath::Max(BestAccuracy, PlayerScore->Accuracy);
			TotalAccuracy += PlayerScore->Accuracy;
			BestStreak = FMath::Max(BestStreak, PlayerScore->Streak);
			TotalStreak += PlayerScore->Streak;
			BestReactionTime = FMath::Min(BestReactionTime, PlayerScore->AvgTimeOffset);
			TotalReactionTime += PlayerScore->AvgTimeOffset;
			BestCompletion = FMath::Max(BestCompletion, PlayerScore->Completion);
			TotalCompletion += PlayerScore->Completion;
		}

		const float ScoreCount = static_cast<float>(ActiveScores.Num());
		const float AverageScore = TotalScore / ScoreCount;
		const float AverageAccuracy = TotalAccuracy / ScoreCount * 100.f;
		const int AverageStreak = FMath::FloorToInt(TotalStreak / ScoreCount);
		const float AvgReactionTime = TotalReactionTime * 100.f / ScoreCount;
		const float AvgTargetsDestroyed = TotalCompletion * 100.f / ScoreCount;

		FNumberFormattingOptions NumberFormattingOptions;
		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MaximumFractionalDigits = 0;
		TextBlock_BestScore->SetText(FText::AsNumber(BestScore, &NumberFormattingOptions));
		TextBlock_AverageScore->SetText(FText::AsNumber(AverageScore, &NumberFormattingOptions));

		static const FTextFormat MillisecondFormat = FTextFormat(
			IBSWidgetInterface::GetWidgetTextFromKey("Scores_MillisecondFormat"));
		TextBlock_BestReactionTime->SetText(FText::Format(MillisecondFormat,
		                                                  FText::AsNumber(
			                                                  BestReactionTime * 1000.f, &NumberFormattingOptions)));
		TextBlock_AverageReactionTime->SetText(FText::Format(MillisecondFormat,
		                                                     FText::AsNumber(
			                                                     AvgReactionTime * 1000.f, &NumberFormattingOptions)));

		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MaximumFractionalDigits = 0;
		TextBlock_BestStreak->SetText(FText::AsNumber(BestStreak, &NumberFormattingOptions));
		TextBlock_AverageStreak->SetText(FText::AsNumber(AverageStreak, &NumberFormattingOptions));

		NumberFormattingOptions.MaximumFractionalDigits = 1;
		NumberFormattingOptions.MinimumFractionalDigits = 1;
		static const FTextFormat PercentFormat = FTextFormat::FromString("{0}%");
		TextBlock_BestAccuracy->SetText(FText::Format(PercentFormat,
		                                              FText::AsNumber(BestAccuracy * 100.f, &NumberFormattingOptions)));
		TextBlock_AverageAccuracy->SetText(FText::Format(PercentFormat,
		                                                 FText::AsNumber(AverageAccuracy, &NumberFormattingOptions)));

		TextBlock_BestTargetsDestroyed->SetText(FText::Format(PercentFormat,
		                                                      FText::AsNumber(
			                                                      BestCompletion * 100.f, &NumberFormattingOptions)));
		TextBlock_AverageTargetsDestroyed->SetText(FText::Format(PercentFormat,
		                                                         FText::AsNumber(
			                                                         AvgTargetsDestroyed, &NumberFormattingOptions)));

		FLineChartSeries ScoreVsTimeLineChartSeries;
		FLineChartSeries StreakVsTimeLineChartSeries;
		FLineChartSeries AverageTargetsDestroyedLineChartSeries;
		FLineChartSeries AverageReactionTimeLineChartSeries;
		FLineChartSeries AccuracyVsTimeLineChartSeries;
		float Index = 0;
		for (const auto& PlayerScore : ActiveScores)
		{
			ScoreVsTimeLineChartSeries.Points.Add({Index, PlayerScore->Score / 1000.f});
			StreakVsTimeLineChartSeries.Points.Add({Index, static_cast<float>(PlayerScore->Streak)});
			AverageReactionTimeLineChartSeries.Points.Add({Index, PlayerScore->AvgTimeOffset * 1000.f});
			AverageTargetsDestroyedLineChartSeries.Points.Add({Index, PlayerScore->Completion * 100.f});
			AccuracyVsTimeLineChartSeries.Points.Add({Index, PlayerScore->Accuracy * 100.f});
			Index++;
		}
		ScoreVsTimeData->Empty(1);
		ScoreVsTimeData->Add(ScoreVsTimeLineChartSeries);
		ScoreVsTime->Redraw();

		StreakVsTimeData->Empty(1);
		StreakVsTimeData->Add(StreakVsTimeLineChartSeries);
		StreakVsTime->Redraw();

		AverageReactionTimeData->Empty(1);
		AverageReactionTimeData->Add(AverageReactionTimeLineChartSeries);
		AverageReactionTime->Redraw();

		AverageTargetsDestroyedData->Empty(1);
		AverageTargetsDestroyedData->Add(AverageTargetsDestroyedLineChartSeries);
		AverageTargetsDestroyed->Redraw();

		AccuracyVsTimeData->Empty(1);
		AccuracyVsTimeData->Add(AccuracyVsTimeLineChartSeries);
		AccuracyVsTime->Redraw();

		LocationAccuracyData->Empty();
		if (CommonScoreInfoMap.Contains(ActiveScores[0]->DefiningConfig))
		{
			const auto& AccuracyData = CommonScoreInfoMap[ActiveScores[0]->DefiningConfig].AccuracyData;
			if (!AccuracyData.AccuracyRows.IsEmpty())
			{
				const auto& AccuracyRows = AccuracyData.AccuracyRows;
				const int RowCount = AccuracyRows.Num();
				const int ColumnCount = AccuracyRows[0].Accuracy.Num();
				LocationAccuracyData->SetNum(ColumnCount);

				// Transpose and reverse order rows
				for (int32 ColumnIndex = 0; ColumnIndex < ColumnCount; ++ColumnIndex)
				{
					auto& NewRow = (*LocationAccuracyData)[ColumnIndex];
					NewRow.Reserve(RowCount);

					for (int32 RowIndex = RowCount - 1; RowIndex >= 0; --RowIndex)
					{
						NewRow.Add(AccuracyRows[RowIndex].Accuracy[ColumnIndex]);
					}
				}
			}
		}
		LocationAccuracy->Redraw();
	}
}

void UGameModeScoreViewerWidget::SetTimesByPlayerScore(TMap<TSharedPtr<FPlayerScore>, FDateTime>&& InTimesByPlayerScore)
{
	TimesByPlayerScore = MoveTemp(InTimesByPlayerScore);
}

void UGameModeScoreViewerWidget::SetActiveScores(const TArray<TSharedPtr<FPlayerScore>>& InActiveScores)
{
	ActiveScores = InActiveScores;
	UpdateDataVisualization();
}

FText UGameModeScoreViewerWidget::HandleGenericDateValueText(const int32 /*SeriesIndex*/, const int32 PointIndex)
{
	if (ActiveScores.IsValidIndex(PointIndex) && TimesByPlayerScore.Contains(ActiveScores[PointIndex]))
	{
		return FText::FromString(
			TimesByPlayerScore[ActiveScores[PointIndex]].ToFormattedString(TEXT("%b %d, %Y, %I:%M %P")));
	}
	return {};
}

FText UGameModeScoreViewerWidget::HandleLocationAccuracyDisplayText(const int32 XIndex, const int32 YIndex)
{
	if (!ActiveScores.IsEmpty() && TimesByPlayerScore.Contains(ActiveScores[0]))
	{
		const auto& AccuracyData = CommonScoreInfoMap[ActiveScores[0]->DefiningConfig].AccuracyData;
		if (!AccuracyData.AccuracyRows.IsEmpty())
		{
			const auto AccuracyRows = AccuracyData.AccuracyRows;
			const int ColumnCount = AccuracyRows[0].Accuracy.Num();
			const int AccuracyRowsIndex = ColumnCount - 1 - YIndex;
			const int AccuracyIndex = XIndex;
			if (AccuracyRows.IsValidIndex(AccuracyRowsIndex) && AccuracyRows[XIndex].Accuracy.IsValidIndex(
				    AccuracyIndex))
			{
				const float Value = AccuracyRows[AccuracyRowsIndex].Accuracy[AccuracyIndex];
				if (Value < 0.f)
				{
					return IBSWidgetInterface::GetWidgetTextFromKey("Scores_NoTargetSpawned");
				}
				FNumberFormattingOptions NumberFormattingOptions;
				NumberFormattingOptions.MaximumFractionalDigits = 0;
				NumberFormattingOptions.MinimumFractionalDigits = 0;
				static const FTextFormat PercentFormat = FTextFormat::FromString("{0}%");
				return FText::Format(PercentFormat, FText::AsNumber(Value * 100.f, &NumberFormattingOptions));
			}
		}
	}
	return {};
}

FText UGameModeScoreViewerWidget::HandleShortDateXAxisFormatter(const int32 XIndex, float)
{
	if (ActiveScores.IsValidIndex(XIndex) && TimesByPlayerScore.Contains(ActiveScores[XIndex]))
	{
		return FText::FromString(TimesByPlayerScore[ActiveScores[XIndex]].ToFormattedString(TEXT("%b %d")));
	}
	return {};
}
