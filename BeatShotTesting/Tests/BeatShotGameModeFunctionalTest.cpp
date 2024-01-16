// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BeatShotGameModeFunctionalTest.h"

#include "Target/TargetManager.h"

ABeatShotGameModeFunctionalTest::ABeatShotGameModeFunctionalTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PlayerSettings_Game = FPlayerSettings_Game();
}

void ABeatShotGameModeFunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	if (!TargetManagerClass || !GameModeDataAsset)
	{
		FinishTest(EFunctionalTestResult::Failed, TEXT("No TargetManagerClass or GameModeDataAsset"));
	}
	PopulateGameModeConfigs();
	GameModeConfig = MakeShareable(new FBSConfig());
	TargetManager = GetWorld()->SpawnActor<ATargetManager>(TargetManagerClass);
}

bool ABeatShotGameModeFunctionalTest::IsReady_Implementation()
{
	return TargetManager && GameModeConfig.IsValid() && GameModeConfigs.Num() == 3;
}

void ABeatShotGameModeFunctionalTest::StartTest()
{
	Super::StartTest();
	StartGameMode();
}

void ABeatShotGameModeFunctionalTest::FinishTest(EFunctionalTestResult TestResult, const FString& Message)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	TargetManager->Clear();
	TargetManager->Destroy();
	GameModeConfig.Reset();
	GameModeConfig = nullptr;
	GameModeConfigs.Empty();
	Super::FinishTest(TestResult, Message);
}

void ABeatShotGameModeFunctionalTest::CleanUp()
{
	Super::CleanUp();
}

void ABeatShotGameModeFunctionalTest::PopulateGameModeConfigs()
{
	const FBS_DefiningConfig DefNormal(EGameModeType::Preset, GameModeToTest, "", EGameModeDifficulty::Normal);
	const FBS_DefiningConfig DefHard(EGameModeType::Preset, GameModeToTest, "", EGameModeDifficulty::Hard);
	const FBS_DefiningConfig DefDeath(EGameModeType::Preset, GameModeToTest, "", EGameModeDifficulty::Death);
	GameModeConfigs.Add(GameModeDataAsset->GetDefaultGameModesMap().FindRef(DefNormal));
	GameModeConfigs.Add(GameModeDataAsset->GetDefaultGameModesMap().FindRef(DefHard));
	GameModeConfigs.Add(GameModeDataAsset->GetDefaultGameModesMap().FindRef(DefDeath));
}

void ABeatShotGameModeFunctionalTest::OnAudioAnalyzerBeat()
{
	TargetManager->OnAudioAnalyzerBeat();

	AssertEqual_Bool(
	TargetManager->SpawnAreaManager->GetActivatedSpawnAreas().Num() <= TargetManager->SpawnAreaManager->
		GetManagedSpawnAreas().Num(), true, "Activated Less than Managed");
	
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateWeakLambda(this, [this]
	{
		DestroyTargets();
	}));
}

void ABeatShotGameModeFunctionalTest::StartGameMode()
{
	const FText GameModeText = UEnum::GetDisplayValueAsText(GameModeConfigs[CurrentIndex].DefiningConfig.BaseGameMode);
	const FText DifficultyText = UEnum::GetDisplayValueAsText(GameModeConfigs[CurrentIndex].DefiningConfig.Difficulty);
	StartStep(GameModeText.ToString() + "." + DifficultyText.ToString());
	*GameModeConfig = GameModeConfigs[CurrentIndex];
	TargetManager->Init(GameModeConfig, PlayerSettings_Game);
	TargetManager->SetShouldSpawn(true);
	GetWorldTimerManager().SetTimer(GameModeTimer, this, &ThisClass::StopGameMode, GameModeDuration, false);
	GetWorldTimerManager().SetTimer(BeatTimer, this, &ThisClass::OnAudioAnalyzerBeat, BeatFrequency, true,
		InitialDelay);
}

void ABeatShotGameModeFunctionalTest::StopGameMode()
{
	GetWorldTimerManager().ClearAllTimersForObject(this);
	if (!TargetManager) FinishTest(EFunctionalTestResult::Failed, TEXT("Null Target Manager"));

	TargetManager->Clear();
	FinishStep();
	CurrentIndex++;

	if (GameModeConfigs.IsValidIndex(CurrentIndex))
	{
		StartGameMode();
	}
	else
	{
		FinishTest(EFunctionalTestResult::Succeeded, TEXT("End of Game Mode Configs"));
	}
}

void ABeatShotGameModeFunctionalTest::DestroyTargets()
{
	if (TargetManager)
	{
		for (const USpawnArea* SpawnArea : TargetManager->SpawnAreaManager->GetActivatedSpawnAreas())
		{
			if (ATarget* Target = TargetManager->ManagedTargets.FindRef(SpawnArea->GetGuid()))
			{
				Target->DamageSelf(true);
			}
		}
	}
}