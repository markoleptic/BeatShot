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
			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(FString());
				UE_LOG(LogTemp, Display, TEXT("Access Token Request failed to successfully connect."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode >= 200 && ResponseCode <= 300)
			{
				TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
				const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(ResponseString);
				FJsonSerializer::Deserialize(JsonReader, JsonObject);
				const FString AccessToken = JsonObject->GetStringField("accessToken");
				if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(AccessToken);
			}
			else
			{
				if (OnAccessTokenResponse.IsBound()) OnAccessTokenResponse.Execute(FString());
				UE_LOG(LogTemp, Display, TEXT("Request Access Token failed Http Status: %d"), ResponseCode);
			}
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::LoginUser(const FLoginPayload& LoginPayload, TSharedPtr<FLoginResponse> LoginResponse)
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
		[LoginResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				LoginResponse->ResponseMsg = "Failed to Connect";
				LoginResponse->HttpStatus = 502;
				if (LoginResponse->OnLoginResponse.IsBound())
					LoginResponse->OnLoginResponse.Execute();
				UE_LOG(LogTemp, Warning, TEXT("Login Request failed to successfully connect."));
				return;
			}
			
			LoginResponse->HttpStatus = Response->GetResponseCode();
			TSharedPtr<FJsonObject> ResponseJsonObject = MakeShareable(new FJsonObject);
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			FJsonSerializer::Deserialize(JsonReader, ResponseJsonObject);
			
			if (LoginResponse->HttpStatus >= 200 && LoginResponse->HttpStatus <= 300)
			{
				LoginResponse->UserID = ResponseJsonObject->GetStringField("userID");
				LoginResponse->DisplayName = ResponseJsonObject->GetStringField("displayName");
				LoginResponse->AccessToken = ResponseJsonObject->GetStringField("accessToken");
				LoginResponse->RefreshToken  = Response->GetHeader("set-cookie");
			}
			else
			{
				UE_LOG(LogTemp, Display, TEXT("Login Request failed Http status: %d"), LoginResponse->HttpStatus);
			}

			if (LoginResponse->OnLoginResponse.IsBound())
				LoginResponse->OnLoginResponse.Execute();
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::PostPlayerScores(const TArray<FPlayerScore>& ScoresToPost, const FString& UserID,
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
	HttpRequest->SetURL(Segment_ApiProfile + UserID + "/savescores");
	HttpRequest->SetVerb("POST");
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetHeader("Content-Type", "application/json");
	HttpRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	HttpRequest->SetContentAsString(ContentString);
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[&OnPostResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				OnPostResponse.Broadcast(EPostScoresResponse::HttpError);
				UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed to successfully connect."));
				return;
			}

			const FString ResponseString = Response->GetContentAsString();
			const int32 ResponseCode = Response->GetResponseCode();

			if (ResponseCode >= 200 && ResponseCode <= 300)
			{
				OnPostResponse.Broadcast(EPostScoresResponse::HttpSuccess);
				UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
			}
			else
			{
				OnPostResponse.Broadcast(EPostScoresResponse::HttpError);
				UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed: %s"), *ResponseString);
			}
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
			if (!bConnectedSuccessfully || !Response.IsValid())
			{
				if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(0, 502);
				UE_LOG(LogTemp, Warning, TEXT("Failed to connect to server while deleting scores."));
				return;
			}

			const int32 ResponseCode = Response->GetResponseCode();
			
			if (ResponseCode >= 200 && ResponseCode <= 300)
			{
				TSharedPtr<FJsonObject> ResponseJsonObject = MakeShareable(new FJsonObject);
				const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
				FJsonSerializer::Deserialize(JsonReader, ResponseJsonObject);
				
				const int32 NumRemoved = static_cast<int32>(ResponseJsonObject->GetNumberField("Number Removed"));
				
				if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(NumRemoved, ResponseCode);
				UE_LOG(LogTemp, Display, TEXT("Successfully deleted scores from database."));
			}
			else
			{
				if (OnDeleteScoresResponse.IsBound()) OnDeleteScoresResponse.Execute(0, ResponseCode);
				UE_LOG(LogTemp, Warning, TEXT("Failed to delete scores: Http Status: %d"), ResponseCode);
			}
		});
	HttpRequest->ProcessRequest();
}

void IHttpRequestInterface::AuthenticateSteamUser(const FString& AuthTicket,TSharedPtr<FSteamAuthTicketResponse> SteamAuthTicketResponse) const
{
	const FHttpRequestRef HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(Endpoint_AuthenticateUserTicketNoRedirect + AuthTicket);
	HttpRequest->SetTimeout(5.f);
	HttpRequest->SetVerb("GET");
	HttpRequest->OnProcessRequestComplete().BindLambda(
		[this, SteamAuthTicketResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			SteamAuthTicketResponse->bConnectedSuccessfully = bConnectedSuccessfully;
			if (!SteamAuthTicketResponse->bConnectedSuccessfully || !Response.IsValid())
			{
				SteamAuthTicketResponse->HttpStatus = 502;
				SteamAuthTicketResponse->ErrorCode = "502";
				SteamAuthTicketResponse->ErrorDesc = "Failed to connect";
				
				UE_LOG(LogTemp, Warning, TEXT("Failed to connect to server while trying to authenticate."));
				
				if (SteamAuthTicketResponse->OnSteamAuthTicketResponse.IsBound())
					SteamAuthTicketResponse->OnSteamAuthTicketResponse.Execute();
				
				return;
			}

			SteamAuthTicketResponse->HttpStatus = Response->GetResponseCode();
			
			TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
			const TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
			FJsonSerializer::Deserialize(JsonReader, JsonObject);
			
			if (SteamAuthTicketResponse->HttpStatus >= 200 && SteamAuthTicketResponse->HttpStatus <= 300)
			{
				SteamAuthTicketResponse->Result = JsonObject->GetStringField("result");
				SteamAuthTicketResponse->SteamID = JsonObject->GetStringField("steamid");
				SteamAuthTicketResponse->OwnerSteamID = JsonObject->GetStringField("ownersteamid");
				SteamAuthTicketResponse->VacBanned = JsonObject->GetBoolField("vacbanned");
				SteamAuthTicketResponse->PublisherBanned = JsonObject->GetBoolField("publisherbanned");
				SteamAuthTicketResponse->DisplayName = JsonObject->GetStringField("displayname");
				SteamAuthTicketResponse->RefreshCookie = Response->GetHeader("set-cookie");
			}
			else
			{
				SteamAuthTicketResponse->ErrorCode = JsonObject->HasField("errorcode")
					? JsonObject->GetStringField("errorcode")
					: "Unknown Error Code";
				SteamAuthTicketResponse->ErrorDesc = JsonObject->HasField("errordesc")
					? JsonObject->GetStringField("errordesc")
					: "Unknown Error Description";
			}

			if (SteamAuthTicketResponse->OnSteamAuthTicketResponse.IsBound())
				SteamAuthTicketResponse->OnSteamAuthTicketResponse.Execute();
		});
	HttpRequest->ProcessRequest();
}
