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

void UBSGA_AimBot::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	SetIgnoreStartLocation(IgnoreStartLocation);
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSGA_AimBot::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBSGA_AimBot::OnTargetActivated(ATarget* SpawnedTarget)
{
	ActiveTargets_AimBot.Enqueue(SpawnedTarget);
	if (ActiveTasks.IsEmpty())
	{
		CheckTargetQueue();
	}
}

ATarget* UBSGA_AimBot::PeekActiveTargets()
{
	ATarget* Target;
	while (!ActiveTargets_AimBot.IsEmpty())
	{
		ActiveTargets_AimBot.Peek(Target);
		if (IsValid(Target))
		{
			return Target;
		}
		PopActiveTargets();
	}
	return nullptr;
}

void UBSGA_AimBot::PopActiveTargets()
{
	ActiveTargets_AimBot.Pop();
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

void UBSGA_AimBot::CheckTargetQueue()
{
	ATarget* ActiveTarget = PeekActiveTargets();
	if (!ActiveTarget)
	{
		return;
	}
	if (TargetLocationIsInIgnoreRange(ActiveTarget->GetActorLocation()))
	{
		PopActiveTargets();
		return;
	}

	UBSAT_AimToTarget* AimToTarget = UBSAT_AimToTarget::AimToTarget(this, FName(), SmoothingCurve, ActiveTarget,
		1.f / ActiveTarget->GetSpawnBeatDelay());
	AimToTarget->OnCancelled.AddDynamic(this, &ThisClass::OnAimToTargetCancelled);
	AimToTarget->OnCompleted.AddDynamic(this, &ThisClass::OnAimToTargetCompleted);
	AimToTarget->ReadyForActivation();
}

void UBSGA_AimBot::OnAimToTargetCancelled()
{
	UBSAbilitySystemComponent* ASC = GetBSAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	PopActiveTargets();
	ASC->TryActivateAbilityByClass(GA_FireGun);
	CheckTargetQueue();
}

void UBSGA_AimBot::OnAimToTargetCompleted()
{
	UBSAbilitySystemComponent* ASC = GetBSAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		return;
	}

	PopActiveTargets();
	ASC->TryActivateAbilityByClass(GA_FireGun);
	CheckTargetQueue();
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
