// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "HttpRequestInterface.generated.h"

/** Broadcast if refresh token is invalid */
DECLARE_DELEGATE_OneParam(FOnAccessTokenResponse, const FString& AccessToken);

/** Broadcast when a login response is received from BeatShot website */
DECLARE_DELEGATE_ThreeParams(FOnLoginResponse, const FPlayerSettings& PlayerSettings, const FString& ResponseMsg, const int32 ResponseCode);

/** Broadcast when a response is received from posting player scores to database */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostScoresResponse, const ELoginState& LoginState);

UINTERFACE()
class UHttpRequestInterface : public UInterface
{
	GENERATED_BODY()
};

class GLOBAL_API IHttpRequestInterface
{
	GENERATED_BODY()

public:
	/** Sends an http post login request to BeatShot website given a LoginPayload. Executes supplied OnLoginResponse
	 *  with a login cookie */
	void LoginUser(const FLoginPayload& LoginPayload, FOnLoginResponse& OnLoginResponse) const;

	/** Requests a short lived access token given a valid login cookie. Executes supplied OnAccessTokenResponse
	 *  with an access token */
	void RequestAccessToken(const FString& LoginCookie, FOnAccessTokenResponse& OnAccessTokenResponse) const;

	/** Checks to see if the user has a refresh token and if it has expired or not */
	static bool IsRefreshTokenValid(const FPlayerSettings& PlayerSettings);

	/* Converts ScoresToPost to a JSON string and sends an http post request to BeatShot website given a valid
	 * access token. Executes supplied OnPostResponse with the login state */
	void PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& Username, const FString& AccessToken, FOnPostScoresResponse& OnPostResponse) const;

private:
	FString LoginEndpoint = "https://beatshot.gg/api/login";
	const FString RefreshEndpoint = "https://beatshot.gg/api/refresh";
	const FString SaveScoresEndpoint = "https://beatshot.gg/api/profile/";
};
