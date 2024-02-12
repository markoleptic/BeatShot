// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "../TestBase/TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"


/** All this does is initialize the target manager with all default game modes. */
IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestInit, FTargetManagerTestWithWorld, "TargetManager.Init",
	EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::
	HighPriorityAndAbove | EAutomationTestFlags::ProductFilter);

void FTestInit::GetTests(TArray<FString>& OutBeautifiedNames, TArray<FString>& OutTestCommands) const
{
	if (InitGameModeDataAsset(TargetManagerTestHelpers::DefaultGameModeDataAssetPath))
	{
		for (const auto& Mode : GameModeDataAsset->GetDefaultGameModesMap())
		{
			const FString GameModeString = UEnum::GetDisplayValueAsText(Mode.Key.BaseGameMode).ToString();
			OutBeautifiedNames.Add(GameModeString);
			OutTestCommands.Add(GameModeString);
			TestMap.Add(GameModeString, Mode.Value);
		}
	}
}

bool FTestInit::RunTest(const FString& Parameters)
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
	TargetManager->Clear();
	
	return true;
}