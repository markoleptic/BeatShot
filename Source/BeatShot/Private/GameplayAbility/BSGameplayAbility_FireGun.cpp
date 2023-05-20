// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGameplayAbility_FireGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BSRecoilComponent.h"
#include "Physics/BSCollisionChannels.h"
#include "Kismet/KismetMathLibrary.h"

UBSGameplayAbility_FireGun::UBSGameplayAbility_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBSGameplayAbility_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                 const FGameplayEventData* TriggerEventData)
{
	UAbilitySystemComponent* Component = CurrentActorInfo->AbilitySystemComponent.Get();
	OnTargetDataReadyCallbackDelegateHandle = Component->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).AddUObject(
		this, &ThisClass::OnTargetDataReadyCallback);

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSGameplayAbility_FireGun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}

		UAbilitySystemComponent* MyAbilityComponent = CurrentActorInfo->AbilitySystemComponent.Get();
		check(MyAbilityComponent);

		// When ability ends, consume target data and remove delegate
		MyAbilityComponent->AbilityTargetDataSetDelegate(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey()).Remove(OnTargetDataReadyCallbackDelegateHandle);
		MyAbilityComponent->ConsumeClientReplicatedTargetData(CurrentSpecHandle, CurrentActivationInfo.GetActivationPredictionKey());

		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UBSGameplayAbility_FireGun::OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag)
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

void UBSGameplayAbility_FireGun::StartTargeting()
{
	UAbilitySystemComponent* Component = CurrentActorInfo->AbilitySystemComponent.Get();
	FScopedPredictionWindow ScopedPrediction(Component, CurrentActivationInfo.GetActivationPredictionKey());
	FHitResult HitResult = SingleWeaponTrace();
	FGameplayAbilityTargetDataHandle TargetData;
	FGameplayAbilityTargetData_SingleTargetHit* SingleTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
	TargetData.Add(SingleTargetData);
	SingleTargetData->HitResult = HitResult;
	OnTargetDataReadyCallback(TargetData, FGameplayTag());
}

FHitResult UBSGameplayAbility_FireGun::SingleWeaponTrace() const
{
	FHitResult HitResult;
	UBSRecoilComponent* RecoilComponent = Cast<UBSRecoilComponent>(GetBSCharacterFromActorInfo()->GetComponentByClass(UBSRecoilComponent::StaticClass()));
	const FRotator CurrentRecoilRotation = RecoilComponent->GetCurrentRecoilRotation();
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(RecoilComponent->GetForwardVector(), CurrentRecoilRotation.Pitch, RecoilComponent->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw, RecoilComponent->GetUpVector());
	const FVector EndTrace = RecoilComponent->GetComponentLocation() + RotatedVector2 * FVector(TraceDistance);
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetAvatarActorFromActorInfo());
	GetWorld()->LineTraceSingleByChannel(HitResult, RecoilComponent->GetComponentLocation(), EndTrace, BS_TraceChannel_Weapon, TraceParams);
	return HitResult;
}
