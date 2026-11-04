// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "BSGameModeConfig/AIConfig.h"
#include "Score/CommonScoreInfo.h"
#include "Target/ReinforcementLearningComponent.h"

namespace
{
/** Alpha = 1, Gamma = 0: the pair reward is written into the QTable verbatim, with no lookahead.
 *  SpawnAreaSize (0, 5, 5) yields the identity 5x5 SpawnArea to QTable mapping. */
UReinforcementLearningComponent* CreateInitializedAgent()
{
	FBS_AIConfig AIConfig;
	AIConfig.Alpha = 1.f;
	AIConfig.Gamma = 0.f;
	AIConfig.Epsilon = 0.f;

	UReinforcementLearningComponent* Agent = NewObject<UReinforcementLearningComponent>(GetTransientPackage());
	Agent->AddToRoot();
	Agent->Init(FRLAgentParams(AIConfig, FCommonScoreInfo(), FIntVector3(0, 5, 5)));
	return Agent;
}

void DestroyAgent(UReinforcementLearningComponent* Agent)
{
	Agent->RemoveFromRoot();
	Agent->MarkAsGarbage();
}

template<typename T>
bool AreAllZero(const TArray<T>& Values)
{
	for (const T& Current : Values)
	{
		if (Current != 0)
		{
			return false;
		}
	}
	return true;
}

/** True when exactly one cell equals Value and every other cell is zero. */
bool HasSingleValue(const TArray<float>& Values, const float Value)
{
	int32 Count = 0;
	for (const float Current : Values)
	{
		if (Current == Value)
		{
			Count++;
		}
		else if (Current != 0.f)
		{
			return false;
		}
	}
	return Count == 1;
}

/** True when exactly one sample count is 1 and every other count is zero. */
bool HasSingleSample(const TArray<int32>& Values)
{
	int32 Count = 0;
	for (const int32 Current : Values)
	{
		if (Current == 1)
		{
			Count++;
		}
		else if (Current != 0)
		{
			return false;
		}
	}
	return Count == 1;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableInitTest,
                                 TEXT("BeatShot.RLQTable.Init"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableInitTest::RunTest(const FString& Parameters)
{
	UReinforcementLearningComponent* Agent = CreateInitializedAgent();

	const nc::NdArray<float> QTable = Agent->GetQTable();

	UTEST_EQUAL("num rows", Agent->GetNumQTableRows(), 25);
	UTEST_EQUAL("num columns", Agent->GetNumQTableColumns(), 25);
	UTEST_EQUAL("row length", Agent->GetQTableRowLength(), 25);
	UTEST_EQUAL("table size", (int32)QTable.size(), 625);
	UTEST_TRUE("QTable starts all zero", AreAllZero(Agent->GetTArray_FromNdArray_QTable()));
	UTEST_TRUE("training samples start all zero", AreAllZero(Agent->GetTArray_FromNdArray_TrainingSamples()));
	UTEST_EQUAL("total training samples", static_cast<int32>(Agent->GetTotalTrainingSamples()), 0);

	DestroyAgent(Agent);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableRewardHitTest,
                                 TEXT("BeatShot.RLQTable.RewardHit"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableRewardHitTest::RunTest(const FString& Parameters)
{
	UReinforcementLearningComponent* Agent = CreateInitializedAgent();

	// Reward is set on the pair whose Second is the reported SpawnArea index, then applied immediately.
	Agent->AddToActiveTargetPairs(2, 5);
	Agent->SetActiveTargetPairReward(5, true);

	const nc::NdArray<float> QTable = Agent->GetQTable();

	UTEST_EQUAL("hit reward written at (2, 5)", QTable(2, 5), -1.f);
	UTEST_TRUE("hit reward is the only non-zero cell", HasSingleValue(Agent->GetTArray_FromNdArray_QTable(), -1.f));
	UTEST_TRUE("exactly one training sample recorded", HasSingleSample(Agent->GetTArray_FromNdArray_TrainingSamples()));
	UTEST_EQUAL("total training samples", static_cast<int32>(Agent->GetTotalTrainingSamples()), 1);

	DestroyAgent(Agent);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableRewardMissTest,
                                 TEXT("BeatShot.RLQTable.RewardMiss"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableRewardMissTest::RunTest(const FString& Parameters)
{
	UReinforcementLearningComponent* Agent = CreateInitializedAgent();

	Agent->AddToActiveTargetPairs(3, 7);
	Agent->SetActiveTargetPairReward(7, false);

	const nc::NdArray<float> QTable = Agent->GetQTable();

	UTEST_EQUAL("miss reward written at (3, 7)", QTable(3, 7), 1.f);
	UTEST_TRUE("miss reward is the only non-zero cell", HasSingleValue(Agent->GetTArray_FromNdArray_QTable(), 1.f));
	UTEST_TRUE("exactly one training sample recorded", HasSingleSample(Agent->GetTArray_FromNdArray_TrainingSamples()));
	UTEST_EQUAL("total training samples", static_cast<int32>(Agent->GetTotalTrainingSamples()), 1);

	DestroyAgent(Agent);
	return true;
}
