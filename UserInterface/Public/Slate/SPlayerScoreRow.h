// Copyright 2022-2025 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Widgets/Views/STableRow.h"

using FBaseGameModeTextMap = TMap<EBaseGameMode, FText>;
using FDifficultyTextMap = TMap<EGameModeDifficulty, FText>;

class USERINTERFACE_API SPlayerScoreRow : public SMultiColumnTableRow<TSharedPtr<FPlayerScore>>
{
public:
	SLATE_BEGIN_ARGS(SPlayerScoreRow)
		{
		}

		SLATE_ARGUMENT(TSharedPtr<FPlayerScore>, Item)
		SLATE_ARGUMENT(FBaseGameModeTextMap*, BaseGameModeTextMap)
		SLATE_ARGUMENT(FDifficultyTextMap*, DifficultyTextMap)
		SLATE_STYLE_ARGUMENT(FTableRowStyle, TableRowStyle)
		SLATE_ARGUMENT(FSlateFontInfo, Font)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs, const TSharedRef<STableViewBase>& InOwnerTableView);

	virtual TSharedRef<SWidget> GenerateWidgetForColumn(const FName& ColumnName) override;

private:
	TSharedPtr<FPlayerScore> Item;
	FBaseGameModeTextMap* BaseGameModeTextMap = nullptr;
	FDifficultyTextMap* DifficultyTextMap = nullptr;
	FSlateFontInfo Font = FCoreStyle::GetDefaultFontStyle("Regular", 24);

public:
	static const FName DateColumnName;
	static const FName GameModeColumnName;
	static const FName DifficultyColumnName;
	static const FName SongColumnName;
	static const FName ScoreColumnName;
	static const FName AccuracyColumnName;
	static const FName CompletionColumnName;
	static const FName StreakColumnName;
	static const FName ShotsFiredColumnName;
	static const FName TargetsHitColumnName;
	static const FName TargetsSpawnedColumnName;
	static const FName ReactionTimeColumnName;
};
