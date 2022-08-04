// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TargetSubsystem.generated.h"

UCLASS()
class BEATAIM_API UTargetSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = StatSubsystem)
		float GetTotalTargetsSpawned();

	UFUNCTION(BlueprintCallable, Category = StatSubsystem)
		void SetTotalTargetsSpawned(float TargetsSpawned);

	UPROPERTY(EditAnywhere, Category = StatSubsystem)
		float TotalTargetsSpawned;

	UPROPERTY(EditAnywhere, Category = StatSubsystem)
		float TotalTargetsHit;



	
};
