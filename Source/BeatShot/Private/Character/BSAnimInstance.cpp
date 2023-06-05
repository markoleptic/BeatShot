﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Character/BSAnimInstance.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Character/BSCharacter.h"
#include "Character/BSCharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(BSAnimInstance)

UBSAnimInstance::UBSAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBSAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

void UBSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UBSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	const ABSCharacter* Character = Cast<ABSCharacter>(GetOwningActor());
	if (!Character)
	{
		return;
	}

	UBSCharacterMovementComponent* CharMoveComp = CastChecked<UBSCharacterMovementComponent>(Character->GetCharacterMovement());
	const FCharacterGroundInfo& GroundInfo = CharMoveComp->GetGroundInfo();
	GroundDistance = GroundInfo.GroundDistance;
}