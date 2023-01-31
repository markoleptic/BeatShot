// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Engine/StaticMeshActor.h"
#include "WallMenu.generated.h"


/**
 * 
 */
UCLASS()
class BEATSHOT_API AWallMenu : public AStaticMeshActor, public ISaveLoadInterface
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


