// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "AbilitySystem/Tasks/BSAT_PerformWeaponTraceSingle.h"
#include "AbilitySystem/Abilities/BSGameplayAbility.h"
#include "Camera/CameraComponent.h"
#include "Character/BSCharacter.h"
#include "Character/BSRecoilComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/BSCollisionChannels.h"

UBSAT_PerformWeaponTraceSingle::UBSAT_PerformWeaponTraceSingle()
{
}

void UBSAT_PerformWeaponTraceSingle::Activate()
{
	Super::Activate();
	
	FHitResult HitResult;
	const bool bSuccess = LineTraceSingle(HitResult);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(bSuccess, HitResult);
	}
	
	EndTask();
}

void UBSAT_PerformWeaponTraceSingle::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(false, FHitResult());
	}
	// Will call EndTask
	Super::ExternalCancel();
}

UBSAT_PerformWeaponTraceSingle* UBSAT_PerformWeaponTraceSingle::PerformWeaponTraceSingle(
	UBSGameplayAbility* OwningAbility, const FName TaskInstanceName, const float TraceDistance)
{
	UBSAT_PerformWeaponTraceSingle* MyObj = NewAbilityTask<UBSAT_PerformWeaponTraceSingle>(OwningAbility,
		TaskInstanceName);
	MyObj->TraceDistance = TraceDistance;
	return MyObj;
}

bool UBSAT_PerformWeaponTraceSingle::LineTraceSingle(FHitResult& HitResult) const
{
	AActor* AvatarActor = Ability->GetAvatarActorFromActorInfo();
	if (!AvatarActor) return false;
	
	USceneComponent* RecoilComponent;
	FRotator CurrentRecoilRotation;
	if (const auto Character = Cast<ABSCharacter>(AvatarActor))
	{
		RecoilComponent = Character->GetRecoilComponent();
		CurrentRecoilRotation = Character->GetRecoilComponent()->GetCurrentRecoilRotation();
	}
	else if (const auto CharacterBase = Cast<ABSCharacterBase>(AvatarActor))
	{
		CurrentRecoilRotation = FRotator::ZeroRotator;
		RecoilComponent = CharacterBase->GetCamera();
	}
	else
	{
		return false;
	}
	
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(RecoilComponent->GetForwardVector(),
		CurrentRecoilRotation.Pitch, RecoilComponent->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw,
		RecoilComponent->GetUpVector());
	const FVector EndTrace = RecoilComponent->GetComponentLocation() + RotatedVector2 * TraceDistance;
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true);
	GetWorld()->LineTraceSingleByChannel(HitResult, RecoilComponent->GetComponentLocation(), EndTrace,
		BS_TraceChannel_Weapon, TraceParams);
	return true;
}
