// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/ScoreViewerWidget.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/Buttons/MenuButton.h"
#include "Widgets/BarChartWidget.h"
#include "Widgets/HeatMapWidget.h"

namespace
{
	TMap<int32, FText> CreateDaysOfWeekMap(const FDateTime& StartDate)
	{
		TMap<int32, FText> DaysOfWeek;
		FDateTime CurrentDate = StartDate;
		while (DaysOfWeek.Num() != 7)
		{
			DaysOfWeek.Add(static_cast<int32>(CurrentDate.GetDayOfWeek()),
				FText::FromString(CurrentDate.ToFormattedString(TEXT("%a"))));
			CurrentDate = CurrentDate + FTimespan::FromDays(1);
		}
		return DaysOfWeek;
	}
}

void UScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	MenuButton_Overview->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_DefaultModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_CustomModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);
	MenuButton_History->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_BSButton);

	MenuButton_Overview->SetDefaults(Box_Overview, MenuButton_DefaultModes);
	MenuButton_DefaultModes->SetDefaults(DefaultGameModeScoreViewerWidget, MenuButton_CustomModes);
	MenuButton_CustomModes->SetDefaults(CustomGameModeScoreViewerWidget, MenuButton_History);
	MenuButton_History->SetDefaults(Box_History, MenuButton_Overview);

	PlayFrequencyData = MakeShared<FHeatMapData>();
	PlayFrequencyData->Options.bDrawSectionIfValueLessThanZero = false;
	PlayFrequencyData->Options.bUseCustomTwoColorInterpolation = true;
	PlayFrequencyData->Options.Padding = {10.f, 10.f, 10.f, 10.f};
	PlayFrequencyAxisData = MakeShared<FHeatMapAxisLabelOptions>();
	PlayFrequencyAxisData->YAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4, 5, 6};

	const FDateTime EndDate = FDateTime::Now();
	const int32 TotalDays = FDateTime::DaysInYear(EndDate.GetYear());
	StartDate = EndDate - FTimespan::FromDays(TotalDays) + FTimespan::FromDays(1);

	TMap<int32, FText> DaysOfWeek = CreateDaysOfWeekMap(StartDate);
	TMap<int32, FText> MonthsOfYear;

	const int32 WeekCount = TotalDays / 7 + 1;
	PlayFrequencyData->Sections.Init({}, WeekCount);
	for (auto& Week : PlayFrequencyData->Sections)
	{
		Week.Init(-1.f, 7);
	}

	StartDow = static_cast<int>(StartDate.GetDayOfWeek());
	for (int32 DayIndex = 0; DayIndex < TotalDays; DayIndex++)
	{
		FDateTime Date = StartDate + FTimespan::FromDays(DayIndex);
		const int32 WeekIndex = (StartDow + DayIndex) / 7;
		const int32 DayOfWeekIndex = static_cast<int>(Date.GetDayOfWeek());
		PlayFrequencyData->Sections[WeekIndex][DayOfWeekIndex] = 0.f;
		if (Date.GetDay() == 1)
		{
			MonthsOfYear.FindOrAdd(WeekIndex) = FText::FromString(Date.ToFormattedString(TEXT("%b")));
			PlayFrequencyAxisData->XAxisLabelsDrawIndices.Add(WeekIndex);
		}
	}

	PlayFrequencyAxisData->XLabelFormatter = TDelegate<FText(int32)>::CreateLambda([MonthsOfYear](const int32 WeekIndex)
	{
		return MonthsOfYear[WeekIndex];
	});
	PlayFrequencyAxisData->YLabelFormatter = TDelegate<FText(int32)>::CreateLambda(
		[DaysOfWeek](const int32 DayOfWeekIndex)
		{
			return DaysOfWeek[DayOfWeekIndex];
		});
	PlayFrequency->SetData(PlayFrequencyData, PlayFrequencyAxisData,
		TDelegate<FText(int32, int32)>::CreateUObject(this, &ThisClass::HandleLocationAccuracyDisplayText),
		TDelegate<FText(int32, int32, float)>::CreateUObject(this, &ThisClass::HandleLocationAccuracyValueText));

	MostPlayedDefaultGameModesData = MakeShared<FBarChartData>();
	MostPlayedDefaultGameModesAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(this,
			&ThisClass::HandleMostPlayedDefaultGameModesXAxisFormatter);
		MostPlayedDefaultGameModesAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(this,
			&ThisClass::HandleMostPlayedDefaultGameModesYAxisFormatter);
		MostPlayedDefaultGameModesAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	MostPlayedCustomGameModesData = MakeShared<FBarChartData>();
	MostPlayedCustomGameModesAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(this,
			&ThisClass::HandleMostPlayedCustomGameModesXAxisFormatter);
		MostPlayedCustomGameModesAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(this,
			&ThisClass::HandleMostPlayedCustomGameModesYAxisFormatter);
		MostPlayedCustomGameModesAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	MostPlayedDefaultGameModes->SetData(MostPlayedDefaultGameModesData, MostPlayedDefaultGameModesAxisData,
		TDelegate<FText(int32)>::CreateUObject(this, &ThisClass::HandleMostPlayedDefaultGameModesDisplayText),
		TDelegate<FText(int32, float)>::CreateUObject(this, &ThisClass::HandleMostPlayedDefaultGameModesValueText));
	MostPlayedCustomGameModes->SetData(MostPlayedCustomGameModesData, MostPlayedCustomGameModesAxisData,
		TDelegate<FText(int32)>::CreateUObject(this, &ThisClass::HandleMostPlayedCustomGameModesDisplayText),
		TDelegate<FText(int32, float)>::CreateUObject(this, &ThisClass::HandleMostPlayedCustomGameModesValueText));

	MenuButton_Overview->SetActive();
	Switcher->SetActiveWidget(MenuButton_Overview->GetAssociatedWidget());
}

