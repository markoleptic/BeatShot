// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/Tasks/BSAbilityTask_TickTrace.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Physics/BSCollisionChannels.h"

UBSAbilityTask_TickTrace::UBSAbilityTask_TickTrace(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
}

void UBSAbilityTask_TickTrace::Activate()
{
	if (!Ability)
	{
		return;
	}
	
	EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBSAbilityTask_TickTrace::OnGameplayEvent));
	CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UBSAbilityTask_TickTrace::OnAbilityCancelled);
	Super::Activate();
}

void UBSAbilityTask_TickTrace::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString UBSAbilityTask_TickTrace::GetDebugString() const
{
	return Super::GetDebugString();
}

void UBSAbilityTask_TickTrace::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
	}
	Super::OnDestroy(AbilityEnded);
}

void UBSAbilityTask_TickTrace::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	PerformSingleWeaponTrace();
}

UBSAbilityTask_TickTrace* UBSAbilityTask_TickTrace::SingleWeaponTrace(UGameplayAbility* OwningAbility, const FName TaskInstanceName, ABSCharacter* Character, const FGameplayTagContainer EventTags,
	const float TraceDistance, const bool bStopWhenAbilityEnds)
{
	UBSAbilityTask_TickTrace* MyObj = NewAbilityTask<UBSAbilityTask_TickTrace>(OwningAbility, TaskInstanceName);
	MyObj->Character = Character;
	MyObj->EventTags = EventTags;
	MyObj->TraceDistance = TraceDistance;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	return MyObj;
}

void UBSAbilityTask_TickTrace::PerformSingleWeaponTrace()
{
	if (!Character)
	{
		return;
	}
	FHitResult HitResult;
	const UCameraComponent* Camera = Character->GetCamera();
	const FRotator CurrentRecoilRotation = Character->GetGun()->GetCurrentRecoilRotation();
	const FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(Camera->GetForwardVector(), CurrentRecoilRotation.Pitch, Camera->GetRightVector());
	const FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, CurrentRecoilRotation.Yaw, Camera->GetUpVector());
	const FVector EndTrace = Camera->GetComponentLocation() + RotatedVector2 * FVector(TraceDistance);
	const FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), /*bTraceComplex=*/ true, /*IgnoreActor=*/ Character);
	GetWorld()->LineTraceSingleByChannel(HitResult, Camera->GetComponentLocation(), EndTrace, BS_TraceChannel_Weapon, TraceParams);
	OnTickTraceHit.Broadcast(HitResult);
}

void UBSAbilityTask_TickTrace::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		EventReceived.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UBSAbilityTask_TickTrace::OnAbilityCancelled()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

