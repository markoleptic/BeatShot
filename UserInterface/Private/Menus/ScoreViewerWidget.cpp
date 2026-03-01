// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "Menus/ScoreViewerWidget.h"
#include "Algo/MaxElement.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "GameModes/CustomGameModeScoreViewerWidget.h"
#include "GameModes/DefaultGameModeScoreViewerWidget.h"
#include "GameModes/GameModeScoreViewerWidget.h"
#include "GameModes/ScoreTable.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/Buttons/MenuButton.h"
#include "Widgets/BarChartWidget.h"
#include "Widgets/HeatMapWidget.h"

namespace
{
constexpr int DaysInWeek = 7;

TMap<int32, FText> CreateDaysOfWeekMap(const FDateTime& StartDate)
{
	TMap<int32, FText> DaysOfWeek;
	FDateTime CurrentDate = StartDate;
	while (DaysOfWeek.Num() != DaysInWeek)
	{
		DaysOfWeek.Add(static_cast<int32>(CurrentDate.GetDayOfWeek()),
		               FText::FromString(CurrentDate.ToFormattedString(TEXT("%a"))));
		CurrentDate = CurrentDate + FTimespan::FromDays(1);
	}
	return DaysOfWeek;
}

void SortGameModePlayTime(TArray<FGameModePlayTime>& GameModePlayTime)
{
	Algo::Sort(GameModePlayTime, [](const FGameModePlayTime& Left, const FGameModePlayTime& Right)
	{
		return Left.PlayTime > Right.PlayTime;
	});
}

template <typename T>
void SortAndUpdateGameModePlayTimeData(const TMap<T, FGameModePlayTime>& PlayTimeMap,
                                       TArray<FGameModePlayTime>& PlayTimeArray,
                                       TArray<float>& Points)
{
	PlayTimeArray.Empty(PlayTimeMap.Num());
	for (const auto& [Key, GameModePlayTime] : PlayTimeMap)
	{
		PlayTimeArray.Add(GameModePlayTime);
	}

	SortGameModePlayTime(PlayTimeArray);

	Points.Empty(PlayTimeArray.Num());
	for (const auto& GameModePlayTime : PlayTimeArray)
	{
		Points.Add(GameModePlayTime.PlayTime);
	}
}
}

void UScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ScoreTable->SetBaseGameModeText(BaseGameModeText);
	ScoreTable->SetDifficultyText(DifficultyText);
	ScoreTable->OnSelectionChangedDelegate.BindUObject(this, &UScoreViewerWidget::OnSelectionChanged_ScoreTable);

	DefaultGameModeScoreViewerWidget->SetBaseGameModeText(BaseGameModeText);
	DefaultGameModeScoreViewerWidget->SetDifficultyText(DifficultyText);

	MenuButton_Overview->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_DefaultModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CustomModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_History->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	BSButton_DeleteSelectedScores->OnBSButtonPressed.AddUObject(
		this, &ThisClass::OnButtonClicked_DeleteSelectedScoresButton);

	MenuButton_Overview->SetDefaults(Box_Overview, MenuButton_DefaultModes);
	MenuButton_DefaultModes->SetDefaults(DefaultGameModeScoreViewerWidget, MenuButton_CustomModes);
	MenuButton_CustomModes->SetDefaults(CustomGameModeScoreViewerWidget, MenuButton_History);
	MenuButton_History->SetDefaults(Box_History, MenuButton_Overview);

	RelativePlayFrequencyData = MakeShared<FHeatMapData>();
	RelativePlayFrequencyData->Options.bDrawSectionIfValueLessThanZero = false;
	RelativePlayFrequencyData->Options.bUseCustomTwoColorInterpolation = true;
	RelativePlayFrequencyData->Options.Padding = {10.f, 10.f, 10.f, 10.f};
	PlayFrequencyAxisData = MakeShared<FHeatMapAxisLabelOptions>();
	PlayFrequencyAxisData->YAxisLabelsDrawIndices = TSet{0, 1, 2, 3, 4, 5, 6};

	const FDateTime EndDate = FDateTime::Now();
	const int32 TotalDays = FDateTime::DaysInYear(EndDate.GetYear());
	StartDate = EndDate - FTimespan::FromDays(TotalDays) + FTimespan::FromDays(1);

	TMap<int32, FText> DaysOfWeek = CreateDaysOfWeekMap(StartDate);
	TMap<int32, FText> MonthsOfYear;

	const int32 WeekCount = TotalDays / DaysInWeek + 1;
	InitialPlayFrequencyData.Init({}, WeekCount);
	for (int WeekIndex = 0; WeekIndex < WeekCount; WeekIndex++)
	{
		InitialPlayFrequencyData[WeekIndex].Init(-1.f, DaysInWeek);
	}

	StartDow = static_cast<int>(StartDate.GetDayOfWeek());
	for (int32 DayIndex = 0; DayIndex < TotalDays; DayIndex++)
	{
		FDateTime Date = StartDate + FTimespan::FromDays(DayIndex);
		const int32 WeekIndex = (StartDow + DayIndex) / DaysInWeek;
		const int32 DayOfWeekIndex = static_cast<int>(Date.GetDayOfWeek());
		InitialPlayFrequencyData[WeekIndex][DayOfWeekIndex] = 0.f;
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
	PlayFrequency->SetData(RelativePlayFrequencyData, PlayFrequencyAxisData,
	                       TDelegate<FText
		                       (int32, int32)>::CreateUObject(this, &ThisClass::HandlePlayFrequencyDisplayText),
	                       TDelegate<FText(int32, int32, float)>::CreateUObject(
		                       this, &ThisClass::HandlePlayFrequencyValueText));

	MostPlayedDefaultGameModesData = MakeShared<FBarChartData>();
	MostPlayedDefaultGameModesAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedDefaultGameModesXAxisFormatter);
		MostPlayedDefaultGameModesAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedDefaultGameModesYAxisFormatter);
		MostPlayedDefaultGameModesAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	MostPlayedCustomGameModesData = MakeShared<FBarChartData>();
	MostPlayedCustomGameModesAxisData = MakeShared<TMap<EAxisType, FAxisLabelOptions>>();
	{
		FAxisLabelOptions XAxisLabelOptions;
		XAxisLabelOptions.StartAtZero = false;
		XAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedCustomGameModesXAxisFormatter);
		MostPlayedCustomGameModesAxisData->Add({EAxisType::X, XAxisLabelOptions});

		FAxisLabelOptions YAxisLabelOptions;
		YAxisLabelOptions.StartAtZero = true;
		YAxisLabelOptions.Formatter = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedCustomGameModesYAxisFormatter);
		MostPlayedCustomGameModesAxisData->Add({EAxisType::Y, YAxisLabelOptions});
	}

	MostPlayedDefaultGameModes->SetData(MostPlayedDefaultGameModesData, MostPlayedDefaultGameModesAxisData,
	                                    TDelegate<FText(int32)>::CreateUObject(
		                                    this, &ThisClass::HandleMostPlayedDefaultGameModesDisplayText),
	                                    TDelegate<FText(int32, float)>::CreateUObject(
		                                    this, &ThisClass::HandleMostPlayedDefaultGameModesValueText));
	MostPlayedCustomGameModes->SetData(MostPlayedCustomGameModesData, MostPlayedCustomGameModesAxisData,
	                                   TDelegate<FText(int32)>::CreateUObject(
		                                   this, &ThisClass::HandleMostPlayedCustomGameModesDisplayText),
	                                   TDelegate<FText(int32, float)>::CreateUObject(
		                                   this, &ThisClass::HandleMostPlayedCustomGameModesValueText));

	MenuButton_Overview->SetActive();
	Switcher->SetActiveWidget(MenuButton_Overview->GetAssociatedWidget());
}

