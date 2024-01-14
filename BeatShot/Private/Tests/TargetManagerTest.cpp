// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Target/TargetManager.h"
#include "Target/Target.h"
#include "Target/TargetManagerPreview.h"
#include "Tests/AutomationCommon.h"

/** This didn't end up working properly, but TargetManagerFunctionalTest did. */

const TCHAR* GameModeDataAssetPath = TEXT("/Game/Blueprints/GameModes/DA_DefaultGameModes.DA_DefaultGameModes");
const TCHAR* TargetManagerAssetPath = TEXT("/Game/Blueprints/Targets/BP_TargetManager.BP_TargetManager");

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FRunAllDefaultGameModesTest, "TargetManager.RunAllDefaultGameModes",
	EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::
	HighPriorityAndAbove | EAutomationTestFlags::ProductFilter)

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FOnAudioAnalyzerBeat, ATargetManager*, TargetManager);

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FLoadTargetManager, UObject*, TargetManager);

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FInitTargetManager, ATargetManager*, TargetManager, const FBSConfig&,
	Config);

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FSimulateTargetHit, TArray<ATarget*>, InManagedTargets);

bool FLoadTargetManager::Update()
{
	TargetManager = StaticLoadObject(UObject::StaticClass(), nullptr, TargetManagerAssetPath);
	if (!TargetManager) return false;
	return true;
}

bool FOnAudioAnalyzerBeat::Update()
{
	TargetManager->OnAudioAnalyzerBeat();
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.05f));
	return true;
}

bool FInitTargetManager::Update()
{
	const FPlayerSettings_Game PlayerSettings_Game;
	TargetManager->Init(Config, PlayerSettings_Game);
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.05f));
	return true;
}

bool FSimulateTargetHit::Update()
{
	for (ATarget* Target : InManagedTargets)
	{
		Target->DamageSelf();
	}
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(0.05f));
	return true;
}

bool FRunAllDefaultGameModesTest::RunTest(const FString& Parameters)
{
	FString MapName = Parameters;
	UWorld* World = FAutomationEditorCommonUtils::CreateNewMap();

	static const FPlayerSettings_Game PlayerSettings_Game = FPlayerSettings_Game();

	UObject* LoadedObject = StaticFindObject(UBSGameModeDataAsset::StaticClass(), nullptr, GameModeDataAssetPath, true);
	TestNotNull("Null StaticFindObject", LoadedObject);
	if (!LoadedObject) return false;

	static UBSGameModeDataAsset* GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
	TestNotNull("Null GameModeDataAsset", GameModeDataAsset);
	if (!GameModeDataAsset) return false;

	UObject* TargetManagerObj = StaticLoadObject(UObject::StaticClass(), nullptr, TargetManagerAssetPath);
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(2.0f));
	TestNotNull("Null TargetManagerObj", TargetManagerObj);
	if (!TargetManagerObj) return false;

	UBlueprint* GeneratedBP = Cast<UBlueprint>(TargetManagerObj);
	TestNotNull("Null GeneratedBP", GeneratedBP);
	FActorSpawnParameters Sp = FActorSpawnParameters();
	ATargetManager* TargetManager = World->SpawnActor<ATargetManager>(GeneratedBP->GeneratedClass,
		DefaultTargetManagerLocation, FRotator::ZeroRotator, Sp);
	TestNotNull("Null TargetManager", TargetManager);
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(1.0f));

	AddInfo("Everything is not null poggers");

	FDamageEventData DamageEventData;
	DamageEventData.EffectCauser = TargetManager;
	DamageEventData.EffectInstigator = TargetManager;
	FBS_DefiningConfig BS_DefiningConfig;
	BS_DefiningConfig.Difficulty = EGameModeDifficulty::Normal;
	BS_DefiningConfig.BaseGameMode = EBaseGameMode::MultiBeat;
	BS_DefiningConfig.GameModeType = EGameModeType::Preset;
	const FBSConfig* Found = GameModeDataAsset->GetDefaultGameModesMap().Find(BS_DefiningConfig);

	ADD_LATENT_AUTOMATION_COMMAND(FInitTargetManager(TargetManager, *Found));
	TargetManager->SpawnAreaManager->bPrintDebug_SpawnAreaStateInfo = true;
	TargetManager->SetShouldSpawn(true);

	ADD_LATENT_AUTOMATION_COMMAND(FOnAudioAnalyzerBeat(TargetManager));
	ADD_LATENT_AUTOMATION_COMMAND(FSimulateTargetHit(TargetManager->GetManagedTargets()));

	/*for (const TPair<FBS_DefiningConfig, FBSConfig>& Pair : GameModeDataAsset->GetDefaultGameModesMap())
	{
		if (Len > 0) break;
		AddInfo("Testing: " + UEnum::GetDisplayValueAsText(Pair.Key.BaseGameMode).ToString());
		AddInfo("------------------------------------------------------------------------");
		TargetManager->Init(Pair.Value, PlayerSettings_Game);
		AddCommand(new FWaitLatentCommand(1.f));
		TargetManager->SpawnAreaManager->bPrintDebug_SpawnAreaStateInfo = true;
		TargetManager->SetShouldSpawn(true);
		DamageEventData.DamageType = Pair.Value.TargetConfig.TargetDamageType;
		DamageEventData.OldValue = Pair.Value.TargetConfig.MaxHealth;
		DamageEventData.NewValue = 0;
		
		for (int i = 0; i < 5; i++)
		{
			AddCommand(new FOnAudioAnalyzerBeat(TargetManager));
			AddCommand(new FWaitLatentCommand(0.1f));
			TestFalse("Empty ManagedTargets", TargetManager->GetManagedTargets().IsEmpty());
			AddCommand(new FWaitLatentCommand(0.1f));
			AddCommand(new FSimulateTargetHit(DamageEventData, TargetManager->GetManagedTargets()));
			AddCommand(new FWaitLatentCommand(0.1f));
		}
		Len++;
	}*/

	return true;
}
