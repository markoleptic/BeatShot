// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Tests/TargetManagerFunctionalTest.h"
#include "Target/TargetManager.h"


ATargetManagerFunctionalTest::ATargetManagerFunctionalTest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void ATargetManagerFunctionalTest::PrepareTest()
{
	Super::PrepareTest();
	if (!TargetManagerClass || !GameModeDataAsset)
	{
		FinishTest(EFunctionalTestResult::Failed, TEXT("No TargetManagerClass or GameModeDataAsset"));
	}
	CurrentIndex = 0;
	PlayerSettings_Game = FPlayerSettings_Game();
	DefaultGameModes = GameModeDataAsset->GetDefaultGameModes();
	TargetManager = GetWorld()->SpawnActor<ATargetManager>(TargetManagerClass);
	TargetManager->Init(DefaultGameModes[CurrentIndex], PlayerSettings_Game);
}

bool ATargetManagerFunctionalTest::IsReady_Implementation()
{
	return TargetManager && !DefaultGameModes.IsEmpty();
}

void ATargetManagerFunctionalTest::StartTest()
{
	Super::StartTest();
	StartGameMode(DefaultGameModes[CurrentIndex]);
}

void ATargetManagerFunctionalTest::FinishTest(EFunctionalTestResult TestResult, const FString& Message)
{
	GetWorldTimerManager().ClearTimer(GameModeTimer);
	GetWorldTimerManager().ClearTimer(BeatTimer);
	GetWorldTimerManager().ClearTimer(DestroyTimer);
	Super::FinishTest(TestResult, Message);
}

void ATargetManagerFunctionalTest::CleanUp()
{
	Super::CleanUp();
	GetWorldTimerManager().ClearTimer(GameModeTimer);
	GetWorldTimerManager().ClearTimer(BeatTimer);
	GetWorldTimerManager().ClearTimer(DestroyTimer);
	TargetManager->Destroy();
}

void ATargetManagerFunctionalTest::OnAudioAnalyzerBeat()
{
	TargetManager->OnAudioAnalyzerBeat();
	AssertEqual_Bool(
		TargetManager->SpawnAreaManager->GetActivatedSpawnAreas().Num() <= TargetManager->SpawnAreaManager->
		GetManagedSpawnAreas().Num(), true, "Activated Less than Managed");
}

void ATargetManagerFunctionalTest::StartGameMode(const FBSConfig& InConfig)
{
	StartStep(UEnum::GetDisplayValueAsText(InConfig.DefiningConfig.BaseGameMode).ToString());
	TargetManager->Init(InConfig, PlayerSettings_Game);
	TargetManager->SetShouldSpawn(true);
	GetWorldTimerManager().SetTimer(GameModeTimer, this, &ATargetManagerFunctionalTest::StopGameMode, GameModeDuration,
		false);
	GetWorldTimerManager().SetTimer(BeatTimer, this, &ATargetManagerFunctionalTest::OnAudioAnalyzerBeat, BeatFrequency,
		true, 1.f);
	GetWorldTimerManager().SetTimer(DestroyTimer, this, &ATargetManagerFunctionalTest::DestroyTargets, BeatFrequency,
		true, 1.05f);
}

void ATargetManagerFunctionalTest::StopGameMode()
{
	GetWorldTimerManager().ClearTimer(DestroyTimer);
	GetWorldTimerManager().ClearTimer(BeatTimer);
	GetWorldTimerManager().ClearTimer(GameModeTimer);
	if (TargetManager)
	{
		TargetManager->Clear();
		FinishStep();
		CurrentIndex += 1;
		if (DefaultGameModes.IsValidIndex(CurrentIndex))
		{
			StartGameMode(DefaultGameModes[CurrentIndex]);
		}
		else
		{
			FinishTest(EFunctionalTestResult::Succeeded, TEXT(""));
		}
	}
	else
	{
		FinishTest(EFunctionalTestResult::Succeeded, TEXT(""));
	}
}

void ATargetManagerFunctionalTest::DestroyTargets()
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