void UScoreViewerWidget::LoadScores(USaveGamePlayerScore* InSaveGamePlayerScore, const bool SwitchToMostRecent)
{
	if (!SaveGamePlayerScore)
	{
		SaveGamePlayerScore = InSaveGamePlayerScore;
		InSaveGamePlayerScore->OnScoresDeleted.AddUObject(this, &UScoreViewerWidget::LoadScores,
		                                                  SaveGamePlayerScore.Get(), false);
	}

	RelativePlayFrequencyData->Sections = InitialPlayFrequencyData;
	PlayFrequencyData = InitialPlayFrequencyData;

	const auto& PlayerScoresPtr = SaveGamePlayerScore->GetPlayerScoresPtr();
	TMap<EBaseGameMode, FGameModePlayTime> PlayTimeByBaseGameMode;
	TMap<FString, FGameModePlayTime> PlayTimeByCustomGameModeName;
	const auto [MostRecentPlayerScore,MostRecentDefaultPlayerScore,MostRecentCustomPlayerScore,
		TotalSecondsInAnyGameMode] = CalculateTimeStatistics(PlayerScoresPtr, PlayTimeByBaseGameMode,
		                                                     PlayTimeByCustomGameModeName);

	const bool PlayerScoresEmpty = PlayerScoresPtr.IsEmpty();
	if (PlayerScoresEmpty)
	{
		Switcher->SetActiveWidget(Box_NoScores);
		MenuButton_Overview->SetInActive();
		MenuButton_DefaultModes->SetInActive();
		MenuButton_CustomModes->SetInActive();
		MenuButton_History->SetInActive();
	}
	MenuButton_Overview->SetIsEnabled(!PlayerScoresEmpty);
	MenuButton_DefaultModes->SetIsEnabled(!PlayerScoresEmpty);
	MenuButton_CustomModes->SetIsEnabled(!PlayerScoresEmpty);
	MenuButton_History->SetIsEnabled(!PlayerScoresEmpty);

	UpdateUserFacingTimeStatisticsLabels(PlayTimeByBaseGameMode, PlayTimeByCustomGameModeName,
	                                     TotalSecondsInAnyGameMode);
	MostPlayedCustomGameModes->Redraw();
	MostPlayedDefaultGameModes->Redraw();
	PlayFrequency->Redraw();
	ScoreTable->SetListItems(PlayerScoresPtr);

	DefaultGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);
	CustomGameModeScoreViewerWidget->SetSaveGamePlayerScore(SaveGamePlayerScore);

	if (MostRecentDefaultPlayerScore)
	{
		DefaultGameModeScoreViewerWidget->SetActiveScores(MostRecentDefaultPlayerScore->DefiningConfig.BaseGameMode,
		                                                  MostRecentDefaultPlayerScore->SongTitle,
		                                                  MostRecentDefaultPlayerScore->DefiningConfig.Difficulty);
	}
	if (MostRecentCustomPlayerScore)
	{
		CustomGameModeScoreViewerWidget->SetActiveScores(MostRecentCustomPlayerScore->DefiningConfig.CustomGameModeName,
		                                                 MostRecentCustomPlayerScore->SongTitle);
	}

	if (SwitchToMostRecent && MostRecentPlayerScore)
	{
		UMenuButton* ButtonToSetActive = MostRecentPlayerScore->DefiningConfig.GameModeType == EGameModeType::Preset
		                                 ? MenuButton_DefaultModes
		                                 : MenuButton_CustomModes;
		ButtonToSetActive->SetActive();
		Switcher->SetActiveWidget(ButtonToSetActive->GetAssociatedWidget());
	}
}

void UScoreViewerWidget::OnButtonClicked_MenuButton(const UBSButton* Button)
{
	Switcher->SetActiveWidget(Cast<UMenuButton>(Button)->GetAssociatedWidget());
}

void UScoreViewerWidget::OnButtonClicked_DeleteSelectedScoresButton(const UBSButton*)
{
	auto SelectedItems = ScoreTable->GetSelectedItems();
	if (!SelectedItems.IsEmpty())
	{
		SaveGamePlayerScore->DeletePlayerScores(SelectedItems);
		// Should trigger LoadScores from USaveGamePlayerScore::OnScoresDeleted
	}
}

