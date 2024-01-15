// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"
#include "Tests/AutomationEditorCommon.h"
#include "Target/TargetManager.h"
#include "Target/Target.h"
#include "Target/TargetManagerPreview.h"
#include "Tests/AutomationCommon.h"

/** This didn't end up working properly, but TargetManagerFunctionalTest did. */
/** This is very cursed and bad. */

const TCHAR* GameModeDataAssetPath = TEXT("/Game/Blueprints/GameModes/DA_DefaultGameModes.DA_DefaultGameModes");
const TCHAR* TargetManagerAssetPath = TEXT("/Game/Blueprints/Targets/BP_TargetManager.BP_TargetManager");

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FOnAudioAnalyzerBeat, TSharedPtr<ATargetManager>, TargetManager);

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FLoadTargetManager, UObject*, TargetManager);

DEFINE_LATENT_AUTOMATION_COMMAND_TWO_PARAMETER(FInitTargetManager, TSharedPtr<ATargetManager>, TargetManager, FBSConfig,
	Config);

DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FSimulateTargetHit, TSharedPtr<ATargetManager>, TargetManager);

bool FLoadTargetManager::Update()
{
	TargetManager = StaticLoadObject(UObject::StaticClass(), nullptr, TargetManagerAssetPath);
	if (!TargetManager) return false;
	return true;
}

bool FOnAudioAnalyzerBeat::Update()
{
	if (!TargetManager) return false;
	TargetManager->OnAudioAnalyzerBeat();
	return true;
}

bool FInitTargetManager::Update()
{
	if (!TargetManager) return false;
	const FPlayerSettings_Game PlayerSettings_Game;
	TSharedPtr<FBSConfig> ConfigPtr = MakeShareable(new FBSConfig(Config));
	TargetManager->Init(ConfigPtr, PlayerSettings_Game);
	TargetManager->SpawnAreaManager->bPrintDebug_SpawnAreaStateInfo = true;
	TargetManager->SetShouldSpawn(true);
	return true;
}

bool FSimulateTargetHit::Update()
{
	if (TargetManager->GetManagedTargets().IsEmpty()) return false;
	for (ATarget* Target : TargetManager->GetManagedTargets())
	{
		Target->DamageSelf(true);
	}
	return true;
}


class FBSAutomationTestBase
{
public:
	FBSAutomationTestBase() : TestRunner(nullptr), World(nullptr), InitialFrameCounter(0)
	{
	}

	FAutomationTestBase& GetTestRunner() const
	{
		check(TestRunner);
		return *TestRunner;
	}

	virtual void SetTestRunner(FAutomationTestBase& AutomationTestInstance) { TestRunner = &AutomationTestInstance; }

	void TickWorld(float Time)
	{
		const float step = 0.1f;
		while (Time > 0.f)
		{
			World->Tick(ELevelTick::LEVELTICK_All, FMath::Min(Time, step));
			Time -= step;
		}
	}

protected:
	FAutomationTestBase* TestRunner;
	UWorld* World;
	uint64 InitialFrameCounter;

public:
	// interface
	virtual ~FBSAutomationTestBase()
	{
		World->DestroyWorld(false);
	};

	virtual bool Init()
	{
		World = FAutomationEditorCommonUtils::CreateNewMap();
		return true;
	}

	virtual bool QueueLatentCommands() { return false; }
	virtual bool InstantTest() { return false; }

	virtual void FinishTest()
	{
	}
};

class FBSTestAllGameModes : public FBSAutomationTestBase
{
	TSharedPtr<ATargetManager> TargetManager;

	UBSGameModeDataAsset* GameModeDataAsset;

public:
	FBSTestAllGameModes(): TargetManager(nullptr), GameModeDataAsset(nullptr)
	{
	}

	virtual bool Init() override
	{
		FBSAutomationTestBase::Init();

		UObject* LoadedObject = StaticFindObject(UBSGameModeDataAsset::StaticClass(), nullptr, GameModeDataAssetPath,
			true);
		if (!LoadedObject) return false;

		GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
		if (!GameModeDataAsset) return false;

		UObject* TargetManagerObj = StaticLoadObject(UObject::StaticClass(), nullptr, TargetManagerAssetPath);
		if (!TargetManagerObj) return false;

		const UBlueprint* GeneratedBP = Cast<UBlueprint>(TargetManagerObj);

		const FActorSpawnParameters SpawnInfo = FActorSpawnParameters();
		TargetManager = MakeShareable(World->SpawnActor<ATargetManager>(GeneratedBP->GeneratedClass,
			DefaultTargetManagerLocation, FRotator::ZeroRotator, SpawnInfo));

		FURL URL;
		World->InitializeActorsForPlay(URL);
		World->BeginPlay();

		return true;
	}

	virtual bool QueueLatentCommands() override
	{
		const FBS_DefiningConfig DefHard(EGameModeType::Preset, EBaseGameMode::MultiBeat, "",
			EGameModeDifficulty::Hard);
		ADD_LATENT_AUTOMATION_COMMAND(
			FInitTargetManager(TargetManager, GameModeDataAsset->GetDefaultGameModesMap().FindRef(DefHard)));
		TickWorld(0.01);
		ADD_LATENT_AUTOMATION_COMMAND(FOnAudioAnalyzerBeat(TargetManager));
		TickWorld(0.01);
		ADD_LATENT_AUTOMATION_COMMAND(FSimulateTargetHit(TargetManager));
		TickWorld(0.01);
		ADD_LATENT_AUTOMATION_COMMAND(FOnAudioAnalyzerBeat(TargetManager));
		TickWorld(0.01);
		ADD_LATENT_AUTOMATION_COMMAND(FSimulateTargetHit(TargetManager));
		TickWorld(0.01);
		return true;
	}

	virtual bool InstantTest() override { return false; }

	virtual void FinishTest() override
	{
		FBSAutomationTestBase::FinishTest();
		TargetManager->Destroy();
		GameModeDataAsset = nullptr;
	};
};


DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FBSFinishTest, FBSAutomationTestBase*, BSTest);

bool FBSFinishTest::Update()
{
	return false;
}


#define BS_IMPLEMENT_LATENT_AUTOMATION_TEST(TestClass, PrettyName, TFlags) \
	IMPLEMENT_SIMPLE_AUTOMATION_TEST(TestClass##_Runner, PrettyName, TFlags) \
	bool TestClass##_Runner::RunTest(const FString& Parameters) \
	{ \
		bool bSuccess = false; \
		TestClass* TestInstance = new TestClass(); \
		TestInstance->SetTestRunner(*this); \
		bSuccess = TestInstance->Init(); \
		if (bSuccess) \
		{ \
			bSuccess = TestInstance->QueueLatentCommands(); \
			ADD_LATENT_AUTOMATION_COMMAND(FBSFinishTest(TestInstance)); \
		} \
		delete TestInstance; \
		return bSuccess; \
	}


BS_IMPLEMENT_LATENT_AUTOMATION_TEST(FBSTestAllGameModes, "TargetManager.RunAllDefaultGameModes",
	EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext | EAutomationTestFlags::
	HighPriorityAndAbove | EAutomationTestFlags::ProductFilter);
