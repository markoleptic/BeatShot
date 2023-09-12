// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "AbilitySystem/Tasks/BSAT_AimToTarget.h"
#include "AbilitySystemGlobals.h"
#include "Target/Target.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBSAT_AimToTarget::UBSAT_AimToTarget()
{
	bTickingTask = true;
}

void UBSAT_AimToTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);
	AimBotTimeline.TickTimeline(DeltaTime);
}

void UBSAT_AimToTarget::Activate()
{
	if (!Ability)
	{
		return;
	}
	
	FOnTimelineFloat OnTimelineFloat;
	OnTimelineFloat.BindUFunction(this, FName("OnTimelineTick"));
	FOnTimelineEvent OnTimelineEvent;
	OnTimelineEvent.BindUFunction(this, FName("OnTimelineCompleted"));
	AimBotTimeline.SetTimelineFinishedFunc(OnTimelineEvent);
	AimBotTimeline.AddInterpFloat(RotationCurve, OnTimelineFloat);
	
	EventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(EventTags, FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UBSAT_AimToTarget::OnGameplayEvent));
	CancelledHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UBSAT_AimToTarget::OnAbilityCancelled);
	
	AimBotTimeline.SetPlayRate(TimelinePlaybackRate);
	AimBotTimeline.PlayFromStart();
	
	SetWaitingOnAvatar();
}

void UBSAT_AimToTarget::ExternalCancel()
{
	OnAbilityCancelled();
	Super::ExternalCancel();
}

FString UBSAT_AimToTarget::GetDebugString() const
{
	return Super::GetDebugString();
}

void UBSAT_AimToTarget::OnDestroy(bool AbilityEnded)
{
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(CancelledHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopTimeline();
		}
	}
	Super::OnDestroy(AbilityEnded);
}

UBSAT_AimToTarget* UBSAT_AimToTarget::AimBot(UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			AController* Controller,
			UCurveFloat* RotationCurve,
			ATarget* TargetToDestroy,
			FGameplayTagContainer EventTags,
			float TimelinePlaybackRate,
			bool bStopWhenAbilityEnds)
{
	UBSAT_AimToTarget* MyObj = NewAbilityTask<UBSAT_AimToTarget>(OwningAbility, TaskInstanceName);
	MyObj->Controller = Controller;
	MyObj->EventTags = EventTags;
	MyObj->Target = TargetToDestroy;
	MyObj->TimelinePlaybackRate = TimelinePlaybackRate;
	MyObj->RotationCurve = RotationCurve;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartRotation = Controller->GetControlRotation();
	MyObj->StartRotation.Normalize();
	
	return MyObj;
}

void UBSAT_AimToTarget::OnAbilityCancelled()
{
	/*StopTimeline();
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(FGameplayTag(), FGameplayEventData());
	}*/
}

void UBSAT_AimToTarget::OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		EventReceived.Broadcast(FGameplayTag(), FGameplayEventData());
	}
}

void UBSAT_AimToTarget::OnTimelineTick(const float Alpha) const
{
	FVector Loc;
	FRotator Rot;
	Controller->GetActorEyesViewPoint(Loc, Rot);
	Controller->SetControlRotation(UKismetMathLibrary::RLerp(StartRotation, UKismetMathLibrary::FindLookAtRotation(Loc, Target->GetActorLocation()), Alpha, true));
}

void UBSAT_AimToTarget::OnTimelineCompleted()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCompleted.Broadcast(FGameplayTag(), FGameplayEventData());
	}
	EndTask();
}

void UBSAT_AimToTarget::StopTimeline()
{
	AimBotTimeline.Stop();
}
