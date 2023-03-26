// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CoreUObject.h"
#include "SaveLoadInterface.h"
THIRD_PARTY_INCLUDES_START
#pragma push_macro("check")
#undef check
#pragma warning (push)
#pragma warning (disable : 4191)
#pragma warning (disable : 4686)
#include "NumCpp/Public/NumCpp.hpp"
#pragma warning (pop)
#pragma pop_macro("check")
THIRD_PARTY_INCLUDES_END
#include "RLBase.generated.h"

/** A struct representing the inputs for a Reinforcement Learning Algorithm */
USTRUCT()
struct FAlgoInput
{
	GENERATED_BODY()

	int32 StateIndex;
	int32 ActionIndex;
	int32 StateIndex_2;
	int32 ActionIndex_2;
	float Reward;

	FAlgoInput()
	{
		StateIndex = -1;
		ActionIndex = -1;
		StateIndex_2 = -1;
		ActionIndex_2 = -1;
		Reward = 1;
	}

	FAlgoInput(const int32 InStateIndex, const int32 InActionIndex, const int32 InStateIndex_2, const int32 InActionIndex_2, float InReward)
	{
		StateIndex = InStateIndex;
		ActionIndex = InActionIndex;
		StateIndex_2 = InStateIndex_2;
		ActionIndex_2 = InActionIndex_2;
		Reward = InReward;
	}
};

/** A struct to pass the Agent upon Initialization */
USTRUCT()
struct FRLAgentParams
{
	GENERATED_BODY()

	EGameModeActorName GameModeActorName;
	FString CustomGameModeName;
	int32 Size;
	float InAlpha;
	float InGamma;
	float InEpsilon;
	int32 Rows;
	int32 Columns;

	FRLAgentParams()
	{
		GameModeActorName = EGameModeActorName::Custom;
		CustomGameModeName = "";
		Size = 0;
		InAlpha = 0;
		InGamma = 0;
		InEpsilon = 0;
		Rows = 0;
		Columns = 0;
	}
};

UCLASS()
class BEATSHOT_API URLBase : public UObject, public ISaveLoadInterface
{
	GENERATED_BODY()

	URLBase();

	/** A 2D array where the row and column have size equal to the number of possible spawn points.
	 *  An element in the array represents the expected reward from starting at spawn location RowIndex
	 *  and spawning a target at ColumnIndex. */
	nc::NdArray<float> QTable;

	/** An array that accumulates the current episodes rewards. Not used for gameplay events */
	nc::NdArray<float> EpisodeRewards;

	/** An wrapper around the QTable that also includes which game mode it corresponds to */
	UPROPERTY()
	FQTableWrapper QTableWrapper;

	/** Learning rate, or how much to update the Q-Table rewards when a reward is received */
	float Alpha;
	
	/** Discount factor, or how much to value future rewards vs immediate rewards */
	float Gamma;
	
	/** The exploration/exploitation balance factor. A value = 1 will result in only choosing random values (explore),
	 *  while a value of zero will result in only choosing the max Q-value (exploitation) */
	float Epsilon;

	/** The number of column entries in SpawnCounter size are in a single column entry of the QTable */
	int32 NumColsPerScaledCol;
	
	/** The number of row entries in SpawnCounter size are in a single column entry of the QTable */
	int32 NumRowsPerScaledRow;

	/** How many columns the SpawnCounter total columns are divided into */
	int32 ColScale = 3;

	/** How many rows the SpawnCounter total rows are divided into */
	int32 RowScale = 3;

	nc::NdArray<float> ScaleQTable(const int32 SpawnCounterRows, const int32 SpawnCounterColumns);

	int32 GetQTableIndexFromSpawnCounterIndex(const int32 SpawnCounterIndex) const;

	int32 GetSpawnCounterIndexFromQTableIndex(const int32 QTableIndex) const;

	nc::NdArray<float> GetQTableFromTArray(const FQTableWrapper& InWrapper) const;

	TArray<float> GetTArrayFromQTable(nc::NdArray<float> InQTable);

public:
	void Init(const FRLAgentParams& AgentParams);

	/** Returns the index of the next target to spawn, based on the Epsilon value */
	int32 GetNextActionIndex(const int32 SpawnCounterIndex) const;

	/** Returns a random SpawnCounterIndex */
	int32 GetRandomAction() const;

	/** Returns the index of the next target to spawn, using a greedy approach */
	int32 GetMaxActionIndex(const int32 QTableIndex) const;

	/** Updates a Q-Table element entry after a target has been spawned and either timed out or destroyed by player */
	virtual void UpdateQTable(const FAlgoInput In);

	/** Updates EpisodeRewards by appending to the array */
	virtual void UpdateEpisodeRewards(const float RewardReceived);

	/** Prints the Q-Table to Unreal console */
	void PrintRewards();

	/** Saves the Q-Table to slot */
	void SaveQTable();
};
