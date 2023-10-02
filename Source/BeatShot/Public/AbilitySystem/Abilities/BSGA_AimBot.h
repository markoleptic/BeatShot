// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameplayAbility.h"
#include "BSGA_AimBot.generated.h"

class UBSGA_FireGun;

UCLASS()
class BEATSHOT_API UBSGA_AimBot : public UBSGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBSGA_AimBot();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
								 const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
							bool bWasCancelled) override;

	UFUNCTION()
	void OnTargetAddedToQueue();

	/** Sets the IgnoreStartLocation and the bool variables associated with it */
	void SetIgnoreStartLocation(const FVector& In);

	/** The Aim Bot will ignore targets with a location greater than a positive value or less than a negative value */
	UPROPERTY(EditDefaultsOnly, Category="BeatShot")
	FVector IgnoreStartLocation = FVector::ZeroVector;

	/** The interpolation curve to use during AimToTarget task */
	UPROPERTY(EditDefaultsOnly, Category="BeatShot")
	UCurveFloat* SmoothingCurve;

	/** The ability to try and activate to destroy the target */
	UPROPERTY(EditDefaultsOnly, Category="BeatShot")
	TSubclassOf<UBSGameplayAbility> GA_FireGun;

private:
	UFUNCTION()
	void OnAimToTargetCancelled();

	UFUNCTION()
	void OnAimToTargetCompleted();

	bool TargetLocationIsInIgnoreRange(const FVector& Loc) const;
	
	bool bYPositive;
	bool bYNegative;
	bool bYZero;
	
	bool bZPositive;
	bool bZNegative;
	bool bZZero;
};
