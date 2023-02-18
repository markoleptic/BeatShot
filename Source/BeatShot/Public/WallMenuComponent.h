// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Components/StaticMeshComponent.h"
#include "WallMenuComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEATSHOT_API UWallMenuComponent : public UStaticMeshComponent, public ISaveLoadInterface
{
	GENERATED_BODY()
	
	UWallMenuComponent();
	
	virtual void BeginPlay() override;
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
