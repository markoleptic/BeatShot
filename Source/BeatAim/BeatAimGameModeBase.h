// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SpiderShotSelector.h"
#include "GameFramework/GameModeBase.h"
#include "BeatAimGameModeBase.generated.h"

class ASpiderShotSelector;
UCLASS()
class BEATAIM_API ABeatAimGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Called when an Actor receives damage
	void ActorDied(AActor* DeadActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Properties")
	FTimerHandle SpiderShotGameLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Countdown")
	FTimerHandle CountDown;

protected:
	virtual void BeginPlay() override;

	// Called when SpiderShotSelector receives damage
	void StartSpiderShot();

	// Called at the end of SpiderShotGameLength timer
	void EndSpiderShot();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UDefaultGameInstance* GI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ADefaultPlayerController* DefaultPlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ADefaultCharacter* DefaultCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ATargetSpawner* TargetSpawner;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class ASphereTarget* SphereTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
	bool GameModeSelected;

	template<class T>
	void HandleGameStart(T* Actor);
};

template <class T>
void ABeatAimGameModeBase::HandleGameStart(T* Actor)
{
	if (Actor->IsA(ASpiderShotSelector::StaticClass()))
	{
		GameModeSelected = true;
		GetWorldTimerManager().SetTimer(CountDown, this, &ABeatAimGameModeBase::StartSpiderShot, 3.f, false);
	}
}
