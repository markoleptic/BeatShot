// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/ScoreTable.h"
#include "Internationalization/BreakIterator.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Slate/SPlayerScoreRow.h"

TSharedRef<SWidget> UScoreTable::RebuildWidget()
{
	const TSharedRef<SHeaderRow> Header = SNew(SHeaderRow)
		.Style(&HeaderRowStyle);
	Header->AddColumn(MakeColumn(SPlayerScoreRow::DateColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::GameModeColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::DifficultyColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::SongColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::ScoreColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::AccuracyColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::CompletionColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::StreakColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::ShotsFiredColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::TargetsHitColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::TargetsSpawnedColumnName));
	Header->AddColumn(MakeColumn(SPlayerScoreRow::ReactionTimeColumnName));

	SlateWidget = SNew(SListView<TSharedPtr<FPlayerScore>>)
		.ListItemsSource(&ListItems)
		.OnGenerateRow_UObject(this, &UScoreTable::OnGenerateRow)
		.SelectionMode(ESelectionMode::None)
		.HeaderRow(Header);

	return SlateWidget.ToSharedRef();
}

void UScoreTable::SetBaseGameModeText(const TMap<EBaseGameMode, FText>& InBaseGameModeText)
{
	BaseGameModeText = InBaseGameModeText;
}

void UScoreTable::SetDifficultyText(const TMap<EGameModeDifficulty, FText>& InGameModeDifficultyText)
{
	DifficultyText = InGameModeDifficultyText;
}

void UScoreTable::SetListItems(const TArray<TSharedPtr<FPlayerScore>>& InListItems)
{
	ListItems.Empty(InListItems.Num());
	for (const auto& Item : InListItems)
	{
		ListItems.Add(Item);
	}

	if (SlateWidget)
	{
		SortItems();
		SlateWidget->RequestListRefresh();
	}
}

void UScoreTable::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	if (SlateWidget)
	{
		SlateWidget.Reset();
	}
}

TSharedRef<ITableRow> UScoreTable::OnGenerateRow(TSharedPtr<FPlayerScore> Item,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return SNew(SPlayerScoreRow, OwnerTable)
		.Item(Item)
		.BaseGameModeTextMap(&BaseGameModeText)
		.DifficultyTextMap(&DifficultyText)
		.TableRowStyle(&TableRowStyle)
		.Font(TableCellFont);
}

void UScoreTable::OnSortColumn(EColumnSortPriority::Type InSortPriority, const FName& InColumnId,
	EColumnSortMode::Type InSortMode)
{
	PrimarySortedColumn = InColumnId;
	PrimarySortMode = InSortMode;

	if (SlateWidget)
	{
		SortItems();
		SlateWidget->RequestListRefresh();
	}
}

EColumnSortPriority::Type UScoreTable::GetColumnSortPriority(const FName InColumnId) const
{
	if (InColumnId == PrimarySortedColumn)
	{
		return EColumnSortPriority::Primary;
	}

	return EColumnSortPriority::Max;
}

EColumnSortMode::Type UScoreTable::GetSortModeForColumn(const FName InColumnId) const
{
	return InColumnId == PrimarySortedColumn ? PrimarySortMode : EColumnSortMode::None;
}

