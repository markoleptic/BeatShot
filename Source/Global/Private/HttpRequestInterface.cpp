#include "HttpRequestInterface.h"
#include "HttpModule.h"
#include "JsonObjectConverter.h"
#include "SaveGamePlayerScore.h"
#include "SaveGamePlayerSettings.h"
#include "Interfaces/IHttpResponse.h"


bool IHttpRequestInterface::IsRefreshTokenValid(const FPlayerSettings PlayerSettings)
{
	if (PlayerSettings.HasLoggedInHttp == true)
	{
		if (PlayerSettings.LoginCookie.IsEmpty())
		{
			return false;
		}
		FDateTime CookieExpireDate;
		const int32 ExpiresStartPos = PlayerSettings.LoginCookie.Find("Expires=", ESearchCase::CaseSensitive,
		                                                              ESearchDir::FromStart, 0);
		const FString RightChopped = PlayerSettings.LoginCookie.RightChop(ExpiresStartPos + 8);
		const FString CookieExpireString = RightChopped.Left(
			RightChopped.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0));
		FDateTime::ParseHttpDate(CookieExpireString, CookieExpireDate);

		if ((FDateTime::UtcNow() + FTimespan::FromDays(1) < CookieExpireDate))
		{
			return true;
		}
	}
	return false;
}

void IHttpRequestInterface::LoginUser(const FLoginPayload& LoginPayload, FOnLoginResponse& OnLoginResponse) const
{
	const TSharedRef<FJsonObject> LoginObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, LoginObject, 0, 0);
	FString LoginString;
	const TSharedRef<TJsonWriter<>> LoginWriter = TJsonWriterFactory<>::Create(&LoginString);
	FJsonSerializer::Serialize(LoginObject, LoginWriter);
	const FHttpRequestRef LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->SetURL(LoginEndpoint);
	LoginRequest->SetVerb("POST");
	LoginRequest->SetHeader("Content-Type", "application/json");
	LoginRequest->SetContentAsString(LoginString);
	LoginRequest->OnProcessRequestComplete().BindLambda(
		[this, &OnLoginResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			FPlayerSettings PlayerSettings;
			if (Response->GetResponseCode() != 200)
			{
				UE_LOG(LogTemp, Display, TEXT("Login Request Failed."));
				OnLoginResponse.Execute(PlayerSettings, Response->GetContentAsString(), Response->GetResponseCode());
				return;
			}
			// create Json object to access string fields
			const FString LoginResponseString = Response->GetContentAsString();
			TSharedPtr<FJsonObject> LoginResponseObj;
			const TSharedRef<TJsonReader<>> LoginResponseReader = TJsonReaderFactory<>::Create(LoginResponseString);
			FJsonSerializer::Deserialize(LoginResponseReader, LoginResponseObj);
			PlayerSettings.HasLoggedInHttp = true;
			PlayerSettings.Username = LoginResponseObj->GetStringField("username");
			PlayerSettings.LoginCookie = Response->GetHeader("set-cookie");
			OnLoginResponse.Execute(PlayerSettings, Response->GetContentAsString(), Response->GetResponseCode());
			UE_LOG(LogTemp, Display, TEXT("Login successful for %s"), *PlayerSettings.Username);
		});
	LoginRequest->ProcessRequest();
}

void IHttpRequestInterface::RequestAccessToken(const FString LoginCookie, FOnAccessTokenResponse& OnAccessTokenResponse) const
{
	// not currently using RefreshToken parameter but may change in future to double check token
	const FHttpRequestRef AccessTokenRequest = FHttpModule::Get().CreateRequest();
	AccessTokenRequest->SetURL(RefreshEndpoint);
	AccessTokenRequest->SetVerb("GET");
	AccessTokenRequest->SetHeader("Cookie", LoginCookie);
	AccessTokenRequest->OnProcessRequestComplete().BindLambda(
		[this, &OnAccessTokenResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response,
		                               bool bConnectedSuccessfully)
		{
			if (Response->GetResponseCode() != 200)
			{
				if (!OnAccessTokenResponse.ExecuteIfBound(""))
				{
					UE_LOG(LogTemp, Display, TEXT("OnAccessTokenResponse not bound."));
				}
				return;
			}
			/* Convert response to Json object to access string fields */
			const FString ResponseString = Response->GetContentAsString();
			TSharedPtr<FJsonObject> ResponseObj;
			const TSharedRef<TJsonReader<>> ResponseReader = TJsonReaderFactory<>::Create(ResponseString);
			FJsonSerializer::Deserialize(ResponseReader, ResponseObj);
			if (!OnAccessTokenResponse.ExecuteIfBound(ResponseObj->GetStringField("accessToken")))
			{
				UE_LOG(LogTemp, Display, TEXT("OnAccessTokenResponse not bound."));
			}
			UE_LOG(LogTemp, Display, TEXT("Successful Access Token Response"));
		});
	AccessTokenRequest->ProcessRequest();
}

void IHttpRequestInterface::PostPlayerScores(const TArray<FPlayerScore> ScoresToPost, const FString Username,
                                             const FString AccessToken, FOnPostScoresResponse& OnPostResponse) const
{
	FJsonScore JsonScores;
	// Add all elements that haven't been saved to database to the JsonScores Scores array
	for (const FPlayerScore PlayerScoreObject : ScoresToPost)
	{
		if (!PlayerScoreObject.bSavedToDatabase)
		{
			JsonScores.Scores.Add(PlayerScoreObject);
		}
	}

	// convert JsonScores struct to JSON
	const TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(
		FJsonScore::StaticStruct(),
		&JsonScores,
		OutJsonObject);
	FString OutputString;
	const TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);

	UE_LOG(LogTemp, Display, TEXT("FJsonScore: %s"), *OutputString);
	// ReSharper disable once StringLiteralTypo
	const FString Endpoint = SaveScoresEndpoint + Username + "/savescores";
	const FHttpRequestRef SendScoreRequest = FHttpModule::Get().CreateRequest();
	SendScoreRequest->OnProcessRequestComplete().BindLambda(
		[this, &OnPostResponse](FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
		{
			if (Response->GetResponseCode() != 200)
			{
				OnPostResponse.Execute(ELoginState::InvalidHttp);
				UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed: %s"), *Response->GetContentAsString());
				return;
			}
			UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
			OnPostResponse.Execute(ELoginState::LoggedInHttp);
		});
	SendScoreRequest->SetURL(Endpoint);
	SendScoreRequest->SetVerb("POST");
	SendScoreRequest->SetHeader("Content-Type", "application/json");
	SendScoreRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	SendScoreRequest->SetContentAsString(OutputString);
	SendScoreRequest->ProcessRequest();
}
