// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable StringLiteralTypo
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
	const FString CookieExpireString = RightChopped.Left(RightChopped.Find(";", ESearchCase::IgnoreCase,
		ESearchDir::FromStart, 0));
	FDateTime::ParseHttpDate(CookieExpireString, CookieExpireDate);

	if (FDateTime::UtcNow() + FTimespan::FromDays(1) < CookieExpireDate)
	{
		return true;
	}
	return false;
}

void IHttpRequestInterface::RequestAccessToken(const FString& RefreshToken,
	FOnAccessTokenResponse& OnAccessTokenResponse)
{
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Endpoint_Refresh);
	HttpRequest->SetVerb("GET");
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetHeader("Cookie", RefreshToken);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnAccessTokenResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully)
			{
				if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(FString());
				UE_LOG(LogTemp, Display, TEXT("Access Token Request failed to successfully connect."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode != 200)
			{
				if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(FString());
				UE_LOG(LogTemp, Display, TEXT("Request Access Token failed: %s"), *ResponseString);
				return;
			}
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
			FJsonSerializer::Deserialize(JsonReader, JsonObject);

			const FString AccessToken = JsonObject->GetStringField("accessToken");
			
			if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(AccessToken);
			UE_LOG(LogTemp, Display, TEXT("Successful Access Token Response"));
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::LoginUser(const FLoginPayload& LoginPayload, FOnLoginResponse& OnLoginResponse)
{
	FString ContentString;
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, JsonObject);
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(JsonObject, JsonWriter);
	
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Endpoint_Login);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetContentAsString(ContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnLoginResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			FLoginResponse LoginResponse = FLoginResponse();
			if (!bConnectedSuccessfully)
			{
				if (OnLoginResponse.IsBound()) OnLoginResponse.Execute(LoginResponse, "Failed to Connect", 502);
				UE_LOG(LogTemp, Display, TEXT("Login Request failed to successfully connect."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode != 200)
			{
				if (OnLoginResponse.IsBound()) OnLoginResponse.Execute(LoginResponse, ResponseString, ResponseCode);
				UE_LOG(LogTemp, Display, TEXT("Login Request Failed: %s"), *ResponseString);
				return;
			}
			
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
			FJsonSerializer::Deserialize(JsonReader, JsonObject);
			
			LoginResponse.UserID = JsonObject->GetStringField("userID");
			LoginResponse.DisplayName = JsonObject->GetStringField("displayName");
			LoginResponse.AccessToken = JsonObject->GetStringField("accessToken");
			LoginResponse.RefreshToken = Response->GetHeader("set-cookie");

			if (OnLoginResponse.IsBound()) OnLoginResponse.Execute(LoginResponse, ResponseString, ResponseCode);
			UE_LOG(LogTemp, Display, TEXT("Logged in %s %s"), *LoginResponse.UserID, *LoginResponse.DisplayName);
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& Username,
	const FString& AccessToken, FOnPostScoresResponse& OnPostResponse)
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
	
	FString ContentString;
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonScore::StaticStruct(), &JsonScores, JsonObject);
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(JsonObject, JsonWriter);
	UE_LOG(LogTemp, Display, TEXT("FJsonScore: %s"), *ContentString);
	
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Segment_ApiProfile + Username + "/savescores");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	HttpRequest->SetContentAsString(ContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnPostResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully)
			{
				OnPostResponse.Broadcast(EPostScoresResponse::HttpError);
				UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed to successfully connect."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode != 200)
			{
				OnPostResponse.Broadcast(EPostScoresResponse::HttpError);
				UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed: %s"), *ResponseString);
				return;
			}
			OnPostResponse.Broadcast(EPostScoresResponse::HttpSuccess);
			UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::PostFeedback(const FJsonFeedback& InFeedback,
	FOnPostFeedbackResponse& OnPostFeedbackResponse)
{
	FString ContentString;
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonFeedback::StaticStruct(), &InFeedback, JsonObject);
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(JsonObject, JsonWriter);
	
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Endpoint_SendFeedback);
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetVerb("POST");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetContentAsString(ContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnPostFeedbackResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully)
			{
				if (OnPostFeedbackResponse.IsBound()) OnPostFeedbackResponse.Execute(false);
				UE_LOG(LogTemp, Display, TEXT("Failed to connect to send feedback."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode != 200)
			{
				if (OnPostFeedbackResponse.IsBound()) OnPostFeedbackResponse.Execute(false);
				UE_LOG(LogTemp, Display, TEXT("Failed to send feedback: %s"), *ResponseString);
				return;
			}

			if (OnPostFeedbackResponse.IsBound()) OnPostFeedbackResponse.Execute(true);
			UE_LOG(LogTemp, Display, TEXT("Successfully sent feedback."));
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::DeleteScores(const FString CustomGameModeName, const FString& Username,
	const FString& AccessToken, FOnDeleteScoresResponse& OnDeleteScoresResponse)
{
	FString ContentString;
	const FJsonDeleteScores JsonDelete = FJsonDeleteScores(CustomGameModeName);
	const TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FJsonDeleteScores::StaticStruct(), &JsonDelete, JsonObject);
	const TSharedRef<TJsonWriter<>> JsonWriter = TJsonWriterFactory<>::Create(&ContentString);
	FJsonSerializer::Serialize(JsonObject, JsonWriter);

	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Segment_ApiProfile + Username + "/deletescores");
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetVerb("DELETE");
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	HttpRequest->SetContentAsString(ContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnDeleteScoresResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully)
			{
				if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(0, 502);
				UE_LOG(LogTemp, Display, TEXT("Failed to connect to server while deleting scores"));
				return;
			}
			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();
			
			if (ResponseCode != 200)
			{
				if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(0, ResponseCode);
				UE_LOG(LogTemp, Display, TEXT("Failed to delete scores: Error Code: %d"), ResponseCode);
				return;
			}

			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
			FJsonSerializer::Deserialize(JsonReader, JsonObject);
			
			const int32 NumRemoved = static_cast<int32>(JsonObject->GetNumberField("Number Removed"));
			
			if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(NumRemoved, ResponseCode);
			UE_LOG(LogTemp, Display, TEXT("Successfully deleted scores from database."));
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::AuthenticateSteamUser(const FString& AuthTicket,
	FOnTicketWebApiResponse& OnTicketWebApiResponse) const
{
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Endpoint_AuthenticateUserTicketNoRedirect + AuthTicket);
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, &OnTicketWebApiResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response,
		bool bConnectedSuccessfully)
		{
			FSteamAuthTicketResponse TicketResponse = FSteamAuthTicketResponse();
			
			if (!bConnectedSuccessfully)
			{
				TicketResponse.ErrorCode = "502";
				TicketResponse.ErrorDesc = "Failed to connect";
				
				if (OnTicketWebApiResponse.IsBound()) OnTicketWebApiResponse.Execute(TicketResponse, false);
				UE_LOG(LogTemp, Display, TEXT("Failed to connect to server while deleting scores"));
				return;
			}
			
			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();
			TSharedPtr<FJsonObject> JsonObject;
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
			FJsonSerializer::Deserialize(JsonReader, JsonObject);
			
			if (ResponseCode != 200)
			{
				TicketResponse.ErrorCode = JsonObject->HasField("errorcode")
					? JsonObject->GetStringField("errorcode")
					: "Unknown Error Code";
				TicketResponse.ErrorDesc = JsonObject->HasField("errordesc")
					? JsonObject->GetStringField("errordesc")
					: "Unknown Error Description";
				
				if (OnTicketWebApiResponse.IsBound()) OnTicketWebApiResponse.Execute(TicketResponse, false);
				UE_LOG(LogTemp, Display, TEXT("Failed to authenticate: %s"), *TicketResponse.ErrorDesc);
				return;
			}

			TicketResponse.Result = JsonObject->GetStringField("result");
			TicketResponse.SteamID = JsonObject->GetStringField("steamid");
			TicketResponse.OwnerSteamID = JsonObject->GetStringField("ownersteamid");
			TicketResponse.VacBanned = JsonObject->GetBoolField("vacbanned");
			TicketResponse.PublisherBanned = JsonObject->GetBoolField("publisherbanned");
			TicketResponse.DisplayName = JsonObject->GetStringField("displayname");
			TicketResponse.RefreshCookie = Response->GetHeader("set-cookie");

			if (OnTicketWebApiResponse.IsBound()) OnTicketWebApiResponse.Execute(TicketResponse, true);
			UE_LOG(LogTemp, Display, TEXT("Steam auth success for %s"), *TicketResponse.SteamID);
		});
	HttpRequest->ProcessRequest();
}
