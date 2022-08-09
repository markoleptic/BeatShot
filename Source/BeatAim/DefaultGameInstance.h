// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DefaultGameInstance.generated.h"

class ASphereTarget;
/**
 * 
 */
UCLASS()
class BEATAIM_API UDefaultGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ADefaultCharacter* DefaultCharacterRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ATargetSpawner* TargetSpawnerRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	ASphereTarget* SphereTargetRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ABeatAimGameModeBase* GameModeBaseRef;

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterDefaultCharacter(ADefaultCharacter* DefaultCharacter);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterTargetSpawner(ATargetSpawner* TargetSpawner);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterSphereTarget(ASphereTarget* SphereTarget);

	UFUNCTION(BlueprintCallable, Category = "References")
	void RegisterGameModeBase(ABeatAimGameModeBase* GameModeBase);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TargetsHit = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Score")
	float ShotsFired = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player Score")
	float TargetsSpawned = 0;

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void IncrementTargetsSpawned();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void IncrementShotsFired();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void IncrementTargetsHit();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetTargetsHit();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetShotsFired();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	float GetTargetsSpawned();
};
