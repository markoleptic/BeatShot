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
#include "Mappings/GameModeAndDifficultyStringMap.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/BSWidgetInterface.h"
#include "Utilities/Buttons/MenuButton.h"
#include "Widgets/BarChartWidget.h"
#include "Widgets/HeatMapWidget.h"

namespace
{
TArray<FPlayerScore> GenerateTestScores(int32 NumEntries)
{
	TArray<FPlayerScore> Scores;
	Scores.Reserve(NumEntries);

	const TArray<FString> Songs = {"Neon Skyline", "Digital Pulse", "Starlight Drive", "Velocity", "Arc Reactor"};

	const TArray SongLengths = {71.f, 88.9f, 150.f, 200.f, 360.f,};

	const TArray BaseModes = {
		EBaseGameMode::SingleBeat,
		EBaseGameMode::MultiBeat,
		EBaseGameMode::BeatGrid,
		EBaseGameMode::BeatTrack
	};

	const TArray Difficulties = {
		EGameModeDifficulty::Normal,
		EGameModeDifficulty::Hard,
		EGameModeDifficulty::Death
	};

	const FDateTime Now = FDateTime::UtcNow();
	const FTimespan UtcOffset = Now - FDateTime::UtcNow();

	for (int32 i = 0; i < 50; ++i)
	{
		FPlayerScore Score;

		// ----- Defining Config -----
		Score.DefiningConfig.GameModeType = EGameModeType::Custom;
		Score.DefiningConfig.CustomGameModeName = FString::Printf(TEXT("CustomMode_%d"), 1);
		Score.DefiningConfig.BaseGameMode = BaseModes[0];
		Score.SongTitle = Songs[0];
		Score.SongLength = SongLengths[0];

		Score.TargetsSpawned = FMath::RandRange(50, 300);
		Score.TargetsHit = FMath::RandRange(FMath::RoundToInt(Score.TargetsSpawned * 0.5f), Score.TargetsSpawned);
		Score.ShotsFired = Score.TargetsHit + FMath::RandRange(0, 80);
		Score.Accuracy = Score.ShotsFired > 0 ? static_cast<float>(Score.TargetsHit) / Score.ShotsFired : 0.f;
		Score.Completion = Score.TargetsSpawned > 0 ? static_cast<float>(Score.TargetsHit) / Score.TargetsSpawned : 0.f;

		Score.Score = FMath::RandRange(30000.f, 99999.f);
		Score.Streak = FMath::RandRange(1, Score.TargetsHit);
		Score.AvgTimeOffset = FMath::RandRange(0.02f, 0.45f);
		Score.TotalTimeOffset = Score.TargetsHit * Score.AvgTimeOffset;


		// ----- Tracking Mode Stats -----
		if (Score.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack || Score.DefiningConfig.BaseGameMode ==
		    EBaseGameMode::ChargedBeatTrack)
		{
			Score.TotalPossibleDamage = FMath::RandRange(200.f, 1000.f);
		}

		// ----- Timestamp -----
		FDateTime SessionTime = Now - FTimespan::FromMinutes(FMath::RandRange(0, 60 * 24 * 30));
		Score.Time = SessionTime.ToIso8601();
		Score.LocalDateTime = SessionTime + UtcOffset;

		Scores.Add(MoveTemp(Score));
	}
	
	
	for (int32 i = 0; i < NumEntries; ++i)
	{
		FPlayerScore Score;

		// ----- Defining Config -----
		Score.DefiningConfig.GameModeType = FMath::RandBool() ? EGameModeType::Preset : EGameModeType::Custom;

		if (Score.DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			Score.DefiningConfig.BaseGameMode = BaseModes[FMath::RandRange(0, BaseModes.Num() - 1)];

			Score.DefiningConfig.Difficulty = Difficulties[FMath::RandRange(0, Difficulties.Num() - 1)];
		}
		else
		{
			Score.DefiningConfig.CustomGameModeName = FString::Printf(TEXT("CustomMode_%d"), FMath::RandRange(1, 5));

			Score.DefiningConfig.BaseGameMode = BaseModes[FMath::RandRange(0, BaseModes.Num() - 1)];
		}

		// ----- Song -----
		int SongIndex = FMath::RandRange(0, Songs.Num() - 1);
		Score.SongTitle = Songs[SongIndex];

		Score.SongLength = SongLengths[SongIndex];

		// ----- Gameplay Stats -----
		Score.TargetsSpawned = FMath::RandRange(50, 300);
		Score.TargetsHit = FMath::RandRange(FMath::RoundToInt(Score.TargetsSpawned * 0.5f), Score.TargetsSpawned);
		Score.ShotsFired = Score.TargetsHit + FMath::RandRange(0, 80);
		Score.Accuracy = Score.ShotsFired > 0 ? static_cast<float>(Score.TargetsHit) / Score.ShotsFired : 0.f;
		Score.Completion = Score.TargetsSpawned > 0 ? static_cast<float>(Score.TargetsHit) / Score.TargetsSpawned : 0.f;

		Score.Score = FMath::RandRange(30000.f, 99999.f);
		Score.Streak = FMath::RandRange(1, Score.TargetsHit);
		Score.AvgTimeOffset = FMath::RandRange(0.02f, 0.45f);
		Score.TotalTimeOffset = Score.TargetsHit * Score.AvgTimeOffset;


		// ----- Tracking Mode Stats -----
		if (Score.DefiningConfig.BaseGameMode == EBaseGameMode::BeatTrack || Score.DefiningConfig.BaseGameMode ==
		    EBaseGameMode::ChargedBeatTrack)
		{
			Score.TotalPossibleDamage = FMath::RandRange(200.f, 1000.f);
		}

		// ----- Timestamp -----
		FDateTime SessionTime = Now - FTimespan::FromMinutes(FMath::RandRange(0, 60 * 24 * 30));
		Score.Time = SessionTime.ToIso8601();
		Score.LocalDateTime = SessionTime + UtcOffset;

		Scores.Add(MoveTemp(Score));
	}

	return Scores;
}

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
EGameModePlayTimeUnits SortAndUpdateGameModePlayTimeData(const TMap<T, FGameModePlayTime>& PlayTimeMap,
                                                         TArray<FGameModePlayTime>& PlayTimeArray,
                                                         TArray<float>& Points)
{
	PlayTimeArray.Empty(PlayTimeMap.Num());
	float MaxPlayTime = 0;
	for (const auto& [Key, GameModePlayTime] : PlayTimeMap)
	{
		PlayTimeArray.Add(GameModePlayTime);
		MaxPlayTime = FMath::Max(MaxPlayTime, GameModePlayTime.PlayTime);
	}

	SortGameModePlayTime(PlayTimeArray);
	EGameModePlayTimeUnits GameModePlayTimeUnits;
	float DivideBy;
	if (MaxPlayTime < 3600.f)
	{
		DivideBy = 60.f;
		GameModePlayTimeUnits = EGameModePlayTimeUnits::Minutes;
	}
	else
	{
		DivideBy = 3600.f;
		GameModePlayTimeUnits = EGameModePlayTimeUnits::Hours;
	}
	Points.Empty(PlayTimeArray.Num());
	for (const auto& GameModePlayTime : PlayTimeArray)
	{
		Points.Add(GameModePlayTime.PlayTime / DivideBy);
	}
	return GameModePlayTimeUnits;
}

int32 ComputeLabelPrecision(const double Step)
{
	if (Step <= 0.0)
	{
		return 0;
	}

	return FMath::Max(0, static_cast<int32>(FMath::CeilToInt(-FMath::LogX(10.0, Step))));
}

FText FormatTime(const float Seconds, TOptional<EGameModePlayTimeUnits> Units = TOptional<EGameModePlayTimeUnits>())
{
	FNumberFormattingOptions NumberFormattingOptions;
	static const FTextFormat SpaceSeparatedFormat = FTextFormat::FromString("{0} {1}");
	if (Units)
	{
		FText Text;
		switch (Units.GetValue())
		{
		case EGameModePlayTimeUnits::Minutes:
			{
				NumberFormattingOptions.SetMaximumFractionalDigits(1);
				const float Minutes = Seconds;
				Text = FText::Format(SpaceSeparatedFormat, FText::AsNumber(Minutes, &NumberFormattingOptions),
				                     IBSWidgetInterface::GetWidgetTextFromKey("Scores_Minutes"));
			}
			break;
		case EGameModePlayTimeUnits::Hours:
			{
				NumberFormattingOptions.SetMaximumFractionalDigits(2);
				const float Hours = Seconds;
				Text = FText::Format(SpaceSeparatedFormat, FText::AsNumber(Hours, &NumberFormattingOptions),
				                     IBSWidgetInterface::GetWidgetTextFromKey("Scores_Hours"));
			}
			break;
		}
		return Text;
	}
	else
	{
		if (Seconds < 3600.f)
		{
			NumberFormattingOptions.SetMaximumFractionalDigits(1);
			return FText::Format(SpaceSeparatedFormat, FText::AsNumber(Seconds / 60.f, &NumberFormattingOptions),
			                     IBSWidgetInterface::GetWidgetTextFromKey("Scores_Minutes"));
		}
		else
		{
			NumberFormattingOptions.SetMaximumFractionalDigits(2);
			return FText::Format(SpaceSeparatedFormat, FText::AsNumber(Seconds / 3600.f, &NumberFormattingOptions),
			                     IBSWidgetInterface::GetWidgetTextFromKey("Scores_Hours"));
		}
	}
}
}

void UScoreViewerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ScoreTable->OnSelectionChangedDelegate.BindUObject(this, &UScoreViewerWidget::OnSelectionChanged_ScoreTable);
	MenuButton_Overview->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_DefaultModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_CustomModes->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	MenuButton_History->OnBSButtonPressed.AddUObject(this, &ThisClass::OnButtonClicked_MenuButton);
	BSButton_DeleteSelectedScores->OnBSButtonPressed.AddUObject(
		this, &ThisClass::OnButtonClicked_DeleteSelectedScoresButton);
	BSButton_DeleteSelectedScores->SetIsEnabled(false);

	MenuButton_Overview->SetDefaults(Box_Overview, MenuButton_DefaultModes);
	MenuButton_DefaultModes->SetDefaults(DefaultGameModeScoreViewerWidget, MenuButton_CustomModes);
	MenuButton_CustomModes->SetDefaults(CustomGameModeScoreViewerWidget, MenuButton_History);
	MenuButton_History->SetDefaults(Box_History, MenuButton_Overview);

	NormalizedPlayFrequencyData = MakeShared<TArray<TArray<float>>>();
	PlayFrequency->GetOptions()->DrawSectionIfValueLessThanZero = false;
	PlayFrequency->GetOptions()->UseCustomTwoColorInterpolation = true;
	PlayFrequency->GetOptions()->UseSectionSpacing = false;
	PlayFrequency->GetOptions()->AxisOptions[EAxisType::Y].DrawIndices = TSet{0, 1, 2, 3, 4, 5, 6};
	PlayFrequency->GetOptions()->AxisOptions[EAxisType::X].DrawIndices = TSet<int32>{};

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
			PlayFrequency->GetOptions()->AxisOptions[EAxisType::X].DrawIndices->Add(WeekIndex);
		}
	}

	PlayFrequency->GetOptions()->AxisOptions[EAxisType::X].AxisLabelGenerator = TDelegate<FText
		(int32, float)>::CreateLambda([MonthsOfYear](const int32 WeekIndex, float)
	{
		return MonthsOfYear[WeekIndex];
	});
	PlayFrequency->GetOptions()->AxisOptions[EAxisType::Y].AxisLabelGenerator = TDelegate<FText
		(int32, float)>::CreateLambda([DaysOfWeek](const int32 DayOfWeekIndex, float)
		{
			return DaysOfWeek[DayOfWeekIndex];
		});
	PlayFrequency->GetOptions()->TooltipTitleTextGenerator = TDelegate<FText (int32, int32)>::CreateUObject(
		this, &ThisClass::HandlePlayFrequencyDisplayText);
	PlayFrequency->GetOptions()->TooltipTextGenerator = TDelegate<FText(int32, int32, float)>::CreateUObject(
		this, &ThisClass::HandlePlayFrequencyValueText);
	PlayFrequency->SetData(NormalizedPlayFrequencyData);

	MostPlayedDefaultGameModesData = MakeShared<TArray<float>>();
	{
		auto& XAxisOptions = MostPlayedDefaultGameModes->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedDefaultGameModesXAxisFormatter);

		auto& YAxisOptions = MostPlayedDefaultGameModes->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisGridOptions.MinGridLineCount = 3;
		YAxisOptions.AxisLabelGenerator = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedDefaultGameModesYAxisFormatter);
	}

	MostPlayedCustomGameModesData = MakeShared<TArray<float>>();
	{
		auto& XAxisOptions = MostPlayedCustomGameModes->GetOptions()->AxisOptions[EAxisType::X];
		XAxisOptions.AxisGridOptions.StartAtZero = false;
		XAxisOptions.AxisLabelGenerator = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedCustomGameModesXAxisFormatter);

		auto& YAxisOptions = MostPlayedCustomGameModes->GetOptions()->AxisOptions[EAxisType::Y];
		YAxisOptions.AxisGridOptions.StartAtZero = true;
		YAxisOptions.AxisGridOptions.MinGridLineCount = 3;
		YAxisOptions.AxisLabelGenerator = TDelegate<FText(int32, float)>::CreateUObject(
			this, &ThisClass::HandleMostPlayedCustomGameModesYAxisFormatter);
	}

	MostPlayedDefaultGameModes->GetOptions()->TooltipTitleTextGenerator = TDelegate<FText(int32)>::CreateUObject(
		this, &ThisClass::HandleMostPlayedDefaultGameModesDisplayText);
	MostPlayedDefaultGameModes->GetOptions()->TooltipTextGenerator = TDelegate<FText(int32, float)>::CreateUObject(
		this, &ThisClass::HandleMostPlayedDefaultGameModesValueText);
	MostPlayedDefaultGameModes->SetData(MostPlayedDefaultGameModesData, TDelegate<void(float)>::CreateLambda(
		                                    [this](const float Value)
		                                    {
			                                    DefaultGameModePrecision = ComputeLabelPrecision(Value);
		                                    }));

	MostPlayedCustomGameModes->GetOptions()->TooltipTitleTextGenerator = TDelegate<FText(int32)>::CreateUObject(
		this, &ThisClass::HandleMostPlayedCustomGameModesDisplayText);
	MostPlayedCustomGameModes->GetOptions()->TooltipTextGenerator = TDelegate<FText(int32, float)>::CreateUObject(
		this, &ThisClass::HandleMostPlayedCustomGameModesValueText);
	MostPlayedCustomGameModes->SetData(MostPlayedCustomGameModesData, TDelegate<void(float)>::CreateLambda(
		                                   [this](const float Value)
	                                   {
		                                   CustomGameModePrecision = ComputeLabelPrecision(Value);
	                                   }));

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

	for (const auto& Score : GenerateTestScores(500))
	{
		SaveGamePlayerScore->AddPlayerScoreInstance(Score);
	}

	*NormalizedPlayFrequencyData = InitialPlayFrequencyData;
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
	const auto SelectedItems = ScoreTable->GetSelectedItems();
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

	DefaultGameModePlayTimeUnits = SortAndUpdateGameModePlayTimeData(PlayTimeByBaseGameMode, DefaultGameModePlayTime,
	                                                                 *MostPlayedDefaultGameModesData);
	switch (DefaultGameModePlayTimeUnits)
	{
	case EGameModePlayTimeUnits::Minutes:
		MostPlayedDefaultGameModes->GetOptions()->AxisOptions[EAxisType::Y].AxisTitle =
			IBSWidgetInterface::GetWidgetTextFromKey("Scores_TotalTimePlayedMinutes");
		break;
	case EGameModePlayTimeUnits::Hours:
		MostPlayedDefaultGameModes->GetOptions()->AxisOptions[EAxisType::Y].AxisTitle =
			IBSWidgetInterface::GetWidgetTextFromKey("Scores_TotalTimePlayedHours");
		break;
	}

	CustomGameModePlayTimeUnits = SortAndUpdateGameModePlayTimeData(PlayTimeByCustomGameModeName,
	                                                                CustomGameModePlayTime,
	                                                                *MostPlayedCustomGameModesData);
	switch (CustomGameModePlayTimeUnits)
	{
	case EGameModePlayTimeUnits::Minutes:
		MostPlayedCustomGameModes->GetOptions()->AxisOptions[EAxisType::Y].AxisTitle =
			IBSWidgetInterface::GetWidgetTextFromKey("Scores_TotalTimePlayedMinutes");
		break;
	case EGameModePlayTimeUnits::Hours:
		MostPlayedCustomGameModes->GetOptions()->AxisOptions[EAxisType::Y].AxisTitle =
			IBSWidgetInterface::GetWidgetTextFromKey("Scores_TotalTimePlayedHours");
		break;
	}

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
		auto& RelativePlayFrequencySections = *NormalizedPlayFrequencyData.Get();
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

	static const FTextFormat TimeForFormat = FTextFormat(IBSWidgetInterface::GetWidgetTextFromKey("Scores_TimeFor"));

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
		TextBlock_MostPlayedDefaultMode->SetText(
			GameModeAndDifficultyStringMap->FindTextFromEnum(MostPlayedDefaultGameMode));
		TextBlock_TimePlayedForMostPlayedDefaultModeLabel->SetText(FText::Format(TimeForFormat,
			GameModeAndDifficultyStringMap->FindTextFromEnum(MostPlayedDefaultGameMode)));
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
	static const FTextFormat PlayFrequencyDisplayFormat = FTextFormat::FromString("{0}\n{1}");
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
	static const FTextFormat PlayFrequencyValueFormat = FTextFormat(
		IBSWidgetInterface::GetWidgetTextFromKey("Scores_TotalForWeek"));
	return FText::Format(PlayFrequencyValueFormat, FormatTime(Total));
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesDisplayText(const int32 Index)
{
	if (DefaultGameModePlayTime.IsValidIndex(Index))
	{
		return GameModeAndDifficultyStringMap->FindTextFromEnum(DefaultGameModePlayTime[Index].BaseGameMode);
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesValueText(int32, const float Value)
{
	return FormatTime(Value, DefaultGameModePlayTimeUnits);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesDisplayText(const int32 Index)
{
	return FText::FromString(CustomGameModePlayTime[Index].CustomGameModeName);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesValueText(const int32 /*Index*/, const float Value)
{
	return FormatTime(Value, CustomGameModePlayTimeUnits);
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesXAxisFormatter(const int32 Index, float)
{
	if (DefaultGameModePlayTime.IsValidIndex(Index))
	{
		return GameModeAndDifficultyStringMap->FindTextFromEnum(DefaultGameModePlayTime[Index].BaseGameMode);
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedDefaultGameModesYAxisFormatter(int32, const float Value)
{
	FNumberFormattingOptions NumberFormattingOptions;

	switch (DefaultGameModePlayTimeUnits)
	{
	case EGameModePlayTimeUnits::Minutes:
		NumberFormattingOptions.SetMaximumFractionalDigits(0);
		break;
	case EGameModePlayTimeUnits::Hours:
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		break;
	}

	return FText::AsNumber(Value, &NumberFormattingOptions);
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesXAxisFormatter(const int32 Index, float)
{
	if (CustomGameModePlayTime.IsValidIndex(Index))
	{
		return FText::FromString(CustomGameModePlayTime[Index].CustomGameModeName);
	}
	return {};
}

FText UScoreViewerWidget::HandleMostPlayedCustomGameModesYAxisFormatter(const int32 /*Index*/, const float Value)
{
	FNumberFormattingOptions NumberFormattingOptions;

	switch (CustomGameModePlayTimeUnits)
	{
	case EGameModePlayTimeUnits::Minutes:
		NumberFormattingOptions.SetMaximumFractionalDigits(CustomGameModePrecision);
		break;
	case EGameModePlayTimeUnits::Hours:
		NumberFormattingOptions.SetMaximumFractionalDigits(1);
		break;
	}

	return FText::AsNumber(Value, &NumberFormattingOptions);
}


