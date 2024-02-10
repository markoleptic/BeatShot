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
	for (const auto GameMode : TEnumRange<EBaseGameMode>())
	{
		FString Out = UEnum::GetDisplayValueAsText(GameMode).ToString();
		OutBeautifiedNames.Add(Out);
		OutTestCommands.Add(FString::FromInt(static_cast<int32>(GameMode)));
	}
}

bool FTestInit::RunTest(const FString& Parameters)
{
	if (!bInitialized)
	{
		SetGameModeDataAssetPath(TargetManagerTestHelpers::DefaultGameModeDataAssetPath);
		if (!Init())
		{
			return false;
		}
	}

	const FBS_DefiningConfig DefHard(EGameModeType::Preset, static_cast<EBaseGameMode>(FCString::Atoi(*Parameters)), "",
		EGameModeDifficulty::Hard);

	BSConfig = MakeShareable(new FBSConfig(GameModeDataAsset->GetDefaultGameModesMap().FindRef(DefHard)));
	const FPlayerSettings_Game GameSettings;
	TargetManager->Init(BSConfig, GameSettings);
	
	return true;
}