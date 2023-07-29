// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "System/SteamManager.h"
#include <steam/isteamutils.h>
#include "Containers/UnrealString.h"
#include "BSGameInstance.h"

// Stats array which will hold data about the stats and their state
TArray<FSteamStat> g_Stats =
{
	// Number of games played Multi-Beat
	_STAT_ID( 1, Stat_Int, "NumGamesPlayed_GM1"),
	// Number of games played Single-Beat
	_STAT_ID( 2, Stat_Int, "NumGamesPlayed_GM2"),
	// Number of games played Beat-Track
	_STAT_ID( 3, Stat_Int, "NumGamesPlayed_GM3"),
	// Number of games played Beat-Grid
	_STAT_ID( 4, Stat_Int, "NumGamesPlayed_GM4"),
	// Number of games played Cluster-Beat
	_STAT_ID( 5, Stat_Int, "NumGamesPlayed_GM5"),
	// Number of games played Charged Beat-Track
	_STAT_ID( 6, Stat_Int, "NumGamesPlayed_GM6"),
	// Number of games played Custom Game Mode
	_STAT_ID( 8, Stat_Int, "NumGamesPlayed_Custom"),
};

// Achievement array which will hold data about the achievements and their state
TArray<FSteamAchievement> g_Achievements =
{
	_ACH_ID(ACH_PlayAnyGM, "PlayAnyGM"),
	_ACH_ID(ACH_Participant_GM1, "Participant_GM1"),
	_ACH_ID(ACH_Participant_GM2, "Participant_GM2"),
	_ACH_ID(ACH_Participant_GM3, "Participant_GM3"),
	_ACH_ID(ACH_Participant_GM4, "Participant_GM4"),
	_ACH_ID(ACH_Participant_GM5, "Participant_GM5"),
	_ACH_ID(ACH_Participant_GM6, "Participant_GM6"),
	_ACH_ID(ACH_Enthusiast_GM1, "Enthusiast_GM1"),
	_ACH_ID(ACH_Enthusiast_GM2, "Enthusiast_GM2"),
	_ACH_ID(ACH_Enthusiast_GM3, "Enthusiast_GM3"),
	_ACH_ID(ACH_Enthusiast_GM4, "Enthusiast_GM4"),
	_ACH_ID(ACH_Enthusiast_GM5, "Enthusiast_GM5"),
	_ACH_ID(ACH_Enthusiast_GM6, "Enthusiast_GM6"),
	_ACH_ID(ACH_Enjoyer_GM1, "Enjoyer_GM1"),
	_ACH_ID(ACH_Enjoyer_GM2, "Enjoyer_GM2"),
	_ACH_ID(ACH_Enjoyer_GM3, "Enjoyer_GM3"),
	_ACH_ID(ACH_Enjoyer_GM4, "Enjoyer_GM4"),
	_ACH_ID(ACH_Enjoyer_GM5, "Enjoyer_GM5"),
	_ACH_ID(ACH_Enjoyer_GM6, "Enjoyer_GM6"),
	_ACH_ID(ACH_Participant_Custom, "Participant_Custom"),
	_ACH_ID(ACH_Enthusiast_Custom, "Enthusiast_Custom"),
	_ACH_ID(ACH_Enjoyer_Custom, "Enjoyer_Custom"),
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
	
	bInitializedStats = RequestStats();

	// TODO: Find place to call this
	//SteamAPI_RunCallbacks();
}