FCalculateTimeStatisticsResult UScoreViewerWidget::CalculateTimeStatistics(
	const TArray<TSharedPtr<FPlayerScore>>& PlayerScoresPtr,
	TMap<EBaseGameMode, FGameModePlayTime>& PlayTimeByBaseGameMode,
	TMap<FString, FGameModePlayTime>& PlayTimeByCustomGameModeName)
{
	FCalculateTimeStatisticsResult CalculateTimeStatisticsResult;

	FDateTime MostRecentDefaultTime = FDateTime::MinValue();
	FDateTime MostRecentCustomTime = FDateTime::MinValue();
	TSharedPtr<FPlayerScore> MostRecentDefaultScore;
	TSharedPtr<FPlayerScore> MostRecentCustomScore;
	float TotalSecondsInAnyGameMode = 0.0f;

	for (const auto& PlayerScore : PlayerScoresPtr)
	{
		if (PlayerScore->LocalDateTime >= StartDate)
		{
			const int32 DayIndex = (PlayerScore->LocalDateTime - StartDate).GetTotalDays();
			const int32 WeekIndex = (StartDow + DayIndex) / DaysInWeek;
			const int32 DayOfWeekIndex = static_cast<int32>(PlayerScore->LocalDateTime.GetDayOfWeek());
			PlayFrequencyData[WeekIndex][DayOfWeekIndex] += PlayerScore->SongLength;
		}

		TotalSecondsInAnyGameMode += PlayerScore->SongLength;

		if (PlayerScore->DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			if (PlayerScore->LocalDateTime > MostRecentDefaultTime)
			{
				MostRecentDefaultTime = PlayerScore->LocalDateTime;
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
			if (PlayerScore->LocalDateTime > MostRecentCustomTime)
			{
				MostRecentCustomTime = PlayerScore->LocalDateTime;
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

	UpdateRelativePlayFrequency();
	SortAndUpdateGameModePlayTimeData(PlayTimeByBaseGameMode, DefaultGameModePlayTime,
	                                  MostPlayedDefaultGameModesData->Points);
	SortAndUpdateGameModePlayTimeData(PlayTimeByCustomGameModeName, CustomGameModePlayTime,
	                                  MostPlayedCustomGameModesData->Points);

	TSharedPtr<FPlayerScore> MostRecentPlayerScore;
	if (MostRecentDefaultScore && MostRecentCustomScore)
	{
		MostRecentPlayerScore = MostRecentDefaultTime > MostRecentCustomTime
		                        ? MostRecentDefaultScore
		                        : MostRecentCustomScore;
	}
	else if (MostRecentDefaultScore)
	{
		MostRecentPlayerScore = MostRecentDefaultScore;
	}
	else if (MostRecentCustomScore)
	{
		MostRecentPlayerScore = MostRecentCustomScore;
	}

	return FCalculateTimeStatisticsResult{
		.MostRecentPlayerScore = MoveTemp(MostRecentPlayerScore),
		.MostRecentDefaultPlayerScore = MoveTemp(MostRecentDefaultScore),
		.MostRecentCustomPlayerScore = MoveTemp(MostRecentCustomScore),
		.TotalSecondsInAnyGameMode = TotalSecondsInAnyGameMode
	};
}


void UScoreViewerWidget::UpdateRelativePlayFrequency()
{
	float MaxPlayTimeInOneDay = 0.0f;
	for (const auto& Week : PlayFrequencyData)
	{
		if (!Week.IsEmpty())
		{
			MaxPlayTimeInOneDay = FMath::Max(MaxPlayTimeInOneDay, *Algo::MaxElement(Week));
		}
	}
	if (MaxPlayTimeInOneDay > 0.f)
	{
		auto& RelativePlayFrequencySections = RelativePlayFrequencyData->Sections;
		for (int WeekIndex = 0; WeekIndex < PlayFrequencyData.Num(); WeekIndex++)
		{
			const auto& AbsolutePlayFrequencyWeek = PlayFrequencyData[WeekIndex];
			auto& RelativePlayFrequencyWeek = RelativePlayFrequencySections[WeekIndex];
			for (int DayOfWeekIndex = 0; DayOfWeekIndex < AbsolutePlayFrequencyWeek.Num(); DayOfWeekIndex++)
			{
				const float AbsolutePlayTimeInSeconds = AbsolutePlayFrequencyWeek[DayOfWeekIndex];
				RelativePlayFrequencyWeek[DayOfWeekIndex] = AbsolutePlayTimeInSeconds > 0.f
				                                            ? AbsolutePlayTimeInSeconds / MaxPlayTimeInOneDay
				                                            : AbsolutePlayTimeInSeconds;
			}
		}
	}
}

void UScoreViewerWidget::UpdateUserFacingTimeStatisticsLabels(
	const TMap<EBaseGameMode, FGameModePlayTime>& PlayTimeByBaseGameMode,
	const TMap<FString, FGameModePlayTime>& PlayTimeByCustomGameModeName,
	const float TotalTimeInAnyGameMode)
{
	TextBlock_TotalTimeInAnyGameMode->SetText(FormatTime(TotalTimeInAnyGameMode));

	EBaseGameMode MostPlayedDefaultGameMode = EBaseGameMode::None;
	float TimePlayedForMostPlayedDefaultMode = 0.0f;
	for (const auto& [BaseGameMode, GameModePlayTime] : PlayTimeByBaseGameMode)
	{
		if (GameModePlayTime.PlayTime > TimePlayedForMostPlayedDefaultMode)
		{
			MostPlayedDefaultGameMode = BaseGameMode;
			TimePlayedForMostPlayedDefaultMode = GameModePlayTime.PlayTime;
		}
	}
	if (MostPlayedDefaultGameMode != EBaseGameMode::None)
	{
		TextBlock_MostPlayedDefaultMode->SetText(BaseGameModeText[MostPlayedDefaultGameMode]);
		TextBlock_TimePlayedForMostPlayedDefaultModeLabel->SetText(FText::Format(TimeForFormat,
			BaseGameModeText[MostPlayedDefaultGameMode]));
		TextBlock_TimePlayedForMostPlayedDefaultMode->SetText(FormatTime(TimePlayedForMostPlayedDefaultMode));

		TextBlock_MostPlayedDefaultModeLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_MostPlayedDefaultMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_TimePlayedForMostPlayedDefaultModeLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_TimePlayedForMostPlayedDefaultMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TextBlock_MostPlayedDefaultMode->SetText(FText{});
		TextBlock_TimePlayedForMostPlayedDefaultModeLabel->SetText(FText{});
		TextBlock_TimePlayedForMostPlayedDefaultMode->SetText(FText{});

		TextBlock_MostPlayedDefaultModeLabel->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_MostPlayedDefaultMode->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_TimePlayedForMostPlayedDefaultModeLabel->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_TimePlayedForMostPlayedDefaultMode->SetVisibility(ESlateVisibility::Collapsed);
	}

	FString MostPlayedCustomGameModeName;
	float TimePlayedForMostPlayedCustomMode = 0.0f;
	for (const auto& [CustomGameModeName, GameModePlayTime] : PlayTimeByCustomGameModeName)
	{
		if (GameModePlayTime.PlayTime > TimePlayedForMostPlayedCustomMode)
		{
			MostPlayedCustomGameModeName = CustomGameModeName;
			TimePlayedForMostPlayedCustomMode = GameModePlayTime.PlayTime;
		}
	}
	if (!MostPlayedCustomGameModeName.IsEmpty())
	{
		const FText Text = FText::FromString(MostPlayedCustomGameModeName);
		TextBlock_MostPlayedCustomMode->SetText(Text);
		TextBlock_TimePlayedForMostPlayedCustomModeLabel->SetText(FText::Format(TimeForFormat, Text));
		TextBlock_TimePlayedForMostPlayedCustomMode->SetText(FormatTime(TimePlayedForMostPlayedCustomMode));

		TextBlock_MostPlayedCustomModeLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_MostPlayedCustomMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_TimePlayedForMostPlayedCustomModeLabel->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		TextBlock_TimePlayedForMostPlayedCustomMode->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TextBlock_MostPlayedCustomMode->SetText(FText{});
		TextBlock_TimePlayedForMostPlayedCustomModeLabel->SetText(FText{});
		TextBlock_TimePlayedForMostPlayedCustomMode->SetText(FText{});

		TextBlock_MostPlayedCustomModeLabel->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_MostPlayedCustomMode->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_TimePlayedForMostPlayedCustomModeLabel->SetVisibility(ESlateVisibility::Collapsed);
		TextBlock_TimePlayedForMostPlayedCustomMode->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UScoreViewerWidget::OnSelectionChanged_ScoreTable(const bool HasSelection)
{
	BSButton_DeleteSelectedScores->SetIsEnabled(HasSelection);
}

FText UScoreViewerWidget::HandlePlayFrequencyDisplayText(const int32 WeekIndex, const int32 DayOfWeekIndex)
{
	const float Value = PlayFrequencyData[WeekIndex][DayOfWeekIndex];
	const int32 DayIndex = WeekIndex * DaysInWeek + (DayOfWeekIndex - StartDow);
	const FDateTime Date = StartDate + FTimespan::FromDays(DayIndex);
	const FText DateText = FText::FromString(Date.ToFormattedString(TEXT("%Y-%m-%d")));
	return FText::Format(PlayFrequencyDisplayFormat, DateText, FormatTime(Value));
}

FText UScoreViewerWidget::HandlePlayFrequencyValueText(const int32 WeekIndex, int32, float)
{
	float Total = 0.f;
	for (const float CurrentValue : PlayFrequencyData[WeekIndex])
	{
		if (CurrentValue > 0.f)
		{
			Total += CurrentValue;
		}
	}

	return FText::Format(PlayFrequencyValueFormat, FormatTime(Total));
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
	return FormatTime(Value);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesDisplayText(const int32 Index)
{
	return FText::FromString(CustomGameModePlayTime[Index].CustomGameModeName);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesValueText(const int32 Index, const float Value)
{
	return FormatTime(Value);
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
	return FText::AsNumber(Value / 3600.f, &NumberFormattingOptions);
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

FText UScoreViewerWidget::FormatTime(const float Seconds)
{
	FNumberFormattingOptions NumberFormattingOptions;
	if (Seconds < 3600.f)
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		return FText::Format(SpaceSeparatedFormat, FText::AsNumber(Seconds / 60.f, &NumberFormattingOptions),
		                     FText::FromString("Minutes"));
	}
	else
	{
		NumberFormattingOptions.SetMaximumFractionalDigits(2);
		return FText::Format(SpaceSeparatedFormat, FText::AsNumber(Seconds / 3600.f, &NumberFormattingOptions),
		                     FText::FromString("Hours"));
	}
}
