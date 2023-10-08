// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "SaveGamePlayerScore.h"
#include "HttpRequestInterface.generated.h"

/** Describes if player scores were posted or not */
UENUM(BlueprintType)
enum class EPostScoresResponse : uint8
{
	ZeroScore UMETA(DisplayName="ZeroScore"),
	UnsavedGameMode UMETA(DisplayName="UnsavedGameMode"),
	NoAccount UMETA(DisplayName="NoAccount"),
	HttpError UMETA(DisplayName="HttpError"),
	HttpSuccess UMETA(DisplayName="HttpSuccess"),
};

ENUM_RANGE_BY_FIRST_AND_LAST(EPostScoresResponse, EPostScoresResponse::ZeroScore, EPostScoresResponse::HttpSuccess);

/** Simple login payload */
USTRUCT(BlueprintType)
struct FLoginPayload
{
	GENERATED_BODY()

	UPROPERTY()
	FString Username;

	UPROPERTY()
	FString Email;

	UPROPERTY()
	FString Password;

	FLoginPayload()
	{
		Username = "";
		Email = "";
		Password = "";
	}

	FLoginPayload(const FString& InUsername, const FString& InEmail, const FString& InPassword)
	{
		Username = InUsername;
		Email = InEmail;
		Password = InPassword;
	}
};

/** Login Response object */
USTRUCT(BlueprintType)
struct FLoginResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString UserID;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	FString AccessToken;

	UPROPERTY()
	FString RefreshToken;

	FLoginResponse() = default;
};

/** Response object returned as JSON from authentication using SteamAuthTicket */
USTRUCT(BlueprintType)
struct FSteamAuthTicketResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString Result;

	UPROPERTY()
	FString SteamID;

	UPROPERTY()
	FString OwnerSteamID;

	UPROPERTY()
	bool VacBanned;

	UPROPERTY()
	bool PublisherBanned;

	UPROPERTY()
	FString DisplayName;

	UPROPERTY()
	FString RefreshCookie;

	UPROPERTY()
	FString ErrorCode;

	UPROPERTY()
	FString ErrorDesc;

	FSteamAuthTicketResponse(): VacBanned(false), PublisherBanned(false)
	{
	}
};

/** Broadcast if refresh token is invalid */
DECLARE_DELEGATE_OneParam(FOnAccessTokenResponse, const FString& AccessToken);

/** Broadcast when a login response is received from BeatShot website */
DECLARE_DELEGATE_ThreeParams(FOnLoginResponse, const FLoginResponse& LoginResponse, const FString& ResponseMsg,
	const int32 ResponseCode);

/** Broadcast when a response is received from posting player scores to database */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostScoresResponse, const EPostScoresResponse& Response);

/** Broadcast when a response is received from deleting player scores to database */
DECLARE_DELEGATE_TwoParams(FOnDeleteScoresResponse, const int32 NumRemoved, const int32 ResponseCode);

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

/** Used to pass the json body containing the CustomGameModeName to delete */
USTRUCT(BlueprintType)
struct FJsonDeleteScores
{
	GENERATED_BODY()

	UPROPERTY()
	FString CustomGameModeName;

	FJsonDeleteScores()
	{
		CustomGameModeName = "";
	}

	FJsonDeleteScores(const FString& InCustomGameModeName)
	{
		CustomGameModeName = InCustomGameModeName;
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
	static void PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& Username,
		const FString& AccessToken, FOnPostScoresResponse& OnPostResponse);

	/** Makes a POST request to BeatShot website which emails the feedback. Executes supplied OnPostFeedbackResponse */
	static void PostFeedback(const FJsonFeedback& InFeedback, FOnPostFeedbackResponse& OnPostFeedbackResponse);

	/** Makes a DELETE request to BeatShot website which deletes all scores matching the CustomGameModeName and userID */
	static void DeleteScores(const FString CustomGameModeName, const FString& Username, const FString& AccessToken,
		FOnDeleteScoresResponse& OnDeleteScoresResponse);

	// Can't be static since Game Instance calls on Async thread

	/** Makes a GET request to BeatShot website that uses the AuthenticateUserTicket request from the
	 *  ISteamUserAuthInterface. Executes supplied OnTicketWebApiResponse */
	void AuthenticateSteamUser(const FString& AuthTicket, FOnTicketWebApiResponse& OnTicketWebApiResponse) const;
};