// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MultiBeat.h"
#include "WideSpreadMultiBeat.generated.h"

/**
 * 
 */
UCLASS()
class BEATSHOT_API AWideSpreadMultiBeat : public AMultiBeat
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

	virtual void HandleGameStart() override;

	virtual void HandleGameRestart(bool ShouldSavePlayerScores) override;

	virtual void StartGameMode() override;

	virtual void EndGameMode() override;

	//template < class T >
	//T* GetController() const
	//{
	//	return Cast<T>(GetController());
	//}
};
