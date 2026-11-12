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

/** Returns the number of non-zero values in the array. */
template<typename T>
int32 CountNonZero(const TArray<T>& Values)
{
	int32 Count = 0;
	for (const T& Current : Values)
	{
		if (Current != 0)
		{
			Count++;
		}
	}
	return Count;
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableSerializationRoundTripTest,
                                 TEXT("BeatShot.RLQTable.SerializationRoundTrip"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableSerializationRoundTripTest::RunTest(const FString& Parameters)
{
	// Learn two distinct cells (a hit at (2,5) and a miss at (3,7)), persist through FCommonScoreInfo,
	// then reload into a fresh agent and assert the exact table state round-trips.
	UReinforcementLearningComponent* Agent = CreateInitializedAgent();

	Agent->AddToActiveTargetPairs(2, 5);
	Agent->SetActiveTargetPairReward(5, true);       // hit  -> QTable(2,5) = -1, 1 sample
	Agent->AddToActiveTargetPairs(3, 7);
	Agent->SetActiveTargetPairReward(7, false);      // miss -> QTable(3,7) = +1, 1 sample

	// Persist the live table via the same production method used when saving a score, then discard the source agent.
	FCommonScoreInfo SavedInfo;
	SavedInfo.UpdateQTable(Agent->GetTArray_FromNdArray_QTable(),
	                      Agent->GetNumQTableRows(),
	                      Agent->GetNumQTableColumns(),
	                      Agent->GetTArray_FromNdArray_TrainingSamples(),
	                      static_cast<int32>(Agent->GetTotalTrainingSamples()));
	DestroyAgent(Agent);

	FBS_AIConfig AIConfig;
	AIConfig.Alpha = 1.f;
	AIConfig.Gamma = 0.f;
	AIConfig.Epsilon = 0.f;
	FRLAgentParams NewParams(AIConfig, SavedInfo, FIntVector3(0, 5, 5));
	UReinforcementLearningComponent* Reloaded = NewObject<UReinforcementLearningComponent>(GetTransientPackage());
	Reloaded->AddToRoot();
	Reloaded->Init(NewParams);

	const nc::NdArray<float> ReloadedQTable = Reloaded->GetQTable();

	UTEST_EQUAL("rows preserved", Reloaded->GetNumQTableRows(), 25);
	UTEST_EQUAL("cols preserved", Reloaded->GetNumQTableColumns(), 25);
	UTEST_EQUAL("size preserved", static_cast<int32>(ReloadedQTable.size()), 625);
	UTEST_EQUAL("re-loaded QTable(2,5) matches", ReloadedQTable(2, 5), -1.f);
	UTEST_EQUAL("re-loaded QTable(3,7) matches", ReloadedQTable(3, 7), 1.f);
	UTEST_EQUAL("re-loaded QTable(0,0) zero", ReloadedQTable(0, 0), 0.f);
	UTEST_EQUAL("exactly two non-zero re-loaded training samples", CountNonZero(Reloaded->GetTArray_FromNdArray_TrainingSamples()), 2);
	UTEST_EQUAL("re-loaded total training samples", static_cast<int32>(Reloaded->GetTotalTrainingSamples()), 2);

	DestroyAgent(Reloaded);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableChooseLearnedActionTest,
                                 TEXT("BeatShot.RLQTable.ChoosesLearnedAction"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableChooseLearnedActionTest::RunTest(const FString& Parameters)
{
	// BeatShot is a deliberate-weakness trainer: the RL agent should prefer next-spawns that produced a MISS
	// (higher Q-value, Reward = +1) over the ones that produced a HIT (lower Q-value, Reward = -1). Learn two
	// competing outcomes from the same previous spawn, then in exploit mode (Epsilon = 0) assert the chooser
	// returns the miss cell.
	FBS_AIConfig AIConfig;
	AIConfig.Alpha = 1.f;
	AIConfig.Gamma = 0.f;
	AIConfig.Epsilon = 0.f;
	AIConfig.ReinforcementLearningMode = EReinforcementLearningMode::Exploration;
	FRLAgentParams Params(AIConfig, FCommonScoreInfo(), FIntVector3(0, 5, 5)); // identity: spawn i -> QTable cell i

	UReinforcementLearningComponent* Agent = NewObject<UReinforcementLearningComponent>(GetTransientPackage());
	Agent->AddToRoot();
	Agent->Init(Params);

	// Two next-spawns from the same previous state: 5 was a hit, 6 was a miss.
	Agent->AddToActiveTargetPairs(2, 5);   // hit  -> QTable(2,5) = -1 (lower)
	Agent->SetActiveTargetPairReward(5, true);
	Agent->AddToActiveTargetPairs(2, 6);   // miss -> QTable(2,6) = +1 (higher)
	Agent->SetActiveTargetPairReward(6, false);

	const TArray<int32> Candidates = { 5, 6 };
	const int32 Chose = Agent->ChooseNextActionIndex(2, Candidates);

	UTEST_EQUAL("exploit chooser prefers the miss (higher Q-value) cell", Chose, 6);

	DestroyAgent(Agent);
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRLQTableSerializationSizeGuardTest,
                                 TEXT("BeatShot.RLQTable.SerializationSizeGuard"),
                                 EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                 EAutomationTestFlags::EngineFilter)
bool FRLQTableSerializationSizeGuardTest::RunTest(const FString& Parameters)
{
	// Simulate the "valid-shape but empty-array" save that Clear() can produce: NumQTableRows/Columns claim 25x25
	// but the underlying arrays have 0 elements. Init must resolve to a clean zero 25x25 (no corruption),
	// while TotalTrainingSamples still carries over unconditionally (documenting the asymmetry).
	FCommonScoreInfo MalformedInfo;
	MalformedInfo.QTable.Empty();
	MalformedInfo.TrainingSamples.Empty();
	MalformedInfo.NumQTableRows = 25;
	MalformedInfo.NumQTableColumns = 25;
	MalformedInfo.TotalTrainingSamples = 7;

	FBS_AIConfig AIConfig;
	AIConfig.Alpha = 1.f;
	AIConfig.Gamma = 0.f;
	AIConfig.Epsilon = 0.f;
	FRLAgentParams NewParams(AIConfig, MalformedInfo, FIntVector3(0, 5, 5));
	UReinforcementLearningComponent* Reloaded = NewObject<UReinforcementLearningComponent>(GetTransientPackage());
	Reloaded->AddToRoot();
	Reloaded->Init(NewParams);

	const nc::NdArray<float> ReloadedQTable = Reloaded->GetQTable();

	UTEST_EQUAL("rows still 25", Reloaded->GetNumQTableRows(), 25);
	UTEST_EQUAL("cols still 25", Reloaded->GetNumQTableColumns(), 25);
	UTEST_EQUAL("size resolved to 625", static_cast<int32>(ReloadedQTable.size()), 625);
	UTEST_TRUE("QTable resolved to clean zeros", AreAllZero(Reloaded->GetTArray_FromNdArray_QTable()));
	UTEST_TRUE("TrainingSamples resolved to clean zeros", AreAllZero(Reloaded->GetTArray_FromNdArray_TrainingSamples()));
	UTEST_EQUAL("total training samples carried over", static_cast<int32>(Reloaded->GetTotalTrainingSamples()), 7);

	DestroyAgent(Reloaded);
	return true;
}
