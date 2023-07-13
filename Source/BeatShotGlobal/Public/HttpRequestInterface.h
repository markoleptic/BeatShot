// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "GlobalStructs.h"
#include "HttpRequestInterface.generated.h"

/** Broadcast if refresh token is invalid */
DECLARE_DELEGATE_OneParam(FOnAccessTokenResponse, const FString& AccessToken);

/** Broadcast when a login response is received from BeatShot website */
DECLARE_DELEGATE_ThreeParams(FOnLoginResponse, const FLoginResponse& LoginResponse, const FString& ResponseMsg, const int32 ResponseCode);

/** Broadcast when a response is received from posting player scores to database */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostScoresResponse, const EPostScoresResponse& Response);

/** Broadcast when a response is received from posting player feedback to database */
DECLARE_DELEGATE_OneParam(FOnPostFeedbackResponse, const bool bSuccess);

/** Broadcast when a response is received from a GetAuthTicketForWebApi request */
DECLARE_DELEGATE_TwoParams(FOnTicketWebApiResponse, const FSteamAuthTicketResponse& Response, const bool bSuccess);

/** Used to convert PlayerScoreArray to database scores */
USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerScore> Scores;
};

/** Used to create a feedback Json object */
USTRUCT(BlueprintType)
struct FJsonFeedback
{
	GENERATED_BODY()

	UPROPERTY()
	FString Title;

	UPROPERTY()
	FString Content;

	FJsonFeedback()
	{
		Title = FString();
		Content = FString();
	}

	FJsonFeedback(const FString& InTitle, const FString& InContent)
	{
		Title = InTitle;
		Content = InContent;
	}
	
};

/** Interface to allow all other classes in this game to use HTTP request functions */
UINTERFACE()
class UHttpRequestInterface : public UInterface
{
	GENERATED_BODY()
};

/** Interface to allow all other classes in this game to use HTTP request functions */
class BEATSHOTGLOBAL_API IHttpRequestInterface
{
	GENERATED_BODY()

public:
	/** Checks to see if the user has a refresh token and if it has expired or not */
	static bool IsRefreshTokenValid(const FString& RefreshToken);

	/** Requests a short lived access token given a valid login cookie. Executes supplied OnAccessTokenResponse
	 *  with an access token */
	static void RequestAccessToken(const FString& RefreshToken, FOnAccessTokenResponse& OnAccessTokenResponse);
	
	/** Sends an http post login request to BeatShot website given a LoginPayload. Executes supplied OnLoginResponse
	 *  with a login cookie */
	static void LoginUser(const FLoginPayload& LoginPayload, FOnLoginResponse& OnLoginResponse);
	
	/* Converts ScoresToPost to a JSON string and sends an http post request to BeatShot website given a valid
	 * access token. Executes supplied OnPostResponse */
	static void PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& Username, const FString& AccessToken, FOnPostScoresResponse& OnPostResponse);

	/** Makes a POST request to BeatShot website which emails the feedback. Executes supplied OnPostFeedbackResponse */
	static void PostFeedback(const FJsonFeedback& InFeedback, FOnPostFeedbackResponse& OnPostFeedbackResponse);

	/** Makes a GET request to BeatShot website that uses the AuthenticateUserTicket request from the ISteamUserAuthInterface. Executes supplied OnTicketWebApiResponse */
	static void AuthenticateSteamUser(const FString& AuthTicket, FOnTicketWebApiResponse& OnTicketWebApiResponse);

};
