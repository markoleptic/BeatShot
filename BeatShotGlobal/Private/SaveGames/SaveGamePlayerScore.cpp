// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/SaveLoadCommon.h"

namespace
{
/** Prints 5x5 accuracy to log/console.
 *  @param InDefiningConfig defining config
 * 	@param InCommonScoreInfo CommonScoreInfo to print out
 * 	@param Options number formatting options
 */
void PrintAccuracy(const FBS_DefiningConfig& InDefiningConfig,
                   const FCommonScoreInfo& InCommonScoreInfo,
                   const FNumberFormattingOptions& Options)
{
	int32 TotalSpawns = 0;
	int32 TotalHits = 0;
	FString GameModeString;
	FString Line;

	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " +
		                 UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
	}
	else
	{
		GameModeString = InDefiningConfig.CustomGameModeName;
	}

	UE_LOG(LogTemp, Display, TEXT("Cumulative Accuracy for %s:"), *GameModeString);

	for (const FAccuracyRow& AccuracyRow : InCommonScoreInfo.AccuracyData.AccuracyRows)
	{
		for (int i = 0; i < AccuracyRow.Size; i++)
		{
			TotalHits += AccuracyRow.TotalHits[i];
			if (AccuracyRow.TotalSpawns[i] > 0)
			{
				TotalSpawns += AccuracyRow.TotalSpawns[i];
			}
			const float Value = AccuracyRow.Accuracy[i];

			FString PercentString = (Value == -1.f) ? "XXX%" : FText::AsPercent(Value, &Options).ToString();

			if (PercentString.Len() == 2)
			{
				PercentString = "  " + PercentString;
			}
			else if (PercentString.Len() == 3)
			{
				PercentString = " " + PercentString;
			}

			Line += PercentString + "  ";
		}
		UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
		Line.Empty();
	}

	UE_LOG(LogTemp, Display, TEXT("Total Hits: %d Total Spawns: %d"), TotalHits, TotalSpawns);
}

/** Prints full TrainingSamples array to log/console.
 *  @param InDefiningConfig defining config
 * 	@param InCommonScoreInfo CommonScoreInfo to print out
 * 	@param Options number formatting options
 */
void PrintTrainingSamples(const FBS_DefiningConfig& InDefiningConfig,
                          const FCommonScoreInfo& InCommonScoreInfo,
                          const FNumberFormattingOptions& Options)
{
	FString GameModeString;
	FString Line;

	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " +
		                 UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
	}
	else
	{
		GameModeString = InDefiningConfig.CustomGameModeName;
	}
	UE_LOG(LogTemp, Display, TEXT("Full TrainingSamples for %s:"), *GameModeString);

	for (int i = 0; i < InCommonScoreInfo.TrainingSamples.Num(); i++)
	{
		const int32 Value = InCommonScoreInfo.TrainingSamples[i];
		FString LineValue = FText::AsNumber(Value, &Options).ToString();

		if (LineValue.Len() == 1)
		{
			LineValue = "  " + LineValue + "  ";
		}
		if (LineValue.Len() == 2)
		{
			LineValue = " 0" + LineValue + " ";
		}
		else if (LineValue.Len() == 3)
		{
			LineValue = " " + LineValue + " ";;
		}
		else if (LineValue.Len() == 4)
		{
			LineValue = "0" + LineValue;
		}

		Line += LineValue + "  ";

		if (i > 1 && (i + 1) % InCommonScoreInfo.NumQTableRows == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
			Line.Empty();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Total Training Samples: %lld"), InCommonScoreInfo.TotalTrainingSamples);
}
}

USaveGamePlayerScore* USaveGamePlayerScore::LoadFromSlot()
{
	USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<
		USaveGamePlayerScore>(TEXT("ScoreSlot"), 1);
	SaveGamePlayerScore->BuildRuntimeData();
	return SaveGamePlayerScore;
}

void USaveGamePlayerScore::SaveToSlot()
{
	CommitRuntimeData();
	SaveLoadCommon::SaveToSlot(this, TEXT("ScoreSlot"), 1);
}

USaveGamePlayerScore::USaveGamePlayerScore()
{
	PercentFormat.MaximumFractionalDigits = 0;
	PercentFormat.MaximumIntegralDigits = 3;
	PercentFormat.MinimumIntegralDigits = 1;

	QTableFormat.MinimumFractionalDigits = 2;
	QTableFormat.MaximumFractionalDigits = 2;
	QTableFormat.MaximumIntegralDigits = 1;
	QTableFormat.MinimumIntegralDigits = 1;

	TrainingSamplesFormat.MinimumFractionalDigits = 0;
	TrainingSamplesFormat.MaximumFractionalDigits = 0;
	TrainingSamplesFormat.MaximumIntegralDigits = 4;
	TrainingSamplesFormat.MinimumIntegralDigits = 1;
}

