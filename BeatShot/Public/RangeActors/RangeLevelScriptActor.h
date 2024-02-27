// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerSettingsInterface.h"
#include "Engine/LevelScriptActor.h"
#include "RangeLevelScriptActor.generated.h"

class APostProcessVolume;
class ATimeOfDayManager;

/** The base level used for this game */
UCLASS()
class BEATSHOT_API ARangeLevelScriptActor : public ALevelScriptActor, public IBSPlayerSettingsInterface
{
	GENERATED_BODY()

protected:
	ARangeLevelScriptActor();

	virtual void BeginPlay() override;

	/** Callback function to respond to video setting changes */
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings) override;

	UPROPERTY(EditDefaultsOnly, Category = "Lighting|References")
	TSoftObjectPtr<APostProcessVolume> PostProcessVolume;
};
