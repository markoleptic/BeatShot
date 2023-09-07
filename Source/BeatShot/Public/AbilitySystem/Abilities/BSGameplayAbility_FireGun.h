// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameplayAbility.h"
#include "BSGameplayAbility_FireGun.generated.h"

class ABSCharacter;

/** GameplayAbility that provides the user with the ability to fire their gun */
UCLASS()
class BEATSHOT_API UBSGameplayAbility_FireGun : public UBSGameplayAbility
{
	GENERATED_BODY()

public:
	UBSGameplayAbility_FireGun();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	                             const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

protected:
	/** The firing animation to play */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;

	/** How far to trace forward from Character camera */
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float TraceDistance = 100000.f;

	/** Performs non-gameplay related tasks like muzzle flash, camera recoil, and decal spawning */
	UFUNCTION(BlueprintImplementableEvent)
	void OnTargetDataReady(const FGameplayAbilityTargetDataHandle& TargetData);

	/** Calls OnTargetDataReady */
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& InData, FGameplayTag ApplicationTag);

	/** Performs a WeaponTrace and calls OnTargetDataReadyCallback */
	UFUNCTION(BlueprintCallable)
	void StartTargeting();

	/** Performs single bullet trace */
	FHitResult SingleWeaponTrace() const;

private:
	FDelegateHandle OnTargetDataReadyCallbackDelegateHandle;
};
