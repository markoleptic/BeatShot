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

private:
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FPlayerScore> Item, const TSharedRef<STableViewBase>& OwnerTable);

	void OnSortColumn(EColumnSortPriority::Type, const FName&, EColumnSortMode::Type);

	EColumnSortPriority::Type GetColumnSortPriority(const FName InColumnId) const;

	EColumnSortMode::Type GetSortModeForColumn(const FName ColumnId) const;

	void SortItems();

	SHeaderRow::FColumn::FArguments MakeColumn(const FName& InColumnName);


	TSharedPtr<SListView<TSharedPtr<FPlayerScore>>> SlateWidget;

	TMap<EBaseGameMode, FText> BaseGameModeText;

	TMap<EGameModeDifficulty, FText> DifficultyText;

	TArray<TSharedPtr<FPlayerScore>> ListItems;

	FName PrimarySortedColumn = SPlayerScoreRow::DateColumnName;

	UPROPERTY(EditInstanceOnly)
	FTableRowStyle TableRowStyle = FCoreStyle::Get().GetWidgetStyle<FTableRowStyle>("TableView.Row");

	UPROPERTY(EditInstanceOnly)
	FHeaderRowStyle HeaderRowStyle = FCoreStyle::Get().GetWidgetStyle<FHeaderRowStyle>("TableView.Header");

	UPROPERTY(EditInstanceOnly)
	FSlateFontInfo HeaderFont;

	UPROPERTY(EditInstanceOnly)
	FSlateFontInfo TableCellFont;

	EColumnSortMode::Type PrimarySortMode = EColumnSortMode::Ascending;
};
