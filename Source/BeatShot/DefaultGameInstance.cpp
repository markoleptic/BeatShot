// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "GameModeActorBase.h"
#include "JsonObjectConverter.h"
#include "Http.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerSettings.h"
#include "SaveGameAASettings.h"
#include "SaveGamePlayerScore.h"

void UDefaultGameInstance::InitializeGameModeActorStruct(const FGameModeActorStruct NewGameModeActorStruct)
{
	GameModeActorStruct = NewGameModeActorStruct;
}

bool UDefaultGameInstance::IsRefreshTokenValid() const
{
	const FPlayerSettings PlayerSettings = LoadPlayerSettings();
	if (PlayerSettings.HasLoggedInHttp == true)
	{
		if (PlayerSettings.LoginCookie.IsEmpty())
		{
			return false;
		}
		FDateTime CookieExpireDate;
		const int32 ExpiresStartPos = PlayerSettings.LoginCookie.Find("Expires=", ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
		const FString RightChopped = PlayerSettings.LoginCookie.RightChop(ExpiresStartPos + 8);
		const FString CookieExpireString = RightChopped.Left(RightChopped.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0));
		FDateTime::ParseHttpDate(CookieExpireString, CookieExpireDate);

		if ((FDateTime::UtcNow() + FTimespan::FromDays(1) < CookieExpireDate))
		{
			return true;
		}
	}
	return false;
}

FAASettingsStruct UDefaultGameInstance::LoadAASettings() const
{
	USaveGameAASettings* SaveGameAASettings;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("AASettingsSlot"), 2))
	{
		SaveGameAASettings = Cast<USaveGameAASettings>(UGameplayStatics::LoadGameFromSlot(TEXT("AASettingsSlot"), 2));
	}
	else
	{
		SaveGameAASettings = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass()));
	}

	if (SaveGameAASettings)
	{
		return SaveGameAASettings->AASettings;
	}
	return FAASettingsStruct();
}

void UDefaultGameInstance::SaveAASettings(const FAASettingsStruct& AASettingsToSave)
{
	if (USaveGameAASettings* SaveGameAASettingsObject = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass())))
	{
		SaveGameAASettingsObject->AASettings = AASettingsToSave;
		UGameplayStatics::SaveGameToSlot(SaveGameAASettingsObject, TEXT("AASettingsSlot"), 2);
	}
	OnAASettingsChange.Broadcast(AASettingsToSave);
}

TArray<FPlayerScore> UDefaultGameInstance::LoadPlayerScores() const
{
	USaveGamePlayerScore* SaveGamePlayerScore;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
		return SaveGamePlayerScore->PlayerScoreArray;
	}
	SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	return SaveGamePlayerScore->PlayerScoreArray;
}

void UDefaultGameInstance::SavePlayerScores(const TArray<FPlayerScore>& PlayerScoreArrayToSave, const bool bSaveToDatabase)
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreArray = PlayerScoreArrayToSave;

		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
	if (bSaveToDatabase)
	{
		SavePlayerScoresToDatabase();
	}
}

void UDefaultGameInstance::SavePlayerScores(const FPlayerScore PlayerScoreObjectToSave, const bool bSaveToDatabase)
{
	TArray<FPlayerScore> LoadedScores = LoadPlayerScores();
	LoadedScores.Add(PlayerScoreObjectToSave);
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreArray = LoadedScores;
		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
	if (bSaveToDatabase)
	{
		SavePlayerScoresToDatabase();
	}
}

void UDefaultGameInstance::SavePlayerScoresToDatabase()
{
	if (!LoadPlayerSettings().HasLoggedInHttp)
	{
		/** Broadcast custom code for DefaultPlayerController to listen to in case user doesn't have account */
		OnPostPlayerScoresResponse.Broadcast(ELoginState::NewUser);
	}
	else if (!IsRefreshTokenValid())
	{
		/** Broadcast custom code for DefaultPlayerController to listen to in case of invalid refresh token */
		OnPostPlayerScoresResponse.Broadcast(ELoginState::InvalidHttp);
	}
	else
	{
		bIsSavingScores = true;
		RequestAccessToken();
	}
}

void UDefaultGameInstance::SavePlayerSettings(const FPlayerSettings& PlayerSettingsToSave)
{
	if (USaveGamePlayerSettings* SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGamePlayerSettings->PlayerSettings = PlayerSettingsToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerSettings, TEXT("SettingsSlot"), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerSettings Succeeded"));
		}
	}
	OnPlayerSettingsChange.Broadcast(PlayerSettingsToSave);
}

FPlayerSettings UDefaultGameInstance::LoadPlayerSettings() const
{
	USaveGamePlayerSettings* SaveGamePlayerSettings;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass()));
	}
	return SaveGamePlayerSettings->PlayerSettings;
}

void UDefaultGameInstance::LoginUser(const FLoginPayload& LoginPayload)
{
	const TSharedRef<FJsonObject> LoginObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, LoginObject, 0, 0);
	FString LoginString;
	const TSharedRef< TJsonWriter<> > LoginWriter = TJsonWriterFactory<>::Create(&LoginString);
	FJsonSerializer::Serialize(LoginObject, LoginWriter);
	
	const FHttpRequestRef LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->SetURL(LoginEndpoint);
	LoginRequest->SetVerb("POST");
	LoginRequest->SetHeader("Content-Type", "application/json");
	LoginRequest->SetContentAsString(LoginString);
	LoginRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnLoginResponseReceived);
	LoginRequest->ProcessRequest();
}

