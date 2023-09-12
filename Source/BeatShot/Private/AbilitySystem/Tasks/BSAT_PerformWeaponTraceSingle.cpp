// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "AbilitySystem/Tasks/BSAT_PerformWeaponTraceSingle.h"
#include "AbilitySystem/Abilities/BSGameplayAbility.h"
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
	
	if (const UBSGameplayAbility* GameplayAbility = Cast<UBSGameplayAbility>(Ability))
	{
		FHitResult HitResult;
		LineTraceSingle(GameplayAbility->GetBSCharacterFromActorInfo()->GetRecoilComponent(), HitResult);
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(HitResult);
		}
	}
	EndTask();
}

UBSAT_PerformWeaponTraceSingle* UBSAT_PerformWeaponTraceSingle::PerformWeaponTraceSingle(UBSGameplayAbility* OwningAbility, const FName TaskInstanceName, const float TraceDistance)
{
	UBSAT_PerformWeaponTraceSingle* MyObj = NewAbilityTask<UBSAT_PerformWeaponTraceSingle>(OwningAbility, TaskInstanceName);
	MyObj->TraceDistance = TraceDistance;
	return MyObj;
}

void UBSAT_PerformWeaponTraceSingle::LineTraceSingle(const UBSRecoilComponent* RecoilComponent, FHitResult& HitResult) const
{
	if (!RecoilComponent)
	{
		return;
	}
	
	const FRotator CurrentRecoilRotation = RecoilComponent->GetCurrentRecoilRotation();
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(RecoilComponent->GetForwardVector(), CurrentRecoilRotation.Pitch, RecoilComponent->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw, RecoilComponent->GetUpVector());
	const FVector EndTrace = RecoilComponent->GetComponentLocation() + RotatedVector2 * TraceDistance;
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true /*IgnoreActor=*/ );
	GetWorld()->LineTraceSingleByChannel(HitResult, RecoilComponent->GetComponentLocation(), EndTrace, BS_TraceChannel_Weapon, TraceParams);
}
