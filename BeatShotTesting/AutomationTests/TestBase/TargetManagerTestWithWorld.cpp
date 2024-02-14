﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetManagerTestWithWorld.h"
#include "Target/TargetManager.h"

bool FTargetManagerTestWithWorld::Init()
{
	World = UWorld::CreateWorld(EWorldType::Game, false);
	GameInstance = NewObject<UGameInstance>();
	GameInstance->AddToRoot();
	World->SetGameInstance(GameInstance);
	
	FWorldContext &WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
	WorldContext.SetCurrentWorld(World);
	
	GameInstance->Init();
	World->InitializeActorsForPlay(FURL());
	World->BeginPlay();
	InitialFrameCounter = GFrameCounter;

	if (InitTargetManager())
	{
		bInitialized = true;
		return true;
	}

	AddError("Failed Initialization");
	return false;
}

bool FTargetManagerTestWithWorld::InitTargetManager()
{
	UObject* TargetManagerObj = StaticLoadObject(UObject::StaticClass(), nullptr,
		TargetManagerTestHelpers::TargetManagerAssetPath);
	if (!TargetManagerObj)
	{
		AddError("Failed to load Target Manager");
		return false;
	}
		
	const UBlueprint* GeneratedBP = Cast<UBlueprint>(TargetManagerObj);
	
	const FActorSpawnParameters SpawnInfo = FActorSpawnParameters();
	TargetManager = World->SpawnActor<ATargetManager>(GeneratedBP->GeneratedClass,
		Constants::DefaultTargetManagerLocation, FRotator::ZeroRotator, SpawnInfo);
	
	if (!TargetManager)
	{
		AddError("Failed to spawn Target Manager");
		return false;
	}
	
	TargetManager->AddToRoot();
	TargetManager->DispatchBeginPlay();
	TargetManager->SpawnableSpawnAreasExecutionTimeDelegate.BindRaw(this,
		&FTargetManagerTestWithWorld::OnSpawnableSpawnAreasExecutionTime);
	
	return true;
}

bool FTargetManagerTestWithWorld::InitGameModeDataAsset(const FString& InPath) const
{
	if (GameModeDataAsset) return true;
	
	UObject* LoadedObject = StaticLoadObject(UBSGameModeDataAsset::StaticClass(), nullptr, *InPath);
	if (!LoadedObject)
	{
		return false;
	}

	GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
	if (!GameModeDataAsset)
	{
		return false;
	}

	return true;
}

TObjectPtr<USpawnAreaManagerComponent> FTargetManagerTestWithWorld::GetSpawnAreaManager() const
{
	if (TargetManager)
	{
		return TargetManager->SpawnAreaManager;
	}
	return TObjectPtr<USpawnAreaManagerComponent>(nullptr);
}

TMap<FGuid, ATarget*> FTargetManagerTestWithWorld::GetManagedTargets() const
{
	if (TargetManager)
	{
		return TargetManager->ManagedTargets;
	}
	return TMap<FGuid, ATarget*>();
}

void FTargetManagerTestWithWorld::TickWorld(float Time)
{
	constexpr float Step = 0.1f;
	while (Time > 0.f)
	{
		World->Tick(ELevelTick::LEVELTICK_All, FMath::Min(Time, Step));
		Time -= Step;
		GFrameCounter++;
	}
}

void FTargetManagerTestWithWorld::OnSpawnableSpawnAreasExecutionTime(const double Time)
{
	SpawnableSpawnAreasTime += Time;
}