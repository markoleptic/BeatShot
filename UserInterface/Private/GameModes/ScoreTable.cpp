// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.


#include "GameModes/ScoreTable.h"
#include "Fonts/FontMeasure.h"
#include "Internationalization/BreakIterator.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Slate/SPlayerScoreRow.h"

TSharedRef<SWidget> UScoreTable::RebuildWidget()
{
	if (HeaderColumnTextWidth.IsEmpty())
	{
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->
			GetFontMeasureService();
		for (const auto& [Name, Text] : HeaderColumnText)
		{
			HeaderColumnTextWidth.Add(Name, FontMeasure->Measure(Text, HeaderFont).X);
		}
		for (const auto& [BaseGameMode, Text] : BaseGameModeText)
		{
			BaseGameModeCellTextWidth.Add(BaseGameMode, FontMeasure->Measure(Text, TableCellFont).X);
		}
		for (const auto& [Difficulty, Text] : DifficultyText)
		{
			DifficultyCellTextWidth.Add(Difficulty, FontMeasure->Measure(Text, TableCellFont).X);
		}
		{
			const auto TestText = FText::FromString("8888-88-88, 88:88PM");
			DateCellTextWidth = FontMeasure->Measure(TestText, TableCellFont).X;
		}
		{
			static auto FormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(0).
			                                                           SetMaximumFractionalDigits(0).
			                                                           SetMinimumIntegralDigits(6).
			                                                           SetMaximumIntegralDigits(6);
			const auto TestText = FText::AsNumber(888888, &FormattingOptions);
			ScoreCellTextWidth = FontMeasure->Measure(TestText, TableCellFont).X;
		}
		{
			static const FTextFormat PercentFormat = FTextFormat::FromString("{0}%");
			static auto FormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(1).
			                                                           SetMaximumFractionalDigits(1).
			                                                           SetMinimumIntegralDigits(2).
			                                                           SetMaximumIntegralDigits(3);
			const auto TestText = FText::Format(PercentFormat, FText::AsNumber(100.0, &FormattingOptions));
			PercentCellTextWidth = FontMeasure->Measure(TestText, TableCellFont).X;
		}
		{
			static auto FormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(0).
			                                                           SetMaximumFractionalDigits(0);
			const auto TestText = FText::AsNumber(8888, &FormattingOptions);
			CounterCellTextWidth = FontMeasure->Measure(TestText, TableCellFont).X;
		}
		{
			static auto FormattingOptions = FNumberFormattingOptions().SetMinimumFractionalDigits(0).
			                                                           SetMaximumFractionalDigits(0);
			const auto TestText = FText::AsNumber(888888.f, &FormattingOptions);
			ReactionTimeCellTextWidth = FontMeasure->Measure(TestText, TableCellFont).X;
		}
	}

	Header = SNew(SHeaderRow)
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
		.SelectionMode(ESelectionMode::Type::Multi)
		.HeaderRow(Header)
		.OnSelectionChanged_UObject(this, &UScoreTable::OnSelectionChanged);

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
		const TSharedRef<FSlateFontMeasure> FontMeasure = FSlateApplication::Get().GetRenderer()->
			GetFontMeasureService();
		const auto& Columns = Header->GetColumns();
		for (int i = 0; i < Columns.Num(); i++)
		{
			const FName& ColumnName = Columns[i].ColumnId;
			float Width = HeaderColumnTextWidth[ColumnName];

			if (ColumnName == SPlayerScoreRow::DateColumnName)
			{
				Width = FMath::Max(Width, DateCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::ScoreColumnName)
			{
				Width = FMath::Max(Width, ScoreCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::AccuracyColumnName)
			{
				Width = FMath::Max(Width, PercentCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::CompletionColumnName)
			{
				Width = FMath::Max(Width, PercentCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::StreakColumnName)
			{
				Width = FMath::Max(Width, CounterCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::ShotsFiredColumnName)
			{
				Width = FMath::Max(Width, CounterCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::TargetsHitColumnName)
			{
				Width = FMath::Max(Width, CounterCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::TargetsSpawnedColumnName)
			{
				Width = FMath::Max(Width, CounterCellTextWidth);
			}
			else if (ColumnName == SPlayerScoreRow::ReactionTimeColumnName)
			{
				Width = FMath::Max(Width, ReactionTimeCellTextWidth);
			}
			else
			{
				FText Text;
				for (const auto& Item : InListItems)
				{
					if (ColumnName == SPlayerScoreRow::GameModeColumnName)
					{
						if (Item->DefiningConfig.GameModeType == EGameModeType::Preset)
						{
							Text = *BaseGameModeText.Find(Item->DefiningConfig.BaseGameMode);
						}
						else if (Item->DefiningConfig.GameModeType == EGameModeType::Custom)
						{
							Text = FText::FromString(Item->DefiningConfig.CustomGameModeName);
						}
					}
					else if (ColumnName == SPlayerScoreRow::DifficultyColumnName)
					{
						if (Item->DefiningConfig.GameModeType == EGameModeType::Preset)
						{
							Text = *DifficultyText.Find(Item->DefiningConfig.Difficulty);
						}
					}
					else if (ColumnName == SPlayerScoreRow::SongColumnName)
					{
						Text = FText::FromString(Item->SongTitle);
					}
					Width = FMath::Max(FontMeasure->Measure(Text, TableCellFont).X, Width);
				}
			}

			Header->SetColumnWidth(ColumnName, Width + 10.f);
		}

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

TArray<TSharedPtr<FPlayerScore>> UScoreTable::GetSelectedItems() const
{
	return SlateWidget ? SlateWidget->GetSelectedItems() : TArray<TSharedPtr<FPlayerScore>>();
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

void UScoreTable::OnSortColumn(EColumnSortPriority::Type,
                               const FName& InColumnId,
                               const EColumnSortMode::Type InSortMode)
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

	auto CompareColumn = [this](const TSharedPtr<FPlayerScore>& L,
	                            const TSharedPtr<FPlayerScore>& R,
	                            const FName& Column)
	{
		if (Column == SPlayerScoreRow::DateColumnName)
		{
			return L->LocalDateTime < R->LocalDateTime;
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
			const bool bDateLess = A->LocalDateTime < B->LocalDateTime;
			const bool bDateGreater = B->LocalDateTime < A->LocalDateTime;

			if (bDateLess != bDateGreater)
			{
				return bDateLess; // usually ascending date
			}
		}

		return false;
	});
}

void UScoreTable::OnSelectionChanged(TSharedPtr<FPlayerScore>, ESelectInfo::Type)
{
	OnSelectionChangedDelegate.ExecuteIfBound(!SlateWidget->GetSelectedItems().IsEmpty());
}

SHeaderRow::FColumn::FArguments UScoreTable::MakeColumn(const FName& InColumnName)
{
	return SHeaderRow::FColumn::FArguments{}.ColumnId(InColumnName).HAlignHeader(HAlign_Center).
	                                         SortMode_UObject(this, &UScoreTable::GetSortModeForColumn, InColumnName).
	                                         OnSort_UObject(this, &UScoreTable::OnSortColumn).HeaderContent()[SNew(
		STextBlock)
	.Text(HeaderColumnText[InColumnName])
	.Font(HeaderFont)
	.AutoWrapText(false)];
}
