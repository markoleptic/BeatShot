// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "SaveLoadInterface.h"
#include "Engine/GameInstance.h"
#include "BSGameInstance.generated.h"

class AGameModeBase;
class USteamManager;
class ASphereTarget;
class ABSPlayerController;

UCLASS()
class BEATSHOT_API UBSGameInstance : public UGameInstance, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void Init() override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Steamworks")
	USteamManager* SteamManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steamworks")
	bool IsSteamOverlayActive;

	const bool EnableUSteamManagerFeatures = true;

	/** Fire this from Blueprints to tell the actor when to initialize CPP elements */
	UFUNCTION(BlueprintCallable, Category = "Steamworks")
	bool InitializeCPPElements();

	/** A function pair that can be called externally executes OnSteamOverlayIsActive() */
	void OnSteamOverlayIsOn();
	void OnSteamOverlayIsOff();

	UFUNCTION()
	void OnSteamOverlayIsActive(bool bIsOverlayActive) const;

	UFUNCTION()
	void StartGameMode() const;

	UFUNCTION()
	void HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState);

	/** The defining game mode options that are populated from a menu widget, and accessed by the GameMode */
	UPROPERTY(EditDefaultsOnly)
	FBSConfig BSConfig;

	UPROPERTY()
	FGameModeTransitionState LastGameModeTransitionState;

	/** Adds an actor's FOnPlayerSettingsChanged_Game delegate to the call list */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_Game& Delegate);

	/** Adds an actor's FOnPlayerSettingsChanged_AudioAnalyzer delegate to the call list */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_AudioAnalyzer& Delegate);

	/** Adds an actor's FOnPlayerSettingsChanged_User delegate to the call list */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_User& Delegate);

	/** Adds an actor's FOnPlayerSettingsChanged_CrossHair delegate to the call list */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_CrossHair& Delegate);

	/** Adds an actor's FOnPlayerSettingsChanged_VideoAndSound delegate to the call list */
	void AddDelegateToOnPlayerSettingsChanged(FOnPlayerSettingsChanged_VideoAndSound& Delegate);

	FOnPlayerSettingsChanged_Game& GetPublicGameSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_Game;}
	FOnPlayerSettingsChanged_AudioAnalyzer& GetPublicAudioAnalyzerSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_AudioAnalyzer;}
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_User;}
	FOnPlayerSettingsChanged_CrossHair& GetPublicCrossHairSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_CrossHair;}
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_VideoAndSound;}
	
private:
	
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_AudioAnalyzer(const FPlayerSettings_AudioAnalyzer& AudioAnalyzerSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
	virtual void OnPlayerSettingsChanged_CrossHair(const FPlayerSettings_CrossHair& CrossHairSettings) override;
	virtual void OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings) override;
};
