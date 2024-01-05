// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "System/SteamManager.h"
#include <steam/isteamutils.h>
#include "Containers/UnrealString.h"
#include "BSGameInstance.h"

// Stats array which will hold data about the stats and their state
TSet<FSteamStat> g_Stats = {
	// Number of games played Multi-Beat
	_STAT_ID(1, Stat_Int, "NumGamesPlayed_GM1"),
	// Number of games played Single-Beat
	_STAT_ID(2, Stat_Int, "NumGamesPlayed_GM2"),
	// Number of games played Beat-Track
	_STAT_ID(3, Stat_Int, "NumGamesPlayed_GM3"),
	// Number of games played Beat-Grid
	_STAT_ID(4, Stat_Int, "NumGamesPlayed_GM4"),
	// Number of games played Cluster-Beat
	_STAT_ID(5, Stat_Int, "NumGamesPlayed_GM5"),
	// Number of games played Charged Beat-Track
	_STAT_ID(6, Stat_Int, "NumGamesPlayed_GM6"),
	// Number of games played Custom Game Mode
	_STAT_ID(8, Stat_Int, "NumGamesPlayed_Custom"),
};

// Achievement array which will hold data about the achievements and their state
TSet<FSteamAchievement> g_Achievements = {
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
	AppId = 0;
	StatsData = TSet<FSteamStat>();
	AchievementData = TSet<FSteamAchievement>();
	bInitializedStats = false;
	DefaultGameInstance = nullptr;
}

void USteamManager::InitializeSteamManager()
{
	if (SteamUtils())
	{
		AppId = SteamUtils()->GetAppID();
	}
	OnSteamOverlayActiveDelegate.Register(this, &USteamManager::OnSteamOverlayActive);
	OnAuthTicketForWebApiResponseDelegate.Register(this, &USteamManager::OnAuthTicketForWebApiResponse);
	OnUserStatsReceivedDelegate.Register(this, &USteamManager::OnUserStatsReceived);
	OnUserStatsStoredDelegate.Register(this, &USteamManager::OnUserStatsStored);
	OnAchievementStoredDelegate.Register(this, &USteamManager::OnAchievementStored);
	StatsData = g_Stats;
	AchievementData = g_Achievements;
	bInitializedStats = RequestStats();

	// Online Subsystem Steam calls these
	// SteamAPI_Init()
	// SteamAPI_RunCallbacks();
}

void USteamManager::AssignGameInstance(UBSGameInstance* InDefaultGameInstance)
{
	DefaultGameInstance = InDefaultGameInstance;
}

void USteamManager::UpdateStat(const char* StatAPIName, ESteamStatType StatType, int IntValue, float FloatValue)
{
	if (!bInitializedStats) return;
	for (FSteamStat& Stat : StatsData)
	{
		if (Stat.APIName == StatAPIName)
		{
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
		}
		break;
	}
}

void USteamManager::UpdateStat_NumGamesPlayed(const EBaseGameMode GameMode, int IntValue)
{
	if (!bInitializedStats) return;
	
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
}

bool USteamManager::CreateAuthTicketForWebApi(TSharedPtr<FOnAuthTicketForWebApiResponseCallbackHandler> CallbackHandler)
{
	if (!SteamUser()) return false;
	ActiveCallbacks.Enqueue(CallbackHandler);
	CallbackHandler->Handle = SteamUser()->GetAuthTicketForWebApi("kxuhYhcZQyDdFtpS");
	return true;
}

