// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveLoadInterface.h"
#include "Engine/GameInstance.h"
#include "BSGameInstance.generated.h"

class SLoadingScreenWidget;
class ATimeOfDayManager;
class USteamManager;

/** Base GameInstance for this game */
UCLASS()
class BEATSHOT_API UBSGameInstance : public UGameInstance, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	virtual void Init() override;
	virtual FGameInstancePIEResult StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer,
		const FGameInstancePIEParameters& Params) override;

	/** Called after a level has been loaded */
	void OnPostLoadMapWithWorld(UWorld* World);

	/** Called after the loading screen widget has completed its fade out. */
	void OnLoadingScreenFadeOutComplete();

	/** Prepares the loading screen widget for the MoviePlayer. */
	void PrepareLoadingScreen();
	
	/** Called when the game instance is started either normally or through PIE. */
	virtual void OnStart() override;

	/** Runs a hardware benchmark if not done previously, loads video settings, initializes DLSS settings, and saves video settings. */
	void InitVideoSettings();

public:
	/** Handles game mode transitions initiated by Main Menu, Pause Menu, or Post Game Menu */
	void HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState);

	/** Returns shared pointer to defining game mode options configuration */
	TSharedPtr<FBSConfig> GetBSConfig() const { return BSConfig; }

	/** Returns the object that interfaces with Steam API. */
	TObjectPtr<USteamManager> GetSteamManager() const { return SteamManager; }

	/** Returns actor that manages the time of day in the Range level. */
	TObjectPtr<ATimeOfDayManager> GetTimeOfDayManager() const { return TimeOfDayManager; }

	/** Sets the actor that manages the time of day in the Range level. */
	void SetTimeOfDayManager(const TObjectPtr<ATimeOfDayManager> InManager) { TimeOfDayManager = InManager; }

	/** Handles saving scores to database, called by BSGameMode. */
	void SavePlayerScoresToDatabase(const bool bWasValidToSave);
	
	/** A function pair that can be called externally, executes OnSteamOverlayIsActive(). */
	void OnSteamOverlayIsOn();
	void OnSteamOverlayIsOff();

	/** Binds an actor's Game Settings delegate to the Game Instance's OnPlayerSettingsChanged_Game function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_Game to all actors subscribed for updates. */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_Game& Delegate);

	/** Binds an actor's Audio Analyzer Settings delegate to the Game Instance's OnPlayerSettingsChanged_AudioAnalyzer
	 *  function, which broadcasts OnPlayerSettingsChangedDelegate_AudioAnalyzer to all actors subscribed for updates. */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_AudioAnalyzer& Delegate);

	/** Binds an actor's User Settings delegate to the Game Instance's OnPlayerSettingsChanged_User function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_User to all actors subscribed for updates. */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_User& Delegate);

	/** Binds an actor's CrossHair Settings delegate to the Game Instance's OnPlayerSettingsChanged_CrossHair function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_CrossHair to all actors subscribed for updates. */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_CrossHair& Delegate);

	/** Binds an actor's Video and Sound Settings delegate to the Game Instance's OnPlayerSettingsChanged_VideoAndSound
	 *  function, which broadcasts OnPlayerSettingsChangedDelegate_VideoAndSound to all actors subscribed for updates. */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_VideoAndSound& Delegate);

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_Game, which can be used to add a function to the
	 *  delegate's call list. The function is then called any time a Game Setting is changed. */
	FOnPlayerSettingsChanged_Game& GetPublicGameSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_Game;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_AudioAnalyzer, which can be used to add a function
	 *  to the delegate's call list. The function is then called any time an Audio Analyzer Setting is changed. */
	FOnPlayerSettingsChanged_AudioAnalyzer& GetPublicAudioAnalyzerSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_AudioAnalyzer;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_User, which can be used to add a function to the
	 *  delegate's call list. The function is then called any time a User Setting is changed. */
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_User;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_CrossHair, which can be used to add a function to
	 *  the delegate's call list. The function is then called any time a CrossHair Setting is changed. */
	FOnPlayerSettingsChanged_CrossHair& GetPublicCrossHairSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_CrossHair;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_VideoAndSound, which can be used to add a function
	 *  to the delegate's call list. The function is then called any time a Video And Sound Setting is changed. */
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_VideoAndSound;
	}

protected:
	void SetBSConfig(const FBSConfig& InConfig);
	
	/** Unpauses player controller and fades the screen to black. When fade to black is finished, the MainMenu,
	 *  Pause Menu, and Post Game Menu are all hidden if currently shown. If the level is the Range, the countdown
	 *  is shown, otherwise the Range level is opened.  */
	void StartGameMode(const bool bIsRestart) const;

	/** Creates SteamManager object and allows it initialize. Assigns Game Instance and binds to functions. */
	void InitializeSteamManager();

	/** Pauses the game and shows the Pause Screen if the overlay is active. */
	void OnSteamOverlayIsActive(bool bIsOverlayActive) const;

	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings) override;
	virtual void OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings) override;

	/** Object that interfaces with Steam API. */
	UPROPERTY()
	TObjectPtr<USteamManager> SteamManager;

	/** Actor that manages the time of day in the Range level. */
	UPROPERTY()
	TObjectPtr<ATimeOfDayManager> TimeOfDayManager;

	/** Shared pointer to loading screen slate widget. */
	TSharedPtr<SLoadingScreenWidget> LoadingScreenWidget;
	
	/** The defining game mode options that are populated from a menu widget, and accessed by the GameMode. */
	TSharedPtr<FBSConfig> BSConfig;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* GlobalSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* MenuSound;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundMix* GlobalSoundMix;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loading Screen")
	USlateWidgetStyleAsset* SlateWidgetStyleAsset;

	/** Whether or not the Steam Overlay is active. */
	bool IsSteamOverlayActive = false;
	
	/** Whether or not to postpone QuitGame until after a save game Http response. */
	bool bQuitToDesktopAfterSave = false;

	/** Whether or not the loading screen is the initial one, which changes how the loading screen is rendered. */
	bool bIsInitialLoadingScreen = true;
};
