// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class ATimeOfDayManager;

/** The base level used for this game */
UCLASS()
class BEATSHOT_API ARangeLevelScriptActor : public ALevelScriptActor, public ISaveLoadInterface
{
	GENERATED_BODY()

protected:
	ARangeLevelScriptActor();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<ATimeOfDayManager> TimeOfDayManager;

	/** Changes TimeOfDay */
	UFUNCTION()
	void OnStreakThresholdPassed();

	/** Callback function to respond to NightMode change from WallMenu */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
};
