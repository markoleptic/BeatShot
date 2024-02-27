// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BSPlayerSettingsInterface.generated.h"

struct FPlayerSettings;
struct FPlayerSettings_Game;
struct FPlayerSettings_AudioAnalyzer;
struct FPlayerSettings_User;
struct FPlayerSettings_CrossHair;
struct FPlayerSettings_VideoAndSound;
class USaveGamePlayerSettings;

/** Broadcast when game specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_Game, const FPlayerSettings_Game& GameSettings);

/** Broadcast when Audio Analyzer specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_AudioAnalyzer,
	const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings);

/** Broadcast when User specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_User, const FPlayerSettings_User& UserSettings);

/** Broadcast when CrossHair specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_CrossHair, const FPlayerSettings_CrossHair&
	CrossHairSettings);

/** Broadcast when VideoAndSound specific settings are changed and saved */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChanged_VideoAndSound,
	const FPlayerSettings_VideoAndSound& VideoAndSoundSettings);

UINTERFACE()
class UBSPlayerSettingsInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BEATSHOTGLOBAL_API IBSPlayerSettingsInterface
{
	GENERATED_BODY()
	
public:
	/** Loads all player settings from slot */
	static FPlayerSettings LoadPlayerSettings();

	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_User& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_VideoAndSound& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_CrossHair& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_Game& InSettingsStruct);
	/** Saves specific sub-setting struct, preserving all other settings */
	void SavePlayerSettings(const FPlayerSettings_AudioAnalyzer& InSettingsStruct);
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	
protected:
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_Game& NewGameSettings) {}
	
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_AudioAnalyzer& NewAudioAnalyzerSettings) {}
	
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_User& NewUserSettings) {}
	
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_CrossHair& NewCrossHairSettings) {}
	
	virtual void OnPlayerSettingsChanged(const FPlayerSettings_VideoAndSound& NewVideoAndSoundSettings) {}
	
	/** The delegate that is broadcast when this class saves Game settings */
	FOnPlayerSettingsChanged_Game OnPlayerSettingsChangedDelegate_Game;
	/** The delegate that is broadcast when this class saves AudioAnalyzer settings */
	FOnPlayerSettingsChanged_AudioAnalyzer OnPlayerSettingsChangedDelegate_AudioAnalyzer;
	/** The delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User OnPlayerSettingsChangedDelegate_User;
	/** The delegate that is broadcast when this class saves CrossHair settings */
	FOnPlayerSettingsChanged_CrossHair OnPlayerSettingsChangedDelegate_CrossHair;
	/** The delegate that is broadcast when this class saves VideoAndSound settings */
	FOnPlayerSettingsChanged_VideoAndSound OnPlayerSettingsChangedDelegate_VideoAndSound;
};