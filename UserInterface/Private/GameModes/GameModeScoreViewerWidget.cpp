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

	auto GenericDateFormatter = TDelegate<FText(int32, float)>::CreateUObject(this,
		&ThisClass::HandleGenericDateXAxisFormatter);

	ScoreVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	ScoreVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = GenericDateFormatter;
		ScoreVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(0);
			return FText::AsNumber(Value / 1000.f, &NumberFormattingOptions);
		});
		ScoreVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	StreakVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	StreakVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = GenericDateFormatter;
		StreakVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(0);
			return FText::AsNumber(Value, &NumberFormattingOptions);
		});
		StreakVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AverageTargetsDestroyedData = MakeShared<TArray<FLineChartSeries>>();
	AverageTargetsDestroyedAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = GenericDateFormatter;
		AverageTargetsDestroyedAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			return FText::Format(GetPercentFormat(), FText::AsNumber(Value * 100.f, &NumberFormattingOptions));
		});
		AverageTargetsDestroyedAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AverageReactionTimeData = MakeShared<TArray<FLineChartSeries>>();
	AverageReactionTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = GenericDateFormatter;
		AverageReactionTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 0;
			NumberFormattingOptions.MinimumFractionalDigits = 0;
			return FText::AsNumber(Value, &NumberFormattingOptions);
		});
		AverageReactionTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	AccuracyVsTimeData = MakeShared<TArray<FLineChartSeries>>();
	AccuracyVsTimeAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = GenericDateFormatter;
		AccuracyVsTimeAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateLambda([](int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			return FText::Format(GetPercentFormat(), FText::AsNumber(Value * 100.f, &NumberFormattingOptions));
		});
		AccuracyVsTimeAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}


	LocationAccuracyData = MakeShared<FHeatMapData>();
	LocationAccuracyData->Options.bDrawSectionIfValueLessThanZero = false;
	LocationAccuracyAxisData = MakeShared<FHeatMapAxisLabelOptions>();
	LocationAccuracyAxisData->XAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4};
	LocationAccuracyAxisData->YAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4};
	auto LocationAccuracyValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda([](int32, int32, float)
	{
		return FText();
	});

	auto ScoreVsTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(1);
			return FText::AsNumber(Value, &NumberFormattingOptions);
		});

	auto StreakVsTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.SetMaximumFractionalDigits(0);
			return FText::AsNumber(Value, &NumberFormattingOptions);
		});

	auto AverageReactionTimeValueTextGetter = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 0;
			NumberFormattingOptions.MinimumFractionalDigits = 0;
			return FText::Format(GetMillisecondFormat(), FText::AsNumber(Value, &NumberFormattingOptions));
		});

	auto GenericPercentValueTextDelegate = TDelegate<FText(int32, int32, float)>::CreateLambda(
		[](int32, int32, const float Value)
		{
			FNumberFormattingOptions NumberFormattingOptions;
			NumberFormattingOptions.MaximumFractionalDigits = 1;
			NumberFormattingOptions.MinimumFractionalDigits = 1;
			return FText::Format(GetPercentFormat(), FText::AsNumber(Value * 100.f, &NumberFormattingOptions));
		});


	auto GenericDateValueTextDelegate = TDelegate<FText(int32, int32)>::CreateUObject(this,
		&ThisClass::HandleGenericDateValueText);

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
		TDelegate<FText(int32, int32)>::CreateUObject(this, &ThisClass::HandleLocationAccuracyDisplayText),
		LocationAccuracyValueTextGetter);
}

