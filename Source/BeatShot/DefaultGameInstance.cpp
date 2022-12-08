// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "DefaultGameMode.h"
#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "JsonObjectConverter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Http.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerSettings.h"
#include "SaveGameAASettings.h"
#include "SaveGamePlayerScore.h"

void UDefaultGameInstance::RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter)
{
	DefaultCharacterRef = DefaultCharacter;
}

void UDefaultGameInstance::RegisterTargetSpawner(ATargetSpawner* TargetSpawner)
{
	TargetSpawnerRef = TargetSpawner;
	OnTargetSpawnerInit.Broadcast(TargetSpawner);
}

void UDefaultGameInstance::RegisterSphereTarget(ASphereTarget* SphereTarget)
{
	SphereTargetRef = SphereTarget;
	SphereTargetArray.Add(SphereTarget);
}

void UDefaultGameInstance::RegisterGameModeBase(AGameModeBase* GameModeBase)
{
	GameModeBaseRef = GameModeBase;
}

void UDefaultGameInstance::RegisterGameModeActorBase(AGameModeActorBase* GameModeActorBase)
{
	GameModeActorBaseRef = GameModeActorBase;
}

void UDefaultGameInstance::RegisterPlayerController(ADefaultPlayerController* DefaultPlayerController)
{
	DefaultPlayerControllerRef = DefaultPlayerController;
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

void UDefaultGameInstance::SaveAASettings(const FAASettingsStruct AASettingsToSave) const
{
	if (USaveGameAASettings* SaveGameAASettingsObject = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass())))
	{
		SaveGameAASettingsObject->AASettings = AASettingsToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGameAASettingsObject, TEXT("AASettingsSlot"), 2))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveAASettings Succeeded"));
		}
	}
	OnAASettingsChange.Broadcast(AASettingsToSave);
}

void UDefaultGameInstance::ChangeVolume(USoundClass* SoundClassToChange, USoundMix* SoundMix, const float Volume, float GlobalVolume) const
{
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), SoundMix, SoundClassToChange, round(Volume)/100);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), SoundMix);
}

TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> UDefaultGameInstance::LoadPlayerScores() const
{
	USaveGamePlayerScore* SaveGamePlayerScore;
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
		return SaveGamePlayerScore->PlayerScoreMap;
	}
	SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	return SaveGamePlayerScore->PlayerScoreMap;
}

void UDefaultGameInstance::SavePlayerScores(const TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave, const bool bSaveToDatabase)
{
	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreMap = PlayerScoreMapToSave;

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
	if (IsRefreshTokenValid())
	{
		bIsSavingScores = true;
		RequestAccessToken(LoadPlayerSettings().LoginCookie);
	}
	else
	{
		OnPostPlayerScoresResponse.Broadcast("Invalid Refresh Token", 401);
	}
}

void UDefaultGameInstance::SavePlayerSettings(const FPlayerSettings PlayerSettingsToSave) const
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
	UE_LOG(LogTemp, Warning, TEXT("Settings loaded to Game Instance"));
	return SaveGamePlayerSettings->PlayerSettings;
}

void UDefaultGameInstance::LoginUser(FLoginPayload LoginPayload)
{
	const TSharedRef<FJsonObject> LoginObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, LoginObject, 0, 0);
	FString LoginString;
	const TSharedRef< TJsonWriter<> > LoginWriter = TJsonWriterFactory<>::Create(&LoginString);
	FJsonSerializer::Serialize(LoginObject, LoginWriter);

	const FHttpRequestRef LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnLoginResponseReceived);
	LoginRequest->SetURL(LoginEndpoint);
	LoginRequest->SetVerb("POST");
	LoginRequest->SetHeader("Content-Type", "application/json");
	LoginRequest->SetContentAsString(LoginString);
	LoginRequest->ProcessRequest();
}

void UDefaultGameInstance::OnLoginResponseReceived(FHttpRequestPtr Request, const FHttpResponsePtr Response, bool bConnectedSuccessfully) const
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

void UDefaultGameInstance::RequestAccessToken(FString RefreshToken)
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
		OnRefreshTokenResponse.Broadcast(false);
		PlayerSettings.LoginCookie = "";
		SavePlayerSettings(PlayerSettings);
		UE_LOG(LogTemp, Display, TEXT("Access Token Request Failed. Resetting login cookie"));
		return;
	}
	OnRefreshTokenResponse.Broadcast(true);
	// convert response to Json object to access string fields
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

	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> Map = LoadPlayerScores();
	FJsonScore JsonScores;
	// iterate through all elements in PlayerScoreMap
	for (const TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : Map)
	{
		// get array of player scores for any given game mode & song from current key value
		TArray<FPlayerScore> TempArray = Elem.Value.PlayerScoreArray;
		for (FPlayerScore& PlayerScoreObject : TempArray)
		{
			if (!PlayerScoreObject.bSavedToDatabase)
			{
				JsonScores.Scores.Add(PlayerScoreObject);
			}
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
		UE_LOG(LogTemp, Display, TEXT("Send Scores Request Failed."));
		OnPostPlayerScoresResponse.Broadcast(Response->GetContentAsString(), Response->GetResponseCode());
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Successfully saved scores to database."));
	OnPostPlayerScoresResponse.Broadcast(Response->GetContentAsString(), Response->GetResponseCode());
	
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> Map = LoadPlayerScores();
	// iterate through all elements in PlayerScoreMap
	for (const TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : Map)
	{
		// get array of player scores for any given game mode & song from current key value
		TArray<FPlayerScore> TempArray = Elem.Value.PlayerScoreArray;
		for (FPlayerScore& PlayerScoreObject : TempArray)
		{
			PlayerScoreObject.bSavedToDatabase = true;
		}
	}
	SavePlayerScores(Map, false);
}

