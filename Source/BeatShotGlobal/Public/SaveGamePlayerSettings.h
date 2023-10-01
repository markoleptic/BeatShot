// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "GameFramework/SaveGame.h"
#include "SaveGamePlayerSettings.generated.h"

UCLASS()
class BEATSHOTGLOBAL_API USaveGamePlayerSettings : public USaveGame
{     
	GENERATED_BODY()

public:
	/** Returns a copy of PlayerSettings */
	FPlayerSettings GetPlayerSettings() const;

	/** Saves Game specific settings, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_Game& InGameSettings);

	/** Saves Audio Analyzer specific settings, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings);

	/** Saves User specific settings, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_User& InUserSettings);

	/** Saves CrossHair specific settings, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings);

	/** Saves VideoAndSound settings, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);
	
private:
	UPROPERTY()
	FPlayerSettings PlayerSettings;
};
