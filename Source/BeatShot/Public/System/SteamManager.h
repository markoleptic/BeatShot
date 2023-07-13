// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include "SteamManager.generated.h"

class UBSGameInstance;

/** Class responsible for linking to Steam */
UCLASS()
class BEATSHOT_API USteamManager : public UObject
{
	GENERATED_BODY()

public:
	USteamManager();
	~USteamManager();

	void InitializeSteamManager();

	void AssignGameInstance(UBSGameInstance* InDefaultGameInstance);

	UPROPERTY()
	UBSGameInstance* DefaultGameInstance;

	void ResetWebApiTicket() { WebApiTicket.Empty(); }

private:
	/**Steam Callback Setups Using STEAM_CALLBACK_MANUAL() */
	
	STEAM_CALLBACK_MANUAL(USteamManager, OnSteamOverlayActive, GameOverlayActivated_t, OnSteamOverlayActiveCallback);
	STEAM_CALLBACK_MANUAL(USteamManager, OnTicketForWebApiResponse, GetTicketForWebApiResponse_t, OnTicketForWebApiResponseCallback);

	FString WebApiTicket;
};
