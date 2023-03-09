// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#include "BSCharacterMovementComponent.h"

UBSCharacterMovementComponent::UBSCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
}

float UBSCharacterMovementComponent::GetMaxSpeed() const
{
	if (bRequestToStartSprinting)
	{
		return SprintSpeedMultiplier * Super::GetMaxSpeed();
	}
	return Super::GetMaxSpeed();
}

void UBSCharacterMovementComponent::StartSprinting()
{
	bRequestToStartSprinting = true;
}

void UBSCharacterMovementComponent::StopSprinting()
{
	bRequestToStartSprinting = false;
}