/** Callback for when a ticket for a WebApiResponse is generated */
void USteamManager::OnAuthTicketForWebApiResponse(GetTicketForWebApiResponse_t* pParam)
{
	TSharedPtr<FOnAuthTicketForWebApiResponseCallbackHandler> CallbackHandler;
	if (ActiveCallbacks.Dequeue(CallbackHandler))
	{
		BytesToHex(pParam->m_rgubTicket, pParam->m_cubTicket, CallbackHandler->Ticket);
		CallbackHandler->Result = pParam->m_eResult;
		CallbackHandler->OnAuthTicketForWebApiReady.ExecuteIfBound();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to dequeue a CallbackHandler."));
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

/** Callback for anytime RequestStats() is called. Updates the member variables AchievementData
 *  and StatsData to reflect the latest stats and achievement data returned from Steam. */
void USteamManager::OnUserStatsReceived(UserStatsReceived_t* pCallback)
{
	if (pCallback->m_nGameID != AppId || pCallback->m_eResult != k_EResultOK) return;
	
	for (FSteamAchievement& Achievement : AchievementData)
	{
		SteamUserStats()->GetAchievement(Achievement.APIName, &Achievement.bAchieved);
		float Percent;
		SteamUserStats()->GetAchievementAchievedPercent(Achievement.APIName, &Percent);
		UE_LOG(LogTemp, Warning, TEXT("Achievement.APIName %hs Percent: %f"), Achievement.APIName, Percent);
	}

	for (FSteamStat& Stat : StatsData)
	{
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
		UE_LOG(LogTemp, Warning, TEXT("Stat.APIName %hs IntValue: %d FloatDesc: %f"), Stat.APIName, Stat.IntValue, Stat.FloatValue);
	}

	UE_LOG(LogTemp, Warning, TEXT("Received stats from Steam"));
	bInitializedStats = true;
}

/** Callback for anytime you attempt to store stats on Steam */
void USteamManager::OnUserStatsStored(UserStatsStored_t* pCallback)
{
	// We may get callbacks for other games' stats arriving, ignore them
	if (pCallback->m_nGameID != AppId) return;
	
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
		UE_LOG(LogTemp, Warning, TEXT("StatsStored - failed, %d" ), pCallback->m_eResult);
	}
	
}

/** Callback for anytime Achievements are successfully stored on Steam */
void USteamManager::OnAchievementStored(UserAchievementStored_t* pCallback)
{
	if (AppId == pCallback->m_nGameID)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stored Achievement for Steam"));
	}
}

bool USteamManager::RequestStats()
{
	if (!SteamUserStats() || !SteamUser() || !SteamUser()->BLoggedOn())
	{
		return false;
	}
	return SteamUserStats()->RequestCurrentStats();
}

bool USteamManager::StoreStats()
{
	if (!bInitializedStats || !SteamUserStats()) return false;
	
	// load stats
	for (FSteamStat& Stat : StatsData)
	{
		switch (Stat.StatType)
		{
		case Stat_Int:
			SteamUserStats()->SetStat(Stat.APIName, Stat.IntValue);
			break;
		case Stat_Float:
			SteamUserStats()->SetStat(Stat.APIName, Stat.FloatValue);
			break;
		case Stat_AvgRate:
			SteamUserStats()->UpdateAvgRateStat(Stat.APIName, Stat.FloatAvgNumerator,
				Stat.FloatAvgDenominator);
			// The averaged result is calculated for us
			SteamUserStats()->GetStat(Stat.APIName, &Stat.FloatValue);
			break;
		default:
			break;
		}
	}
	return SteamUserStats()->StoreStats();
}

bool USteamManager::SetAchievement(const char* ID) const
{
	if (!bInitializedStats || SteamUserStats()) return false;
	SteamUserStats()->SetAchievement(ID);
	return SteamUserStats()->StoreStats();
}

FSteamStat* USteamManager::GetStat_NumGamesPlayed(const EBaseGameMode GameMode)
{
	const char* APIName;
	switch (GameMode)
	{
	case EBaseGameMode::MultiBeat:
	case EBaseGameMode::MultiBeatPrecision:
	case EBaseGameMode::MultiBeatSpeed:
		APIName = "NumGamesPlayed_GM1";
		break;
	case EBaseGameMode::SingleBeat:
		APIName = "NumGamesPlayed_GM2";
		break;
	case EBaseGameMode::BeatTrack:
		APIName = "NumGamesPlayed_GM3";
		break;
	case EBaseGameMode::BeatGrid:
		APIName = "NumGamesPlayed_GM4";
		break;
	case EBaseGameMode::ClusterBeat:
		APIName = "NumGamesPlayed_GM5";
		break;
	case EBaseGameMode::ChargedBeatTrack:
		APIName = "NumGamesPlayed_GM6";
		break;
	case EBaseGameMode::None:
	default:
		APIName = "NumGamesPlayed_Custom";
		break;
	}
	for (FSteamStat& Stat : StatsData)
	{
		if (Stat.APIName == APIName)
		{
			return &Stat;
		}
	}
	return nullptr;	
}
