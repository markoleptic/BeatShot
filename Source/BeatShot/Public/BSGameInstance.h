// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "SaveGameCustomGameMode.h"
#include "SaveLoadInterface.h"
#include "Engine/GameInstance.h"
#include "BSGameInstance.generated.h"

class AGameModeBase;
class USteamManager;
class ASphereTarget;
class AGameModeActorBase;
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

	UPROPERTY(EditDefaultsOnly)
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY()
	FGameModeTransitionState LastGameModeTransitionState;

	bool bLastSavedShowOpenFileDialog;

	FOnPlayerSettingsChange OnPlayerSettingsChange;
};
