// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#include "BSCharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "GameFramework/Character.h"

UBSCharacterMovementComponent::UBSCharacterMovementComponent()
{
	SprintSpeedMultiplier = 1.4f;
	ADSSpeedMultiplier = 0.5f;
}

float UBSCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed();
	ABSCharacter* Owner = Cast<ABSCharacter>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() No Owner"), *FString(__FUNCTION__));
		return Super::GetMaxSpeed();
	}

	UE_LOG(LogTemp, Display, TEXT("Owner->GetMoveSpeed() %f"), Owner->GetMoveSpeed());
	
	if (RequestToStartSprinting)
	{
		return Owner->GetMoveSpeed() * SprintSpeedMultiplier;
	}
	
	if (RequestToStartADS)
	{
		return Owner->GetMoveSpeed() * ADSSpeedMultiplier;
	}
	
	return Owner->GetMoveSpeed();
}

void UBSCharacterMovementComponent::StartSprinting()
{
	RequestToStartSprinting = true;
}

void UBSCharacterMovementComponent::StopSprinting()
{
	RequestToStartSprinting = false;
}

void UBSCharacterMovementComponent::StartAimDownSights()
{
	RequestToStartADS = true;
}

void UBSCharacterMovementComponent::StopAimDownSights()
{
	RequestToStartADS = false;
}