void UScoreTable::SortItems()
{
	if (PrimarySortedColumn.IsNone() || ListItems.Num() == 0)
	{
		return; // No column selected for sorting or nothing to sort.
	}

	auto CompareColumn = [this](const TSharedPtr<FPlayerScore>& L, const TSharedPtr<FPlayerScore>& R,
		const FName& Column)
	{
		if (Column == SPlayerScoreRow::DateColumnName)
		{
			FDateTime ParsedTimeL;
			FDateTime::ParseIso8601(*L->Time, ParsedTimeL);
			FDateTime ParsedTimeR;
			FDateTime::ParseIso8601(*R->Time, ParsedTimeR);
			return ParsedTimeL < ParsedTimeR;
		}

		if (Column == SPlayerScoreRow::GameModeColumnName)
		{
			FString TextL, TextR;
			if (L->DefiningConfig.GameModeType == EGameModeType::Preset)
			{
				TextL = BaseGameModeText[L->DefiningConfig.BaseGameMode].ToString();
			}
			else if (L->DefiningConfig.GameModeType == EGameModeType::Custom)
			{
				TextL = L->DefiningConfig.CustomGameModeName;
			}
			if (R->DefiningConfig.GameModeType == EGameModeType::Preset)
			{
				TextR = BaseGameModeText[R->DefiningConfig.BaseGameMode].ToString();
			}
			else if (R->DefiningConfig.GameModeType == EGameModeType::Custom)
			{
				TextR = R->DefiningConfig.CustomGameModeName;
			}
			return TextL < TextR;
		}

		if (Column == SPlayerScoreRow::DifficultyColumnName)
		{
			FString TextL, TextR;
			if (L->DefiningConfig.GameModeType == EGameModeType::Preset)
			{
				TextL = DifficultyText[L->DefiningConfig.Difficulty].ToString();
			}
			if (R->DefiningConfig.GameModeType == EGameModeType::Preset)
			{
				TextR = DifficultyText[R->DefiningConfig.Difficulty].ToString();
			}
			return TextL < TextR;
		}
		if (Column == SPlayerScoreRow::SongColumnName)
		{
			return L->SongTitle < R->SongTitle;
		}
		if (Column == SPlayerScoreRow::ScoreColumnName)
		{
			return L->Score < R->Score;
		}
		if (Column == SPlayerScoreRow::AccuracyColumnName)
		{
			return L->Accuracy < R->Accuracy;
		}
		if (Column == SPlayerScoreRow::CompletionColumnName)
		{
			return L->Completion < R->Completion;
		}
		if (Column == SPlayerScoreRow::StreakColumnName)
		{
			return L->Streak < R->Streak;
		}
		if (Column == SPlayerScoreRow::ShotsFiredColumnName)
		{
			return L->ShotsFired < R->ShotsFired;
		}
		if (Column == SPlayerScoreRow::TargetsHitColumnName)
		{
			return L->TargetsHit < R->TargetsHit;
		}
		if (Column == SPlayerScoreRow::TargetsSpawnedColumnName)
		{
			return L->TargetsSpawned < R->TargetsSpawned;
		}
		if (Column == SPlayerScoreRow::ReactionTimeColumnName)
		{
			return L->AvgTimeOffset < R->AvgTimeOffset;
		}
		return false;
	};


	ListItems.Sort([this, CompareColumn](const TSharedPtr<FPlayerScore>& A, const TSharedPtr<FPlayerScore>& B)
	{
		if (PrimarySortMode != EColumnSortMode::None)
		{
			const bool bLess = CompareColumn(A, B, PrimarySortedColumn);
			const bool bGreater = CompareColumn(B, A, PrimarySortedColumn);

			if (bLess != bGreater)
			{
				return PrimarySortMode == EColumnSortMode::Ascending ? bLess : bGreater;
			}
		}

		if (PrimarySortedColumn != SPlayerScoreRow::DateColumnName)
		{
			FDateTime ParsedTimeA;
			FDateTime::ParseIso8601(*A->Time, ParsedTimeA);
			FDateTime ParsedTimeB;
			FDateTime::ParseIso8601(*B->Time, ParsedTimeB);
			const bool bDateLess = ParsedTimeA < ParsedTimeB;
			const bool bDateGreater = ParsedTimeB < ParsedTimeA;

			if (bDateLess != bDateGreater)
			{
				return bDateLess; // usually ascending date
			}
		}

		return false;
	});
}

SHeaderRow::FColumn::FArguments UScoreTable::MakeColumn(const FName& InColumnName)
{
	return SHeaderRow::FColumn::FArguments{}.ColumnId(InColumnName).DefaultLabel(FText::FromName(InColumnName)).
	                                         SortMode_UObject(this, &UScoreTable::GetSortModeForColumn, InColumnName).
	                                         OnSort_UObject(this, &UScoreTable::OnSortColumn).HAlignHeader(
		                                         HAlign_Center).HeaderContent()[SNew(STextBlock)
	.Text(FText::FromName(InColumnName))
	.Font(HeaderFont)
	.AutoWrapText(true)
	.LineBreakPolicy(FBreakIterator::CreateCamelCaseBreakIterator())];
}
