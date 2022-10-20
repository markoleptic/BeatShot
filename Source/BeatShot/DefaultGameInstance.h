// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "SaveGameAASettings.h"
#include "Engine/GameInstance.h"
#include "Interfaces/IHttpRequest.h"
#include "DefaultGameInstance.generated.h"

class USaveGamePlayerScore;
class USaveGamePlayerSettings;
class ADefaultGameMode;
class ATargetSpawner;
class ADefaultCharacter;
class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAASettingsChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSettingsChange);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerScoresChange);

/**
 * 
 */
UCLASS()
class BEATSHOT_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

	virtual void Init() override;

public:

	//References

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ADefaultCharacter* DefaultCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ATargetSpawner* TargetSpawnerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ASphereTarget* SphereTargetRef;

	// Only used to make sure all targets are destroyed at the end of game
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TArray<ASphereTarget*> SphereTargetArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeBase* GameModeBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeActorBase* GameModeActorBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ADefaultPlayerController* DefaultPlayerControllerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	FGameModeActorStruct GameModeActorStruct;

	// Register Functions

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterTargetSpawner(ATargetSpawner* TargetSpawner);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterSphereTarget(ASphereTarget* SphereTarget);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeBase(AGameModeBase* GameModeBase);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeActorBase(AGameModeActorBase* GameModeActorBase);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterPlayerController(ADefaultPlayerController* DefaultPlayerController);

	// Settings Menu setter and getter functions

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float InputSensitivity);

	UFUNCTION(BlueprintCallable)
	float GetSensitivity();

	UFUNCTION(BlueprintCallable)
	float GetTargetSpawnCD();

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float InputVolume);

	UFUNCTION(BlueprintCallable)
	float GetMasterVolume();

	UFUNCTION(BlueprintCallable)
	void SetMenuVolume(float InputVolume);

	UFUNCTION(BlueprintCallable)
	float GetMenuVolume();

	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float InputVolume);

	UFUNCTION(BlueprintCallable)
	float GetMusicVolume();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	EGameModeActorName GameModeActorName;

	// Audio Analyzer Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	USaveGameAASettings* SaveGameAASettings;

	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	FAASettingsStruct LoadAASettings();

	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void SaveAASettings(FAASettingsStruct AASettingsToSave);

	// Player Scores Loading / Saving

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scoring")
	USaveGamePlayerScore* SaveGamePlayerScore;

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> LoadPlayerScores();

	// local saving of scores
	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void SavePlayerScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave);

	// First sends an access token request, then calls savescores with the accesstoken
	UFUNCTION(BlueprintCallable, Category = "DataBase")
	void SavePlayerScoresToDatabase(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave);

	// Player Settings: Video, Music, Controls

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	USaveGamePlayerSettings* SaveGamePlayerSettings;

	UFUNCTION(BlueprintCallable, Category = "Settings Menu")
	void SavePlayerSettings();


	UFUNCTION(BlueprintCallable, Category = "Settings Menu")
	void LoadPlayerSettings();

	// delegates

	UPROPERTY(BlueprintAssignable)
	FOnAASettingsChange OnAASettingsChange;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerSettingsChange OnPlayerSettingsChange;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerScoresChange OnPlayerScoresChange;

private:

	UFUNCTION(Category = "Authorization")
	void LoginUser(FLoginPayload LoginPayload);

	UFUNCTION(Category = "Authorization")
	void RequestAccessToken(FString RefreshToken);

	UFUNCTION(Category = "Authorization")
	bool IsRefreshTokenValid();

	void OnLoginResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnAccessTokenResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnSendScoresResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	UPROPERTY(BlueprintReadOnly, Category = "Authorization", meta = (AllowPrivateAccess = true))
	FString Username;

	UPROPERTY(BlueprintReadOnly, Category = "Authorization", meta = (AllowPrivateAccess = true))
	FString LoginEndpoint = "http://localhost:3000/api/login";

	UPROPERTY(BlueprintReadOnly, Category = "Authorization", meta = (AllowPrivateAccess = true))
	FString RefreshEndpoint = "http://localhost:3000/api/refresh";

	UPROPERTY(BlueprintReadWrite, Category = "Authorization", meta = (AllowPrivateAccess = true))
	FString ScoresEndpoint;

	FString SaveScoresEndpoint;

	FString LoadScoresEndpoint;
};

