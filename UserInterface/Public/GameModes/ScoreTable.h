// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Slate/SPlayerScoreRow.h"
#include "Widgets/Views/SListView.h"
#include "ScoreTable.generated.h"


UCLASS(BlueprintType)
class USERINTERFACE_API UScoreTable : public UWidget
{
	GENERATED_BODY()

public:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	void SetBaseGameModeText(const TMap<EBaseGameMode, FText>& InBaseGameModeText);

	void SetDifficultyText(const TMap<EGameModeDifficulty, FText>& InGameModeDifficultyText);

	void SetListItems(const TArray<TSharedPtr<FPlayerScore>>& InListItems);

	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	TArray<TSharedPtr<FPlayerScore>> GetSelectedItems() const;

	TDelegate<void(bool)> OnSelectionChangedDelegate;

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FPlayerScore> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnSortColumn(EColumnSortPriority::Type, const FName&, EColumnSortMode::Type);

	EColumnSortPriority::Type GetColumnSortPriority(const FName InColumnId) const;

	EColumnSortMode::Type GetSortModeForColumn(const FName InColumnId) const;

	void SortItems();

	void OnSelectionChanged(TSharedPtr<FPlayerScore> Selection, ESelectInfo::Type SelectInfo);

	SHeaderRow::FColumn::FArguments MakeColumn(const FName& InColumnName);

	TSharedPtr<SListView<TSharedPtr<FPlayerScore>>> SlateWidget;

	TSharedPtr<SHeaderRow> Header;

	TMap<EBaseGameMode, FText> BaseGameModeText;

	TMap<EGameModeDifficulty, FText> DifficultyText;

	TArray<TSharedPtr<FPlayerScore>> ListItems;

	FName PrimarySortedColumn = SPlayerScoreRow::DateColumnName;

	UPROPERTY(EditAnywhere)
	FTableRowStyle TableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");

	UPROPERTY(EditAnywhere)
	FHeaderRowStyle HeaderRowStyle = FCoreStyle::Get().GetWidgetStyle<FHeaderRowStyle>("TableView.Header");

	UPROPERTY(EditAnywhere)
	FSlateFontInfo HeaderFont;

	UPROPERTY(EditAnywhere)
	FSlateFontInfo TableCellFont;

	UPROPERTY(EditAnywhere)
	TMap<FName, FText> HeaderColumnText = {
		{SPlayerScoreRow::DateColumnName, FText()}, {SPlayerScoreRow::GameModeColumnName, FText()},
		{SPlayerScoreRow::DifficultyColumnName, FText()}, {SPlayerScoreRow::SongColumnName, FText()},
		{SPlayerScoreRow::ScoreColumnName, FText()}, {SPlayerScoreRow::AccuracyColumnName, FText()},
		{SPlayerScoreRow::CompletionColumnName, FText()}, {SPlayerScoreRow::StreakColumnName, FText()},
		{SPlayerScoreRow::ShotsFiredColumnName, FText()}, {SPlayerScoreRow::TargetsHitColumnName, FText()},
		{SPlayerScoreRow::TargetsSpawnedColumnName, FText()}, {SPlayerScoreRow::ReactionTimeColumnName, FText()},
	};

	EColumnSortMode::Type PrimarySortMode = EColumnSortMode::Ascending;

	TMap<FName, float> HeaderColumnTextWidth;
	TMap<EBaseGameMode, float> BaseGameModeCellTextWidth;
	TMap<EGameModeDifficulty, float> DifficultyCellTextWidth;
	float DateCellTextWidth = 0.0f;
	float ScoreCellTextWidth = 0.0f;
	float PercentCellTextWidth = 0.0f;
	float CounterCellTextWidth = 0.0f;
	float ReactionTimeCellTextWidth = 0.0f;
};
