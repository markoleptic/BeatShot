// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include "SteamManager.generated.h"

class UBSGameInstance;

DECLARE_DELEGATE_OneParam(FOnAuthTicketForWebApiReady, const bool bSuccess)

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

	/** Calls GetAuthTicketForWebApi using Steam Api, callback is OnTicketForWebApiResponse */
	void CreateAuthTicketForWebApi();
	
	/** Returns true if the WebApiTicket has been filled, otherwise false */
	bool IsAuthTicketReady() const { return !WebApiTicket.IsEmpty(); }

	/** Returns the WebApiTicket */
	FString GetWebApiTicket() { return WebApiTicket; }

	/** Clears the WebApiTicket */
	UFUNCTION()
	void ResetWebApiTicket() { WebApiTicket.Empty(); }

	/** Delegate executed when a ticket for a WebApiResponse is generated or failed to generate */
	FOnAuthTicketForWebApiReady OnAuthTicketForWebApiReady;

private:
	/**Steam Callback Setups Using STEAM_CALLBACK_MANUAL() */
	
	STEAM_CALLBACK_MANUAL(USteamManager, OnSteamOverlayActive, GameOverlayActivated_t, OnSteamOverlayActiveCallback);
	STEAM_CALLBACK_MANUAL(USteamManager, OnTicketForWebApiResponse, GetTicketForWebApiResponse_t, OnTicketForWebApiResponseCallback);

	FString WebApiTicket;
};