void UScoreViewerWidget::LoadScores(USaveGamePlayerScore* InSaveGamePlayerScore, const bool SwitchToMostRecent)
{
	SaveGamePlayerScore = InSaveGamePlayerScore;
	DefaultGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);
	CustomGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);

	const auto& PlayerScoresPtr = SaveGamePlayerScore->GetPlayerScoresPtr();
	if (PlayerScoresPtr.IsEmpty())
	{
		Switcher->SetActiveWidget(Box_NoScores);

		MenuButton_Overview->SetInActive();
		MenuButton_DefaultModes->SetInActive();
		MenuButton_CustomModes->SetInActive();
		MenuButton_History->SetInActive();

		MenuButton_Overview->SetIsEnabled(false);
		MenuButton_DefaultModes->SetIsEnabled(false);
		MenuButton_CustomModes->SetIsEnabled(false);
		MenuButton_History->SetIsEnabled(false);
	}
	else
	{
		FDateTime MostRecentDefaultTime = FDateTime::MinValue();
		FDateTime MostRecentCustomTime = FDateTime::MinValue();
		TSharedPtr<FPlayerScore> MostRecentDefaultScore;
		TSharedPtr<FPlayerScore> MostRecentCustomScore;
		TMap<EBaseGameMode, FGameModePlayTime> PlayTimeByBaseGameMode;
		TMap<FString, FGameModePlayTime> PlayTimeByCustomGameModeName;

		for (const auto& PlayerScore : PlayerScoresPtr)
		{
			FDateTime ParsedTime;
			FDateTime::ParseIso8601(*PlayerScore->Time, ParsedTime);
			if (ParsedTime >= StartDate)
			{
				const int32 DayIndex = (ParsedTime - StartDate).GetTotalDays();
				const int32 WeekIndex = (StartDow + DayIndex) / 7;
				const int32 DayOfWeekIndex = static_cast<int32>(ParsedTime.GetDayOfWeek());
				PlayFrequencyData->Sections[WeekIndex][DayOfWeekIndex] += PlayerScore->SongLength;
			}

			if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Preset)
			{
				if (ParsedTime > MostRecentDefaultTime)
				{
					MostRecentDefaultTime = ParsedTime;
					MostRecentDefaultScore = PlayerScore;
				}
				const auto& BaseGameMode = PlayerScore->DefiningConfig.BaseGameMode;
				auto& Current = PlayTimeByBaseGameMode.FindOrAdd(BaseGameMode);
				Current.BaseGameMode = BaseGameMode;
				Current.GameModeType = PlayerScore->DefiningConfig.GameModeType;
				Current.PlayTime += PlayerScore->SongLength;
			}
			else if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Custom)
			{
				if (ParsedTime > MostRecentCustomTime)
				{
					MostRecentCustomTime = ParsedTime;
					MostRecentCustomScore = PlayerScore;
				}
				const auto& CustomGameModeName = PlayerScore->DefiningConfig.CustomGameModeName;
				auto& Current = PlayTimeByCustomGameModeName.FindOrAdd(CustomGameModeName);
				Current.BaseGameMode = EBaseGameMode::None;
				Current.GameModeType = PlayerScore->DefiningConfig.GameModeType;
				Current.CustomGameModeName = CustomGameModeName;
				Current.PlayTime += PlayerScore->SongLength;
			}
		}

		DefaultGameModePlayTime.Empty(PlayTimeByBaseGameMode.Num());
		for (const auto& [GameModeType, GameModePlayTime] : PlayTimeByBaseGameMode)
		{
			DefaultGameModePlayTime.Add(GameModePlayTime);
		}

		CustomGameModePlayTime.Empty(PlayTimeByCustomGameModeName.Num());
		for (const auto& [CustomGameModeName, GameModePlayTime] : PlayTimeByCustomGameModeName)
		{
			CustomGameModePlayTime.Add(GameModePlayTime);
		}

		Algo::Sort(DefaultGameModePlayTime, [](const FGameModePlayTime& Left, const FGameModePlayTime& Right)
		{
			return Left.PlayTime > Right.PlayTime;
		});
		Algo::Sort(CustomGameModePlayTime, [](const FGameModePlayTime& Left, const FGameModePlayTime& Right)
		{
			return Left.PlayTime > Right.PlayTime;
		});

		MostPlayedDefaultGameModesData->Points.Empty(DefaultGameModePlayTime.Num());
		for (const auto& GameModePlayTime : DefaultGameModePlayTime)
		{
			MostPlayedDefaultGameModesData->Points.Add(GameModePlayTime.PlayTime);
		}

		MostPlayedCustomGameModesData->Points.Empty(CustomGameModePlayTime.Num());
		for (const auto& GameModePlayTime : CustomGameModePlayTime)
		{
			MostPlayedCustomGameModesData->Points.Add(GameModePlayTime.PlayTime);
		}

		if (MostRecentDefaultScore)
		{
			DefaultGameModeScoreViewerWidget->SetActiveScores(MostRecentDefaultScore->DefiningConfig.BaseGameMode,
				MostRecentDefaultScore->SongTitle, MostRecentDefaultScore->DefiningConfig.Difficulty);
		}
		if (MostRecentCustomScore)
		{
			CustomGameModeScoreViewerWidget->SetActiveScores(MostRecentCustomScore->DefiningConfig.CustomGameModeName,
				MostRecentCustomScore->SongTitle);
		}

		if (SwitchToMostRecent && (MostRecentDefaultScore || MostRecentCustomScore))
		{
			bool SwitchToDefault = true;
			if (MostRecentDefaultScore && MostRecentCustomScore && MostRecentDefaultTime < MostRecentCustomTime)
			{
				SwitchToDefault = false;
			}
			else if (MostRecentCustomScore)
			{
				SwitchToDefault = false;
			}

			if (SwitchToDefault)
			{
				MenuButton_DefaultModes->SetActive();
				Switcher->SetActiveWidget(MenuButton_DefaultModes->GetAssociatedWidget());
			}
			else
			{
				MenuButton_CustomModes->SetActive();
				Switcher->SetActiveWidget(MenuButton_CustomModes->GetAssociatedWidget());
			}
		}
		else
		{
			MenuButton_Overview->SetActive();
			Switcher->SetActiveWidget(MenuButton_Overview->GetAssociatedWidget());
		}
	}
}

