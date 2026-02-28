// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeConfig/DefiningConfig.h"
#include "GameFramework/SaveGame.h"
#include "Score/CommonScoreInfo.h"
#include "Score/PlayerScore.h"
#include "SaveGamePlayerScore.generated.h"


UCLASS()
class BEATSHOTGLOBAL_API USaveGamePlayerScore : public USaveGame
{
	GENERATED_BODY()

public:
	USaveGamePlayerScore();

	/** Loads or creates a save game. */
	static USaveGamePlayerScore* LoadFromSlot();

	/** Saves the save game to disk. */
	void SaveToSlot();

	float GetHighScore(const FPlayerScore& PlayerScoreToMatch) const;

	const TArray<TSharedPtr<FPlayerScore>>& GetPlayerScoresPtr() const;

	/** Adds a new entry to PlayerScoreArray. Does not save.
	 *  @param InPlayerScore player score instance to add
	 */
	void AddPlayerScoreInstance(const FPlayerScore& InPlayerScore);

	/** Removes matches scores and saves to slot if removed > 0. */
	void DeletePlayerScores(const TArray<TSharedPtr<FPlayerScore>>& ScoresToDelete);

	/** @return a copy of CommonScoreInfo. */
	TMap<FBS_DefiningConfig, FCommonScoreInfo> GetCommonScoreInfo() const;

	/** Finds or Adds an entry to CommonScoreInfo map for the given Defining Config. Does not save.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @return Copy of found CommonScoreInfo or a default constructed if it does not exist.
	 */
	FCommonScoreInfo FindOrAddCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig);

	/** Replaces the CommonScoreInfo entry with InCommonScoreInfo if one is found using InDefiningConfig. Otherwise, it
	 *  adds a new entry. Does not save.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @param InCommonScoreInfo the CommonScoreInfo to replace the existing, or the newly added entry
	 */
	void SetOrAddCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo);

	/** Clears the QTable for an FCommonScoreInfo instance that matches the Defining Config. Saves to slot if anything
	 *  changed.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @return True if matching config was found.
	 */
	bool ResetQTable(const FBS_DefiningConfig& InDefiningConfig);

	/** Removes an FCommonScoreInfo instance and matching scores that matches the Defining Config. Saves to slot if 
	 *  anything was removed.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo and matching scores
	 *  @return Number of scores removed.
	 */
	int32 RemoveCommonScoreInfoAndMatchingPlayerScores(const FBS_DefiningConfig& InDefiningConfig);

	/** Removes FCommonScoreInfo instances and associated scores for all custom game modes. Saves to slot if anything
	 *  was removed.
	 *  @return Number of scores removed.
	 */
	int32 RemoveAllCommonScoreInfoAndMatchingPlayerScores();

	/** @return A map containing song names and durations for saved scores. */
	TMap<FString, float> CreateSongDurationMap() const;

	/** Prints full QTable array to log/console.
	 * 	@param InDefiningConfig defining config
	 * 	@param InCommonScoreInfo CommonScoreInfo to print out
	 * 	@param Options number formatting options
	 */
	static void PrintQTable(const FBS_DefiningConfig& InDefiningConfig,
	                        const FCommonScoreInfo& InCommonScoreInfo,
	                        const FNumberFormattingOptions& Options);

	TMulticastDelegate<void()> OnScoresDeleted;

private:
	/** Populates PlayerScoreArrayPtr. */
	void BuildRuntimeData();

	/** Populates PlayerScoreArray from PlayerScoreArrayPtr. */
	void CommitRuntimeData();

	/** @param InPlayerScore the player score to compare to existing
	 *  @return whether there exists a score with the same time. 
	 */
	bool ContainsExistingTime(const FPlayerScore& InPlayerScore);

	/** Removes scores but doesn't save to slot. */
	int32 DeletePlayerScoresInternal(const TArray<TSharedPtr<FPlayerScore>>& ScoresToDelete);

	/** Array containing all saved score instances. */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;

	TArray<TSharedPtr<FPlayerScore>> PlayerScoreArrayPtr;

	/** Map containing common score info for each unique defining config. */
	UPROPERTY()
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfo;

	FNumberFormattingOptions PercentFormat;
	FNumberFormattingOptions QTableFormat;
	FNumberFormattingOptions TrainingSamplesFormat;
};
