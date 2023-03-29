// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGameplayAbility_TrackGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/BSCollisionChannels.h"

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
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UBSGameplayAbility_TrackGun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
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

void UBSGameplayAbility_TrackGun::StartTargeting()
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

FHitResult UBSGameplayAbility_TrackGun::SingleWeaponTrace() const
{
	FHitResult HitResult;
	const UCameraComponent* Camera = GetBSCharacterFromActorInfo()->GetCamera();
	const FRotator CurrentRecoilRotation = GetBSCharacterFromActorInfo()->GetGun()->GetCurrentRecoilRotation();
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(Camera->GetForwardVector(), CurrentRecoilRotation.Pitch, Camera->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw, Camera->GetUpVector());
	const FVector EndTrace = Camera->GetComponentLocation() + RotatedVector2 * FVector(TraceDistance);
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ GetAvatarActorFromActorInfo());
	GetWorld()->LineTraceSingleByChannel(HitResult, Camera->GetComponentLocation(), EndTrace, BS_TraceChannel_Weapon, TraceParams);
	return HitResult;
}
