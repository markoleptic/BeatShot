// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSPlayerScoreInterface.h"
#include "SaveGamePlayerScore.h"
#include "SaveLoadCommon.h"

using namespace SaveLoadCommon;

TArray<FPlayerScore> IBSPlayerScoreInterface::LoadPlayerScores()
{
	if (const USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		return SaveGamePlayerScore->GetPlayerScores();
	}
	return TArray<FPlayerScore>();
}

TArray<FPlayerScore> IBSPlayerScoreInterface::LoadPlayerScores_UnsavedToDatabase()
{
	if (const USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		return SaveGamePlayerScore->GetPlayerScores_UnsavedToDatabase();
	}
	return TArray<FPlayerScore>();
}

void IBSPlayerScoreInterface::SetAllPlayerScoresSavedToDatabase()
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->SetAllScoresSavedToDatabase();
		SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

TArray<FPlayerScore> IBSPlayerScoreInterface::GetMatchingPlayerScores(const FPlayerScore& PlayerScore)
{
	return LoadPlayerScores().FilterByPredicate([&PlayerScore](const FPlayerScore& ComparePlayerScore)
	{
		if (ComparePlayerScore == PlayerScore)
		{
			return true;
		}
		return false;
	});
}

void IBSPlayerScoreInterface::SavePlayerScoreInstance(const FPlayerScore& PlayerScoreToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->AddPlayerScoreInstance(PlayerScoreToSave);
		SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

/* --------------------------- */
/* ---- Common Score Info ---- */
/* --------------------------- */

FCommonScoreInfo IBSPlayerScoreInterface::FindOrAddCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	FCommonScoreInfo CommonScoreInfo;
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->FindOrAddCommonScoreInfo(DefiningConfig, CommonScoreInfo);
	}
	return CommonScoreInfo;
}

void IBSPlayerScoreInterface::SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig,
	const FCommonScoreInfo& CommonScoreInfoToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->SaveCommonScoreInfo(DefiningConfig, CommonScoreInfoToSave);
		SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

int32 IBSPlayerScoreInterface::RemoveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		const int32 NumRemoved = SaveGamePlayerScore->RemoveCommonScoreInfo(DefiningConfig);
		if (SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1)) return NumRemoved;
	}
	return 0;
}

int32 IBSPlayerScoreInterface::ResetQTable(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = LoadFromSlot<USaveGamePlayerScore>(TEXT("ScoreSlot"), 1))
	{
		const int32 NumCleared = SaveGamePlayerScore->ResetQTable(DefiningConfig);
		if (SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1)) return NumCleared;
	}
	return 0;
}
