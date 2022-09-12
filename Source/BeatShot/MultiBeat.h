// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "MultiBeat.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API AMultiBeat : public AGameModeActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMultiBeat();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void HandleGameStart() override;

	virtual void HandleGameRestart() override;

	virtual void StartGameMode() override;

	virtual void EndGameMode() override;
	
};