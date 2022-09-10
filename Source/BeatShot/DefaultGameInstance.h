// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorStruct.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class UDefaultStatSaveGame;
class ADefaultGameMode;
class ATargetSpawner;
class ADefaultCharacter;
class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;
class AProjectile;
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

	// Probably not being used by anything besides Projectile
	// TODO: remove the calls by projectile to register this
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AProjectile* ProjectileRef;

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

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterProjectile(AProjectile* Projectile);

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
	// TODO: Maybe move this to DefaultGameMode

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	FAASettingsStruct AASettingsStruct;

	UFUNCTION(BlueprintCallable)
	void SaveAudioAnalyzerSettings(FAASettingsStruct AASettingsStructToSave);

	UFUNCTION(BlueprintCallable)
	FAASettingsStruct LoadAudioAnalyzerSettings();

	// Controls, Music, Video Settings

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	FPlayerSettings PlayerSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	UDefaultStatSaveGame* SaveSettingsGameInstance;

	UFUNCTION(BlueprintCallable)
	void SaveSettings();

	UFUNCTION(BlueprintCallable)
	void LoadSettings();

	// Player Scores
	// TODO: Move any handling of scores to GameModeActorBase

	UFUNCTION(BlueprintCallable)
	void SaveScores(FPlayerScore PlayerScoreStructToSave);

	UFUNCTION(BlueprintCallable)
	void LoadScores();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	UDefaultStatSaveGame* SaveScoreGameInstance;

	// Used to load previously saved PlayerScores, accessed by GameModeActorBase
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	TArray<FPlayerScore> ArrayOfPlayerScoreStructs;

private:
};