void USaveGamePlayerScore::BuildRuntimeData()
{
	PlayerScoreArrayPtr.Empty();
	PlayerScoreArrayPtr.Reserve(PlayerScoreArray.Num());
	const FTimespan UtcOffset = FDateTime::Now() - FDateTime::UtcNow();
	for (const auto& PlayerScore : PlayerScoreArray)
	{
		auto PlayerScorePtr = MakeShared<FPlayerScore>(PlayerScore);
		FDateTime ParsedTime;
		FDateTime::ParseIso8601(*PlayerScorePtr->Time, ParsedTime);
		PlayerScorePtr->LocalDateTime = ParsedTime + UtcOffset;
		PlayerScoreArrayPtr.Add(PlayerScorePtr);
	}
	PlayerScoreArray.Empty();
}

void USaveGamePlayerScore::CommitRuntimeData()
{
	PlayerScoreArray.Empty(PlayerScoreArrayPtr.Num());
	for (const TSharedPtr<FPlayerScore>& Ptr : PlayerScoreArrayPtr)
	{
		PlayerScoreArray.Add(*Ptr);
	}
}

float USaveGamePlayerScore::GetHighScore(const FPlayerScore& PlayerScoreToMatch) const
{
	float HighScore = PlayerScoreToMatch.Score;

	for (const TSharedPtr<FPlayerScore>& PlayerScore : PlayerScoreArrayPtr)
	{
		if (PlayerScore->HasMatchingDefiningConfigAndSong(PlayerScoreToMatch))
		{
			HighScore = FMath::Max(HighScore, PlayerScoreToMatch.Score);
		}
	}

	return HighScore;
}

const TArray<TSharedPtr<FPlayerScore>>& USaveGamePlayerScore::GetPlayerScoresPtr() const
{
	return PlayerScoreArrayPtr;
}

void USaveGamePlayerScore::AddPlayerScoreInstance(const FPlayerScore& InPlayerScore)
{
	if (!ContainsExistingTime(InPlayerScore))
	{
		PlayerScoreArrayPtr.Add(MakeShared<FPlayerScore>(InPlayerScore));
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Existing Score with the same Time found"));
	}
}

void USaveGamePlayerScore::DeletePlayerScores(const TArray<TSharedPtr<FPlayerScore>>& ScoresToDelete)
{
	if (DeletePlayerScoresInternal(ScoresToDelete) > 0)
	{
		SaveToSlot();
		OnScoresDeleted.Broadcast();
	}
}

bool USaveGamePlayerScore::ContainsExistingTime(const FPlayerScore& InPlayerScore)
{
	const TSharedPtr<FPlayerScore>* Found = PlayerScoreArrayPtr.FindByPredicate(
		[&InPlayerScore](const TSharedPtr<FPlayerScore>& CompareScore)
		{
			return InPlayerScore.Time.Equals(CompareScore->Time);
		});
	return Found ? true : false;
}

int32 USaveGamePlayerScore::DeletePlayerScoresInternal(const TArray<TSharedPtr<FPlayerScore>>& ScoresToDelete)
{
	int RemovedScores = 0;
	for (const auto& Score : ScoresToDelete)
	{
		RemovedScores += PlayerScoreArrayPtr.Remove(Score);
	}
	return RemovedScores;
}

TMap<FBS_DefiningConfig, FCommonScoreInfo> USaveGamePlayerScore::GetCommonScoreInfo() const
{
	return CommonScoreInfo;
}

FCommonScoreInfo USaveGamePlayerScore::FindOrAddCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig)
{
	return CommonScoreInfo.FindOrAdd(InDefiningConfig);
}

void USaveGamePlayerScore::SetOrAddCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig,
                                                   const FCommonScoreInfo& InCommonScoreInfo)
{
	CommonScoreInfo.FindOrAdd(InDefiningConfig) = InCommonScoreInfo;

#if !UE_BUILD_SHIPPING
	if (InCommonScoreInfo.NumQTableRows != 0 && InCommonScoreInfo.QTable.Num() > 0)
	{
		PrintAccuracy(InDefiningConfig, InCommonScoreInfo, PercentFormat);
		PrintQTable(InDefiningConfig, InCommonScoreInfo, QTableFormat);
		PrintTrainingSamples(InDefiningConfig, InCommonScoreInfo, TrainingSamplesFormat);
	}
#endif
}

