// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/Steamworks/Steamv151/sdk/public/steam/steam_api.h"
#include "SaveGameCustomGameMode.h"
#include "SaveLoadInterface.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class AGameModeBase;
class USteamManager;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;

UCLASS()
class BEATSHOT_API UDefaultGameInstance : public UGameInstance, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void Init() override;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Steamworks")
	USteamManager* SteamManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Steamworks")
	bool IsSteamOverlayActive;

	const bool EnableUSteamManagerFeatures = true;

	/* * Fire this from Blueprints to tell the actor when to initialize CPP elements */
	UFUNCTION(BlueprintCallable, Category = "Steamworks")
	bool InitializeCPPElements();

	/*A function pair that can be called externally executes OnSteamOverlayIsActive()*/
	void OnSteamOverlayIsOn();
	void OnSteamOverlayIsOff();

	UFUNCTION()
	void OnSteamOverlayIsActive(bool bIsOverlayActive) const;
	
	UFUNCTION()
	void StartGameMode() const;
	
	UFUNCTION()
	void HandleGameModeTransition(const FGameModeTransitionState& NewGameModeTransitionState);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY()
	FGameModeTransitionState LastGameModeTransitionState;
	
	bool bLastSavedShowOpenFileDialog;

	FOnPlayerSettingsChange OnPlayerSettingsChange;
};


