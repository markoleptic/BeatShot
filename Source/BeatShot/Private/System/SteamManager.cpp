// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "System/SteamManager.h"
#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include "Containers/UnrealString.h"
#include "BSGameInstance.h"

USteamManager::USteamManager()
{
}

USteamManager::~USteamManager()
{
}

void USteamManager::InitializeSteamManager()
{
	OnSteamOverlayActiveCallback.Register(this, &ThisClass::OnSteamOverlayActive);
	OnTicketForWebApiResponseCallback.Register(this, &ThisClass::OnTicketForWebApiResponse);
}

void USteamManager::AssignGameInstance(UBSGameInstance* InDefaultGameInstance)
{
	DefaultGameInstance = InDefaultGameInstance;
}

void USteamManager::CreateAuthTicketForWebApi()
{
	if (SteamUser())
	{
		SteamUser()->GetAuthTicketForWebApi("kxuhYhcZQyDdFtpS");
	}
}

/** Callback for when steam overlay is toggled */
void USteamManager::OnSteamOverlayActive(GameOverlayActivated_t* pParam)
{
	const bool bIsCurrentOverlayActive = pParam->m_bActive != 0;
	//DefaultGameInstance; //So that the call list reference on the Lambda works
	if (bIsCurrentOverlayActive)
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			DefaultGameInstance->OnSteamOverlayIsOn();
		});
	}
	else
	{
		AsyncTask(ENamedThreads::GameThread, [this]()
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
		if (OnAuthTicketForWebApiReady.IsBound()) OnAuthTicketForWebApiReady.Execute(true);
	}
	else
	{
		if (OnAuthTicketForWebApiReady.IsBound()) OnAuthTicketForWebApiReady.Execute(false);
	}
}
