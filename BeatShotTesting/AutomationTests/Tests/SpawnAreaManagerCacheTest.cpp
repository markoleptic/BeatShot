// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "SaveGames/SaveGamePlayerScore.h"
#include "SaveGames/SaveGamePlayerSettings.h"
#include "Target/SpawnAreaManagerComponent.h"
#include "Target/Target.h"
#include "Target/TargetManager.h"

IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestFlagCache, FTargetManagerTestWithWorld,
                                         "BeatShot.SpawnAreaManager.FlagCache",
                                         EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
                                         EAutomationTestFlags_HighPriorityAndAbove | EAutomationTestFlags::
                                         ProductFilter);

void FTestFlagCache::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	if (InitGameModeDataAsset(TargetManagerTestHelpers::DefaultGameModeDataAssetPath))
	{
		for (const auto& Mode : GameModeDataAsset->GetGameModesMap())
		{
			const FString GameModeString = UEnum::GetDisplayValueAsText(Mode.Key.BaseGameMode).ToString();
			OutBeautifiedNames.Add(GameModeString);
			OutTestCommands.Add(GameModeString);
			TestMap.Add(GameModeString, Mode.Value);
		}
	}
}

bool FTestFlagCache::RunTest(const FString& Parameters)
{
	if (!Init())
	{
		return false;
	}

	const auto FoundConfig = TestMap.Find(Parameters);
	if (!FoundConfig)
	{
		AddError(FString::Printf(TEXT("Failed to find Config for Parameters: %s"), *Parameters));
		return false;
	}

	BSConfig = MakeShared<FBSConfig>(*FoundConfig);
	TargetManager->Init(BSConfig, FCommonScoreInfo(), FPlayerSettings_Game());

	USpawnAreaManagerComponent* Manager = GetSpawnAreaManager();

	const FIntVector3 SpawnAreaSize = Manager->GetSpawnAreaSize();
	const int32 NumSpawnAreas = SpawnAreaSize.Y * SpawnAreaSize.Z;
	UTEST_TRUE("at least one spawn area", NumSpawnAreas > 0);

	const int32 BaseManaged = Manager->GetNumManaged();
	const int32 BaseActivated = Manager->GetNumActivated();
	const int32 BaseDeactivated = Manager->GetNumDeactivated();

	// Init may have pre-flagged areas (upfront spawning modes). Probe until one area accepts a fresh Guid;
	// flagging a managed or invalid area early-returns without cache side effects.
	int32 ManagedIndex = INDEX_NONE;
	FGuid ManagedGuid;
	for (int i = 0; i < NumSpawnAreas && ManagedIndex == INDEX_NONE; i++)
	{
		const FGuid ProbeGuid = FGuid::NewGuid();
		Manager->FlagSpawnAreaAsManaged(i, ProbeGuid);
		if (Manager->GetNumManaged() == BaseManaged + 1)
		{
			ManagedIndex = i;
			ManagedGuid = ProbeGuid;
		}
	}

	// Counts expected after selection; the pre-managed fallback below does not change the managed/deactivated counts.
	int32 ExpectedManaged = BaseManaged + 1;
	int32 ExpectedDeactivated = BaseDeactivated + 1;

	if (ManagedIndex == INDEX_NONE)
	{
		// Init pre-managed every area (upfront spawning mode): no fresh Guid can be added, so pick an existing
		// managed target that has not been activated yet.
		UTEST_TRUE("all spawn areas pre-managed by Init", BaseManaged == NumSpawnAreas);

		ExpectedManaged = BaseManaged;
		ExpectedDeactivated = BaseDeactivated;

		const TMap<FGuid, ATarget*> ManagedTargets = GetManagedTargets();
		for (const TTuple<FGuid, ATarget*>& Pair : ManagedTargets)
		{
			const int32 Index = Manager->GetSpawnAreaIndex(Pair.Key);
			if (Index != INDEX_NONE && !Pair.Value->IsActivated())
			{
				ManagedIndex = Index;
				ManagedGuid = Pair.Key;
				break;
			}
		}
	}

	UTEST_TRUE("found a managed, unactivated area", ManagedIndex != INDEX_NONE);

	UTEST_EQUAL("managed count as expected", Manager->GetNumManaged(), ExpectedManaged);
	UTEST_EQUAL("guid resolves to the selected index", Manager->GetSpawnAreaIndex(ManagedGuid), ManagedIndex);
	UTEST_EQUAL("unknown guid resolves to none", Manager->GetSpawnAreaIndex(FGuid::NewGuid()), -1);
	UTEST_EQUAL("deactivated count as expected", Manager->GetNumDeactivated(), ExpectedDeactivated);

	// Activating an unknown Guid is rejected without cache side effects.
	const FGuid UnknownGuid = FGuid::NewGuid();
	Manager->FlagSpawnAreaAsActivated(UnknownGuid, FVector(1.f));
	UTEST_EQUAL("unknown guid activation is a no-op", Manager->GetNumActivated(), BaseActivated);

	Manager->FlagSpawnAreaAsActivated(ManagedGuid, FVector(1.f));

	UTEST_EQUAL("activated count increases by one", Manager->GetNumActivated(), BaseActivated + 1);
	UTEST_EQUAL("deactivated count decreases by one", Manager->GetNumDeactivated(), ExpectedDeactivated - 1);
	UTEST_EQUAL("most recent area is the activated one", Manager->GetMostRecentSpawnAreaIndex(), ManagedIndex);

	// A second activation of the same area is rejected; the cache is unchanged.
	Manager->FlagSpawnAreaAsActivated(ManagedGuid, FVector(2.f));
	UTEST_EQUAL("re-activation rejected, activated unchanged", Manager->GetNumActivated(), BaseActivated + 1);

	// Clear empties every cache and invalidates the Guid.
	TargetManager->Clear();

	UTEST_EQUAL("managed count cleared", Manager->GetNumManaged(), 0);
	UTEST_EQUAL("activated count cleared", Manager->GetNumActivated(), 0);
	UTEST_EQUAL("deactivated count cleared", Manager->GetNumDeactivated(), 0);
	UTEST_EQUAL("guid no longer resolves", Manager->GetSpawnAreaIndex(ManagedGuid), -1);
	UTEST_EQUAL("most recent area reset", Manager->GetMostRecentSpawnAreaIndex(), -1);

	CleanUpWorld();

	return true;
}
