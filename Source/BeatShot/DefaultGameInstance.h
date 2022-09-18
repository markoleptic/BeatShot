// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "SaveGameAASettings.h"
#include "Engine/GameInstance.h"
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

	UFUNCTION(BlueprintCallable, Category = "Scoring")
	void SavePlayerScores(TMap<FGameModeActorStruct, FPlayerScoreArrayWrapper> PlayerScoreMapToSave);

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
};
