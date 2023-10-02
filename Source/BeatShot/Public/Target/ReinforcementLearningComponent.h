// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "BeatShot/BeatShot.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetMathLibrary.h"

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

/** A struct to pass the Agent upon Initialization */
USTRUCT()
struct FRLAgentParams
{
	GENERATED_BODY()

	FBS_AIConfig AIConfig;
	float SpawnAreasHeight;
	float SpawnAreasWidth;
	int32 Size;
	TArray<float> QTable;
	TArray<int32> TrainingSamples;
	int64 TotalTrainingSamples;
	
	FRLAgentParams()
	{
		AIConfig = FBS_AIConfig();
		Size = 0;
		QTable = TArray<float>();
		TrainingSamples = TArray<int32>();
		SpawnAreasHeight = 0.f;
		SpawnAreasWidth = 0.f;
		TotalTrainingSamples = 0;
		
	}

	FRLAgentParams(const FBS_AIConfig& InAIConfig, const int32 InSize, const float InHeight, const float InWidth,
		const TArray<float>& InQTable, const TArray<int32>& InTrainingSamples, const int32 InTotalTrainingSamples)
	{
		AIConfig = InAIConfig;
		SpawnAreasHeight = InHeight;
		SpawnAreasWidth = InWidth;
		Size = InSize;
		QTable = InQTable;
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

	/** Adds two consecutively spawned targets to ActiveTargetPairs immediately after NextWorldLocation has been spawned */
	void AddToActiveTargetPairs(const int32 PreviousIndex, const int32 CurrentIndex);

	/** Updates a TargetPair's reward based on if hit or not. SpawnAreaIndex corresponds to the CurrentIndex of a TargetPair. Removes from ActiveTargetPairs and adds to TargetPairs queue */
	void SetActiveTargetPairReward(const int32 SpawnAreaIndex, const bool bHit);

	/** Updates RLAgent's QTable until TargetPairs queue is empty */
	void ClearCachedTargetPairs();
	
	/** Returns the SpawnCounter index of the next target to spawn, based on the Epsilon value */
	int32 ChooseNextActionIndex(const TArray<int32>& SpawnAreaIndices) const;

private:
	/** Returns a random SpawnArea index from the provided SpawnAreaIndices */
	static int32 ChooseRandomActionIndex(const TArray<int32>& SpawnAreaIndices);

	/** First computes a reverse-sorted array containing QTable indices where the rewards is highest. Then checks to see if there is a valid SpawnCounter index that corresponds to the QTable index.
	 *  If there isn't, it goes to the next QTable index until there are no options left, in which case it returns INDEX_NONE. Returns a SpawnCounter index */
	int32 ChooseBestActionIndex(const TArray<int32>& SpawnAreaIndices) const;

	/** Updates a Q-Table element entry after a target has been spawned and either timed out or destroyed by player */
	virtual void UpdateQTable(const FAlgoInput& In);
	
public:
	/** Returns the number of columns or width */
	int32 GetWidth() const { return ScaledWidth; }
	
	/** Returns the number of rows or height */
	int32 GetHeight() const { return ScaledHeight; }

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
	
private:
	template<typename T>
	static TArray<T> GetTArrayFromNdArray(const nc::NdArray<T>& InArray);
	
	/** Converts a TArray of floats to an NdArray */
	template<typename T>
	static nc::NdArray<T> GetNdArrayFromTArray(const TArray<T>& InTArray);
	
	/** Returns the QTable index corresponding to the maximum reward from starting at QTableIndex, using a greedy approach. Used to update Q-Table, but not to get actual spawn locations */
	int32 GetIndex_MaxAction(const int32 SpawnAreaIndex) const;
	
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
	void PrintMaxAverageIndices(const nc::NdArray<unsigned>& MaxIndicesReverseSorted, const nc::NdArray<float>& MaxesReverseSorted) const;

	/** Delegate that broadcasts when the QTable is updated. Used to broadcast to widgets */
	FOnQTableUpdate OnQTableUpdate;

	/** Delegate that broadcasts when the component wants to check the validity of a SpawnArea index */
	FOnSpawnAreaValidityRequest OnSpawnAreaValidityRequest;
	
private:
	/** The mode that the RLC is operating in */
	EReinforcementLearningMode ReinforcementLearningMode;

	/** Defines how to use the hyper-parameters Alpha, Gamma, and Epsilon */
	EReinforcementLearningHyperParameterMode HyperParameterMode;
	
	/** A 2D array where the row and column have size equal to the number of possible spawn points. An element in the array represents the expected reward from starting at spawn location RowIndex
	 *  and spawning a target at ColumnIndex. Its a scaled down version of the SpawnCounter where each each point in Q-Table represents multiple points in a square area inside the SpawnCounter */
	nc::NdArray<float> QTable;

	nc::NdArray<int32> TrainingSamples;

	/** Learning rate, or how much to update the Q-Table rewards when a reward is received */
	float Alpha;
	
	/** Discount factor, or how much to value future rewards vs immediate rewards */
	float Gamma;
	
	/** The exploration/exploitation balance factor. A value = 1 will result in only choosing random values (explore),
	 *  while a value of zero will result in only choosing the max Q-value (exploitation) */
	float Epsilon;
	
	/** The number of rows in SpawnAreas array */
	int32 SpawnAreasHeight;

	/** The number of columns in SpawnAreas array */
	int32 SpawnAreasWidth;

	/** The size of the SpawnAreas array */
	int32 SpawnAreasSize;

	/** How many rows the SpawnAreasHeight is divided into */
	int32 ScaledHeight = 5;

	/** The number of the SpawnAreasWidth is divided into */
	int32 ScaledWidth = 5;

	/** The size of both dimensions of the QTable (ScaledHeight * ScaledWidth) */
	int32 ScaledSize;

	/** SpawnAreasHeight divided by ScaledHeight */
	int32 HeightScaleFactor;

	/** SpawnAreasWidth divided by ScaledWidth */
	int32 WidthScaleFactor;

	/** An array of structs where each element represents one QTable index that maps to multiple SpawnCounter indices */
	TArray<FQTableIndex> QTableIndices;

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

template <typename T>
TArray<T> UReinforcementLearningComponent::GetTArrayFromNdArray(const nc::NdArray<T>& InArray)
{
	const int32 RowSize = InArray.numRows();
	const int32 ColSize = InArray.numCols();
	
	TArray<T> Out;
	Out.Init(0.f, InArray.size());
	
	for(int j = 0; j < ColSize; j++)
	{
		for(int i = 0; i < RowSize; i++)
		{
			Out[RowSize * j + i] = InArray(i, j);
		}
	}
	return Out;
}

template <typename T>
nc::NdArray<T> UReinforcementLearningComponent::GetNdArrayFromTArray(const TArray<T>& InTArray)
{
	const int32 RowSize = UKismetMathLibrary::Sqrt(InTArray.Num());
	const int32 ColSize = RowSize;
	
	nc::NdArray<T> Out = nc::zeros<T>(RowSize, ColSize);
	
	for (int j = 0; j < ColSize; j++)
	{
		for (int i = 0; i < RowSize; i++)
		{
			Out(i, j) = InTArray[RowSize * j + i];
		}
	}
	return Out;
}