void USteamManager::ShutdownSteamManager()
{
	StatsData.Empty();
	AchievementData.Empty();
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
	for (int i = 0; i < StatsData.Num(); ++i)
	{
		if (StatsData[i].APIName == StatAPIName)
		{
			FSteamStat &Stat = StatsData[i];
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

void USteamManager::UpdateStat_NumGamesPlayed(const EBaseGameMode GameMode, int IntValue)
{
	if (!bInitializedStats)
	{
		return;
	}

	if (FSteamStat* Stat = GetStat_NumGamesPlayed(GameMode))
	{
		FSteamStat& Ref = *Stat;
		Ref.IntValue += IntValue;
		StoreStats();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find FSteamStat for GameMode"));
	}
	
	/*for (int i = 0; i < StatsData.Num(); ++i)
	{
		if (StatsData[i].APIName == StatAPIName)
		{
			FSteamStat &Stat = StatsData[i];
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
	}*/
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
			for (int i = 0; i < AchievementData.Num(); ++i)
			{
				FSteamAchievement &Ach = AchievementData[i];
				SteamUserStats()->GetAchievement(Ach.APIName, &Ach.bAchieved);
				//const char* AchieveName = SteamUserStats()->GetAchievementDisplayAttribute(Ach.APIName, "name");
				//const char* AchieveDesc = SteamUserStats()->GetAchievementDisplayAttribute(Ach.APIName, "desc");
			}

			// load stats
			for (int i = 0; i < StatsData.Num(); ++i)
			{
				FSteamStat &Stat = StatsData[i];
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
				//UE_LOG(LogTemp, Warning, TEXT("Stat.APIName %hs IntValue: %d FloatDesc: %f"), Stat.APIName, Stat.IntValue, Stat.FloatValue);
			}
			
			UE_LOG(LogTemp, Warning, TEXT("Received stats from Steam"));
			bInitializedStats = true;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("RequestStats - failed, %d"), pCallback->m_eResult);
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
			UE_LOG(LogTemp, Warning, TEXT("Stored stats for Steam"));
		}
		else if (pCallback->m_eResult == k_EResultInvalidParam)
		{
			// One or more stats we set broke a constraint. They've been reverted,
			// and we should re-iterate the values now to keep in sync.
			UE_LOG(LogTemp, Warning, TEXT("StoreStats - some failed to validate"));
			// Fake up a callback here so that we re-load the values.
			UserStatsReceived_t callback;
			callback.m_eResult = k_EResultOK;
			callback.m_nGameID = AppId;
			OnUserStatsReceived(&callback);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("StatsStored - failed, %d" ), pCallback->m_eResult );
		}
	}
}

/** Callback for anytime Achievements are successfully stored on Steam */
void USteamManager::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	// we may get callbacks for other games' stats arriving, ignore them
	if (AppId == pCallback->m_nGameID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stored Achievement for Steam"));
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
	// Request user stats
	return SteamUserStats()->RequestCurrentStats();
}

bool USteamManager::StoreStats()
{
	if ( bInitializedStats )
	{
		// load stats
		for (int i = 0; i < StatsData.Num(); ++i)
		{
			FSteamStat& CurrentStat = StatsData[i];
			switch (CurrentStat.StatType)
			{
			case Stat_Int:
				SteamUserStats()->SetStat(CurrentStat.APIName, CurrentStat.IntValue);
				break;
			case Stat_Float:
				SteamUserStats()->SetStat(CurrentStat.APIName, CurrentStat.FloatValue);
				break;
			case Stat_AvgRate:
				SteamUserStats()->UpdateAvgRateStat(CurrentStat.APIName, CurrentStat.FloatAvgNumerator, CurrentStat.FloatAvgDenominator);
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

bool USteamManager::SetAchievement(const char* ID) const
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

bool USteamManager::SetAchievement(const ESteamAchievement InSteamAchievement) const
{
	// Have we received a call back from Steam yet?
	if (bInitializedStats)
	{
		if (const char* ID = GetAchievementIDFromESteamAchievement(InSteamAchievement))
		{
			SteamUserStats()->SetAchievement(ID);
			return SteamUserStats()->StoreStats();
		}
	}
	// If not then we can't set achievements yet
	return false;
}

const char* USteamManager::GetAchievementIDFromESteamAchievement(const ESteamAchievement InSteamAchievement) const
{
	const FSteamAchievement* Found = AchievementData.FindByPredicate([&] (const FSteamAchievement& SteamAchStruct)
	{
		return SteamAchStruct.ID == InSteamAchievement;
	});
	return Found ? Found->APIName : nullptr;
}

FSteamStat* USteamManager::GetStat_NumGamesPlayed(const EBaseGameMode GameMode)
{
	int ID;
	switch (GameMode)
	{
	case EBaseGameMode::MultiBeat:
		ID = 1;
		break;
	case EBaseGameMode::SingleBeat:
		ID = 2;
		break;
	case EBaseGameMode::BeatTrack:
		ID = 3;
		break;
	case EBaseGameMode::BeatGrid:
		ID = 4;
		break;
	case EBaseGameMode::ClusterBeat:
		ID = 5;
		break;
	case EBaseGameMode::ChargedBeatTrack:
		ID = 6;
		break;
	default: 
		ID = 8;
		break;
	}
	return StatsData.FindByPredicate([&] (const FSteamStat& SteamStat)
	{
		return SteamStat.ID == ID;
	});
}
