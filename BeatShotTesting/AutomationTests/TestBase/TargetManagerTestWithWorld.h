// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TargetManagerTestBase.h"
#include "BSGameModeDataAsset.h"
#include "Misc/AutomationTest.h"

/** Base class for unit tests dealing with the TargetManager, with a world. */
class FTargetManagerTestWithWorld : public FTargetManagerTestBase
{
	typedef ::FTargetManagerTestBase Super;
	
public:
	FTargetManagerTestWithWorld(const FString& InName, const bool bInComplexTask):
		Super(InName, bInComplexTask), World(nullptr), GameInstance(nullptr), InitialFrameCounter(0),
	TimeSpentInSpawnableSpawnAreas(0.0f), TargetManager(nullptr), bInitialized(false), GameModeDataAsset(nullptr)
	{}
	
	virtual ~FTargetManagerTestWithWorld() override
	{
		if (TargetManager)
		{
			if (AActor* Actor = Cast<AActor>(TargetManager))
			{
				Actor->RemoveFromRoot();
				Actor->Destroy();
			}
		}
		if (BSConfig.IsValid())
		{
			BSConfig.Reset();
		}
		TargetManager = nullptr;
		BSConfig = nullptr;
		GameModeDataAsset = nullptr;
		
		if (GameInstance)
		{
			GameInstance->Shutdown();
			GameInstance->RemoveFromRoot();
			GameInstance = nullptr;
		}
		
		GFrameCounter = InitialFrameCounter;
		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
	}
	virtual bool Init() override;
	
protected:
	/** Ticks the World and increments GFrameCounter. */
	void TickWorld(float Time);
	
	/** The World created in Init. */
	UWorld* World;

	/** Game Instance for the World. */
	UGameInstance *GameInstance;

	/** GFrameCounter prior to any increments. */
	uint64 InitialFrameCounter;

	/** Total time spent executing the GetSpawnableSpawnAreas function. */
	double TimeSpentInSpawnableSpawnAreas;

	/** Increments TimeSpentInSpawnableSpawnAreas. */
	void OnSpawnableSpawnAreasExecutionTime(const double Time);
	
	/** Initializes the Target Manager. */
	virtual bool InitTargetManager();

	/** Initializes the Target Manager. */
	virtual bool InitGameModeDataAsset(const FString& InPath) const;
	
	/** Returns the SpawnAreaManager. */
	TObjectPtr<USpawnAreaManagerComponent> GetSpawnAreaManager() const;

	/** Returns the ManagedTargets map. */
	TMap<FGuid, ATarget*> GetManagedTargets() const;
	
	/** Pointer to the Target Manager. */
	ATargetManager* TargetManager;

	/** Shared pointer to the Game mode config. Should be initialized in actual tests. */
	TSharedPtr<FBSConfig> BSConfig;
	
	/** Whether or not Init has been called in the base class (FTargetManagerTestBase) */
	bool bInitialized;

	/** Pointer to Game Mode Data Asset containing read-only game mode config data.
	 *  Mutable so that it can be set during GetTests. */
	mutable UBSGameModeDataAsset* GameModeDataAsset;

	/** Map to pull game mode configs from during tests. */
	mutable TMap<FString, FBSConfig> TestMap;
};