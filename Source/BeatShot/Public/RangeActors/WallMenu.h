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

	virtual void Destroyed() override;

protected:
	/** Displays which settings are on/off etc by lighting the correct words */
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeSettings(const FPlayerSettings PlayerSettings);

	/** Overriden version so blueprint can use */
	UFUNCTION(BlueprintCallable)
	virtual FPlayerSettings LoadPlayerSettings() const override;

	/** Overriden version so blueprint can use, also broadcasts GameInstance's OnPlayerSettingsChanged */
	UFUNCTION(BlueprintCallable)
	virtual void SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave) override;

	UFUNCTION()
	void OnPlayerSettingsChanged(const FPlayerSettings& PlayerSettings);
};