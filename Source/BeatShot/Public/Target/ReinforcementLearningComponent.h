// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "MatrixFunctions.h"
#include "Components/ActorComponent.h"

class UCompositeCurveTable;
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

#include "ReinforcementLearningComponent.generated.h"

/** A struct representing two consecutively activated targets, used to keep track of the reward associated between two points */
USTRUCT()
struct FTargetPair
{
	GENERATED_BODY()

	/** The SpawnArea index of the target spawned before Second */
	int32 First;

	/** The SpawnArea index of the target spawned after First */
	int32 Second;

private:
	/** The reward for spawning a target at First and then spawning a target at Second */
	float Reward;

public:
	FTargetPair()
	{
		First = INDEX_NONE;
		Second = INDEX_NONE;
		Reward = 0.f;
	}

	FTargetPair(const int32 PreviousPointIndex, const int32 CurrentPointIndex)
	{
		First = PreviousPointIndex;
		Second = CurrentPointIndex;
		Reward = 0.f;
	}

	FORCEINLINE bool operator ==(const FTargetPair& Other) const
	{
		return First == Other.First && Second == Other.Second;
	}

	/** Sets the reward for this TargetPair */
	void SetReward(const float InReward)
	{
		Reward = InReward;
	}

	/** Returns the reward for this TargetPair */
	float GetReward() const { return Reward; }
};

/** A struct representing the parameters for the QTable Update function */
USTRUCT()
struct FQTableUpdateParams
{
	GENERATED_BODY()

	/** The pair of targets that were activated */
	FTargetPair TargetPair;

	/** The QTable row index for the first target */
	int32 StateIndex;

	/** The QTable column index for the second target */
	int32 ActionIndex;

	/** The QTable row index of the second target */
	int32 StateIndex_2;

	/** The QTable column index for the index of maximum reward, starting from State_Index_2 */
	int32 ActionIndex_2;

	FQTableUpdateParams()
	{
		TargetPair = FTargetPair();
		StateIndex = -1;
		ActionIndex = -1;
		StateIndex_2 = -1;
		ActionIndex_2 = -1;
	}

	FQTableUpdateParams(const FTargetPair& InTargetPair)
	{
		TargetPair = InTargetPair;
		StateIndex = -1;
		ActionIndex = -1;
		StateIndex_2 = -1;
		ActionIndex_2 = -1;
	}
};

/** SpawnArea index to QTable Index Pair */
USTRUCT()
struct FSpawnAreaQTableIndexPair
{
	GENERATED_BODY()

	int32 QTableIndex;
	int32 SpawnAreaIndex;

	FSpawnAreaQTableIndexPair()
	{
		SpawnAreaIndex = INDEX_NONE;
		QTableIndex = INDEX_NONE;
	}

	FSpawnAreaQTableIndexPair(const int32 InSpawnAreaIndex, const int32 InQTableIndex)
	{
		SpawnAreaIndex = InSpawnAreaIndex;
		QTableIndex = InQTableIndex;
	}

	FORCEINLINE bool operator ==(const FSpawnAreaQTableIndexPair& Other) const
	{
		if (Other.QTableIndex == QTableIndex && Other.SpawnAreaIndex == SpawnAreaIndex)
		{
			return true;
		}
		return false;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSpawnAreaQTableIndexPair& Struct)
	{
		return HashCombine(GetTypeHash(Struct.QTableIndex), Struct.SpawnAreaIndex);
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnQTableUpdate, const TArray<float>& UpdatedQTable);

/** A struct to pass the component upon Initialization */
USTRUCT()
struct FRLAgentParams
{
	GENERATED_BODY()

	FBS_AIConfig AIConfig;
	float SpawnAreasHeight;
	float SpawnAreasWidth;
	int32 NumQTableRows;
	int32 NumQTableColumns;
	TArray<float> QTable;
	TArray<int32> TrainingSamples;
	int64 TotalTrainingSamples;

	FRLAgentParams()
	{
		AIConfig = FBS_AIConfig();
		QTable = TArray<float>();
		TrainingSamples = TArray<int32>();
		SpawnAreasHeight = 0.f;
		SpawnAreasWidth = 0.f;
		TotalTrainingSamples = 0;
		NumQTableRows = 0;
		NumQTableColumns = 0;
	}

