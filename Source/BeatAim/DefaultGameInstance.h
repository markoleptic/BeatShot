// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BeatAimGameModeBase.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class ATargetSpawner;
class ADefaultCharacter;
class ABeatAimGameModeBase;
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
	ABeatAimGameModeBase* GameModeBaseRef;

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
	void RegisterGameModeBase(ABeatAimGameModeBase* GameModeBase);

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

private:
	// Game Mode Variables Associated with Scoring

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float TargetsHit = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float ShotsFired = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float TargetsSpawned = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float Score = 0;

	UPROPERTY(VisibleAnywhere, Category = "Player Score")
		float HighScore = 0;

	// Settings Menu options so that options are saved even if character hasn't spawned

	UPROPERTY(VisibleAnywhere, Category = "Settings Menu")
		float Sensitivity = 12.59;

	UPROPERTY(VisibleAnywhere, Category = "Settings Menu")
		float TargetSpawnCD = 0.35f;

};
