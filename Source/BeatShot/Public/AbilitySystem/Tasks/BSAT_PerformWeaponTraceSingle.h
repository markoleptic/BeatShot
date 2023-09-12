// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "BSAT_PerformWeaponTraceSingle.generated.h"

class UBSRecoilComponent;
class UBSGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPerformWeaponTraceDelegate, const FHitResult&, HitResult);

UCLASS()
class BEATSHOT_API UBSAT_PerformWeaponTraceSingle : public UAbilityTask
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UBSAT_PerformWeaponTraceSingle();

	virtual void Activate() override;

	/** Broadcast when the task has obtained a HitResult */
	UPROPERTY(BlueprintAssignable)
	FPerformWeaponTraceDelegate OnCompleted;

	/** Broadcast if the ability task was cancelled */
	UPROPERTY(BlueprintAssignable)
	FPerformWeaponTraceDelegate OnCancelled;

	/**
	 * If a BSCharacter is the found on the OwningAbility, a Line Trace is performed starting at the Recoil Component's transform. Traces forward by TraceDistance.
	 * @param OwningAbility The owning ability of this task
	 * @param TaskInstanceName Task instance name
	 * @param TraceDistance How far to trace the line forward
	 */
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UBSAT_PerformWeaponTraceSingle* PerformWeaponTraceSingle(UBSGameplayAbility* OwningAbility, const FName TaskInstanceName, const float TraceDistance);

	
	void LineTraceSingle(const UBSRecoilComponent* RecoilComponent, FHitResult& HitResult) const;

private:
	/** How far to trace forward from Character camera */
	float TraceDistance = 100000.f;
};
