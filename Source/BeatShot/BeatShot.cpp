// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "BeatShot.h"
#include "Modules/ModuleManager.h"

FSpawnPoint* FindSpawnPointFromIndex(TArray<FSpawnPoint>& InSpawnPointArray, const int32 InIndex)
{
	return InSpawnPointArray.FindByPredicate([&InIndex] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.Index == InIndex)
		{
			return true;
		}
		return false;
	});
}

FSpawnPoint* FindSpawnPointFromLocation(TArray<FSpawnPoint>& InSpawnPointArray, const FVector& InLocation)
{
	return InSpawnPointArray.FindByPredicate([&InLocation](const FSpawnPoint& SpawnPoint)
	{
		if (InLocation.Y >= SpawnPoint.Point.Y && InLocation.Y < SpawnPoint.Point.Y + SpawnPoint.IncrementY &&
			(InLocation.Z >= SpawnPoint.Point.Z && InLocation.Z < SpawnPoint.Point.Z + SpawnPoint.IncrementZ))
		{
			return true;
		}
		return false;
	});
}

TArray<FSpawnPoint> GetRecentSpawnPoints(const TArray<FSpawnPoint>& InSpawnPointArray)
{
	return InSpawnPointArray.FilterByPredicate([] (const FSpawnPoint& SpawnPoint)
	{
		if (SpawnPoint.bIsRecent)
		{
			return true;
		}
		return false;
	});
}

FSpawnPoint* FindSpawnPointFromGuid(TArray<FSpawnPoint>& InSpawnPointArray, const FGuid& InGuid)
{
	return InSpawnPointArray.FindByPredicate([&InGuid](const FSpawnPoint& SpawnPoint)
{
	if (SpawnPoint.TargetGuid == InGuid)
	{
		return true;
	}
	return false;
});
}

void FBeatShot::StartupModule()
{
	FDefaultGameModuleImpl::StartupModule();
}

void FBeatShot::ShutdownModule()
{
	FDefaultGameModuleImpl::ShutdownModule();
}

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, BeatShot, "BeatShot");
