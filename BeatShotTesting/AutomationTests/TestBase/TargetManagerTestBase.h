// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameModeDataAsset.h"
#include "Misc/AutomationTest.h"

namespace TargetManagerTestHelpers
{
	inline const TCHAR* DefaultGameModeDataAssetPath = TEXT("/Game/Blueprints/GameModes/DA_DefaultGameModes.DA_DefaultGameModes");
	inline const TCHAR* EnumTagMapDataAssetPath = TEXT("/Game/Blueprints/DA_EnumTagMap.DA_EnumTagMap");
	inline const TCHAR* TargetManagerAssetPath = TEXT("/Game/Blueprints/Targets/BP_TargetManager.BP_TargetManager");
	inline const TCHAR* TargetAssetPath = TEXT("/Game/Blueprints/Targets/BP_Target.BP_Target");
}

class ATarget;
class ATargetManager;
class USpawnAreaManagerComponent;

/** Base class for unit tests dealing with the TargetManager. */
class FTargetManagerTestBase : public FAutomationTestBase
{
public:
	FTargetManagerTestBase(const FString& InName, const bool bInComplexTask) : FAutomationTestBase(InName,
		bInComplexTask), TargetManager(nullptr), GameModeDataAsset(nullptr), bInitialized(false),
		GameModeDataAssetPath(TargetManagerTestHelpers::DefaultGameModeDataAssetPath)
	{}

	virtual ~FTargetManagerTestBase() override
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
	}

	virtual uint32 GetTestFlags() const override
	{
		return EAutomationTestFlags::CommandletContext | EAutomationTestFlags::EditorContext |
			EAutomationTestFlags:: HighPriorityAndAbove | EAutomationTestFlags::EngineFilter;
	}
	virtual bool IsStressTest() const { return false; }
	virtual uint32 GetRequiredDeviceNum() const override { return 1; }
	virtual FString GetBeautifiedTestName() const override { return "TargetManager"; }
	
	/** Initializes stuff. */
	virtual bool Init();
	
protected:
	/** Initializes the Target Manager. */
	virtual bool InitTargetManager();

	/** Initializes the Target Manager. */
	virtual bool InitGameModeDataAsset();
	
	/** Returns the SpawnAreaManager. */
	TObjectPtr<USpawnAreaManagerComponent> GetSpawnAreaManager() const;

	/** Returns the ManagedTargets map. */
	TMap<FGuid, ATarget*> GetManagedTargets() const;
	
	/** Pointer to the Target Manager. */
	ATargetManager* TargetManager;

	/** Shared pointer to the Game mode config. Should be initialized in actual tests. */
	TSharedPtr<FBSConfig> BSConfig;

	/** Pointer to Game Mode Data Asset containing read-only game mode config data.
	 *  Mutable so that it can be set during GetTests. */
	mutable UBSGameModeDataAsset* GameModeDataAsset;
	
	/** Whether or not Init has been called in the base class (FTargetManagerTestBase) */
	bool bInitialized;

	/** Path to pull the GameModeDataAsset from. Mutable so that it can be set during GetTests. */
	mutable FString GameModeDataAssetPath;
};