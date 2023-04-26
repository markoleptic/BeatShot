// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SaveLoadInterface.h"
#include "WallMenu.generated.h"

UCLASS()
class BEATSHOT_API AWallMenu : public AActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void BeginPlay() override;

protected:
	/** Displays which settings are on/off etc by lighting the correct words */
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeSettings(const FPlayerSettings_Game& GameSettings, const FPlayerSettings_User& UserSettings);

	/** Overriden version so blueprint can use */
	UFUNCTION(BlueprintCallable)
	virtual FPlayerSettings LoadPlayerSettings() const override;

	/** Overriden version so blueprint can use, also broadcasts GameInstance's OnPlayerSettingsChanged */
	UFUNCTION(BlueprintCallable)
	virtual void SavePlayerSettings(const FPlayerSettings_Game& InGameSettings) override;
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
};
