// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGamePlayerScore.h"


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

TArray<FPlayerScore> USaveGamePlayerScore::GetPlayerScores() const
{
	return PlayerScoreArray;
}

TArray<FPlayerScore> USaveGamePlayerScore::GetPlayerScores_UnsavedToDatabase() const
{
	TArray<FPlayerScore> UnsavedScores;
	for (const FPlayerScore& Score : PlayerScoreArray)
	{
		if (!Score.bSavedToDatabase)
		{
			UnsavedScores.Add(Score);
		}
	}
	return UnsavedScores;
}

void USaveGamePlayerScore::SavePlayerScoreInstance(const FPlayerScore& InPlayerScore)
{
	if (!ContainsExistingTime(InPlayerScore))
	{
		PlayerScoreArray.Add(InPlayerScore);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Existing Score with the same Time found"));
	}
}

void USaveGamePlayerScore::SetAllScoresSavedToDatabase()
{
	for (FPlayerScore& Score : PlayerScoreArray)
	{
		Score.bSavedToDatabase = true;
	}
}

bool USaveGamePlayerScore::ContainsExistingTime(const FPlayerScore& InPlayerScore)
{
	const FPlayerScore* Found = PlayerScoreArray.FindByPredicate([&InPlayerScore](const FPlayerScore& CompareScore)
	{
		return InPlayerScore.Time.Equals(CompareScore.Time);
	});
	return Found ? true : false;
}

TMap<FBS_DefiningConfig, FCommonScoreInfo> USaveGamePlayerScore::GetCommonScoreInfo() const
{
	return CommonScoreInfo;
}

FCommonScoreInfo USaveGamePlayerScore::FindCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig)
{
	FCommonScoreInfo* Found = CommonScoreInfo.Find(InDefiningConfig);
	return Found ? *Found : FCommonScoreInfo();
}

void USaveGamePlayerScore::SaveCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo)
{
	CommonScoreInfo.FindOrAdd(InDefiningConfig) = InCommonScoreInfo;


#if !UE_BUILD_SHIPPING
	if (InCommonScoreInfo.QTableRowSize != 0 && InCommonScoreInfo.QTable.Num() > 0)
	{
		PrintAccuracy(InDefiningConfig, InCommonScoreInfo, PercentFormat);
		PrintQTable(InDefiningConfig, InCommonScoreInfo, QTableFormat);
		PrintTrainingSamples(InDefiningConfig, InCommonScoreInfo, TrainingSamplesFormat);
	}
#endif
}

void USaveGamePlayerScore::PrintQTable(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options)
{
	FString GameModeString;

	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " + UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
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

		if (i > 1 && (i + 1) % InCommonScoreInfo.QTableRowSize == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
			Line.Empty();
		}
	}
}

void USaveGamePlayerScore::PrintAccuracy(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options)
{
	int32 TotalSpawns = 0;
	int32 TotalHits = 0;
	FString GameModeString;
	FString Line;

	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " + UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
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
			TotalSpawns += AccuracyRow.TotalSpawns[i];
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

void USaveGamePlayerScore::PrintTrainingSamples(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo, const FNumberFormattingOptions& Options)
{
	
	FString GameModeString;
	FString Line;
	
	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " + UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
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

		if (i > 1 && (i + 1) % InCommonScoreInfo.QTableRowSize == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
			Line.Empty();
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Total Training Samples: %lld"), InCommonScoreInfo.TotalTrainingSamples);
}
