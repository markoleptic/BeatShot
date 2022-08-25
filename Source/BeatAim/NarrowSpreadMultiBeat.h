// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiBeat.h"
#include "NarrowSpreadMultiBeat.generated.h"

/**
 * 
 */
UCLASS()
class BEATAIM_API ANarrowSpreadMultiBeat : public AMultiBeat
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANarrowSpreadMultiBeat();

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
