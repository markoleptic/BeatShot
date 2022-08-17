// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DefaultCharacter.h"
#include "GameModeActorBase.generated.h"

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetGameModeSelected(bool IsSelected);

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	bool IsGameModeSelected();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void ResetPlayerStats();

	UFUNCTION(BlueprintCallable)
	AGameModeActorBase* GetCurrentGameModeClass();

	UFUNCTION(BlueprintCallable)
	virtual void SetCurrentGameModeClass(AGameModeActorBase* GameModeActor);

	// Blueprint event used to stop BP_AAPlayer and BP_AATracker during pause game
	UFUNCTION(BlueprintImplementableEvent)
	void StopAAPlayerAndTracker();

public:
	virtual void HandleGameStart();

	virtual void HandleGameRestart();

	virtual void StartGameMode();

	virtual void EndGameMode();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle GameModeLengthTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float GameModeLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	FTimerHandle CountDownTimer;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

private:

	bool GameModeSelected;

	AGameModeActorBase* CurrentGameModeClass;
};