void UDefaultGameInstance::OnLoginResponseReceived(FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Login Request Failed."));
		OnLoginResponse.Broadcast(Response->GetContentAsString(), Response->GetResponseCode());
		return;
	}

	// create Json object to access string fields
	const FString LoginResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> LoginResponseObj;
	const TSharedRef<TJsonReader<>> LoginResponseReader = TJsonReaderFactory<>::Create(LoginResponseString);
	FJsonSerializer::Deserialize(LoginResponseReader, LoginResponseObj);

	FPlayerSettings PlayerSettings = LoadPlayerSettings();
	PlayerSettings.HasLoggedInHttp = true;
	PlayerSettings.Username = LoginResponseObj->GetStringField("username");
	PlayerSettings.LoginCookie = Response->GetHeader("set-cookie");
	SavePlayerSettings(PlayerSettings);

	OnLoginResponse.Broadcast(Response->GetContentAsString(), Response->GetResponseCode());
	UE_LOG(LogTemp, Display, TEXT("Login successful for %s"), *PlayerSettings.Username);
}

void UDefaultGameInstance::RequestAccessToken()
{
	// not currently using RefreshToken parameter but may change in future to double check token
	const FHttpRequestRef AccessTokenRequest = FHttpModule::Get().CreateRequest();
	AccessTokenRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnAccessTokenResponseReceived);
	AccessTokenRequest->SetURL(RefreshEndpoint);
	AccessTokenRequest->SetVerb("GET");
	AccessTokenRequest->SetHeader("Cookie", LoadPlayerSettings().LoginCookie);
	AccessTokenRequest->ProcessRequest();
}

void UDefaultGameInstance::OnAccessTokenResponseReceived(FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (Response->GetResponseCode() != 200)
	{
		FPlayerSettings PlayerSettings = LoadPlayerSettings();
		PlayerSettings.LoginCookie = "";
		SavePlayerSettings(PlayerSettings);
		OnRefreshTokenResponse.Broadcast(false);
		UE_LOG(LogTemp, Display, TEXT("Access Token Request Failed. Resetting login cookie"));
		
		if (bIsSavingScores)
		{
			/** Broadcast custom code for DefaultPlayerController to listen to in case of invalid refresh token */
			OnPostPlayerScoresResponse.Broadcast(ELoginState::InvalidHttp);
		}
		return;
	}
	
	OnRefreshTokenResponse.Broadcast(true);
	UE_LOG(LogTemp, Display, TEXT("Successful Access Token Response"));
	
	/** Convert response to Json object to access string fields */
	const FString ResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> ResponseObj;
	const TSharedRef<TJsonReader<>> ResponseReader = TJsonReaderFactory<>::Create(ResponseString);
	FJsonSerializer::Deserialize(ResponseReader, ResponseObj);
	
	if (bIsSavingScores)
	{
		PostPlayerScores(ResponseObj->GetStringField("accessToken"), Response->GetResponseCode());
	}
}

void UDefaultGameInstance::PostPlayerScores(const FString AccessToken, const int32 ResponseCode)
{
	if (ResponseCode != 200)
	{
		return;
	}

	TArray<FPlayerScore> PlayerScoreArray = LoadPlayerScores();
	FJsonScore JsonScores;
	// Add all elements that haven't been saved to database to the JsonScores Scores array
	for (FPlayerScore& PlayerScoreObject : PlayerScoreArray)
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
	const TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);


	UE_LOG(LogTemp, Display, TEXT("FJsonScore: %s"), *OutputString);
	// ReSharper disable once StringLiteralTypo
	const FString Endpoint = SaveScoresEndpoint + LoadPlayerSettings().Username + "/savescores";
	const FHttpRequestRef SendScoreRequest = FHttpModule::Get().CreateRequest();
	SendScoreRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnPostPlayerScoresResponseReceived);
	SendScoreRequest->SetURL(Endpoint);
	SendScoreRequest->SetVerb("POST");
	SendScoreRequest->SetHeader("Content-Type", "application/json");
	SendScoreRequest->SetHeader("Authorization", "Bearer " + AccessToken);
	SendScoreRequest->SetContentAsString(OutputString);
	SendScoreRequest->ProcessRequest();
}

void UDefaultGameInstance::OnPostPlayerScoresResponseReceived(FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	bIsSavingScores = false;
	if (Response->GetResponseCode() != 200)
	{
		OnPostPlayerScoresResponse.Broadcast(ELoginState::InvalidHttp);
		UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed: %s"), *Response->GetContentAsString());
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
	OnPostPlayerScoresResponse.Broadcast(ELoginState::LoggedInHttp);
	
	TArray<FPlayerScore> PlayerScoreArray = LoadPlayerScores();
	// Save the player scores after marking them as saved to database
	for (FPlayerScore& PlayerScoreObject : PlayerScoreArray)
	{
		PlayerScoreObject.bSavedToDatabase = true;
	}
	SavePlayerScores(PlayerScoreArray, false);
}

