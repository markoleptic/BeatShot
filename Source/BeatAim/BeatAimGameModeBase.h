// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BeatAimGameModeBase.generated.h"

UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	void ActorDied(AActor* DeadActor);

	FTimerHandle TimerHandle;

protected:
	virtual void BeginPlay() override;

	void StartSpiderShot();

	void EndSpiderShot();

	void PlayerGameModeSelected(FString GameMode);

private:

	class ADefaultPlayerController* DefaultPlayerController;

	class ADefaultCharacter* DefaultCharacter;

	// Eventually make more efficient way to reference actor
	class ATargetSpawner* TargetSpawnerRef;

	// Eventually make more efficient way to reference actor
	void FindAllActors(UWorld* World, TArray<ATargetSpawner*>& Out);

	FString GameModeSelector;
};
