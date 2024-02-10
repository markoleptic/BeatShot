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
		Super(InName, bInComplexTask), World(nullptr), GameInstance(nullptr), InitialFrameCounter(0)
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
	UWorld* World;
	UGameInstance *GameInstance;
	uint64 InitialFrameCounter;
};









	