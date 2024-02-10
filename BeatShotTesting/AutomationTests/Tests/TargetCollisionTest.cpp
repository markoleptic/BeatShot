// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"
#include "Tests/AutomationCommon.h"

IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTargetCollisionTest, FTargetManagerTestWithWorld,
	"TargetManager.TargetCollision", EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
	EAutomationTestFlags:: HighPriorityAndAbove | EAutomationTestFlags::EngineFilter);

void FTargetCollisionTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	OutBeautifiedNames.Add("Collision");
	OutTestCommands.Add("Collision");
}

bool FTargetCollisionTest::RunTest(const FString& Parameters)
{
	if (!bInitialized)
	{
		SetGameModeDataAssetPath("/Game/Blueprints/GameModes/DA_CollisionTest.DA_CollisionTest");
		if (!Init())
		{
			return false;
		}
	}

	const FBS_DefiningConfig Def(EGameModeType::Custom, EBaseGameMode::None, Parameters, EGameModeDifficulty::None);
	BSConfig = MakeShareable(new FBSConfig(GameModeDataAsset->GetDefaultGameModesMap().FindRef(Def)));
	const FPlayerSettings_Game GameSettings;
	int32 TotalTargetsSpawned = 0;
	
	for (int Iter = 0; Iter < 50; Iter++)
	{
		TargetManager->Init(BSConfig, GameSettings);
		TargetManager->SetShouldSpawn(true);
		TargetManager->OnAudioAnalyzerBeat();

		ADD_LATENT_AUTOMATION_COMMAND(FWaitForNextEngineFrameCommand());
		
		TMap<FGuid, ATarget*> ManagedTargets = GetManagedTargets();
		TotalTargetsSpawned += ManagedTargets.Num();
		
		TArray<FSphere> Spheres;
		for (auto Target : GetManagedTargets())
		{
			Spheres.Emplace(FSphere(Target.Value->GetActorLocation(), Target.Value->GetTargetScale_Current().X * SphereTargetRadius));
		}
		for (int i = 0; i < Spheres.Num(); i++)
		{
			for (int j = i + 1; j < Spheres.Num(); j++)
			{
				FString What = FString::Printf(TEXT("Sphere at %s with radius %.2lf intersecting sphere at %s with radius %.2lf"),
					*Spheres[i].Center.ToCompactString(), Spheres[i].W, *Spheres[j].Center.ToCompactString(), Spheres[j].W);
				TestFalse(What, Spheres[i].Intersects(Spheres[j]));
			}
		}

		TargetManager->Clear();
		ADD_LATENT_AUTOMATION_COMMAND(FWaitForNextEngineFrameCommand());
	}
	
	AddInfo(FString::Printf(TEXT("Total targets spawned with no intersection: %d"), TotalTargetsSpawned));
	
	return true;
}
