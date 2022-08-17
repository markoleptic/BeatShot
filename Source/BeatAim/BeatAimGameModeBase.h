// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DefaultCharacter.h"
#include "WideSpreadMultiBeat.h"
#include "GameFramework/GameModeBase.h"
#include "BeatAimGameModeBase.generated.h"

UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// Called when an Actor receives damage, determines which function
	void ActorReceivedDamage(AActor* DeadActor);

	// Reference Game Instance
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	class UDefaultGameInstance* GI;

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	void SetGameModeSelected(bool IsSelected);

	UFUNCTION(BlueprintCallable, Category = "Game Properties")
	bool IsGameModeSelected();

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void UpdatePlayerStats(bool ShotFired, bool TargetHit, bool TargetSpawned);

	UFUNCTION(BlueprintCallable, Category = "Player Score")
	void ResetPlayerStats();

	//........eventually replace, used for HandleGameRestart()
	//UFUNCTION(BlueprintCallable)
	//TSubclassOf<AActor> GetCurrentGameModeClass();

	//........eventually replace, used for HandleGameRestart()
	//UFUNCTION(BlueprintCallable)
	//void SetCurrentGameModeClass(TSubclassOf<AActor> GameModeStaticClass);

	// Blueprint event used to stop BPAAPlayer and BPAATracker during pause game
	UFUNCTION(BlueprintImplementableEvent)
	void StopAAPlayerAndTracker();

private:

	bool GameModeSelected;

	//........eventually replace, used for HandleGameRestart()
	//TSubclassOf<AActor> CurrentGameModeClass;

	//UPROPERTY(EditAnywhere, Category = "Spawn Properties")
	//TSubclassOf<AWideSpreadMultiBeat> WideSpreadMultiBeatClass;
};
