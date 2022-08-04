// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetSubsystem.h"

float UTargetSubsystem::GetTotalTargetsSpawned()
{
	return TotalTargetsSpawned;
}

void UTargetSubsystem::SetTotalTargetsSpawned(float TargetsSpawned)
{
	TotalTargetsSpawned = TargetsSpawned;
}


