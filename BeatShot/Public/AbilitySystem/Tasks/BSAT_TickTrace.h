// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BSAT_TickTrace.generated.h"

class UBSAbilitySystemComponent;
class ABSCharacter;

/** Delegate type used in the AimToTarget ability task */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FTickTraceDelegate, FGameplayTag, EventTag, FGameplayEventData, EventData);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTickTraceHit, const FHitResult&, HitResult);

/** Task used to trace a line from the gun to where the owner is facing on tick */
UCLASS()
class BEATSHOT_API UBSAT_TickTrace : public UAbilityTask
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UBSAT_TickTrace();

	virtual void Activate() override;
	virtual void ExternalCancel() override;
	virtual FString GetDebugString() const override;
	virtual void OnDestroy(bool AbilityEnded) override;
	virtual void TickTask(float DeltaTime) override;

	/** How far to trace forward from Character camera */
	float TraceDistance = 100000.f;

	UPROPERTY(BlueprintAssignable)
	FOnTickTraceHit OnTickTraceHit;

	/** Broadcast when the task has finished aiming */
	UPROPERTY(BlueprintAssignable)
	FTickTraceDelegate OnCompleted;

	/** Broadcast if the task was interrupted */
	UPROPERTY(BlueprintAssignable)
	FTickTraceDelegate OnInterrupted;

	/** Broadcast if the ability task was explicitly cancelled by another ability */
	UPROPERTY(BlueprintAssignable)
	FTickTraceDelegate OnCancelled;

	/** Broadcast when a triggering gameplay events occurred */
	UPROPERTY(BlueprintAssignable)
	FTickTraceDelegate EventReceived;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UBSAT_TickTrace* SingleWeaponTrace(UGameplayAbility* OwningAbility, const FName TaskInstanceName,
		ABSCharacter* Character, const FGameplayTagContainer EventTags, const float TraceDistance,
		const bool bStopWhenAbilityEnds);

private:
	UPROPERTY()
	ABSCharacter* Character;

	UPROPERTY()
	FGameplayTagContainer EventTags;

	UPROPERTY()
	bool bStopWhenAbilityEnds;

	void PerformSingleWeaponTrace();

	void OnGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) const;

	void OnAbilityCancelled();

	FDelegateHandle CancelledHandle;
	FDelegateHandle EventHandle;
};
