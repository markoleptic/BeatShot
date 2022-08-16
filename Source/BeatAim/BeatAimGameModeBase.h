// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "SpiderShotSelector.h"
#include "BeatAimGameModeBase.generated.h"

class ASpidershotGameModeBase;
UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void SetGameModeSelected(bool IsSelected);

	//UFUNCTION(BlueprintCallable)
	//virtual void HandleGameStart(TSubclassOf<AActor> GameModeSelector);

public:
	// Called when an Actor receives damage, determines which function
	void ActorReceivedDamage(AActor* DeadActor);

	// Timers

	//.................rename to GameModeLength
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	FTimerHandle SpiderShotGameLength;

	//.................rename to GameModeLengthTimerHandleLength
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game Properties")
	float SpiderShotTimerHandleLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	FTimerHandle CountDown;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	bool IsGameModeSelected();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void ResetPlayerStats();

	//........eventually replace, used for HandleGameRestart()
	UFUNCTION(BlueprintCallable)
	TSubclassOf<AActor> GetCurrentGameModeClass();

	//........eventually replace, used for HandleGameRestart()
	UFUNCTION(BlueprintCallable)
	void SetCurrentGameModeClass(TSubclassOf<AActor> GameModeStaticClass);

	// Blueprint event used to stop BPAAPlayer and BPAATracker during pause game
	UFUNCTION(BlueprintImplementableEvent)
	void StopAAPlayerAndTracker();

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties")
	//ASpidershotGameModeBase* SpidershotGameModeBaseREF;

	UFUNCTION(BlueprintCallable)
	void HandleGameRestart(TSubclassOf<AActor> GameModeSelector);

	UFUNCTION(BlueprintCallable)
	void HandleGameStart(TSubclassOf<AActor> GameModeSelector);

	UFUNCTION(BlueprintCallable)
	void StartSpiderShot();

	UFUNCTION(BlueprintCallable)
	void EndSpiderShot();

private:

	bool GameModeSelected;

	//........eventually replace, used for HandleGameRestart()
	TSubclassOf<AActor> CurrentGameModeClass;
};
