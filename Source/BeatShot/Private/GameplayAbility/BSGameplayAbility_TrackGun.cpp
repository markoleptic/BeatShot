// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGameplayAbility_TrackGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameplayAbility/Tasks/BSAbilityTask_TickTrace.h"

UBSGameplayAbility_TrackGun::UBSGameplayAbility_TrackGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBSGameplayAbility_TrackGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* Component = CurrentActorInfo->AbilitySystemComponent.Get();
	
	OnTargetDataReadyCallbackDelegateHandle = Component->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(
		this, &ThisClass::OnTargetDataReadyCallback);

	TickTraceTask = UBSAbilityTask_TickTrace::SingleWeaponTrace(this, NAME_None, GetBSCharacterFromActorInfo(), FGameplayTagContainer(), TraceDistance, false);
	TickTraceTask->OnTickTraceHit.AddDynamic(this, &UBSGameplayAbility_TrackGun::OnTickTraceHitResultHit);
	TickTraceTask->ReadyForActivation();
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSGameplayAbility_TrackGun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (TickTraceTask)
	{
		TickTraceTask->EndTask();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBSGameplayAbility_TrackGun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (TickTraceTask)
	{
		TickTraceTask->EndTask();
	}
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UBSGameplayAbility_TrackGun::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
{
	UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
	if (MyAbilityComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
	{
		FScopedPredictionWindow ScopedPrediction(MyAbilityComponent);

		// Take ownership of the target data to make sure no callbacks into game code invalidate it out from under us
		const FGameplayAbilityTargetDataHandle LocalTargetDataHandle(MoveTemp(const_cast<FGameplayAbilityTargetDataHandle&>(InData)));

		const bool bShouldNotifyServer = CurrentActorInfo->IsLocallyControlled() && !CurrentActorInfo->IsNetAuthority();
		if (bShouldNotifyServer)
		{
			MyAbilityComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey(), LocalTargetDataHandle, ApplicationTag,
																  MyAbilityComponent->ScopedPredictionKey);
		}

		if (CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo))
		{
			// Let the blueprint do stuff like apply effects to the targets
			OnTargetDataReady(LocalTargetDataHandle);
		}
		else
		{
			K2_EndAbility();
		}
	}

	// We've processed the data
	MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());
}

void UBSGameplayAbility_TrackGun::OnTickTraceHitResultHit(const FHitResult& HitResult)
{
	FGameplayAbilityTargetDataHandle TargetData;
	FGameplayAbilityTargetData_SingleTargetHit* SingleTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData.Add(SingleTargetData);
	SingleTargetData->HitResult = HitResult;
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}