	FRLAgentParams(const FBS_AIConfig& InAIConfig, const float InSpawnAreaHeight, const float InSpawnAreaWidth,
		const TArray<float>& InQTable, const int32 InNumQTableRows, const int32 InNumQTableColumns,
		const TArray<int32>& InTrainingSamples, const int32 InTotalTrainingSamples)
	{
		AIConfig = InAIConfig;
		SpawnAreasHeight = InSpawnAreaHeight;
		SpawnAreasWidth = InSpawnAreaWidth;
		QTable = InQTable;
		NumQTableRows = InNumQTableRows;
		NumQTableColumns = InNumQTableColumns;
		TrainingSamples = InTrainingSamples;
		TotalTrainingSamples = InTotalTrainingSamples;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEATSHOT_API UReinforcementLearningComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UReinforcementLearningComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "ReinforcementLearningComponent")
	UCompositeCurveTable* CompositeCurveTable_HyperParameters;

public:
	virtual void DestroyComponent(bool bPromoteChildren) override;

	/** Initializes the QTable, called by TargetManager */
	void Init(const FRLAgentParams& AgentParams);

	/** Adds two activated spawned targets to ActiveTargetPairs */
	void AddToActiveTargetPairs(const int32 SpawnAreaIndex_First, const int32 SpawnAreaIndex_Second);

	/** Updates a TargetPair's reward based on if hit or not. SpawnAreaIndex corresponds to TargetPair.Second.
	 *  Removes from ActiveTargetPairs and adds to TargetPairs queue */
	void SetActiveTargetPairReward(const int32 SpawnAreaIndex, const bool bHit);

	/** Updates the QTable until TargetPairs queue is empty */
	void ClearCachedTargetPairs();

	/** Returns the SpawnCounter index of the next target to spawn, based on the Epsilon value */
	int32 ChooseNextActionIndex(const TArray<int32>& SpawnAreaIndices, const int32 PreviousSpawnAreaIndex) const;

private:
	/** Returns a random SpawnArea index from the provided SpawnAreaIndices */
	static int32 ChooseRandomActionIndex(const TArray<int32>& SpawnAreaIndices);

	/** Returns the SpawnArea index that leads to the greatest reward. Calls GetIndices_MaximizeFirst or
	 *  GetIndices_MaximizeSecond depending on the input previous index and iterates through the indices
	 *  until it finds a corresponding SpawnArea index from the input array. */
	int32 ChooseBestActionIndex(const TArray<int32>& SpawnAreaIndices, const int32 PreviousSpawnAreaIndex) const;

	/** Updates the QTable from the QTableUpdateParams */
	virtual void UpdateQTable(FQTableUpdateParams& UpdateParams);

public:
	/** Returns the number of rows or height */
	int32 GetNumQTableRows() const { return M; }

	/** Returns the number of columns or width */
	int32 GetNumQTableColumns() const { return N; }

	/** Returns the number of training samples the component has trained with this session */
	int64 GetTotalTrainingSamples() const { return TotalTrainingSamples; }

	/** The mode that the RLC is operating in */
	EReinforcementLearningMode GetReinforcementLearningMode() const { return ReinforcementLearningMode; }

	/** Returns a copy of the QTable */
	nc::NdArray<float> GetQTable() const { return QTable; }

	/** Returns the number of columns (Row Length) for the full QTable */
	int32 GetQTableRowLength() const { return GetQTable().numCols(); }

	/** Returns a TArray version of the full QTable */
	TArray<float> GetTArray_FromNdArray_QTable() const { return GetTArrayFromNdArray<float>(GetQTable()); }

	/** Returns a TArray version of the full TrainingSamples */
	TArray<int32> GetTArray_FromNdArray_TrainingSamples() const { return GetTArrayFromNdArray<int32>(TrainingSamples); }

	/** Returns a TArray version of the averaged flipped QTable, used to update widget */
	TArray<float> GetTArray_FromNdArray_QTableAvg() const;

