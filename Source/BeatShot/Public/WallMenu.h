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

	UFUNCTION(BlueprintCallable)
	virtual FPlayerSettings LoadPlayerSettings() const override;

	UFUNCTION(BlueprintCallable)
	virtual void SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave) override;

	FOnPlayerSettingsChange OnPlayerSettingsChange;
};
