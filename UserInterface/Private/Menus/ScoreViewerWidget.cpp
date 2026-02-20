// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/ScoreViewerWidget.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/Buttons/MenuButton.h"
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

	PlayFrequencyAxisData = MakeShared<FHeatMapAxisLabelOptions>();
	PlayFrequencyAxisData->YAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4, 5, 6};

	const FDateTime EndDate = FDateTime::Now();
	const int32 TotalDays = FDateTime::DaysInYear(EndDate.GetYear());
	const FDateTime StartDate = EndDate - FTimespan::FromDays(TotalDays) + FTimespan::FromDays(1);

	TMap<int32, FText> DaysOfWeek = CreateDaysOfWeekMap(StartDate);
	TMap<int32, FText> MonthsOfYear;

	const int32 WeekCount = TotalDays / 7 + 1;
	PlayFrequencyData->Sections.Init({}, WeekCount);
	for (auto& Week : PlayFrequencyData->Sections)
	{
		Week.Init(-1, 7);
	}

	int32 StartDow = static_cast<int>(StartDate.GetDayOfWeek());
	for (int32 DayIndex = 0; DayIndex < TotalDays; DayIndex++)
	{
		FDateTime Date = StartDate + FTimespan::FromDays(DayIndex);
		const int32 WeekIndex = (StartDow + DayIndex) / 7;
		const int32 DayOfWeekIndex = static_cast<int>(Date.GetDayOfWeek());
		PlayFrequencyData->Sections[WeekIndex][DayOfWeekIndex] = DayOfWeekIndex / 7.f;

		if (Date.GetDay() == 1)
		{
			MonthsOfYear.FindOrAdd(WeekIndex) = FText::FromString(Date.ToFormattedString(TEXT("%b")));
			PlayFrequencyAxisData->XAxisLabelsDrawIndices.Add(WeekIndex);
		}
	}

	PlayFrequencyAxisData->XLabelFormatter = [MonthsOfYear](const int32 WeekIndex)
	{
		return MonthsOfYear[WeekIndex];
	};
	PlayFrequencyAxisData->YLabelFormatter = [DaysOfWeek](const int32 DayOfWeekIndex)
	{
		return DaysOfWeek[DayOfWeekIndex];
	};

	const FTextFormat Format = FTextFormat::FromString("{0}\n{1} {2}");

	auto DisplayTextGetter = [this, Format, StartDate, StartDow](const int32 WeekIndex, const int32 DayOfWeekIndex)
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
		return FText::Format(Format, Args);
	};

	const FTextFormat Format2 = FTextFormat::FromString("Total for week: {0} {1}");

	auto ValueTextGetter = [this, Format2](const int32 WeekIndex, const int32 /*DayOfWeekIndex*/, const float /*Value*/)
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

		return FText::Format(Format2, Args);
	};

	PlayFrequency->SetData(PlayFrequencyData, PlayFrequencyAxisData, DisplayTextGetter, ValueTextGetter);

	MenuButton_Overview->SetActive();
	Switcher->SetActiveWidget(MenuButton_Overview->GetAssociatedWidget());
}

void UScoreViewerWidget::LoadScores(USaveGamePlayerScore* SaveGamePlayerScore, const bool SwitchToMostRecent)
{
	const auto& PlayerScoresRef = SaveGamePlayerScore->GetPlayerScoresRef();
	if (PlayerScoresRef.IsEmpty())
	{
		// TODO: Hide stuff and show empty message
	}
	else
	{
		DefaultGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);
		CustomGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);
		if (SwitchToMostRecent)
		{
			FDateTime MostRecentTime = FDateTime::MinValue();
			EGameModeType MostRecentGameModeType = EGameModeType::None;
			for (const auto& PlayerScore : PlayerScoresRef)
			{
				FDateTime ParsedTime;
				FDateTime::ParseIso8601(*PlayerScore.Time, ParsedTime);
				if (ParsedTime > MostRecentTime)
				{
					MostRecentTime = ParsedTime;
					MostRecentGameModeType = PlayerScore.DefiningConfig.GameModeType;
				}
			}
			if (MostRecentGameModeType == EGameModeType::Preset)
			{
				MenuButton_DefaultModes->SetActive();
				Switcher->SetActiveWidget(MenuButton_DefaultModes->GetAssociatedWidget());
			}
			else if (MostRecentGameModeType == EGameModeType::Custom)
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
