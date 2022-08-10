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

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	//FTimerHandle TimeSinceTargetSpawn;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target Properties")
	//float CurrentGameMaxLifeSpan;

	//UFUNCTION(BlueprintCallable, Category = "Player Stats")

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UPROPERTY(VisibleAnywhere, BlueprintReadonly, Category = "References")
	bool GameModeSelected;

	UFUNCTION(BlueprintCallable, Category = "Player Score")
		void UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
		void ResetPlayerStats();

private:

	template<class T>
	void HandleGameStart(T* Actor);

	void ShowPlayerHUD();
};

template <class T>
void ABeatAimGameModeBase::HandleGameStart(T* Actor)
{
	if (Actor->IsA(ASpiderShotSelector::StaticClass()))
	{
		GameModeSelected = true;
		ResetPlayerStats();
		ShowPlayerHUD();
		GetWorldTimerManager().SetTimer(CountDown, this, &ABeatAimGameModeBase::StartSpiderShot, 3.f, false);
	}
}
