// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultCharacter.h"
#include "GameFramework/GameModeBase.h"
#include "SpiderShotSelector.h"
#include "BeatAimGameModeBase.generated.h"

UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Called when SpiderShotSelector receives damage
	UFUNCTION()
	void StartSpiderShot();

	UFUNCTION()
	// Called at the end of SpiderShotGameLength timer
	void EndSpiderShot();

public:
	// Called when an Actor receives damage
	void ActorDied(AActor* DeadActor);

	// Timers

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties")
	FTimerHandle SpiderShotGameLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	FTimerHandle CountDown;

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "References")
	bool GameModeSelected;

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void ResetPlayerStats();

	UFUNCTION(BlueprintCallable)
		void HandleGameStart(TSubclassOf<AActor> GameModeSelector);
	//template<class T>
	//void HandleGameStart(T* Actor);

	UFUNCTION(BlueprintCallable)
		void HandleGameRestart(TSubclassOf<AActor> GameModeSelector);

	UFUNCTION(BlueprintCallable)
		TSubclassOf<AActor> GetCurrentGameModeClass();

	UFUNCTION(BlueprintCallable)
		void SetCurrentGameModeClass(TSubclassOf<AActor> GameModeStaticClass);

	UFUNCTION(BlueprintImplementableEvent)
		void StopAAPlayerAndTracker();

private:

	TSubclassOf<AActor> CurrentGameModeClass;

	void ShowPlayerHUD();
};

//template <class T>
//void ABeatAimGameModeBase::HandleGameStart(T* GameModeSelector)
//{
//	if (GameModeSelector->IsA(ASpiderShotSelector::StaticClass()))
//	{
//		GameModeSelected = true;
//		ResetPlayerStats();
//		ShowPlayerHUD();
//		GetWorldTimerManager().SetTimer(CountDown, this, &ABeatAimGameModeBase::StartSpiderShot, 3.f, false);
//	}
//}