void UGameModeScoreViewerWidget::UpdateActiveScores()
{
	float BestScore = 0.0f;
	float BestAccuracy = 0.0f;
	int BestStreak = 0;
	float BestReactionTime = 0.0f;
	float BestTargetsDestroyed = 0.f;
	for (const auto& PlayerScore : ActiveScores)
	{
		BestScore = FMath::Max(BestScore, PlayerScore->Score);
		BestAccuracy = FMath::Max(BestAccuracy, PlayerScore->Accuracy);
		BestStreak = FMath::Max(BestStreak, PlayerScore->Streak);
		BestReactionTime = FMath::Min(BestReactionTime, PlayerScore->AvgTimeOffset);
		BestTargetsDestroyed = FMath::Max(BestTargetsDestroyed, PlayerScore->Completion);
	}

	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.MaximumFractionalDigits = 0;
	NumberFormattingOptions.MinimumFractionalDigits = 0;
	TextBlock_BestScore->SetText(FText::AsNumber(BestScore, &NumberFormattingOptions));
	TextBlock_BestReactionTime->SetText(FText::AsNumber(BestReactionTime, &NumberFormattingOptions));
	TextBlock_BestStreak->SetText(FText::AsNumber(BestStreak, &NumberFormattingOptions));
	NumberFormattingOptions.MaximumFractionalDigits = 1;
	NumberFormattingOptions.MinimumFractionalDigits = 1;
	const FTextFormat& PercentFormat = GetPercentFormat();
	TextBlock_BestAccuracy->SetText(FText::Format(PercentFormat,
		FText::AsNumber(BestAccuracy * 100.f, &NumberFormattingOptions)));
	TextBlock_BestTargetsDestroyed->SetText(FText::Format(PercentFormat,
		FText::AsNumber(BestTargetsDestroyed * 100.f, &NumberFormattingOptions)));


	float TotalScore = 0.0f;
	float TotalAccuracy = 0.0f;
	int TotalStreak = 0;
	float TotalReactionTime = 0.0f;
	int TotalTargetsDestroyed = 0;
	for (const auto& PlayerScore : ActiveScores)
	{
		TotalScore += PlayerScore->Score;
		TotalAccuracy += PlayerScore->Accuracy;
		TotalStreak += PlayerScore->Streak;
		TotalReactionTime += PlayerScore->AvgTimeOffset;
		TotalTargetsDestroyed += PlayerScore->Completion;
	}
	const float AverageScore = TotalScore / ActiveScores.Num();
	const float AverageAccuracy = TotalAccuracy / ActiveScores.Num();
	const int AverageStreak = FMath::FloorToInt(static_cast<float>(TotalStreak) / ActiveScores.Num());
	const float AvgReactionTime = TotalReactionTime / ActiveScores.Num();
	const int AvgTargetsDestroyed = FMath::FloorToInt(static_cast<float>(TotalTargetsDestroyed) / ActiveScores.Num());

	NumberFormattingOptions.MaximumFractionalDigits = 0;
	NumberFormattingOptions.MinimumFractionalDigits = 0;
	TextBlock_AverageScore->SetText(FText::AsNumber(AverageScore, &NumberFormattingOptions));
	TextBlock_AverageReactionTime->SetText(FText::AsNumber(AvgReactionTime, &NumberFormattingOptions));
	TextBlock_AverageStreak->SetText(FText::AsNumber(AverageStreak, &NumberFormattingOptions));
	NumberFormattingOptions.MaximumFractionalDigits = 1;
	NumberFormattingOptions.MinimumFractionalDigits = 1;
	TextBlock_AverageAccuracy->SetText(FText::Format(PercentFormat,
		FText::AsNumber(AverageAccuracy * 100.f, &NumberFormattingOptions)));
	TextBlock_AverageTargetsDestroyed->SetText(FText::Format(PercentFormat,
		FText::AsNumber(AvgTargetsDestroyed * 100.f, &NumberFormattingOptions)));

	FLineChartSeries ScoreVsTimeLineChartSeries;
	FLineChartSeries StreakVsTimeLineChartSeries;
	FLineChartSeries AverageTargetsDestroyedLineChartSeries;
	FLineChartSeries AverageReactionTimeLineChartSeries;
	FLineChartSeries AccuracyVsTimeLineChartSeries;
	float Index = 0;
	for (const auto& PlayerScore : ActiveScores)
	{
		ScoreVsTimeLineChartSeries.Points.Add({Index, PlayerScore->Score});
		StreakVsTimeLineChartSeries.Points.Add({Index, static_cast<float>(PlayerScore->Streak)});
		AverageReactionTimeLineChartSeries.Points.Add({Index, PlayerScore->AvgTimeOffset});
		AverageTargetsDestroyedLineChartSeries.Points.Add({Index, PlayerScore->Completion});
		AccuracyVsTimeLineChartSeries.Points.Add({Index, PlayerScore->Accuracy});
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
}

FText UGameModeScoreViewerWidget::HandleGenericDateValueText(const int32 XIndex, const int32 YIndex)
{
	return FText::FromString(TimesByPlayerScore[ActiveScores[XIndex]].ToFormattedString(TEXT("%b %d, %Y, %I:%M %P")));
}

FText UGameModeScoreViewerWidget::HandleLocationAccuracyDisplayText(const int32 XIndex, const int32 YIndex)
{
	const float Value = CommonScoreInfoMap[ActiveScores[0]->DefiningConfig].AccuracyData.AccuracyRows[XIndex].Accuracy[
		YIndex];
	if (Value < 0.f)
	{
		return FText::FromString("No target has spawned here.");
	}
	return FText::Format(GetPercentFormat(), Value);
}

FText UGameModeScoreViewerWidget::HandleGenericDateXAxisFormatter(const int32 XIndex, float)
{
	return FText::FromString(TimesByPlayerScore[ActiveScores[XIndex]].ToFormattedString(TEXT("%b %d")));
}