bool USaveGamePlayerScore::ResetQTable(const FBS_DefiningConfig& InDefiningConfig)
{
	FCommonScoreInfo* Found = CommonScoreInfo.Find(InDefiningConfig);

	if (!Found)
	{
		return false;
	}

	Found->ResetQTable();
	SaveToSlot();
	return true;
}

int32 USaveGamePlayerScore::RemoveCommonScoreInfoAndMatchingPlayerScores(const FBS_DefiningConfig& InDefiningConfig)
{
	TArray<TSharedPtr<FPlayerScore>> ScoresToDelete;
	for (const auto& PlayerScore : PlayerScoreArrayPtr)
	{
		if (PlayerScore->DefiningConfig == InDefiningConfig)
		{
			ScoresToDelete.Add(PlayerScore);
		}
	}
	const int32 NumScoresRemoved = DeletePlayerScoresInternal(ScoresToDelete);
	const int32 NumCommonScoreInfosRemoved = CommonScoreInfo.Remove(InDefiningConfig);
	if (NumScoresRemoved > 0 || NumCommonScoreInfosRemoved > 0)
	{
		SaveToSlot();
		if (NumScoresRemoved > 0)
		{
			OnScoresDeleted.Broadcast();
		}
	}
	UE_LOG(LogTemp, Display, TEXT("%d Common Score Infos removed when removing a custom game mode."),
	       NumCommonScoreInfosRemoved);
	UE_LOG(LogTemp, Display, TEXT("%d matching scores removed when removing a custom game mode."), NumScoresRemoved);
	return NumScoresRemoved;
}

int32 USaveGamePlayerScore::RemoveAllCommonScoreInfoAndMatchingPlayerScores()
{
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfoToRemove = CommonScoreInfo.FilterByPredicate(
		[](const TPair<FBS_DefiningConfig, FCommonScoreInfo>& Pair)
		{
			return Pair.Key.GameModeType == EGameModeType::Custom;
		});
	for (const TPair<FBS_DefiningConfig, FCommonScoreInfo>& Pair : CommonScoreInfoToRemove)
	{
		CommonScoreInfo.Remove(Pair.Key);
	}
	TArray<TSharedPtr<FPlayerScore>> ScoresToDelete;
	for (const auto& PlayerScore : PlayerScoreArrayPtr)
	{
		if (CommonScoreInfoToRemove.Contains(PlayerScore->DefiningConfig))
		{
			ScoresToDelete.Add(PlayerScore);
		}
	}
	const int32 NumScoresRemoved = DeletePlayerScoresInternal(ScoresToDelete);
	const int32 NumCommonScoreInfosRemoved = CommonScoreInfoToRemove.Num();
	if (NumScoresRemoved > 0 || NumCommonScoreInfosRemoved > 0)
	{
		SaveToSlot();
		if (NumScoresRemoved > 0)
		{
			OnScoresDeleted.Broadcast();
		}
	}
	UE_LOG(LogTemp, Display, TEXT("%d Common Score Infos removed when removing a custom game mode."),
	       NumCommonScoreInfosRemoved);
	UE_LOG(LogTemp, Display, TEXT("%d matching scores removed when removing a custom game mode."), NumScoresRemoved);
	return NumScoresRemoved;
}

TMap<FString, float> USaveGamePlayerScore::CreateSongDurationMap() const
{
	TMap<FString, float> Map;
	for (const TSharedPtr<FPlayerScore>& Score : PlayerScoreArrayPtr)
	{
		Map.Add(Score->SongTitle, Score->SongLength);
	}
	return Map;
}

void USaveGamePlayerScore::PrintQTable(const FBS_DefiningConfig& InDefiningConfig,
                                       const FCommonScoreInfo& InCommonScoreInfo,
                                       const FNumberFormattingOptions& Options)
{
	FString GameModeString;

	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " +
		                 UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
	}
	else
	{
		GameModeString = InDefiningConfig.CustomGameModeName;
	}
	UE_LOG(LogTemp, Display, TEXT("Full QTable for %s:"), *GameModeString);

	FString Line;
	for (int i = 0; i < InCommonScoreInfo.QTable.Num(); i++)
	{
		const float Value = InCommonScoreInfo.QTable[i];
		FString LineValue = FText::AsNumber(Value, &Options).ToString();

		if (Value == 0.f)
		{
			LineValue = " " + LineValue;
		}
		else if (Value > 0.f)
		{
			LineValue = "+" + LineValue;
		}

		Line += LineValue + "  ";

		if (i > 1 && (i + 1) % InCommonScoreInfo.NumQTableRows == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
			Line.Empty();
		}
	}
}
