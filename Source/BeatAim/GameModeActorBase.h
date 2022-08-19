// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefaultCharacter.h"
#include "GameModeActorStruct.h"
#include "GameModeActorBase.generated.h"

class ASphereTarget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateScoresToHUD, FPlayerScore, NewPlayerScoreStruct);

UCLASS()
class BEATAIM_API AGameModeActorBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGameModeActorBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void SaveGame();

	void LoadGame();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void HandleGameStart();

	UFUNCTION(BlueprintCallable)
	virtual void HandleGameRestart();

	virtual void StartGameMode();

	virtual void EndGameMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FGameModeActorStruct GameModeActorStruct;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FPlayerScore PlayerScoreStruct;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Game Properties")
	FUpdateScoresToHUD UpdateScoresToHUD;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float CountdownTimerLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float GameModeLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Countdown")
	FTimerHandle CountDownTimer;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UFUNCTION()
	void UpdateTargetsHit();

	UFUNCTION()
	void UpdateHighScore();

	UFUNCTION()
	void UpdateScore(float TimeElapsed);

	UFUNCTION()
	void UpdateTargetsSpawned();

	UFUNCTION()
	void UpdateShotsFired();

private:

	//AGameModeActorBase* CurrentGameModeClass;

	FVector StartLocation = { 1806.f,2000.f,92.f };

	FRotator StartRotation = FRotator::ZeroRotator;

};