	/** Returns a TArray version of the maximum flipped QTable, used to update widget */
	TArray<float> GetTArray_FromNdArray_QTableMax() const;

private:
	/** Returns an array of Second Location Indices where the each index represents a column that leads to the greatest reward */
	TArray<int32> GetIndices_MaximizeSecond(const int32 InPreviousIndex) const;

	/** Returns an array of First Location Indices where the each index represents a row that leads to the greatest reward */
	TArray<int32> GetIndices_MaximizeFirst() const;

	/** Converts a SpawnAreaIndex to a QTableIndex */
	int32 GetIndex_FromSpawnArea_ToQTable(const int32 SpawnAreaIndex) const;

	/** Returns all SpawnCounter indices corresponding to the QTableIndex */
	TArray<int32> GetSpawnAreaIndexRange(const int32 QTableIndex) const;

	/** Returns the first TargetPair with the matching CurrentIndex */
	FTargetPair FindTargetPairByCurrentIndex(const int32 InCurrentIndex);

public:
	/** Broadcasts OnQTableUpdate delegate */
	void UpdateQTableWidget() const;

	/** Prints the Q-Table to Unreal console */
	void PrintRewards() const;

	/** Prints the MaxIndices and MaxValues corresponding to the choices the component currently has */
	void PrintMaxAverageIndices() const;

	void PrintGetMaxIndex(const int32 PreviousIndex, const float MaxValue, const nc::NdArray<float>& PreviousRow,
		const nc::NdArray<unsigned>& ReverseSortedIndices) const;

	/** Delegate that broadcasts when the QTable is updated. Used to broadcast to widgets */
	FOnQTableUpdate OnQTableUpdate;

	/** Whether to broadcast the average or max QTable when broadcasting OnQTableUpdate */
	bool bBroadcastAverageOnQTableUpdate;

	/** Whether or not to print QTable updates to log */
	bool bPrintDebug_QTableUpdate;

	/** Whether or not to print finding the max index to log */
	bool bPrintDebug_GetMaxIndex;

	/** Whether or not to print finding the best action index to log */
	bool bPrintDebug_ChooseBestActionIndex;

private:
	/** The mode that the RLC is operating in */
	EReinforcementLearningMode ReinforcementLearningMode;

	/** Defines how to use the hyper-parameters Alpha, Gamma, and Epsilon */
	EReinforcementLearningHyperParameterMode HyperParameterMode;

	/** A 2D array where the row and column have size equal to the number of possible spawn points.
	 *  An element in the array represents the expected reward from starting at spawn location RowIndex
	 *  and spawning a target at ColumnIndex. Its a scaled down version of the SpawnArea where each
	 *  each point in Q-Table represents multiple points in a square area inside the SpawnArea */
	nc::NdArray<float> QTable;

	/** A 2D array that holds the number of updates at each QTable index */
	nc::NdArray<int32> TrainingSamples;

	/** Learning rate, or how much to update the Q-Table rewards when a reward is received */
	float Alpha;

	/** Discount factor, or how much to value future rewards vs immediate rewards */
	float Gamma;

	/** The exploration/exploitation balance factor. A value = 1 will result in only choosing random values (explore),
	 *  while a value of zero will result in only choosing the max Q-value (exploitation) */
	float Epsilon;

	/** Number of rows of the QTable */
	int32 M;

	/** Number of columns of the QTable */
	int32 N;

	/** An array of structs where each element represents a unique SpawnArea index and QTable index */
	TArray<FSpawnAreaQTableIndexPair> SpawnAreaToQTableIndexMap;

	/** A map where each key is a QTable row/column that maps to multiple SpawnArea indices */
	TMap<int32, FGenericIndexMapping> QTableToSpawnAreaIndexMap;

	/** An FIFO queue of (PreviousLocation, NextLocation) that represent destroyed or timed out targets */
	TQueue<FTargetPair> TargetPairs;

	/** An array of (PreviousLocation, NextLocation), where NextLocation has not been destroyed or expired.
	 *  Added directly after being spawned or activated, removed and added to TargetPairs queue upon being destroyed */
	TArray<FTargetPair> ActiveTargetPairs;

	FNumberFormattingOptions IntegerFormatting;
	FNumberFormattingOptions FloatFormatting;

	/** The number of samples collected starting from when the component was activated */
	int64 TotalTrainingSamples;
};
