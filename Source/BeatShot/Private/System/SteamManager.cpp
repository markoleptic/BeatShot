// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "System/SteamManager.h"
#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include "Containers/UnrealString.h"
#include "BSGameInstance.h"

USteamManager::USteamManager()
{
	UE_LOG(LogTemp, Display, TEXT("USteamManager Constructor Called"));
}

USteamManager::~USteamManager()
{
}

void USteamManager::InitializeSteamManager()
{
	UE_LOG(LogTemp, Display, TEXT("Initializing USteamManager"));
	OnSteamOverlayActiveCallback.Register(this, &USteamManager::OnSteamOverlayActive);
	OnTicketForWebApiResponseCallback.Register(this, &USteamManager::OnTicketForWebApiResponse);
}

void USteamManager::AssignGameInstance(UBSGameInstance* InDefaultGameInstance)
{
	DefaultGameInstance = InDefaultGameInstance;
}

/** Callback for when steam overlay is toggled */
void USteamManager::OnSteamOverlayActive(GameOverlayActivated_t* pParam)
{
	const bool bIsCurrentOverlayActive = pParam->m_bActive != 0;
	//DefaultGameInstance; //So that the call list reference on the Lambda works
	if (bIsCurrentOverlayActive)
	{
		AsyncTask(ENamedThreads::GameThread, [&]()
		{
			DefaultGameInstance->OnSteamOverlayIsOn();
		});
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [&]()
		{
			DefaultGameInstance->OnSteamOverlayIsOff();
		});
	}
}

/** Callback for when a ticket for a WebApiResponse is generated */
void USteamManager::OnTicketForWebApiResponse(GetTicketForWebApiResponse_t* pParam)
{
	if (pParam->m_eResult == k_EResultOK)
	{
		BytesToHex(pParam->m_rgubTicket, pParam->m_cubTicket, WebApiTicket);
		AsyncTask(ENamedThreads::GameThread, [&]()
		{
			DefaultGameInstance->OnAuthTicketForWebApiResponseReady(WebApiTicket);
		});
	}
}
