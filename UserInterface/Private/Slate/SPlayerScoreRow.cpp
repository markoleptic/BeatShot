// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "Slate/SPlayerScoreRow.h"
#include "SlateOptMacros.h"
#include "SaveGames/SaveGamePlayerScore.h"

const FName SPlayerScoreRow::DateColumnName = FName("Date");
const FName SPlayerScoreRow::GameModeColumnName = FName("GameMode");
const FName SPlayerScoreRow::DifficultyColumnName = FName("Difficulty");
const FName SPlayerScoreRow::SongColumnName = FName("Song");
const FName SPlayerScoreRow::ScoreColumnName = FName("Score");
const FName SPlayerScoreRow::AccuracyColumnName = FName("Accuracy");
const FName SPlayerScoreRow::CompletionColumnName = FName("Completion");
const FName SPlayerScoreRow::StreakColumnName = FName("Streak");
const FName SPlayerScoreRow::ShotsFiredColumnName = FName("ShotsFired");
const FName SPlayerScoreRow::TargetsHitColumnName = FName("TargetsHit");
const FName SPlayerScoreRow::TargetsSpawnedColumnName = FName("TargetsSpawned");
const FName SPlayerScoreRow::ReactionTimeColumnName = FName("ReactionTime");

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SPlayerScoreRow::Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView)
{
	Item = InArgs._Item;
	BaseGameModeTextMap = InArgs._BaseGameModeTextMap;
	DifficultyTextMap = InArgs._DifficultyTextMap;
	checkf(BaseGameModeTextMap, TEXT("BaseGameModeTextMap must not be null."));
	checkf(DifficultyTextMap, TEXT("DifficultyTextMap must not be null."));
	Font = InArgs._Font;
	FSuperRowType::Construct(FTableRowArgs().Style(InArgs._TableRowStyle), InOwnerTableView);
}

TSharedRef<SWidget> SPlayerScoreRow::GenerateWidgetForColumn(const FName& ColumnName)
{
	static const auto PercentFormatter = [](const float Value)
	{
		static const auto ZeroDecimalNumberFormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(1).
			SetMaximumFractionalDigits(1).SetMinimumIntegralDigits(2).SetMaximumIntegralDigits(3);
		static const FTextFormat PercentFormat = FTextFormat::FromString("{0}%");
		return FText::Format(PercentFormat, FText::AsNumber(Value * 100.f, &ZeroDecimalNumberFormattingOptions));
	};

	static auto ZeroDecimalTimesOneHundredFormatter = [](const float Value)
	{
		static const auto ZeroDecimalNumberFormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(0).
			SetMaximumFractionalDigits(0);
		return FText::AsNumber(Value * 100.f, &ZeroDecimalNumberFormattingOptions);
	};

	static auto ZeroDecimalFormatter = [](const float Value)
	{
		static const auto ZeroDecimalNumberFormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(0).
			SetMaximumFractionalDigits(0);
		return FText::AsNumber(Value, &ZeroDecimalNumberFormattingOptions);
	};

	if (ColumnName == DateColumnName)
	{
		const FText Text = FText::FromString(Item->LocalDateTime.ToFormattedString(TEXT("%Y-%m-%d, %I:%M%P")));
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(Text)
			];
	}
	else if (ColumnName == GameModeColumnName)
	{
		FText Text;
		if (Item->DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			Text = *BaseGameModeTextMap->Find(Item->DefiningConfig.BaseGameMode);
		}
		else if (Item->DefiningConfig.GameModeType == EGameModeType::Custom)
		{
			Text = FText::FromString(Item->DefiningConfig.CustomGameModeName);
		}
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(Text)
			];
	}
	else if (ColumnName == DifficultyColumnName)
	{
		FText Text;
		if (Item->DefiningConfig.GameModeType == EGameModeType::Preset)
		{
			Text = *DifficultyTextMap->Find(Item->DefiningConfig.Difficulty);
		}
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(Text)
			];
	}
	else if (ColumnName == SongColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(FText::FromString(Item->SongTitle))
			];
	}
	else if (ColumnName == ScoreColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalFormatter(Item->Score))
			];
	}
	else if (ColumnName == AccuracyColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(PercentFormatter(Item->Accuracy))
			];
	}
	else if (ColumnName == CompletionColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(PercentFormatter(Item->Completion))
			];
	}
	else if (ColumnName == StreakColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalFormatter(Item->Streak))
			];
	}
	else if (ColumnName == ShotsFiredColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalFormatter(Item->ShotsFired))
			];
	}
	else if (ColumnName == TargetsHitColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalFormatter(Item->TargetsHit))
			];
	}
	else if (ColumnName == TargetsSpawnedColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalFormatter(Item->TargetsSpawned))
			];
	}
	else if (ColumnName == ReactionTimeColumnName)
	{
		return SNew(SBox)
			.Padding(FMargin(4.0, 0.0))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Font(Font)
				.Text(ZeroDecimalTimesOneHundredFormatter(Item->AvgTimeOffset))
			];
	}
	return SNullWidget::NullWidget;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