void UScoreViewerWidget::OnButtonClicked_BSButton(const UBSButton* Button)
{
	Switcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetAssociatedWidget());
}

FText UScoreViewerWidget::HandleLocationAccuracyDisplayText(const int32 WeekIndex, const int32 DayOfWeekIndex)
{
	const float Value = PlayFrequencyData->Sections[WeekIndex][DayOfWeekIndex];
	const int32 DayIndex = WeekIndex * 7 + (DayOfWeekIndex - StartDow);
	const FDateTime Date = StartDate + FTimespan::FromDays(DayIndex);
	const FText DateText = FText::FromString(Date.ToFormattedString(TEXT("%Y-%m-%d")));
	FNumberFormattingOptions NumberFormattingOptions;
	FFormatOrderedArguments Args;
	Args.Add(DateText);
	if (Value < 60.f)
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		Args.Add(FText::AsNumber(Value, &NumberFormattingOptions));
		Args.Add(FText::FromString("Minutes"));
	}
	else
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(2);
		Args.Add(FText::AsNumber(Value / 60.f, &NumberFormattingOptions));
		Args.Add(FText::FromString("Hours"));
	}
	return FText::Format(LocationAccuracyDisplayFormat, Args);
}

FText UScoreViewerWidget::HandleLocationAccuracyValueText(const int32 WeekIndex, int32, float)
{
	float Total = 0.f;
	for (const float CurrentValue : PlayFrequencyData->Sections[WeekIndex])
	{
		if (CurrentValue > 0.f)
		{
			Total += CurrentValue;
		}
	}
	FNumberFormattingOptions NumberFormattingOptions;
	FFormatOrderedArguments Args;
	if (Total < 60.f)
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		Args.Add(FText::AsNumber(Total, &NumberFormattingOptions));
		Args.Add(FText::FromString("Minutes"));
	}
	else
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(2);
		Args.Add(FText::AsNumber(Total / 60.f, &NumberFormattingOptions));
		Args.Add(FText::FromString("Hours"));
	}

	return FText::Format(LocationAccuracyValueFormat, Args);
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesDisplayText(const int32 Index)
{
	if (DefaultGameModePlayTime.IsValidIndex(Index))
	{
		return BaseGameModeText[DefaultGameModePlayTime[Index].BaseGameMode];
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesValueText(int32, const float Value)
{
	FNumberFormattingOptions NumberFormattingOptions;
	FFormatOrderedArguments Args;
	if (Value < 60.f)
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		Args.Add(FText::AsNumber(Value, &NumberFormattingOptions));
		Args.Add(FText::FromString("Minutes"));
	}
	else
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(2);
		Args.Add(FText::AsNumber(Value / 60.f, &NumberFormattingOptions));
		Args.Add(FText::FromString("Hours"));
	}

	return FText::Format(LocationAccuracyValueFormat, Args);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesDisplayText(const int32 Index)
{
	return FText::FromString(CustomGameModePlayTime[Index].CustomGameModeName);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesValueText(const int32 Index, const float Value)
{
	return HandleMostPlayedDefaultGameModesValueText(Index, Value);
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesXAxisFormatter(const int32 Index, float)
{
	if (DefaultGameModePlayTime.IsValidIndex(Index))
	{
		return BaseGameModeText[DefaultGameModePlayTime[Index].BaseGameMode];
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesYAxisFormatter(int32, const float Value)
{
	FNumberFormattingOptions NumberFormattingOptions;
	NumberFormattingOptions.SetMaximumFractionalDigits(1);
	return FText::AsNumber(Value / 60.f, &NumberFormattingOptions);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesXAxisFormatter(const int32 Index, float)
{
	if (CustomGameModePlayTime.IsValidIndex(Index))
	{
		return FText::FromString(CustomGameModePlayTime[Index].CustomGameModeName);
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesYAxisFormatter(const int32 Index, const float Value)
{
	return HandleMostPlayedDefaultGameModesYAxisFormatter(Index, Value);
}
