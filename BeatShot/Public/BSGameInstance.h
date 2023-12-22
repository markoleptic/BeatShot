// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HttpRequestInterface.h"
#include "SaveLoadInterface.h"
#include "Engine/GameInstance.h"
#include "BSGameInstance.generated.h"

class ATimeOfDayManager;
class AGameModeBase;
class USteamManager;
class ATarget;
class ABSPlayerController;

DECLARE_DELEGATE(FOnPCFinishedUsingAuthTicket);

/** Base GameInstance for this game */
UCLASS()
class BEATSHOT_API UBSGameInstance : public UGameInstance, public ISaveLoadInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	virtual void Init() override;

	virtual void OnStart() override;

	void InitVideoSettings();

	virtual void Shutdown() override;

public:
	TObjectPtr<ATimeOfDayManager> TimeOfDayManager;

	UFUNCTION()
	void HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState);

	FBSConfig GetBSConfig() const { return BSConfig; }

	USteamManager* GetSteamManager() const { return SteamManager; }

	/** A function pair that can be called externally, executes OnSteamOverlayIsActive() */
	void OnSteamOverlayIsOn();
	void OnSteamOverlayIsOff();

	/** Called when the Player Controller has shown the Main Menu */
	void OnPlayerControllerReadyForSteamLogin(ABSPlayerController* PlayerController);

	/** Binds an actor's Game Settings delegate to the Game Instance's OnPlayerSettingsChanged_Game function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_Game to all actors subscribed for updates */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_Game& Delegate);

	/** Binds an actor's Audio Analyzer Settings delegate to the Game Instance's OnPlayerSettingsChanged_AudioAnalyzer function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_AudioAnalyzer to all actors subscribed for updates */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_AudioAnalyzer& Delegate);

	/** Binds an actor's User Settings delegate to the Game Instance's OnPlayerSettingsChanged_User function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_User to all actors subscribed for updates */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_User& Delegate);

	/** Binds an actor's CrossHair Settings delegate to the Game Instance's OnPlayerSettingsChanged_CrossHair function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_CrossHair to all actors subscribed for updates */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_CrossHair& Delegate);

	/** Binds an actor's Video and Sound Settings delegate to the Game Instance's OnPlayerSettingsChanged_VideoAndSound function,
	 *  which broadcasts OnPlayerSettingsChangedDelegate_VideoAndSound to all actors subscribed for updates */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_VideoAndSound& Delegate);

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_Game, which can be used to add a function to the delegate's call list.
	 *  The function is then called any time a Game Setting is changed */
	FOnPlayerSettingsChanged_Game& GetPublicGameSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_Game;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_AudioAnalyzer, which can be used to add a function to the delegate's call list.
	 *  The function is then called any time an Audio Analyzer Setting is changed */
	FOnPlayerSettingsChanged_AudioAnalyzer& GetPublicAudioAnalyzerSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_AudioAnalyzer;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_User, which can be used to add a function to the delegate's call list.
	 *  The function is then called any time a User Setting is changed */
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_User;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_CrossHair, which can be used to add a function to the delegate's call list.
	 *  The function is then called any time a CrossHair Setting is changed */
	FOnPlayerSettingsChanged_CrossHair& GetPublicCrossHairSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_CrossHair;
	}

	/** Returns the Game Instance's OnPlayerSettingsChangedDelegate_VideoAndSound, which can be used to add a function to the delegate's call list.
	 *  The function is then called any time a Video And Sound Setting is changed */
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate()
	{
		return OnPlayerSettingsChangedDelegate_VideoAndSound;
	}

protected:
	UFUNCTION()
	void StartGameMode(const bool bIsRestart) const;

	/** Creates SteamManager object and allows it initialize. Assigns Game Instance and binds to functions */
	bool InitializeSteamManager();

	/** Pauses the game and shows the Pause Screen if the overlay is active */
	void OnSteamOverlayIsActive(bool bIsOverlayActive) const;

	/** Checks if bHttpAuthTicketAsyncTaskComplete and bLoginToScoreBrowserAsyncTaskComplete are true and if so calls ResetWebApiTicket on SteamManager */
	void CheckIfAsyncAuthTasksComplete();

	/** Callback for when PlayerController is done using the AuthTicket */
	void OnLoginToScoreBrowserAsyncTaskComplete();

	/** Called after an HTTP request has been sent to BeatShot website and a response was received */
	UFUNCTION()
	void OnAuthTicketForWebApiResponse(const FSteamAuthTicketResponse& Response, const bool bSuccess);

	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void
	OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings) override;
	virtual void
	OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings) override;

	/** Delegate passed to AuthenticateSteamUser Http request which is executed when it receives a response */
	FOnTicketWebApiResponse TicketWebApiResponse;

	/** Delegate passed to a PlayerController with AuthTickets to be called when they're done being used */
	FOnPCFinishedUsingAuthTicket OnPCFinishedUsingAuthTicket;

	/** Object that interfaces with Steam API */
	UPROPERTY()
	TObjectPtr<USteamManager> SteamManager;

	/** The defining game mode options that are populated from a menu widget, and accessed by the GameMode */
	UPROPERTY(EditDefaultsOnly)
	FBSConfig BSConfig;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundMix* GlobalSoundMix;

	/** Whether or not the Steam Overlay is active */
	bool IsSteamOverlayActive;

	/** Whether or not the Steam Manager has been initialized */
	bool bSteamManagerInitialized = false;

	/** Whether or not the AuthTicket async task has been completed */
	bool bHttpAuthTicketAsyncTaskComplete = false;

	/** Whether or not the login to score browser widget async task has been completed */
	bool bLoginToScoreBrowserAsyncTaskComplete = false;
};
