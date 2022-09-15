// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "SaveGameAASettings.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class USaveGamePlayerSettings;
class ADefaultGameMode;
class ATargetSpawner;
class ADefaultCharacter;
class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;
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
	void SetTargetSpawnCD(float NewTargetSpawnCD);

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
	FAASettingsStruct AASettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	USaveGameAASettings* SaveGameAASettings;

	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void LoadAASettings();

	// Player Settings: Video, Music, Controls

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	USaveGamePlayerSettings* SaveGamePlayerSettings;

	UFUNCTION(BlueprintCallable, Category = "Settings Menu")
	void SavePlayerSettings();

	UFUNCTION(BlueprintCallable, Category = "Settings Menu")
	void LoadPlayerSettings();
};
