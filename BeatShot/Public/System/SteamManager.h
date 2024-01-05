// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include <steam/isteamuserstats.h>
#include <steam/steam_api_common.h>
#include "BSGameModeDataAsset.h"
#include "SteamManager.generated.h"

class UBSGameInstance;

#define _ACH_ID(id, name ) { id, #id, name, "", 0, 0 }
#define _STAT_ID(id, type, name) { id, type, name, 0, 0, 0, 0 }

UENUM()
enum ESteamAchievement : uint8
{
	ACH_PlayAnyGM          = 0,
	ACH_Participant_GM1    = 1,
	ACH_Participant_GM2    = 2,
	ACH_Participant_GM3    = 3,
	ACH_Participant_GM4    = 4,
	ACH_Participant_GM5    = 5,
	ACH_Participant_GM6    = 6,
	ACH_Enthusiast_GM1     = 7,
	ACH_Enthusiast_GM2     = 8,
	ACH_Enthusiast_GM3     = 9,
	ACH_Enthusiast_GM4     = 10,
	ACH_Enthusiast_GM5     = 11,
	ACH_Enthusiast_GM6     = 12,
	ACH_Enjoyer_GM1        = 13,
	ACH_Enjoyer_GM2        = 14,
	ACH_Enjoyer_GM3        = 15,
	ACH_Enjoyer_GM4        = 16,
	ACH_Enjoyer_GM5        = 17,
	ACH_Enjoyer_GM6        = 18,
	ACH_Participant_Custom = 19,
	ACH_Enthusiast_Custom  = 20,
	ACH_Enjoyer_Custom     = 21
};

USTRUCT()
struct FSteamAchievement
{
	GENERATED_BODY()

	int ID;
	char CharID[128];
	const char* APIName;
	char Description[256];
	bool bAchieved;
	int IconImage;

	FORCEINLINE bool operator==(const FSteamAchievement& Other) const
	{
		return APIName == Other.APIName;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSteamAchievement& Achievement)
	{
		return GetTypeHash(Achievement.APIName);
	}
};

UENUM()
enum ESteamStatType
{
	Stat_Int     = 0,
	Stat_Float   = 1,
	Stat_AvgRate = 2,
};

USTRUCT()
struct FSteamStat
{
	GENERATED_BODY()

	int ID;
	ESteamStatType StatType;
	const char* APIName;
	int IntValue;
	float FloatValue;
	float FloatAvgNumerator;
	float FloatAvgDenominator;

	FORCEINLINE bool operator==(const FSteamStat& Other) const
	{
		return APIName == Other.APIName;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FSteamStat& Stat)
	{
		return GetTypeHash(Stat.APIName);
	}
};

DECLARE_DELEGATE(FOnAuthTicketForWebApiReady);

USTRUCT()
struct FOnAuthTicketForWebApiResponseCallbackHandler
{
	GENERATED_BODY()
	
	FOnAuthTicketForWebApiReady OnAuthTicketForWebApiReady;
	HAuthTicket Handle;
	EResult Result;
	FString Ticket;

	FOnAuthTicketForWebApiResponseCallbackHandler(): Handle(0), Result(k_EResultFail)
	{
	}
};

/** Class responsible for linking to Steam */
UCLASS()
class BEATSHOT_API USteamManager : public UObject
{
	GENERATED_BODY()

public:
	USteamManager();

	void InitializeSteamManager();

	void AssignGameInstance(UBSGameInstance* InDefaultGameInstance);

	/** Updates locally stored StatData with new IntValue/FloatValue for matching StatAPIName, calls StoreStats() */
	void UpdateStat(const char* StatAPIName, ESteamStatType StatType, int IntValue = 0, float FloatValue = 0.f);

	/** Updates locally stored NumGamesPlayed StatData with new IntValue/FloatValue for matching GameMode, calls StoreStats() */
	void UpdateStat_NumGamesPlayed(const EBaseGameMode GameMode, int IntValue);

	UPROPERTY()
	UBSGameInstance* DefaultGameInstance;

	/** Calls GetAuthTicketForWebApi using Steam Api, callback is OnTicketForWebApiResponse */
	bool CreateAuthTicketForWebApi(TSharedPtr<FOnAuthTicketForWebApiResponseCallbackHandler> CallbackHandler);

private:
	/** Delegate registered with Steam to trigger when a user activates the Steam Overlay */
	STEAM_CALLBACK_MANUAL(USteamManager, OnSteamOverlayActive, GameOverlayActivated_t, OnSteamOverlayActiveDelegate);
	/** Delegate registered with Steam to trigger when a response is received from GetAuthTicketForWebApi */
	STEAM_CALLBACK_MANUAL(USteamManager, OnAuthTicketForWebApiResponse, GetTicketForWebApiResponse_t, OnAuthTicketForWebApiResponseDelegate);
	/** Delegate registered with Steam to trigger anytime RequestStats() is called */
	STEAM_CALLBACK_MANUAL(USteamManager, OnUserStatsReceived, UserStatsReceived_t, OnUserStatsReceivedDelegate);
	/** Delegate registered with Steam to trigger anytime you attempt to store stats on Steam  */
	STEAM_CALLBACK_MANUAL(USteamManager, OnUserStatsStored, UserStatsStored_t, OnUserStatsStoredDelegate);
	/** Delegate registered with Steam to trigger anytime you attempt to store achievements on Steam  */
	STEAM_CALLBACK_MANUAL(USteamManager, OnAchievementStored, UserAchievementStored_t, OnAchievementStoredDelegate);

	/** Wraps an asynchronous call to steam, ISteamUserStats::RequestCurrentStats, requesting the stats
	 *  and achievements of the current user. Needs to be called before setting any stats or achievements. */
	static bool RequestStats();

	/**  Wraps an asynchronous call to steam, ISteamUserStats::StoreStats, that stores the stats of the current
	 *   user on the server. Needs to be called to update the stats of the user. */
	bool StoreStats();

	/** Sets a given achievement to achieved and sends the results to Steam. You can set a given achievement multiple times so you don't need to
	 *  worry about only setting achievements that aren't already set. This is an asynchronous call which will trigger two callbacks:
	 *  OnUserStatsStored() and OnAchievementStored() */
	bool SetAchievement(const char* ID) const;

	/** Returns a pointer to the NumGamesPlayed FSteamStat element corresponding to GameMode */
	FSteamStat* GetStat_NumGamesPlayed(const EBaseGameMode GameMode);

	/** The Steam AppID for this game */
	int64 AppId;

	/** Locally stored and updated Steam Stats struct array */
	TSet<FSteamStat> StatsData;

	/** Locally stored and updated Steam Achievement struct array */
	TSet<FSteamAchievement> AchievementData;

	/** If Steam Stats were successfully initialized */
	bool bInitializedStats;
	
	TQueue<TSharedPtr<FOnAuthTicketForWebApiResponseCallbackHandler>> ActiveCallbacks;
};
