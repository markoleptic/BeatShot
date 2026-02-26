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

	void BuildRuntimeData();

	void CommitRuntimeData();

	/** @return a copy of PlayerScoreArray. */
	TArray<FPlayerScore> GetPlayerScores() const;

	const TArray<TSharedPtr<FPlayerScore>>& GetPlayerScoresPtr() const;

	/** @return a copy of player scores not saved to database. */
	TArray<FPlayerScore> GetPlayerScores_UnsavedToDatabase() const;

	/** Adds a new entry to PlayerScoreArray.
	 *  @param InPlayerScore player score instance to add
	 */
	void AddPlayerScoreInstance(const FPlayerScore& InPlayerScore);

	void DeletePlayerScores(const TArray<TSharedPtr<FPlayerScore>>& ScoresToDelete);

	/** Modifies score instances in PlayerScoreArray. */
	void SetAllScoresSavedToDatabase();

	/** @return a copy of CommonScoreInfo. */
	TMap<FBS_DefiningConfig, FCommonScoreInfo> GetCommonScoreInfo() const;

	/** @return a copy of CommonScoreInfo. */
	TSharedPtr<TMap<FBS_DefiningConfig, FCommonScoreInfo>> GetCommonScoreInfoPtr() const;

	/** Finds or Adds an entry to CommonScoreInfo map for the given Defining Config.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @param OutCommonScoreInfo the found CommonScoreInfo
	 */
	void FindOrAddCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, FCommonScoreInfo& OutCommonScoreInfo);

	/** Replaces the CommonScoreInfo entry with InCommonScoreInfo if one is found using InDefiningConfig. Otherwise, it
	 *  adds a new entry.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @param InCommonScoreInfo the CommonScoreInfo to replace the existing, or the newly added entry
	 */
	void SaveCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig, const FCommonScoreInfo& InCommonScoreInfo);

	/** Clears the QTable for an FCommonScoreInfo instance that matches the Defining Config. 
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @return how many successfully found and cleared.
	 */
	int32 ResetQTable(const FBS_DefiningConfig& InDefiningConfig);

	/** Removes an FCommonScoreInfo instance that matches the Defining Config.
	 *  @param InDefiningConfig key used to find the CommonScoreInfo
	 *  @return how many successfully removed CommonScoreInfo instances were removed.
	 */
	int32 RemoveCommonScoreInfo(const FBS_DefiningConfig& InDefiningConfig);

	/** Removes all FCommonScoreInfo instances for custom game modes.
	 *  @return the number of removed instances.
	 */
	int32 RemoveAllCustomGameModeCommonScoreInfo();

	/** Prints full QTable array to log/console.
	 * 	@param InDefiningConfig defining config
	 * 	@param InCommonScoreInfo CommonScoreInfo to print out
	 * 	@param Options number formatting options
	 */
	static void PrintQTable(const FBS_DefiningConfig& InDefiningConfig,
	                        const FCommonScoreInfo& InCommonScoreInfo,
	                        const FNumberFormattingOptions& Options);

	/** Prints 5x5 accuracy to log/console.
	 *  @param InDefiningConfig defining config
	 * 	@param InCommonScoreInfo CommonScoreInfo to print out
	 * 	@param Options number formatting options
	 */
	static void PrintAccuracy(const FBS_DefiningConfig& InDefiningConfig,
	                          const FCommonScoreInfo& InCommonScoreInfo,
	                          const FNumberFormattingOptions& Options);

	/** Prints full TrainingSamples array to log/console.
	 *  @param InDefiningConfig defining config
	 * 	@param InCommonScoreInfo CommonScoreInfo to print out
	 * 	@param Options number formatting options
	 */
	static void PrintTrainingSamples(const FBS_DefiningConfig& InDefiningConfig,
	                                 const FCommonScoreInfo& InCommonScoreInfo,
	                                 const FNumberFormattingOptions& Options);

private:
	/**
	 * @param InPlayerScore the player score to compare to existing
	 * @return whether there exists a score with the same time.
	 */
	bool ContainsExistingTime(const FPlayerScore& InPlayerScore);

	/** Array containing all saved score instances. */
	UPROPERTY()
	TArray<FPlayerScore> PlayerScoreArray;

	TArray<TSharedPtr<FPlayerScore>> PlayerScoreArrayPtr;

	/** Map containing common score info for each unique defining config. */
	UPROPERTY()
	TMap<FBS_DefiningConfig, FCommonScoreInfo> CommonScoreInfo;

	TSharedPtr<TMap<FBS_DefiningConfig, FCommonScoreInfo>> CommonScoreInfoPtr;

	FNumberFormattingOptions PercentFormat;
	FNumberFormattingOptions QTableFormat;
	FNumberFormattingOptions TrainingSamplesFormat;
};
