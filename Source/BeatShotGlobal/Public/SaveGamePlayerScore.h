// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerScore.generated.h"

UCLASS()
class BEATSHOTGLOBAL_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:
	/** Returns a copy of PlayerScoreArray */
	TArray<FPlayerScore> GetPlayerScores() const;

	/** Returns player scores not saved to database */
	TArray<FPlayerScore> GetPlayerScores_UnsavedToDatabase() const;

	/** Adds a new entry to PlayerScoreArray */
	void SavePlayerScoreInstance(const FPlayerScore& InPlayerScore);

	/** Modifies score instances in PlayerScoreArray */
	void SetAllScoresSavedToDatabase();

	/** Returns a copy of CommonScoreInfo */
	TMap<FBS_DefiningConfig, FCommonScoreInfo> GetCommonScoreInfo() const;

	/** Returns the CommonScoreInfo that matches a given DefiningConfig, or a blank one if none found  */
	FCommonScoreInfo FindCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig);

	/** Finds or Adds the CommonScoreInfo for the given Defining Config */
	void SaveCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo);

private:
	bool ContainsExistingTime(const FPlayerScore& InPlayerScore);
	
	/** Array containing all saved score instances */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;

	/** Map containing common score info for each unique defining config */
	UPROPERTY()
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfo;
};