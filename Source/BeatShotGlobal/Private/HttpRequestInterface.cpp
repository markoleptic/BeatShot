// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "HttpRequestInterface.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "SaveGamePlayerScore.h"
#include "Interfaces/IHttpResponse.h"

bool IHttpRequestInterface::IsRefreshTokenValid(const FString& RefreshToken)
{
	if (RefreshToken.IsEmpty())
	{
		return false;
	}
	FDateTime CookieExpireDate;
	const int32 ExpiresStartPos = RefreshToken.Find("Expires=", ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
	const FString RightChopped = RefreshToken.RightChop(ExpiresStartPos + 8);
	const FString CookieExpireString = RightChopped.Left(RightChopped.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0));
	FDateTime::ParseHttpDate(CookieExpireString, CookieExpireDate);

	if (FDateTime::UtcNow() + FTimespan::FromDays(1) < CookieExpireDate)
	{
		return true;
	}
	return false;
}

void IHttpRequestInterface::RequestAccessToken(const FString& RefreshToken, FOnAccessTokenResponse& OnAccessTokenResponse)
{
	const FHttpRequestRef AccessTokenRequest = FHttpModule::Get().CreateRequest();
	AccessTokenRequest->SetURL(Endpoint_Refresh);
	AccessTokenRequest->SetVerb("GET");
	AccessTokenRequest->SetHeader("Cookie", RefreshToken);
	AccessTokenRequest->OnProcessRequestComplete().BindLambda([&OnAccessTokenResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		if (Response->GetResponseCode() != 200)
		{
			if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(FString());
			return;
		}
		/* Convert response to Json object to access string fields */
		const FString ResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> ResponseObj;
		const TSharedRef<TJsonReader<>> ResponseReader = TJsonReaderFactory<>::Create(ResponseString);
		FJsonSerializer::Deserialize(ResponseReader, ResponseObj);
		if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(ResponseObj->GetStringField("accessToken"));
		UE_LOG(LogTemp, Display, TEXT("Successful Access Token Response"));
	});
	AccessTokenRequest->ProcessRequest();
}

void IHttpRequestInterface::LoginUser(const FLoginPayload& LoginPayload, FOnLoginResponse& OnLoginResponse)
{
	const TSharedRef<FJsonObject> LoginObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, LoginObject, 0, 0);
	FString LoginString;
	const TSharedRef<TJsonWriter<>> LoginWriter = TJsonWriterFactory<>::Create(&LoginString);
	FJsonSerializer::Serialize(LoginObject, LoginWriter);
	
	const FHttpRequestRef LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->SetURL(Endpoint_Login);
	LoginRequest->SetVerb("POST");
	LoginRequest->SetHeader("Content-Type", "application/json");
	LoginRequest->SetContentAsString(LoginString);
	LoginRequest->OnProcessRequestComplete().BindLambda([&OnLoginResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		FLoginResponse LoginResponse = FLoginResponse();
		if (Response->GetResponseCode() != 200)
		{
			UE_LOG(LogTemp, Display, TEXT("Login Request Failed."));
			if (OnLoginResponse.IsBound()) OnLoginResponse.Execute(LoginResponse, Response->GetContentAsString(), Response->GetResponseCode());
			return;
		}
		const FString LoginResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> LoginResponseObj;
		const TSharedRef<TJsonReader<>> LoginResponseReader = TJsonReaderFactory<>::Create(LoginResponseString);
		FJsonSerializer::Deserialize(LoginResponseReader, LoginResponseObj);
		
		LoginResponse.UserID = LoginResponseObj->GetStringField("userID");
		LoginResponse.DisplayName = LoginResponseObj->GetStringField("displayName");
		LoginResponse.AccessToken = LoginResponseObj->GetStringField("accessToken");
		LoginResponse.RefreshToken = Response->GetHeader("set-cookie");

		if (OnLoginResponse.IsBound()) OnLoginResponse.Execute(LoginResponse, Response->GetContentAsString(), Response->GetResponseCode());
		UE_LOG(LogTemp, Display, TEXT("Login successful for %s %s"), *LoginResponse.UserID, *LoginResponse.DisplayName);
	});
	LoginRequest->ProcessRequest();
}

void IHttpRequestInterface::PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& Username, const FString& AccessToken, FOnPostScoresResponse& OnPostResponse) 
{
	FJsonScore JsonScores;
	// Add all elements that haven't been saved to database to the JsonScores Scores array
	for (const FPlayerScore& PlayerScoreObject : ScoresToPost)
	{
		if (!PlayerScoreObject.bSavedToDatabase)
		{
			JsonScores.Scores.Add(PlayerScoreObject);
		}
	}

	// convert JsonScores struct to JSON
	const TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonScore::StaticStruct(), &JsonScores, OutJsonObject);
	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);

	UE_LOG(LogTemp, Display, TEXT("FJsonScore: %s"), *OutputString);
	// ReSharper disable once StringLiteralTypo
	const FString Endpoint = Segment_ApiProfile + Username + "/savescores";
	const FHttpRequestRef SendScoreRequest = FHttpModule::Get().CreateRequest();
	SendScoreRequest->OnProcessRequestComplete().BindLambda([&OnPostResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		if (Response->GetResponseCode() != 200)
		{
			OnPostResponse.Broadcast(EPostScoresResponse::HttpError);
			UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed: %s"), *Response->GetContentAsString());
			return;
		}
		UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
		OnPostResponse.Broadcast(EPostScoresResponse::HttpSuccess);
	});
	SendScoreRequest->SetURL(Endpoint);
	SendScoreRequest->SetVerb("POST");
	SendScoreRequest->SetHeader("Content-Type", "application/json");
	SendScoreRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	SendScoreRequest->SetContentAsString(OutputString);
	SendScoreRequest->ProcessRequest();
}

