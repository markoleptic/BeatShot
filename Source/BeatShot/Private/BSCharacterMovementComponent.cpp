// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#include "BSCharacterMovementComponent.h"

UBSCharacterMovementComponent::UBSCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.0f;
}

void UBSCharacterMovementComponent::SetSprintSpeedMultiplier(float NewSpringSpeedMultiplier)
{
	SprintSpeedMultiplier = NewSpringSpeedMultiplier;
}

float UBSCharacterMovementComponent::GetMaxSpeed() const
{
	return SprintSpeedMultiplier * Super::GetMaxSpeed();
}

void UBSCharacterMovementComponent::StartSprinting()
{
	bRequestToStartSprinting = true;
}

void UBSCharacterMovementComponent::StopSprinting()
{
	bRequestToStartSprinting = false;
}
