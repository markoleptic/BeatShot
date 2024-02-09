// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Target/TargetManager.h"
#include "Target/TargetManagerPreview.h"

const TCHAR* GameModeDataAssetPath = TEXT("/Game/Blueprints/GameModes/DA_DefaultGameModes.DA_DefaultGameModes");
const TCHAR* EnumTagMapDataAssetPath = TEXT("/Game/Blueprints/DA_EnumTagMap.DA_EnumTagMap");
const TCHAR* TargetManagerAssetPath = TEXT("/Game/Blueprints/Targets/BP_TargetManager.BP_TargetManager");
const TCHAR* TargetAssetPath = TEXT("/Game/Blueprints/Targets/BP_Target.BP_Target");

/** Base class for unit tests dealing with the TargetManager. */
class FBSAutomationTestBase : public FAutomationTestBase
{
public:
	FBSAutomationTestBase(const FString& InName, const bool bInComplexTask) :
		FAutomationTestBase(InName, bInComplexTask), World(nullptr), GameModeDataAsset(nullptr), EnumTagMap(nullptr)
	{
	}

	virtual ~FBSAutomationTestBase() override
	{
		if (TargetManager.IsValid())
		{
			TargetManager.Reset();
		}
		if (BSConfig.IsValid())
		{
			BSConfig.Reset();
		}
		TargetManager = nullptr;
		BSConfig = nullptr;
		GameModeDataAsset = nullptr;
		EnumTagMap = nullptr;
		if (World)
		{
			World->DestroyWorld(false);
		}
	}

	virtual uint32 GetTestFlags() const override
	{
		return EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
			EAutomationTestFlags::ProductFilter;
	}

	virtual bool IsStressTest() const { return false; }
	virtual uint32 GetRequiredDeviceNum() const override { return 1; }

protected:
	TObjectPtr<USpawnAreaManagerComponent> GetSpawnAreaManager() const { return TargetManager->SpawnAreaManager; }
	TMap<FGuid, ATarget*> GetManagedTargets() const { return TargetManager->ManagedTargets; }
	virtual FString GetBeautifiedTestName() const override { return "TargetManager"; }
	UWorld* World;
	TSharedPtr<ATargetManager> TargetManager;
	TSharedPtr<FBSConfig> BSConfig;
	UBSGameModeDataAsset* GameModeDataAsset;
	mutable UEnumTagMap* EnumTagMap;
	bool bInitialized = false;

public:
	virtual bool Init()
	{
		//World = FAutomationEditorCommonUtils::CreateNewMap();
		UObject* LoadedObject = StaticFindObject(UBSGameModeDataAsset::StaticClass(), nullptr, GameModeDataAssetPath,
			true);
		if (!LoadedObject) return false;

		GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
		if (!GameModeDataAsset) return false;

		UObject* LoadedEnumTagMap = StaticFindObject(UEnumTagMap::StaticClass(), nullptr, EnumTagMapDataAssetPath,
			true);
		if (!LoadedEnumTagMap) return false;

		EnumTagMap = Cast<UEnumTagMap>(LoadedEnumTagMap);
		if (!EnumTagMap) return false;

		UObject* TargetManagerObj = StaticLoadObject(UObject::StaticClass(), nullptr, TargetManagerAssetPath);
		if (!TargetManagerObj) return false;

		const UBlueprint* GeneratedBP = Cast<UBlueprint>(TargetManagerObj);

		const FActorSpawnParameters SpawnInfo = FActorSpawnParameters();
		TargetManager = MakeShareable(World->SpawnActor<ATargetManager>(GeneratedBP->GeneratedClass,
			DefaultTargetManagerLocation, FRotator::ZeroRotator, SpawnInfo));

		FURL URL;
		World->InitializeActorsForPlay(URL);
		World->BeginPlay();

		bInitialized = true;
		return true;
	}
};

/** All this does is initialize the target manager. */
IMPLEMENT_CUSTOM_COMPLEX_AUTOMATION_TEST(FTestInit, FBSAutomationTestBase, "TargetManager.Init",
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
	TargetManager->SetShouldSpawn(true);
	TestNotNull("SpawnAreaManager not null", GetSpawnAreaManager().Get());
	for (int i = 0; i < 10; i++)
	{
		TargetManager->OnAudioAnalyzerBeat();
	}
	TestFalse("Managed Targets is empty", GetManagedTargets().IsEmpty());
	AddInfo(FString::Printf(TEXT("%d targets spawned"), GetManagedTargets().Num()));
	return true;
}

#endif