void IHttpRequestInterface::PostFeedback(const FJsonFeedback& InFeedback, FOnPostFeedbackResponse& OnPostFeedbackResponse)
{
	const TSharedRef<FJsonObject> FeedbackObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonFeedback::StaticStruct(), &InFeedback, FeedbackObject, 0, 0);
	FString ContentString;
	const TSharedRef<TJsonWriter<>> LoginWriter = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(FeedbackObject, LoginWriter);
	
	const FHttpRequestRef FeedbackRequest = FHttpModule::Get().CreateRequest();
	FeedbackRequest->OnProcessRequestComplete().BindLambda([&OnPostFeedbackResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		if (Response->GetResponseCode() != 200)
		{
			if (OnPostFeedbackResponse.IsBound()) OnPostFeedbackResponse.Execute(false);
			UE_LOG(LogTemp, Display, TEXT("Failed to send feedback: %s"), *Response->GetContentAsString());
			return;
		}
		if (OnPostFeedbackResponse.IsBound()) OnPostFeedbackResponse.Execute(true);
		UE_LOG(LogTemp, Display, TEXT("Successfully sent feedback."));
	});
	
	FeedbackRequest->SetURL(Endpoint_SendFeedback);
	FeedbackRequest->SetVerb("POST");
	FeedbackRequest->SetHeader("Content-Type", "application/json");
	FeedbackRequest->SetContentAsString(ContentString);
	FeedbackRequest->ProcessRequest();
}

void IHttpRequestInterface::AuthenticateSteamUser(const FString& AuthTicket, FOnTicketWebApiResponse& OnTicketWebApiResponse)
{
	const FHttpRequestRef AuthRequest = FHttpModule::Get().CreateRequest();
	AuthRequest->OnProcessRequestComplete().BindLambda([&OnTicketWebApiResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
	{
		// create Json object to access string fields
		const FString JsonResponseString = Response->GetContentAsString();
		TSharedPtr<FJsonObject> ResponseJsonObject;
		const TSharedRef<TJsonReader<>> JsonResponseReader = TJsonReaderFactory<>::Create(JsonResponseString);
		FJsonSerializer::Deserialize(JsonResponseReader, ResponseJsonObject);

		FSteamAuthTicketResponse SteamAuthTicketResponse = FSteamAuthTicketResponse();
		
		if (Response->GetResponseCode() != 200)
		{
			SteamAuthTicketResponse.ErrorCode = ResponseJsonObject->HasField("errorcode") ? ResponseJsonObject->GetStringField("errorcode") : "Unknown Error Code";
			SteamAuthTicketResponse.ErrorDesc = ResponseJsonObject->HasField("errordesc") ? ResponseJsonObject->GetStringField("errordesc") : "Unknown Error Description";

			UE_LOG(LogTemp, Display, TEXT("Failed to authenticate Steam user: %s"), *SteamAuthTicketResponse.ErrorDesc);
			
			if (OnTicketWebApiResponse.IsBound()) OnTicketWebApiResponse.Execute(SteamAuthTicketResponse, false);
			return;
		}

		SteamAuthTicketResponse.Result = ResponseJsonObject->GetStringField("result");
		SteamAuthTicketResponse.SteamID = ResponseJsonObject->GetStringField("steamid");
		SteamAuthTicketResponse.OwnerSteamID = ResponseJsonObject->GetStringField("ownersteamid");
		SteamAuthTicketResponse.VacBanned = ResponseJsonObject->GetBoolField("vacbanned");
		SteamAuthTicketResponse.PublisherBanned = ResponseJsonObject->GetBoolField("publisherbanned");
		SteamAuthTicketResponse.DisplayName = ResponseJsonObject->GetStringField("displayname");
		SteamAuthTicketResponse.RefreshCookie = Response->GetHeader("set-cookie");

		UE_LOG(LogTemp, Display, TEXT("Successful Steam authentication for Steam user: %s"), *SteamAuthTicketResponse.SteamID);
		if (OnTicketWebApiResponse.IsBound()) OnTicketWebApiResponse.Execute(SteamAuthTicketResponse, true);
	});
	AuthRequest->SetURL(Endpoint_AuthenticateUserTicketNoRedirect + AuthTicket);
	AuthRequest->SetVerb("GET");
	AuthRequest->ProcessRequest();
}
