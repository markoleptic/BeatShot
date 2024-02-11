// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "TargetManagerTestBase.h"
#include "Target/TargetManager.h"

bool FTargetManagerTestBase::Init()
{
	if (InitGameModeDataAsset() && InitTargetManager())
	{
		bInitialized = true;
		return true;
	}
	AddError("Failed Initialization");
	return false;
}

bool FTargetManagerTestBase::InitTargetManager()
{
	AddError("Base class does not initialize the Target Manager");
	return false;
}

bool FTargetManagerTestBase::InitGameModeDataAsset()
{
	if (GameModeDataAsset) return true;
	
	UObject* LoadedObject = StaticLoadObject(UBSGameModeDataAsset::StaticClass(), nullptr, *GameModeDataAssetPath);
	if (!LoadedObject)
	{
		AddError("Failed to load Game Mode Data Asset");
		return false;
	}

	GameModeDataAsset = Cast<UBSGameModeDataAsset>(LoadedObject);
	if (!GameModeDataAsset)
	{
		AddError("Failed cast to Game Mode Data Asset");
		return false;
	}

	return true;
}

TObjectPtr<USpawnAreaManagerComponent> FTargetManagerTestBase::GetSpawnAreaManager() const
{
	if (TargetManager)
	{
		return TargetManager->SpawnAreaManager;
	}
	return TObjectPtr<USpawnAreaManagerComponent>(nullptr);
}

TMap<FGuid, ATarget*> FTargetManagerTestBase::GetManagedTargets() const
{
	if (TargetManager)
	{
		return TargetManager->ManagedTargets;
	}
	return TMap<FGuid, ATarget*>();
}