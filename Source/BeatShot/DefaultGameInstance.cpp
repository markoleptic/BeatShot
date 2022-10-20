// Fill out your copyright notice in the Description page of Project Settings.


#include "DefaultGameInstance.h"
#include "DefaultPlayerController.h"
#include "DefaultGameMode.h"
#include "GameModeActorBase.h"
#include "DefaultCharacter.h"
#include "JsonObjectConverter.h"
#include "TargetSpawner.h"
#include "SphereTarget.h"
#include "Kismet/GameplayStatics.h"
#include "SaveGamePlayerSettings.h"
#include "SaveGameAASettings.h"
#include "SaveGamePlayerScore.h"


void UDefaultGameInstance::Init()
{
	LoadPlayerSettings();
	LoadAASettings();
	LoadPlayerScores();
}

bool UDefaultGameInstance::IsRefreshTokenValid()
{
	if (PlayerSettings.HasLoggedIn == true)
	{
		FDateTime CookieExpireDate;
		const int32 ExpiresStartPos = PlayerSettings.LoginCookie.Find("Expires=", ESearchCase::CaseSensitive, ESearchDir::FromStart, 0);
		const FString RightChopped = PlayerSettings.LoginCookie.RightChop(ExpiresStartPos + 8);
		const FString CookieExpireString = RightChopped.Left(RightChopped.Find(";", ESearchCase::IgnoreCase, ESearchDir::FromStart, 0));
		FDateTime::ParseHttpDate(CookieExpireString, CookieExpireDate);

		if ((FDateTime::UtcNow() + FTimespan::FromDays(1) < CookieExpireDate))
		{
			return false;
		}
		return true;
	}
	return true;
}

void UDefaultGameInstance::RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter)
{
	DefaultCharacterRef = DefaultCharacter;
	if (DefaultCharacterRef)
	{
		DefaultCharacterRef->SetSensitivity(GetSensitivity());
	}
}

void UDefaultGameInstance::RegisterTargetSpawner(ATargetSpawner* TargetSpawner)
{
	TargetSpawnerRef = TargetSpawner;
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

void UDefaultGameInstance::SetSensitivity(float InputSensitivity)
{
	PlayerSettings.Sensitivity = InputSensitivity;
	if (DefaultCharacterRef)
	{
		DefaultCharacterRef->SetSensitivity(InputSensitivity);
	}
}

float UDefaultGameInstance::GetSensitivity()
{
	return PlayerSettings.Sensitivity;
}

float UDefaultGameInstance::GetTargetSpawnCD()
{
	return GameModeActorStruct.TargetSpawnCD;
}

void UDefaultGameInstance::SetMasterVolume(float InputVolume)
{
	PlayerSettings.MasterVolume = InputVolume;
}

float UDefaultGameInstance::GetMasterVolume()
{
	return PlayerSettings.MasterVolume;
}

void UDefaultGameInstance::SetMenuVolume(float InputVolume)
{
	PlayerSettings.MenuVolume = InputVolume;
}

float UDefaultGameInstance::GetMenuVolume()
{
	return PlayerSettings.MenuVolume;
}

void UDefaultGameInstance::SetMusicVolume(float InputVolume)
{
	PlayerSettings.MusicVolume = InputVolume;
}

float UDefaultGameInstance::GetMusicVolume()
{
	return PlayerSettings.MusicVolume;
}

FAASettingsStruct UDefaultGameInstance::LoadAASettings()
{
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
		UE_LOG(LogTemp, Warning, TEXT("AASettings loaded to Game Instance"));
		return SaveGameAASettings->AASettings;
	}
	return FAASettingsStruct();
}

void UDefaultGameInstance::SaveAASettings(FAASettingsStruct AASettingsToSave)
{
	if (USaveGameAASettings* SaveGameAASettingsObject = Cast<USaveGameAASettings>(UGameplayStatics::CreateSaveGameObject(USaveGameAASettings::StaticClass())))
	{
		SaveGameAASettingsObject->AASettings = AASettingsToSave;
		if (UGameplayStatics::SaveGameToSlot(SaveGameAASettingsObject, TEXT("AASettingsSlot"), 2))
		{
			UE_LOG(LogTemp, Warning, TEXT("SaveAASettings Succeeded"));
		}
	}
	OnAASettingsChange.Broadcast();
}

TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> UDefaultGameInstance::LoadPlayerScores()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("ScoreSlot"), 1))
	{
		SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::LoadGameFromSlot(TEXT("ScoreSlot"), 1));
		UE_LOG(LogTemp, Display, TEXT("PlayerScores Loaded"));
		return SaveGamePlayerScore->PlayerScoreMap;
	}
	SaveGamePlayerScore = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass()));
	return SaveGamePlayerScore->PlayerScoreMap;
}

void UDefaultGameInstance::SavePlayerScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave)
{
	SavePlayerScoresToDatabase(PlayerScoreMapToSave);

	if (USaveGamePlayerScore* SaveGamePlayerScores = Cast<USaveGamePlayerScore>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerScore::StaticClass())))
	{
		SaveGamePlayerScores->PlayerScoreMap = PlayerScoreMapToSave;

		if (UGameplayStatics::SaveGameToSlot(SaveGamePlayerScores, TEXT("ScoreSlot"), 1))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerScores Succeeded"));
		}
	}
	OnPlayerScoresChange.Broadcast();
}

void UDefaultGameInstance::SavePlayerScoresToDatabase(
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave)
{
	if (IsRefreshTokenValid())
	{
		RequestAccessToken(PlayerSettings.LoginCookie);
	}
	else
	{
		FLoginPayload Login;
		Login.Username = "mark";
		Login.Email = "";
		Login.Password = "Vrc5Mj8kUEEabJM";
		LoginUser(Login);
	}
}

