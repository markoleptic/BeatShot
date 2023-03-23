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
		Reward = -1;
	}

	FAlgoInput(const int32 InStateIndex,  const int32 InActionIndex, const int32 InStateIndex_2, const int32 InActionIndex_2, float InReward)
	{
		StateIndex = InStateIndex;
		ActionIndex = InActionIndex;
		StateIndex_2 = InStateIndex_2;
		ActionIndex_2 = InActionIndex_2;
		Reward = InReward;
	}
};

UCLASS()
class BEATSHOT_API URLBase : public UObject, public ISaveLoadInterface
{
	GENERATED_BODY()

	URLBase();

	nc::NdArray<float> QTable;
	nc::NdArray<float> EpisodeRewards;
	FQTableWrapper QTableWrapper;
	
	float Alpha;
	float Gamma;
	float Epsilon;
	
	int32 ColSize;
	int32 RowSize;

public:
	void Init(EGameModeActorName GameModeActorName, const FString CustomGameModeName, const int32 Size, const float InAlpha, const float InGamma, const float InEpsilon);

	virtual void BeginDestroy() override;
	
	int32 GetNextActionIndex(const int32 Index) const;

	int32 GetMaxActionIndex(const int32 Index) const;

	virtual void UpdateQTable(const FAlgoInput& In);

	virtual void UpdateEpisodeRewards(const float RewardReceived);

	void PrintRewards();

	void SaveQTable();
};
