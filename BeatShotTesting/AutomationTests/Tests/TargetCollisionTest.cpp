﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"

class FTargetCollisionTest : public FTargetManagerTestWithWorld
{
public:
	FTargetCollisionTest(const FString& InName) : FTargetManagerTestWithWorld(InName, true)
	{
		static_assert(
			(EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
				EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
			EAutomationTestFlags::ApplicationContextMask,
			"AutomationTest has no application flag.  It shouldn't run.  See AutomationTest.h.");
		static_assert(
			(((EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::SmokeFilter) || (((
					EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::EngineFilter) || (((
					EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::ProductFilter) || (((
					EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::PerfFilter) || (((
					EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::StressFilter) || (((
					EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
					EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) &
				EAutomationTestFlags::FilterMask) == EAutomationTestFlags::NegativeFilter),
			"All AutomationTests must have exactly 1 filter type specified.  See AutomationTest.h.");
	}

	virtual uint32 GetTestFlags() const override
	{
		return ((EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
			EAutomationTestFlags::HighPriorityAndAbove | EAutomationTestFlags::EngineFilter) & ~(
			EAutomationTestFlags::SmokeFilter));
	}
	virtual bool IsStressTest() const { return true; }
	virtual uint32 GetRequiredDeviceNum() const override { return 1; }
	virtual FString GetTestSourceFileName() const override { return "TargetCollisionTest.cpp"; }
	virtual int32 GetTestSourceFileLine() const override { return 7; }

protected:
	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const override;
	virtual bool RunTest(const FString& Parameters) override;
	virtual FString GetBeautifiedTestName() const override { return "TargetManager.TargetCollision"; }
	void TestIntersection(const FSphere& SphereOne, const FSphere& SphereTwo);
	void AddTestResultInfo();
	TArray<FSphere> GetSpheresFromTargets(const TMap<FGuid, ATarget*>& ManagedTargets);
	void ResetTestVariables();
	
	int32 TotalTargetsSpawned = 0;
	int32 TotalIntersections = 0;
	double MinDistance = 5000.f;
	const int32 NumIterations = 500;
};

namespace
{
	FTargetCollisionTest FTargetCollisionTestAutomationTestInstance(L"FTargetCollisionTest");
}

void FTargetCollisionTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	if (InitGameModeDataAsset("/Game/Blueprints/GameModes/DA_CollisionTest.DA_CollisionTest"))
	{
		for (const auto& Mode : GameModeDataAsset->GetDefaultGameModesMap())
		{
			OutBeautifiedNames.Add(Mode.Key.CustomGameModeName);
			OutTestCommands.Add(Mode.Key.CustomGameModeName);
			TestMap.Add(Mode.Key.CustomGameModeName, Mode.Value);
		}
	}
}

bool FTargetCollisionTest::RunTest(const FString& Parameters)
{
	if (!bInitialized)
	{
		if (!Init())
		{
			return false;
		}
	}
	
	const auto FoundConfig = TestMap.Find(Parameters);
	if (!FoundConfig)
	{
		AddError(FString::Printf(TEXT("Failed to find Config for Parameters: %s"), *Parameters));
		return false;
	}
	
	BSConfig = MakeShareable(new FBSConfig(*FoundConfig));
	TargetManager->Init(BSConfig, FPlayerSettings_Game());
	TargetManager->SetShouldSpawn(true);
	
	for (int Iter = 0; Iter < NumIterations; Iter++)
	{
		TargetManager->OnAudioAnalyzerBeat();
		const auto&& ManagedTargets = GetManagedTargets();
		TotalTargetsSpawned += ManagedTargets.Num();
		const auto&& Spheres = GetSpheresFromTargets(ManagedTargets);
		for (int i = 0; i < Spheres.Num(); i++)
		{
			for (int j = i + 1; j < Spheres.Num(); j++)
			{
				TestIntersection(Spheres[i], Spheres[j]);
			}
		}
		TestTrue("All targets destroyed at iteration end", GetManagedTargets().IsEmpty());
	}
	
	TargetManager->Clear();
	AddTestResultInfo();
	ResetTestVariables();
	
	return true;
}

TArray<FSphere> FTargetCollisionTest::GetSpheresFromTargets(const TMap<FGuid, ATarget*>& ManagedTargets)
{
	TArray<FSphere> Spheres;
	for (auto [Guid, Target] : ManagedTargets)
	{
		Spheres.Emplace(FSphere(Target->GetActorLocation(), Target->GetTargetScale_Current().X * SphereTargetRadius));
		Target->DamageSelf(true);
		TickWorld(UE_KINDA_SMALL_NUMBER);
	}
	return Spheres;
}

void FTargetCollisionTest::TestIntersection(const FSphere& SphereOne, const FSphere& SphereTwo)
{
	const double CenterDist = FVector::Dist(SphereOne.Center, SphereTwo.Center);
	const double RadiusSum = SphereOne.W + SphereTwo.W;
	const bool bIntersects = CenterDist <= RadiusSum;
	if (bIntersects) TotalIntersections++;

	const double Dist = CenterDist - RadiusSum;
	MinDistance = FMath::Min(MinDistance, Dist);

	const FString What = FString::Printf(
		TEXT("Sphere at (%.2f, %.2f) w/ R=%.2lf intersecting sphere at (%.2f, %.2f) w/ R=%.2lf by %.2lf"),
		SphereOne.Center.Y, SphereOne.Center.Z, SphereOne.W, SphereTwo.Center.Y, SphereTwo.Center.Z, SphereTwo.W, Dist);
	TestFalse(What, bIntersects);
}

void FTargetCollisionTest::AddTestResultInfo()
{
	AddInfo(FString::Printf(TEXT("Min Distance between two spheres: %.4lf"), MinDistance));
	AddInfo(FString::Printf(TEXT("Total time spent executing TimeSpentInSpawnableSpawnAreas: %.4lf"),
		TimeSpentInSpawnableSpawnAreas));
	AddInfo(FString::Printf(TEXT("Total targets spawned: %d"), TotalTargetsSpawned));
	AddInfo(FString::Printf(TEXT("Total intersections: %d"), TotalIntersections));
}

void FTargetCollisionTest::ResetTestVariables()
{
	TotalTargetsSpawned = 0;
	TotalIntersections = 0;
	MinDistance = 5000.f;
}