void UDefaultGameInstance::SavePlayerSettings()
{
	if (USaveGamePlayerSettings* SaveGameInstanceToSave = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass())))
	{
		SaveGameInstanceToSave->PlayerSettings = PlayerSettings;
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstanceToSave, TEXT("SettingsSlot"), 0))
		{
			UE_LOG(LogTemp, Warning, TEXT("SavePlayerSettings Succeeded"));
		}
	}
	OnPlayerSettingsChange.Broadcast();
}

void UDefaultGameInstance::LoadPlayerSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(TEXT("SettingsSlot"), 0))
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::LoadGameFromSlot(TEXT("SettingsSlot"), 0));
	}
	else
	{
		SaveGamePlayerSettings = Cast<USaveGamePlayerSettings>(UGameplayStatics::CreateSaveGameObject(USaveGamePlayerSettings::StaticClass()));
	}
	if (SaveGamePlayerSettings)
	{
		PlayerSettings = SaveGamePlayerSettings->PlayerSettings;
		UE_LOG(LogTemp, Warning, TEXT("Settings loaded to Game Instance"));
	}
}

void UDefaultGameInstance::LoginUser(FLoginPayload LoginPayload)
{
	const TSharedRef<FJsonObject> LoginObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(FLoginPayload::StaticStruct(), &LoginPayload, LoginObject, 0, 0);
	FString LoginString;
	TSharedRef< TJsonWriter<> > LoginWriter = TJsonWriterFactory<>::Create(&LoginString);
	FJsonSerializer::Serialize(LoginObject, LoginWriter);

	FHttpRequestRef LoginRequest = FHttpModule::Get().CreateRequest();
	LoginRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnLoginResponseReceived);
	LoginRequest->SetURL(LoginEndpoint);
	LoginRequest->SetVerb("POST");
	LoginRequest->SetHeader("Content-Type", "application/json");
	LoginRequest->SetContentAsString(LoginString);
	LoginRequest->ProcessRequest();
}

void UDefaultGameInstance::RequestAccessToken(FString RefreshToken)
{
	FHttpRequestRef AccessTokenRequest = FHttpModule::Get().CreateRequest();
	AccessTokenRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnAccessTokenResponseReceived);
	AccessTokenRequest->SetURL(RefreshEndpoint);
	AccessTokenRequest->SetVerb("GET");
	AccessTokenRequest->SetHeader("Cookie", PlayerSettings.LoginCookie);
	AccessTokenRequest->ProcessRequest();
}

void UDefaultGameInstance::OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Login Request Failed."));
		return;
	}
	const FString LoginResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> LoginResponseObj;
	const TSharedRef<TJsonReader<>> LoginResponseReader = TJsonReaderFactory<>::Create(LoginResponseString);
	FJsonSerializer::Deserialize(LoginResponseReader, LoginResponseObj);

	UE_LOG(LogTemp, Display, TEXT("Login successful for %s"), *Username);
	PlayerSettings.HasLoggedIn = true;
	PlayerSettings.Username = LoginResponseObj->GetStringField("username");
	PlayerSettings.LoginCookie = Response->GetHeader("set-cookie");
	SavePlayerSettings();
}

void UDefaultGameInstance::OnAccessTokenResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (Response->GetResponseCode() != 200)
	{
		UE_LOG(LogTemp, Display, TEXT("Access Token Request Failed."));
		return;
	}

	const FString RefreshResponseString = Response->GetContentAsString();
	TSharedPtr<FJsonObject> RefreshResponseObj;
	const TSharedRef<TJsonReader<>> RefreshResponseReader = TJsonReaderFactory<>::Create(RefreshResponseString);
	FJsonSerializer::Deserialize(RefreshResponseReader, RefreshResponseObj);

	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> Map = LoadPlayerScores();
	FJsonScore JsonScore;
	// iterate through all elements in PlayerScoreMap
	for (TTuple<FGameModeActorStruct, FPlayerScoreArrayWrapper>& Elem : Map)
	{
		// get array of player scores from current key value
		TArray<FPlayerScore> TempArray = Elem.Value.PlayerScoreArray;

		// iterate through array of player scores to find high score for current game mode / song
		for (FPlayerScore& PlayerScoreObject : TempArray)
		{
			JsonScore.Scores.Add(PlayerScoreObject);
		}
	}

	TSharedRef<FJsonObject> OutJsonObject = MakeShareable(new FJsonObject);
	FJsonObjectConverter::UStructToJsonObject(
		FJsonScore::StaticStruct(), 
		&JsonScore, 
		OutJsonObject);
	FString OutputString;

	TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&OutputString);
	FJsonSerializer::Serialize(OutJsonObject, Writer);

	SaveScoresEndpoint = "http://localhost:3000/api/profile/" + Username + "/savescores";
	FHttpRequestRef SendScoreRequest = FHttpModule::Get().CreateRequest();
	SendScoreRequest->OnProcessRequestComplete().BindUObject(this, &UDefaultGameInstance::OnSendScoresResponseReceived);
	SendScoreRequest->SetURL(SaveScoresEndpoint);
	SendScoreRequest->SetVerb("POST");
	SendScoreRequest->SetHeader("Content-Type", "application/json");
	SendScoreRequest->SetHeader("Authorization", "Bearer " + RefreshResponseObj->GetStringField("accessToken"));
	SendScoreRequest->SetContentAsString(OutputString);
	SendScoreRequest->ProcessRequest();
}

void UDefaultGameInstance::OnSendScoresResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *Response->GetContentAsString());
}

