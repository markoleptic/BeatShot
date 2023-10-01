// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SaveGamePlayerScore.h"


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
	const FPlayerScore* Found = PlayerScoreArray.FindByPredicate([&InPlayerScore] (const FPlayerScore& CompareScore)
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
	FString GameModeString;
	if (InDefiningConfig.CustomGameModeName.IsEmpty())
	{
		GameModeString = UEnum::GetDisplayValueAsText(InDefiningConfig.BaseGameMode).ToString() + " " + UEnum::GetDisplayValueAsText(InDefiningConfig.Difficulty).ToString();
	}
	else
	{
		GameModeString = InDefiningConfig.CustomGameModeName;
	}
	
	FNumberFormattingOptions Options;
	Options.MaximumFractionalDigits = 0;
	Options.MaximumIntegralDigits = 3;
	Options.MinimumIntegralDigits = 1;

	int32 TotalSpawns = 0;
	int32 TotalHits = 0;
	
	UE_LOG(LogTemp, Display, TEXT("Cumulative Accuracy for %s:"), *GameModeString);
	
	for (const FAccuracyRow& AccuracyRow : InCommonScoreInfo.AccuracyData.AccuracyRows)
	{
		FString Line;

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
	}
	
	UE_LOG(LogTemp, Display, TEXT("Total Hits: %d Total Spawns: %d"), TotalHits, TotalSpawns);
	
	UE_LOG(LogTemp, Display, TEXT("Full QTable for %s:"), *GameModeString);

	if (InCommonScoreInfo.QTableRowSize == 0)
	{
		return;
	}
	FString Line;
	FNumberFormattingOptions NumOptions;
	NumOptions.MinimumFractionalDigits = 2;
	NumOptions.MaximumFractionalDigits = 2;
	NumOptions.MaximumIntegralDigits = 1;
	NumOptions.MinimumIntegralDigits = 1;
	
	for (int i = 0; i < InCommonScoreInfo.QTable.Num(); i++)
	{
		const float Value = InCommonScoreInfo.QTable[i];
		FString LineValue = FText::AsNumber(Value, &NumOptions).ToString();
		
		if (Value == 0.f)
		{
			LineValue = " " + LineValue;
		}
		else if (Value > 0.f)
		{
			LineValue = "+" + LineValue;
		}
		
		Line += LineValue + "  ";
		
		if (i % InCommonScoreInfo.QTableRowSize == 0)
		{
			UE_LOG(LogTemp, Display, TEXT("\t %s"), *Line);
			Line.Empty();
		}
	}
	
	UE_LOG(LogTemp, Display, TEXT("Total Training Samples: %lld"), InCommonScoreInfo.QTableTrainingSamples);
}