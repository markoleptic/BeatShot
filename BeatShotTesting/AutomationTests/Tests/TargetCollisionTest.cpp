// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"

IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTargetCollisionTest, FTargetManagerTestWithWorld,
	"TargetManager.TargetCollision", EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
	EAutomationTestFlags:: HighPriorityAndAbove | EAutomationTestFlags::EngineFilter);

void FTargetCollisionTest::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	GameModeDataAssetPath = "/Game/Blueprints/GameModes/DA_CollisionTest.DA_CollisionTest";
	UObject* LoadedObject = StaticLoadObject(UBSGameModeDataAsset::StaticClass(), nullptr, *GameModeDataAssetPath);
	if (LoadedObject)
	{
		GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
		if (GameModeDataAsset)
		{
			for (const auto& Mode : GameModeDataAsset->GetDefaultGameModesMap())
			{
				OutBeautifiedNames.Add(Mode.Key.CustomGameModeName);
				OutTestCommands.Add(Mode.Key.CustomGameModeName);
			}
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

	const FPlayerSettings_Game GameSettings;
	int32 TotalTargetsSpawned = 0;
	int32 TotalIntersections = 0;
	double Min = 5000.f;
	const FBS_DefiningConfig Def(EGameModeType::Custom, EBaseGameMode::None, Parameters, EGameModeDifficulty::None);
	auto FoundConfig = GameModeDataAsset->GetDefaultGameModesMap().Find(Def);
	if (!FoundConfig)
	{
		AddError("Failed to find a Custom Game Mode named CollisionTest");
		return false;
	}
	
	BSConfig = MakeShareable(new FBSConfig(*FoundConfig));
	TargetManager->Init(BSConfig, GameSettings);
	TargetManager->SetShouldSpawn(true);
	
	for (int Iter = 0; Iter < 500; Iter++)
	{
		TargetManager->OnAudioAnalyzerBeat();
		TickWorld(UE_KINDA_SMALL_NUMBER);
		TMap<FGuid, ATarget*> ManagedTargets = GetManagedTargets();
		TotalTargetsSpawned += ManagedTargets.Num();
		
		TArray<FSphere> Spheres;
		for (auto [Guid, Target] : ManagedTargets)
		{
			Spheres.Emplace(FSphere(Target->GetActorLocation(),
				Target->GetTargetScale_Current().X * SphereTargetRadius));
			Target->DamageSelf(true);
			TickWorld(UE_KINDA_SMALL_NUMBER);
		}
		for (int i = 0; i < Spheres.Num(); i++)
		{
			for (int j = i + 1; j < Spheres.Num(); j++)
			{
				const double CenterDist = FVector::Dist(Spheres[i].Center, Spheres[j].Center);
				const double RadiusSum = Spheres[i].W + Spheres[j].W;
				const bool bIntersects = CenterDist <= RadiusSum;
				if (bIntersects) TotalIntersections++;
				
				Min = FMath::Min(Min, CenterDist - RadiusSum);

				FString What = FString::Printf(TEXT("Sphere at (%.2f, %.2f) w/ R=%.2lf "
										"intersecting sphere at (%.2f, %.2f) w/ R=%.2lf by %.2lf"),
					Spheres[i].Center.Y, Spheres[i].Center.Z, Spheres[i].W,
					Spheres[j].Center.Y, Spheres[j].Center.Z,  Spheres[j].W, RadiusSum - CenterDist);
				
				TestFalse(What, bIntersects);
			}
		}
		TestTrue("All managed targets destroyed at iteration end", GetManagedTargets().IsEmpty());
		TickWorld(UE_KINDA_SMALL_NUMBER);
	}
	
	TargetManager->Clear();
	
	AddInfo(FString::Printf(TEXT("Min Distance between two spheres: %.4lf"), Min));
	AddInfo(FString::Printf(TEXT("Total time spent executing TimeSpentInSpawnableSpawnAreas: %.4lf"),
		TimeSpentInSpawnableSpawnAreas));
	AddInfo(FString::Printf(TEXT("Total targets spawned: %d"), TotalTargetsSpawned));
	AddInfo(FString::Printf(TEXT("Total intersections: %d"), TotalIntersections));
	
	return true;
}
