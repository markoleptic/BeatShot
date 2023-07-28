// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "System/SteamManager.h"
#include <steam/isteamutils.h>
#include "Containers/UnrealString.h"
#include "BSGameInstance.h"

// Stats array which will hold data about the stats and their state
FSteamStat g_Stats[] =
{
	_STAT_ID( 1, Stat_Int, "NumGamesPlayed_GM1"),
	_STAT_ID( 2, Stat_Int, "NumGamesPlayed_GM2"),
	_STAT_ID( 3, Stat_Int, "NumGamesPlayed_GM3"),
	_STAT_ID( 4, Stat_Int, "NumGamesPlayed_GM4"),
	_STAT_ID( 5, Stat_Int, "NumGamesPlayed_GM5"),
	_STAT_ID( 6, Stat_Int, "NumGamesPlayed_GM6"),
};

// Defining our achievements
enum EAchievements
{
	ACH_Play_GM1_Once = 0,
};

// Achievement array which will hold data about the achievements and their state
FSteamAchievement g_Achievements[] =
{
	_ACH_ID( ACH_Play_GM1_Once, "PlayGM1Once" )
};

USteamManager::USteamManager()
{
}

void USteamManager::InitializeSteamManager()
{
	OnSteamOverlayActiveCallback.Register(this, &ThisClass::OnSteamOverlayActive);
	OnTicketForWebApiResponseCallback.Register(this, &ThisClass::OnTicketForWebApiResponse);
	OnUserStatsReceivedCallback.Register(this, &ThisClass::OnUserStatsReceived);
	OnUserStatsStoredCallback.Register(this, &ThisClass::OnUserStatsStored);
	OnAchievementStoredCallback.Register(this, &ThisClass::OnAchievementStored);

	AppId = SteamUtils()->GetAppID();
	
	StatsData = g_Stats;
	AchievementData = g_Achievements;
	NumStats = sizeof(StatsData) / sizeof(FSteamStat);
	NumAchievements = sizeof(AchievementData) / sizeof(FSteamAchievement);
	bInitializedStats = RequestStats();

	// TODO: Find place to call this
	//SteamAPI_RunCallbacks();
}

void USteamManager::ShutdownSteamManager()
{
	delete StatsData;
	delete AchievementData;
}

void USteamManager::AssignGameInstance(UBSGameInstance* InDefaultGameInstance)
{
	DefaultGameInstance = InDefaultGameInstance;
}

void USteamManager::UpdateStat(const char* StatAPIName, ESteamStatType StatType, int IntValue, float FloatValue)
{
	if (!bInitializedStats)
	{
		return;
	}
	for (int iStat = 0; iStat < NumStats; ++iStat)
	{
		if (StatsData[iStat].APIName == StatAPIName)
		{
			FSteamStat &Stat = StatsData[iStat];
			switch (StatType)
			{
			case Stat_Int:
				Stat.IntValue += IntValue;
				break;
			case Stat_Float:
			case Stat_AvgRate:
				Stat.FloatValue += FloatValue;
				break;
			default:
				break;
			}
			StoreStats();
			return;
		}
	}
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

/** Callback for anytime RequestStats() is called. Updates the member variables AchievementData
 *  and StatsData to reflect the latest stats and achievement data returned from Steam. */
void USteamManager::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
	if (AppId == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			// load achievements
			for (int iAch = 0; iAch < NumAchievements; ++iAch)
			{
				FSteamAchievement &Ach = AchievementData[iAch];
				
				SteamUserStats()->GetAchievement(Ach.APIName, &Ach.bAchieved);
				const char* AchieveName = SteamUserStats()->GetAchievementDisplayAttribute(Ach.APIName, "name");
				const char* AchieveDesc = SteamUserStats()->GetAchievementDisplayAttribute(Ach.APIName, "desc");
				UE_LOG(LogTemp, Display, TEXT("Ach.APIName %hs DisplayName: %hs AchieveDesc: %hs"), Ach.APIName, AchieveName, AchieveDesc);
			}

			// load stats
			for (int iStat = 0; iStat < NumStats; ++iStat)
			{
				FSteamStat &Stat = StatsData[iStat];
				switch (Stat.StatType)
				{
				case Stat_Int:
					SteamUserStats()->GetStat(Stat.APIName, &Stat.IntValue);
					break;
				case Stat_Float:
				case Stat_AvgRate:
					SteamUserStats()->GetStat(Stat.APIName, &Stat.FloatValue);
					break;
				default:
					break;
				}
			}
			
			UE_LOG(LogTemp, Display, TEXT("Received stats from Steam"));
			bInitializedStats = true;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("RequestStats - failed, %d"), pCallback->m_eResult);
		}
	}
}

/** Callback for anytime you attempt to store stats on Steam */
void USteamManager::OnUserStatsStored(UserStatsStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (AppId == pCallback->m_nGameID)
	{
		if (k_EResultOK == pCallback->m_eResult)
		{
			UE_LOG(LogTemp, Display, TEXT("Stored stats for Steam"));
		}
		else if (pCallback->m_eResult == k_EResultInvalidParam)
		{
			// One or more stats we set broke a constraint. They've been reverted,
			// and we should re-iterate the values now to keep in sync.
			UE_LOG(LogTemp, Display, TEXT("StoreStats - some failed to validate"));
			// Fake up a callback here so that we re-load the values.
			UserStatsReceived_t callback;
			callback.m_eResult = k_EResultOK;
			callback.m_nGameID = AppId;
			OnUserStatsReceived(&callback);
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("StatsStored - failed, %d" ), pCallback->m_eResult );
		}
	}
}

/** Callback for anytime Achievements are successfully stored on Steam */
void USteamManager::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (AppId == pCallback->m_nGameID)
	{
		UE_LOG(LogTemp, Display, TEXT("Stored Achievement for Steam"));
	}
}

bool USteamManager::RequestStats()
{
	// Is Steam loaded? If not we can't get stats.
	if (SteamUserStats() == nullptr || SteamUser() == nullptr)
	{
		return false;
	}
	// Is the user logged on?  If not we can't get stats.
	if (!SteamUser()->BLoggedOn())
	{
		return false;
	}
	// Request user stats.
	return SteamUserStats()->RequestCurrentStats();
}

bool USteamManager::StoreStats()
{
	if ( bInitializedStats )
	{
		// load stats
		for ( int iStat = 0; iStat < NumStats; ++iStat )
		{
			FSteamStat& CurrentStat = StatsData[iStat];
			switch (CurrentStat.StatType)
			{
			case Stat_Int:
				SteamUserStats()->SetStat(CurrentStat.APIName, CurrentStat.IntValue);
				break;
			case Stat_Float:
				SteamUserStats()->SetStat(CurrentStat.APIName, CurrentStat.FloatValue);
				break;
			case Stat_AvgRate:
				SteamUserStats()->UpdateAvgRateStat(CurrentStat.APIName, CurrentStat.FloatAvgNumerator, CurrentStat.FloatAvgDenominator );
				// The averaged result is calculated for us
				SteamUserStats()->GetStat(CurrentStat.APIName, &CurrentStat.FloatValue);
				break;
			default:
				break;
			}
		}
		return SteamUserStats()->StoreStats();
	}
	return false;
}

bool USteamManager::SetAchievement(const char* ID)
{
	// Have we received a call back from Steam yet?
	if (bInitializedStats)
	{
		SteamUserStats()->SetAchievement(ID);
		return SteamUserStats()->StoreStats();
	}
	// If not then we can't set achievements yet
	return false;
}
