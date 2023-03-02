// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "ThirdParty/Steamworks/Steamv153/sdk/public/steam/steam_api.h"
#include "SteamManager.generated.h"

class UDefaultGameInstance;

UCLASS()
class BEATSHOT_API USteamManager : public UObject
{
	GENERATED_BODY()
	
public:
	USteamManager();
	
	~USteamManager();

	void InitializeSteamManager();

	void AssignGameInstance(UDefaultGameInstance* InDefaultGameInstance);

	UPROPERTY()
	UDefaultGameInstance *DefaultGameInstance;
	
private:
	//Steam Callback Setups Using STEAM_CALLBACK_MANUAL()
	STEAM_CALLBACK_MANUAL(USteamManager, OnSteamOverlayActive, GameOverlayActivated_t, OnSteamOverlayActiveCallback);


};
