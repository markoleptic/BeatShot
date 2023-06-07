// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "SpawnPointManagerComponent.h"
#include "BeatShot/BeatShot.h"
#include "Components/ActorComponent.h"

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

	FAlgoInput(const int32 InStateIndex, const int32 InActionIndex, const int32 InStateIndex_2, const int32 InActionIndex_2, const float InReward)
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

	EBaseGameMode DefaultMode;
	FString CustomGameModeName;
	int32 Size;
	TArray<float> InQTable;
	float SpawnPointsHeight;
	float SpawnPointsWidth;
	float InAlpha;
	float InGamma;
	float InEpsilon;

	FRLAgentParams()
	{
		DefaultMode = EBaseGameMode::None;
		CustomGameModeName = "";
		Size = 0;
		InQTable = TArray<float>();
		SpawnPointsHeight = 0.f;
		SpawnPointsWidth = 0.f;
		InAlpha = 0.f;
		InGamma = 0.f;
		InEpsilon = 0.f;
	}
	
};

/** A struct each each element represents one QTable index mapping to multiple SpawnCounter indices */
USTRUCT()
struct FQTableIndex
{
	GENERATED_BODY()

	int32 QTableIndex;
	TArray<int32> SpawnPointIndices;

	FQTableIndex()
	{
		QTableIndex = INDEX_NONE;
		SpawnPointIndices = TArray<int32>();
	}
	FQTableIndex(const int32 InQTableIndex)
	{
		QTableIndex = InQTableIndex;
		SpawnPointIndices = TArray<int32>();
	}

	FORCEINLINE bool operator ==(const FQTableIndex& Other) const
	{
		if (Other.QTableIndex == QTableIndex)
		{
			return true;
		}
		return false;
	}
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BEATSHOT_API UReinforcementLearningComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UReinforcementLearningComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Initializes the QTable, called by TargetManager */
	void Init(const FRLAgentParams& AgentParams);

	/** Updates a Q-Table element entry after a target has been spawned and either timed out or destroyed by player */
	virtual void UpdateQTable(const FAlgoInput& In);

	/** Updates EpisodeRewards by appending to the array */
	virtual void UpdateEpisodeRewards(const float RewardReceived);
	
	/** Returns the QTable index corresponding to the maximum reward from starting at QTableIndex, using a greedy approach. Used to update Q-Table, but not to get actual spawn locations */
	int32 GetMaxActionIndex(const int32 SpawnCounterIndex) const;

	/** Returns the SpawnCounter index of the next target to spawn, based on the Epsilon value */
	int32 ChooseNextActionIndex(const TArray<int32>& SpawnCounterIndices) const;

	/** Prints the Q-Table to Unreal console */
	void PrintRewards() const;

	int32 GetWidth() const { return ScaledWidth; }
	int32 GetHeight() const { return ScaledHeight; }

	/** Returns a TArray version of the averaged flipped QTable, used to update widget */
	TArray<float> GetAveragedTArrayFromQTable() const;

	/** Returns a TArray version of the full QTable */
	TArray<float> GetSaveReadyQTable() const;

	/** Returns a copy of the QTable */
	nc::NdArray<float> GetQTable() const;

	/** Delegate that broadcasts when the QTable is updated */
	FOnQTableUpdate OnQTableUpdate;

	/** Updates a TargetPair's reward based on if hit or not. Removes from ActiveTargetPairs and adds to TargetPairs queue */
	void UpdateReinforcementLearningReward(const FVector& WorldLocation, const bool bHit);

	/** Adds two consecutively spawned targets to ActiveTargetPairs immediately after NextWorldLocation has been spawned */
	void AddToActiveTargetPairs(const FVector& PreviousWorldLocation, const FVector& NextWorldLocation);

	/** Updates RLAgent's QTable until TargetPairs queue is empty */
	void UpdateReinforcementLearningComponent(const USpawnPointManagerComponent* SpawnPointManager);

private:
	/** Returns a random SpawnPoint index from the provided SpawnPointIndices */
	int32 ChooseRandomActionIndex(const TArray<int32>& SpawnCounterIndices) const;

	/** First computes a reverse-sorted array containing QTable indices where the rewards is highest. Then checks to see if there is a valid SpawnCounter index that corresponds to the QTable index.
	 *  If there isn't, it goes to the next QTable index until there are no options left, in which case it returns INDEX_NONE. Returns a SpawnCounter index */
	int32 ChooseBestActionIndex(const TArray<int32>& SpawnCounterIndices) const;

	/** Converts a SpawnCounterIndex to a QTableIndex */
	int32 GetQTableIndexFromSpawnPointIndex(const int32 SpawnCounterIndex) const;

	/** Returns all SpawnCounter indices corresponding to the QTableIndex */
	TArray<int32> GetSpawnPointIndexRange(const int32 QTableIndex) const;
	
	/** Converts a TArray of floats to an NdArray */
	nc::NdArray<float> GetQTableFromTArray(const TArray<float>& InTArray) const;

	/** Converts an NdArray of floats to a TArray of floats, so that it can be serialized and saved */
	static TArray<float> GetTArrayFromQTable(const nc::NdArray<float>& InQTable);

	/** Converts an NdArray of floats to a TArray of floats, so that it can be passed to widget */
	static TArray<float> GetTArrayFromQTable(const nc::NdArray<double>& InQTable);

	/** Broadcasts OnQTableUpdate delegate */
	void UpdateQTableWidget() const;

	/** A 2D array where the row and column have size equal to the number of possible spawn points. An element in the array represents the expected reward from starting at spawn location RowIndex
	 *  and spawning a target at ColumnIndex. Its a scaled down version of the SpawnCounter where each each point in Q-Table represents multiple points in a square area inside the SpawnCounter */
	nc::NdArray<float> QTable;

	/** An array that accumulates the current episodes rewards. Not used for gameplay events */
	nc::NdArray<float> EpisodeRewards;

	/** Learning rate, or how much to update the Q-Table rewards when a reward is received */
	float Alpha;
	
	/** Discount factor, or how much to value future rewards vs immediate rewards */
	float Gamma;
	
	/** The exploration/exploitation balance factor. A value = 1 will result in only choosing random values (explore),
	 *  while a value of zero will result in only choosing the max Q-value (exploitation) */
	float Epsilon;
	
	/** The number of rows in SpawnCounter array */
	int32 SpawnPointsHeight;

	/** The number of columns in SpawnCounter array */
	int32 SpawnPointsWidth;

	/** The size of the SpawnCounter array */
	int32 SpawnPointsSize;

	/** How many rows the SpawnPointsHeight is divided into */
	int32 ScaledHeight = 5;

	/** The number of the SpawnPointsWidth is divided into */
	int32 ScaledWidth = 5;

	/** The size of both dimensions of the QTable (ScaledHeight * ScaledWidth) */
	int32 ScaledSize;

	/** SpawnPointsHeight divided by ScaledHeight */
	int32 HeightScaleFactor;

	/** SpawnPointsWidth divided by ScaledWidth */
	int32 WidthScaleFactor;

	/** An array of structs where each element represents one QTable index that maps to multiple SpawnCounter indices */
	TArray<FQTableIndex> QTableIndices;

	/** An FIFO queue of (PreviousLocation, NextLocation) that represent destroyed or timed out targets */
	TQueue<FTargetPair> TargetPairs;

	/** An array of (PreviousLocation, NextLocation), where NextLocation has not been destroyed or expired.
	 *  Added directly after being spawned or activated, removed and added to TargetPairs queue upon being destroyed */
	TArray<FTargetPair> ActiveTargetPairs;
};
