// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "WideSpreadMultiBeat.generated.h"

/**
 * 
 */
UCLASS()
class BEATAIM_API AWideSpreadMultiBeat : public AGameModeActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWideSpreadMultiBeat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION(BlueprintCallable)
	//AGameModeActorBase* GetCurrentGameModeClass();

	//UFUNCTION(BlueprintCallable)
	//virtual void SetCurrentGameModeClass(AGameModeActorBase* GameModeActor) override;

public:

	UFUNCTION(BlueprintCallable)
	virtual void HandleGameRestart() override;

	virtual void StartGameMode() override;

	virtual void EndGameMode() override;
};
