// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include <steam/isteamfriends.h>
#include <steam/isteamuser.h>
#include <steam/isteamuserstats.h>
#include "GlobalEnums.h"
#include "SteamManager.generated.h"

class UBSGameInstance;

DECLARE_DELEGATE_OneParam(FOnAuthTicketForWebApiReady, const bool bSuccess);

#define _ACH_ID(id, name ) { id, #id, name, "", 0, 0 }
#define _STAT_ID(id, type, name) { id, type, name, 0, 0, 0, 0 }

// Defining our achievements
UENUM()
enum ESteamAchievement : uint8
{
	ACH_PlayAnyGM = 0,
	ACH_Participant_GM1 = 1,
	ACH_Participant_GM2 = 2,
	ACH_Participant_GM3 = 3,
	ACH_Participant_GM4 = 4,
	ACH_Participant_GM5 = 5,
	ACH_Participant_GM6 = 6,
	ACH_Enthusiast_GM1 = 7,
	ACH_Enthusiast_GM2 = 8,
	ACH_Enthusiast_GM3 = 9,
	ACH_Enthusiast_GM4 = 10,
	ACH_Enthusiast_GM5 = 11,
	ACH_Enthusiast_GM6 = 12,
	ACH_Enjoyer_GM1 = 13,
	ACH_Enjoyer_GM2 = 14,
	ACH_Enjoyer_GM3 = 15,
	ACH_Enjoyer_GM4 = 16,
	ACH_Enjoyer_GM5 = 17,
	ACH_Enjoyer_GM6 = 18,
	ACH_Participant_Custom = 19,
	ACH_Enthusiast_Custom = 20,
	ACH_Enjoyer_Custom = 21
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
};

UENUM()
enum ESteamStatType
{
	Stat_Int = 0,
	Stat_Float = 1,
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
};

/** Class responsible for linking to Steam */
UCLASS()
class BEATSHOT_API USteamManager : public UObject
{
	GENERATED_BODY()

public:
	USteamManager();

	void InitializeSteamManager();

	void ShutdownSteamManager();

	void AssignGameInstance(UBSGameInstance* InDefaultGameInstance);

	/** Updates locally stored StatData with new IntValue/FloatValue for matching StatAPIName, calls StoreStats() */
	void UpdateStat(const char* StatAPIName, ESteamStatType StatType, int IntValue = 0, float FloatValue = 0.f);

	/** Updates locally stored NumGamesPlayed StatData with new IntValue/FloatValue for matching GameMode, calls StoreStats() */
	void UpdateStat_NumGamesPlayed(const EBaseGameMode GameMode, int IntValue);
	
	UPROPERTY()
	UBSGameInstance* DefaultGameInstance;

	/** Calls GetAuthTicketForWebApi using Steam Api, callback is OnTicketForWebApiResponse */
	static void CreateAuthTicketForWebApi();
	
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
	STEAM_CALLBACK_MANUAL(USteamManager, OnUserStatsReceived, UserStatsReceived_t, OnUserStatsReceivedCallback);
	STEAM_CALLBACK_MANUAL(USteamManager, OnUserStatsStored, UserStatsStored_t, OnUserStatsStoredCallback);
	STEAM_CALLBACK_MANUAL(USteamManager, OnAchievementStored, UserAchievementStored_t, OnAchievementStoredCallback);

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
	bool SetAchievement(const ESteamAchievement InSteamAchievement) const;

	/** Returns the SteamAPI AchievementID from an ESteamAchievement enum */
	const char* GetAchievementIDFromESteamAchievement(const ESteamAchievement InSteamAchievement) const;

	/** Returns a pointer to the NumGamesPlayed FSteamStat element corresponding to GameMode */
	FSteamStat* GetStat_NumGamesPlayed(const EBaseGameMode GameMode);

	/** The current WebApiTicket retrieved from OnAuthTicketForWebApiReady */
	FString WebApiTicket;

	/** The Steam AppID for this game */
	int64 AppId;

	/** Locally stored and updated Steam Stats struct array */
	TArray<FSteamStat> StatsData;

	/** Locally stored and updated Steam Achievement struct array */
	TArray<FSteamAchievement> AchievementData;

	/** If Steam Stats were successfully initialized */
	bool bInitializedStats;
};
