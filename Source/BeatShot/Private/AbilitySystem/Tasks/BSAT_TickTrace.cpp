// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "AbilitySystem/Tasks/BSAT_TickTrace.h"
#include "AbilitySystemComponent.h"
#include "Character/BSCharacter.h"
#include "Character/BSRecoilComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/BSCollisionChannels.h"

UBSAT_TickTrace::UBSAT_TickTrace()
{
	bTickingTask = true;
}

void UBSAT_TickTrace::Activate()
{
	if (!Ability)
	{
		return;
	}
	
	EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBSAT_TickTrace::OnGameplayEvent));
	CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UBSAT_TickTrace::OnAbilityCancelled);
}

void UBSAT_TickTrace::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString UBSAT_TickTrace::GetDebugString() const
{
	return Super::GetDebugString();
}

void UBSAT_TickTrace::OnDestroy(bool AbilityEnded)
{
	if (AbilitySystemComponent.IsValid())
	{
		AbilitySystemComponent->RemoveGameplayEventTagContainerDelegate(EventTags, EventHandle);
	}
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
	}
	Super::OnDestroy(AbilityEnded);
}

void UBSAT_TickTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	PerformSingleWeaponTrace();
}

UBSAT_TickTrace* UBSAT_TickTrace::SingleWeaponTrace(UGameplayAbility* OwningAbility, const FName TaskInstanceName, ABSCharacter* Character, const FGameplayTagContainer EventTags,
	const float TraceDistance, const bool bStopWhenAbilityEnds)
{
	UBSAT_TickTrace* MyObj = NewAbilityTask<UBSAT_TickTrace>(OwningAbility, TaskInstanceName);
	MyObj->Character = Character;
	MyObj->EventTags = EventTags;
	MyObj->TraceDistance = TraceDistance;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	return MyObj;
}

void UBSAT_TickTrace::PerformSingleWeaponTrace()
{
	if (!Character)
	{
		return;
	}

	FHitResult HitResult;
	UBSRecoilComponent* RecoilComponent = Cast<UBSRecoilComponent>(Character->GetRecoilComponent());
	const FRotator CurrentRecoilRotation = RecoilComponent->GetCurrentRecoilRotation();
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(RecoilComponent->GetForwardVector(), CurrentRecoilRotation.Pitch, RecoilComponent->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw, RecoilComponent->GetUpVector());
	const FVector EndTrace = RecoilComponent->GetComponentLocation() + RotatedVector2 * TraceDistance;
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ Character);
	GetWorld()->LineTraceSingleByChannel(HitResult, RecoilComponent->GetComponentLocation(), EndTrace, BS_TraceChannel_Weapon, TraceParams);
	OnTickTraceHit.Broadcast(HitResult);
}

void UBSAT_TickTrace::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		EventReceived.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UBSAT_TickTrace::OnAbilityCancelled()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

