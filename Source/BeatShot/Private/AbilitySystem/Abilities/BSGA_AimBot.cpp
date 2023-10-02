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
	bYPositive = false;
	bYNegative = false;
	bYZero = true;
	bZPositive = false;
	bZNegative = false;
	bZZero = true;
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
	SetIgnoreStartLocation(IgnoreStartLocation);
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
	ATarget* ActiveTarget = Character->PeekActiveTargets();
	if (!Character || !ActiveTarget)
	{
		return;
	}
	
	if (TargetLocationIsInIgnoreRange(ActiveTarget->GetActorLocation()))
	{
		Character->PopActiveTargets();
		return;
	}
	
	UBSAT_AimToTarget* AimToTarget = UBSAT_AimToTarget::AimToTarget(this, FName(), SmoothingCurve, ActiveTarget, ActiveTarget->GetSpawnBeatDelay() + 0.01f);
	AimToTarget->OnCancelled.AddDynamic(this, &ThisClass::OnAimToTargetCancelled);
	AimToTarget->OnCompleted.AddDynamic(this, &ThisClass::OnAimToTargetCompleted);
	AimToTarget->ReadyForActivation();
}

void UBSGA_AimBot::SetIgnoreStartLocation(const FVector& In)
{
	IgnoreStartLocation = In;
	bYPositive = IgnoreStartLocation.Y > 0.f;
	bYNegative = IgnoreStartLocation.Y < 0.f;
	bYZero = IgnoreStartLocation.Y == 0.f;
	
	bZPositive = IgnoreStartLocation.Z > 0.f;
	bZNegative = IgnoreStartLocation.Z < 0.f;
	bZZero = IgnoreStartLocation.Z == 0.f;
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
	ASC->TryActivateAbilityByClass(GA_FireGun);
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
	ASC->TryActivateAbilityByClass(GA_FireGun);
}

bool UBSGA_AimBot::TargetLocationIsInIgnoreRange(const FVector& Loc) const
{
	if (bYZero && bZZero)
	{
		return false;
	}
	// Only consider Y values
	if (!bYZero && bZZero)
	{
		if ((bYPositive && Loc.Y > IgnoreStartLocation.Y) || (bYNegative && Loc.Y < IgnoreStartLocation.Y))
		{
			return true;
		}
		return false;
	}
	// Only consider Z values
	if (bYZero && !bZZero)
	{
		if ((bZPositive && Loc.Z > IgnoreStartLocation.Z) || (bZNegative && Loc.Z < IgnoreStartLocation.Z))
		{
			return true;
		}
		return false;
	}
	// Consider both
	if (!bYZero && !bZZero)
	{
		if (bZPositive && Loc.Z > IgnoreStartLocation.Z)
		{
			if ((bYPositive && Loc.Y > IgnoreStartLocation.Y) || (bYNegative && Loc.Y < IgnoreStartLocation.Y))
			{
				return true;
			}
		}
		else if (bZNegative && Loc.Z < IgnoreStartLocation.Z)
		{
			if ((bYPositive && Loc.Y > IgnoreStartLocation.Y) || (bYNegative && Loc.Y < IgnoreStartLocation.Y))
			{
				return true;
			}
		}
		return false;
	}
	return false;
}
