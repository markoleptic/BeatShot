// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSPlayerScoreInterface.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "Utilities/SaveLoadCommon.h"

TArray<FPlayerScore> IBSPlayerScoreInterface::LoadPlayerScores()
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		return SaveGamePlayerScore->GetPlayerScores();
	}
	return TArray<FPlayerScore>();
}

TArray<FPlayerScore> IBSPlayerScoreInterface::LoadPlayerScores_UnsavedToDatabase()
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		return SaveGamePlayerScore->GetPlayerScores_UnsavedToDatabase();
	}
	return TArray<FPlayerScore>();
}

void IBSPlayerScoreInterface::SetAllPlayerScoresSavedToDatabase()
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		SaveGamePlayerScore->SetAllScoresSavedToDatabase();
		SaveGamePlayerScore->CommitRuntimeData();
		SaveLoadCommon::SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

TArray<FPlayerScore> IBSPlayerScoreInterface::GetMatchingPlayerScores(const FPlayerScore& PlayerScore)
{
	return LoadPlayerScores().FilterByPredicate([&PlayerScore](const FPlayerScore& ComparePlayerScore)
	{
		if (ComparePlayerScore.HasMatchingDefiningConfigAndSong(PlayerScore))
		{
			return true;
		}
		return false;
	});
}

void IBSPlayerScoreInterface::SavePlayerScoreInstance(const FPlayerScore& PlayerScoreToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		SaveGamePlayerScore->AddPlayerScoreInstance(PlayerScoreToSave);
		SaveGamePlayerScore->CommitRuntimeData();
		SaveLoadCommon::SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

/* --------------------------- */
/* ---- Common Score Info ---- */
/* --------------------------- */

FCommonScoreInfo IBSPlayerScoreInterface::FindOrAddCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	FCommonScoreInfo CommonScoreInfo;
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		SaveGamePlayerScore->FindOrAddCommonScoreInfo(DefiningConfig, CommonScoreInfo);
	}
	return CommonScoreInfo;
}

void IBSPlayerScoreInterface::SaveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig,
	const FCommonScoreInfo& CommonScoreInfoToSave)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		SaveGamePlayerScore->SaveCommonScoreInfo(DefiningConfig, CommonScoreInfoToSave);
		SaveGamePlayerScore->CommitRuntimeData();
		SaveLoadCommon::SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1);
	}
}

int32 IBSPlayerScoreInterface::RemoveCommonScoreInfo(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		const int32 NumRemoved = SaveGamePlayerScore->RemoveCommonScoreInfo(DefiningConfig);
		SaveGamePlayerScore->CommitRuntimeData();
		if (SaveLoadCommon::SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1))
		{
			return NumRemoved;
		}
	}
	return 0;
}

int32 IBSPlayerScoreInterface::ResetQTable(const FBS_DefiningConfig& DefiningConfig)
{
	if (USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<USaveGamePlayerScore>(
		TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore->BuildRuntimeData();
		const int32 NumCleared = SaveGamePlayerScore->ResetQTable(DefiningConfig);
		SaveGamePlayerScore->CommitRuntimeData();
		if (SaveLoadCommon::SaveToSlot(SaveGamePlayerScore, TEXT("ScoreSlot"), 1))
		{
			return NumCleared;
		}
	}
	return 0;
}

USaveGamePlayerScore* IBSPlayerScoreInterface::LoadSaveGamePlayerScore()
{
	USaveGamePlayerScore* SaveGamePlayerScore = SaveLoadCommon::LoadFromSlot<
		USaveGamePlayerScore>(TEXT("ScoreSlot"), 1);
	if (SaveGamePlayerScore)
	{
		SaveGamePlayerScore->BuildRuntimeData();
	}
	return SaveGamePlayerScore;
}
