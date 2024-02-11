// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "TargetManagerTestBase.h"
#include "Misc/AutomationTest.h"

/** Base class for unit tests dealing with the TargetManager, with a world. */
class FTargetManagerTestWithWorld : public FTargetManagerTestBase
{
	typedef ::FTargetManagerTestBase Super;
	
public:
	FTargetManagerTestWithWorld(const FString& InName, const bool bInComplexTask):
		Super(InName, bInComplexTask), World(nullptr), GameInstance(nullptr), InitialFrameCounter(0),
		TimeSpentInSpawnableSpawnAreas(0.0f)
	{}
	
	virtual ~FTargetManagerTestWithWorld() override
	{
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
	virtual bool InitTargetManager() override;
	
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
};









	