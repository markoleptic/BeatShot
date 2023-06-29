// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Components/TimelineComponent.h"
#include "BSAbilityTask_AimToTarget.generated.h"

class UBSAbilitySystemComponent;
class ATarget;
class UCurveFloat;

/** Delegate type used in the AimToTarget ability task */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAimToTargetDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

/** AimBot task */
UCLASS()
class BEATSHOT_API UBSAbilityTask_AimToTarget : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UBSAbilityTask_AimToTarget(const FObjectInitializer& ObjectInitializer);
	
	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;

	/** Broadcast when the task has finished aiming */
	UPROPERTY(BlueprintAssignable)
	FAimToTargetDelegate OnCompleted;

	/** Broadcast if the task was interrupted */
	UPROPERTY(BlueprintAssignable)
	FAimToTargetDelegate OnInterrupted;

	/** Broadcast if the ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
	FAimToTargetDelegate OnCancelled;

	/** Broadcast when a triggering gameplay events occurred */
	UPROPERTY(BlueprintAssignable)
	FAimToTargetDelegate EventReceived;

	/**
	 * Linearly interpolate towards the TargetToDestroy starting from Controller's current rotation when passed in, and wait for it to finish aiming. 
	 * If a gameplay event happens that matches EventTags (or EventTags is empty), the EventReceived delegate will fire with a tag and event data.
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param OwningAbility The owning ability of this task
	 * @param TaskInstanceName Task instance name
	 * @param Controller The controller of the player to apply the aim to
	 * @param RotationCurve The float curve to apply to the linear interpolation
	 * @param TargetToDestroy The target to aim to
	 * @param EventTags Any gameplay events matching this tag will activate the EventReceived callback. If empty, all events will trigger callback
	 * @param TimelinePlaybackRate Speed of the interpolation
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UBSAbilityTask_AimToTarget* AimBot(
			UGameplayAbility* OwningAbility,
			FName TaskInstanceName,
			AController* Controller,
			UCurveFloat* RotationCurve,
			ATarget* TargetToDestroy,
			FGameplayTagContainer EventTags,
			float TimelinePlaybackRate,
			bool bStopWhenAbilityEnds = true);

private:

	UPROPERTY()
	AController* Controller;
	
	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	UCurveFloat* RotationCurve;
	
	UPROPERTY()
	ATarget* Target;

	UPROPERTY()
	float TimelinePlaybackRate;
	
	UPROPERTY()
	bool bStopWhenAbilityEnds;
	
	/** Timeline for interpolating the rotation to aim at the target to destroy */
	UPROPERTY()
	FTimeline AimBotTimeline;

	/** The rotation at the start of the rotation interpolation */
	UPROPERTY()
	FRotator StartRotation;
	
	/** Float interpolation function bound to AimBotTimeline, which sets the rotation of the camera */
	UFUNCTION()
	void OnTimelineTick(const float Alpha) const;
	
	/** Executes when the AimBotTimeline completes */
	UFUNCTION()
	void OnTimelineCompleted();

	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const;

	void OnAbilityCancelled();

	void StopTimeline();
	
	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
