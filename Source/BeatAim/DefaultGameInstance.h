// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatAimGameModeBase.h"
#include "GameModeActorStruct.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class UDefaultStatSaveGame;
class ATargetSpawner;
class ADefaultCharacter;
class AGameModeBase;
class ASphereTarget;
class AGameModeActorBase;
class ADefaultPlayerController;
class AGameModeActorBase;
class AProjectile;
/**
 * 
 */
UCLASS()
class BEATAIM_API UDefaultGameInstance : public UGameInstance
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	TArray<ASphereTarget*> SphereTargetArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeBase* GameModeBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	AGameModeActorBase* GameModeActorBaseRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ADefaultPlayerController* DefaultPlayerControllerRef;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game State")
	FGameModeActorStruct GameModeActorStruct;

	UFUNCTION(BlueprintCallable)
	void SaveSettings();

	UFUNCTION(BlueprintCallable)
	void LoadSettings();

	// Settings Menu options so that options are saved even if character hasn't spawned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	FPlayerSettings PlayerSettings;

	UFUNCTION(BlueprintCallable)
	void SaveScores(FPlayerScore PlayerScoreStructToSave);

	UFUNCTION(BlueprintCallable)
	void LoadScores();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	UDefaultStatSaveGame* SaveScoreGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	UDefaultStatSaveGame* SaveSettingsGameInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings Menu")
	TArray<FPlayerScore> ArrayOfPlayerScoreStructs;

private:
};
