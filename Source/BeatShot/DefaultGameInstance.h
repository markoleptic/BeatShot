// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "SaveGameAASettings.h"
#include "SaveGamePlayerSettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "DefaultGameInstance.generated.h"

class ULoadingScreenWidget;
class USaveGamePlayerScore;
class USaveGamePlayerSettings;
class ADefaultGameMode;
class ATargetSpawner;
class ADefaultCharacter;
class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class USoundClass;
class USoundMix;
class ADefaultPlayerController;

/* Used to convert PlayerScoreArray to database scores */
USTRUCT(BlueprintType)
struct FJsonScore
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FPlayerScore> Scores;
};

/* Simple login payload */
USTRUCT(BlueprintType)
struct FLoginPayload
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Username;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
	FString Email;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Login")
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

/** Broadcast when AudioAnalyzer settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAASettingsChange, const FAASettingsStruct&, RefreshedAASettings);

/** Broadcast when Player Settings are changed and saved */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerSettingsChange, const FPlayerSettings&, RefreshedPlayerSettings);

/** Broadcast if refresh token is invalid */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshTokenResponse, const bool, bSuccessful);

/** Broadcast when a login response is received from Beatshot website */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLoginResponse, const FString, ResponseMsg, const int32, ResponseCode);

/** Broadcast when a response is received from posting player scores to database */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPostPlayerScoresResponse, const FString, ResponseMsg, const int32,
                                             ResponseCode);

UCLASS()
class BEATSHOT_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

#pragma region References

public:
	// Register Functions

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeActorBase(AGameModeActorBase* GameModeActorBase);

	UFUNCTION(BlueprintCallable, Category = "References")
	void InitializeGameModeActorStruct(const FGameModeActorStruct NewGameModeActorStruct);

	UPROPERTY(BlueprintReadWrite, Category = "References")
	ADefaultCharacter* DefaultCharacterRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeActorBase* GameModeActorBaseRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "References")
	FGameModeActorStruct GameModeActorStruct;

#pragma endregion

#pragma region Settings

	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	FAASettingsStruct LoadAASettings() const;

	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void SaveAASettings(const FAASettingsStruct& AASettingsToSave);

	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	FPlayerSettings LoadPlayerSettings() const;

	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SavePlayerSettings(const FPlayerSettings& PlayerSettingsToSave);

#pragma endregion

#pragma region Scoring
	
	TArray<FPlayerScore> LoadPlayerScores() const;
	
	void SavePlayerScores(const TArray<FPlayerScore>& PlayerScoreArrayToSave, bool bSaveToDatabase);

	void SavePlayerScores(FPlayerScore PlayerScoreObjectToSave, const bool bSaveToDatabase);

	/* Database saving of scores. First sends an access token request, then calls save scores with the access token */
	void SavePlayerScoresToDatabase();

private:
	/* Bound to onAccessTokenResponse and removed when this function calls PostPlayerScores */
	void PostPlayerScores(FString AccessTokenFString, int32 ResponseCode);

	bool bIsSavingScores = false;

#pragma endregion

#pragma region HttpRequests

public:
	UFUNCTION(BlueprintCallable, Category = "Authorization")
	void LoginUser(const FLoginPayload& LoginPayload);

	UFUNCTION(BlueprintCallable, Category = "Authorization")
	void RequestAccessToken();

	UFUNCTION(BlueprintCallable, Category = "Authorization")
	bool IsRefreshTokenValid() const;

private:
	/* Bound to OnProcessRequestComplete Login Response */
	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	/* Bound to OnProcessRequestComplete RequestAccessToken Response */
	void OnAccessTokenResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	/* Bound to OnProcessRequestComplete PostPlayerScores Response */
	void OnPostPlayerScoresResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response,
	                                        bool bConnectedSuccessfully);
	
	FString Username;
	FString LoginEndpoint = "https://beatshot.gg/api/login";
	const FString RefreshEndpoint = "https://beatshot.gg/api/refresh";
	const FString SaveScoresEndpoint = "https://beatshot.gg/api/profile/";

#pragma endregion

#pragma region Delegates

public:
	UPROPERTY(BlueprintAssignable)
	FOnAASettingsChange OnAASettingsChange;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSettingsChange OnPlayerSettingsChange;

	UPROPERTY(BlueprintAssignable)
	FOnLoginResponse OnLoginResponse;

	UPROPERTY(BlueprintAssignable)
	FOnPostPlayerScoresResponse OnPostPlayerScoresResponse;

	UPROPERTY(BlueprintAssignable)
	FOnRefreshTokenResponse OnRefreshTokenResponse;

#pragma endregion




};
