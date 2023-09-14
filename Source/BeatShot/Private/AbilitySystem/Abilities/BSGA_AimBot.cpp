// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "AbilitySystem/Abilities/BSGA_AimBot.h"
#include "AbilitySystem/Abilities/BSGA_FireGun.h"
#include "AbilitySystem/Tasks/BSAT_AimToTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "Character/BSCharacter.h"
#include "Target/Target.h"

UBSGA_AimBot::UBSGA_AimBot()
{
	SmoothingCurve = nullptr;
	ActivationPolicy = EBSAbilityActivationPolicy::OnSpawn;
	ActivationGroup = EBSAbilityActivationGroup::Independent;
	AbilityTags.AddTag(FBSGameplayTags().Get().Cheat_AimBot);
	ActivationOwnedTags.AddTag(FBSGameplayTags().Get().Cheat_AimBot);
}

void UBSGA_AimBot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                   const FGameplayEventData* TriggerEventData)
{
	ABSCharacter* Character = GetBSCharacterFromActorInfo();
	if (!Character)
	{
		return;
	}
	Character->OnTargetAddedToQueue.AddUniqueDynamic(this, &ThisClass::OnTargetAddedToQueue);
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSGA_AimBot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBSGA_AimBot::OnTargetAddedToQueue()
{
	ABSCharacter* Character = GetBSCharacterFromActorInfo();
	if (!Character)
	{
		return;
	}

	if (ATarget* ActiveTarget = Character->PeekActiveTargets())
	{
		const FVector Location = ActiveTarget->GetActorLocation();
		if (IgnoreStartLocation != FVector::ZeroVector)
		{
			if ((IgnoreStartLocation.Y > 0.f && IgnoreStartLocation.Y > Location.Y) ||
				(IgnoreStartLocation.Z > 0.f && IgnoreStartLocation.Z > Location.Z) ||
				(IgnoreStartLocation.Y < 0.f && IgnoreStartLocation.Y < Location.Y) ||
				(IgnoreStartLocation.Z < 0.f && IgnoreStartLocation.Z < Location.Z))
			{
				return;
			}
		}
		UBSAT_AimToTarget* AimToTarget = UBSAT_AimToTarget::AimToTarget(this, FName(), SmoothingCurve, ActiveTarget, ActiveTarget->GetSpawnBeatDelay());
		AimToTarget->OnCancelled.AddDynamic(this, &ThisClass::OnAimToTargetCancelled);
		AimToTarget->OnCompleted.AddDynamic(this, &ThisClass::OnAimToTargetCompleted);
		AimToTarget->ReadyForActivation();
	}
}

void UBSGA_AimBot::OnAimToTargetCancelled()
{
	ABSCharacter* Character = GetBSCharacterFromActorInfo();
	UBSAbilitySystemComponent* ASC = GetBSAbilitySystemComponentFromActorInfo();
	if (!Character || !ASC)
	{
		return;
	}
	
	Character->PopActiveTargets();
	bool bActivated = ASC->TryActivateAbilityByClass(GA_FireGun);
}

void UBSGA_AimBot::OnAimToTargetCompleted()
{
	ABSCharacter* Character = GetBSCharacterFromActorInfo();
	UBSAbilitySystemComponent* ASC = GetBSAbilitySystemComponentFromActorInfo();
	if (!Character || !ASC)
	{
		return;
	}
	
	Character->PopActiveTargets();
	bool bActivated = ASC->TryActivateAbilityByClass(GA_FireGun);
}
