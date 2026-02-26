// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSettings/PlayerSettings.h"
#include "SaveGamePlayerSettings.generated.h"

UCLASS()
class BEATSHOTGLOBAL_API USaveGamePlayerSettings : public USaveGame
{
	GENERATED_BODY()

public:
	/** @return a copy of PlayerSettings */
	FPlayerSettings GetPlayerSettings() const;

	/** Saves Game specific settings, preserving all other settings.
	 *  @param InGameSettings Game Settings to save
	 */
	void SavePlayerSettings(const FPlayerSettings_Game& InGameSettings);

	/** Saves Audio Analyzer specific settings, preserving all other settings.
	 *  @param InAudioAnalyzerSettings Audio Analyzer Settings to save
	 */
	void SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InAudioAnalyzerSettings);

	/** Saves User specific settings, preserving all other settings.
	 *  @param InUserSettings User Settings to save
	 */
	void SavePlayerSettings(const FPlayerSettings_User& InUserSettings);

	/** Saves CrossHair specific settings, preserving all other settings.
	 *  @param InCrossHairSettings CrossHair Settings to save
	 */
	void SavePlayerSettings(const FPlayerSettings_CrossHair& InCrossHairSettings);

private:
	UPROPERTY()
	FPlayerSettings PlayerSettings;
};
