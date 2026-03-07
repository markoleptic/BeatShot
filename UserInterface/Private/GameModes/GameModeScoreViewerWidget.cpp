// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/GameModeScoreViewerWidget.h"
#include "Components/TextBlock.h"
#include "MenuOptions/ComboBoxWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Slate/HeatMap.h"
#include "Widgets/HeatMapWidget.h"
#include "Widgets/LineChartWidget.h"

namespace
{
const FTextFormat& GetPercentFormat()
{
	static const FTextFormat Format = FTextFormat::FromString("{0}%");
	return Format;
}

const FTextFormat& GetMillisecondFormat()
{
	static const FTextFormat Format = FTextFormat::FromString("{0}ms");
	return Format;
}
}

void UGameModeScoreViewerWidget::SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore)
{
	CommonScoreInfoMap = InSaveGamePlayerScore->GetCommonScoreInfo();
}

void UGameModeScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	constexpr int MaxTickCount = 20;

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
	ScoreVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = ShortDateXAxisFormatter;
		XAxisLabelOptions.MaxTickCount = MaxTickCount;
		ScoreVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.MinTickStep = 1;
		YAxisLabelOptions.MinTickCount = 3;
		YAxisLabelOptions.Formatter = ZeroDecimalFormatter;
		ScoreVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	StreakVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	StreakVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = ShortDateXAxisFormatter;
		XAxisLabelOptions.MaxTickCount = MaxTickCount;
		StreakVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.MinTickStep = 1;
		YAxisLabelOptions.MinTickCount = 3;
		YAxisLabelOptions.Formatter = ZeroDecimalFormatter;
		StreakVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AverageTargetsDestroyedData = MakeShared<TArray<FLineChartSeries>>();
	AverageTargetsDestroyedAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = ShortDateXAxisFormatter;
		XAxisLabelOptions.MaxTickCount = MaxTickCount;
		AverageTargetsDestroyedAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = ZeroDecimalFormatter;
		YAxisLabelOptions.MinRangeOverride = 0.f;
		YAxisLabelOptions.MaxRangeOverride = 100.f;
		YAxisLabelOptions.TickStepOverride = 10.0f;
		AverageTargetsDestroyedAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AverageReactionTimeData = MakeShared<TArray<FLineChartSeries>>();
	AverageReactionTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = ShortDateXAxisFormatter;
		XAxisLabelOptions.MaxTickCount = MaxTickCount;
		AverageReactionTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.MinTickStep = 1.f;
		YAxisLabelOptions.MinTickCount = 3;
		YAxisLabelOptions.Formatter = ZeroDecimalFormatter;
		AverageReactionTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AccuracyVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	AccuracyVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = ShortDateXAxisFormatter;
		XAxisLabelOptions.MaxTickCount = MaxTickCount;
		AccuracyVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = ZeroDecimalFormatter;
		YAxisLabelOptions.MinRangeOverride = 0.f;
		YAxisLabelOptions.MaxRangeOverride = 100.f;
		YAxisLabelOptions.TickStepOverride = 10.0f;
		AccuracyVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}


	LocationAccuracyData = MakeShared<FHeatMapData>();
	LocationAccuracyData->Options.bDrawSectionIfValueLessThanZero = true;
	LocationAccuracyAxisData = MakeShared<FHeatMapAxisLabelOptions>();
	LocationAccuracyAxisData->XAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4};
	LocationAccuracyAxisData->YAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4};
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
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			return FText::Format(GetMillisecondFormat(), FText::AsNumber(Value, &NumberFormattingOptions));
		});

	const auto GenericPercentValueTextDelegate = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			return FText::Format(GetPercentFormat(), FText::AsNumber(Value, &NumberFormattingOptions));
		});


	const auto GenericDateValueTextDelegate = TDelegate<FText(int32, int32)>::CreateUObject(
		this, &ThisClass::HandleGenericDateValueText);

	ScoreVsTime->SetData(ScoreVsTimeData, ScoreVsTimeAxisData, GenericDateValueTextDelegate,
	                     ScoreVsTimeValueTextGetter);
	StreakVsTime->SetData(StreakVsTimeData, StreakVsTimeAxisData, GenericDateValueTextDelegate,
	                      StreakVsTimeValueTextGetter);
	AverageReactionTime->SetData(AverageReactionTimeData, AverageReactionTimeAxisData, GenericDateValueTextDelegate,
	                             AverageReactionTimeValueTextGetter);
	AverageTargetsDestroyed->SetData(AverageTargetsDestroyedData, AverageTargetsDestroyedAxisData,
	                                 GenericDateValueTextDelegate, GenericPercentValueTextDelegate);
	AccuracyVsTime->SetData(AccuracyVsTimeData, AccuracyVsTimeAxisData, GenericDateValueTextDelegate,
	                        GenericPercentValueTextDelegate);
	LocationAccuracy->SetData(LocationAccuracyData, LocationAccuracyAxisData,
	                          TDelegate<FText(int32, int32)>::CreateUObject(
		                          this, &ThisClass::HandleLocationAccuracyDisplayText),
	                          LocationAccuracyValueTextGetter);
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

		LocationAccuracyData->Sections.Empty();
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

		NumberFormattingOptions.MinimumFractionalDigits = 1;
		NumberFormattingOptions.MaximumFractionalDigits = 1;
		TextBlock_BestReactionTime->SetText(FText::Format(GetMillisecondFormat(),
		                                                  FText::AsNumber(
			                                                  BestReactionTime * 100.f, &NumberFormattingOptions)));
		TextBlock_AverageReactionTime->SetText(FText::Format(GetMillisecondFormat(),
		                                                     FText::AsNumber(
			                                                     AvgReactionTime, &NumberFormattingOptions)));

		NumberFormattingOptions.MinimumFractionalDigits = 0;
		NumberFormattingOptions.MaximumFractionalDigits = 0;
		TextBlock_BestStreak->SetText(FText::AsNumber(BestStreak, &NumberFormattingOptions));
		TextBlock_AverageStreak->SetText(FText::AsNumber(AverageStreak, &NumberFormattingOptions));

		NumberFormattingOptions.MaximumFractionalDigits = 1;
		NumberFormattingOptions.MinimumFractionalDigits = 1;
		const FTextFormat& PercentFormat = GetPercentFormat();
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
			AverageReactionTimeLineChartSeries.Points.Add({Index, PlayerScore->AvgTimeOffset * 100.f});
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

		LocationAccuracyData->Sections.Empty();
		if (CommonScoreInfoMap.Contains(ActiveScores[0]->DefiningConfig))
		{
			const auto& AccuracyRows = CommonScoreInfoMap[ActiveScores[0]->DefiningConfig].AccuracyData.AccuracyRows;
			LocationAccuracyData->Sections.Init({}, AccuracyRows.Num());
			for (int i = 0; i < AccuracyRows.Num(); i++)
			{
				const auto& AccuracyRow = AccuracyRows[i];
				LocationAccuracyData->Sections[i].Reserve(AccuracyRow.Size);
				for (const float Accuracy : AccuracyRow.Accuracy)
				{
					LocationAccuracyData->Sections[i].Add(Accuracy);
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
		const auto& AccuracyRows = CommonScoreInfoMap[ActiveScores[0]->DefiningConfig].AccuracyData.AccuracyRows;
		if (AccuracyRows.IsValidIndex(XIndex) && AccuracyRows[XIndex].Accuracy.IsValidIndex(YIndex))
		{
			const float Value = AccuracyRows[XIndex].Accuracy[YIndex];
			if (Value < 0.f)
			{
				return FText::FromString("No target has spawned here.");
			}
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 0;
			NumberFormattingOptions.MinimumFractionalDigits = 0;
			return FText::Format(GetPercentFormat(), FText::AsNumber(Value * 100.f, &NumberFormattingOptions));
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
