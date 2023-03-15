// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSGameplayAbility.h"
#include "Projectile.h"
#include "BSGameplayAbility_FireGun.generated.h"


class ABSCharacter;
UCLASS()
class BEATSHOT_API UBSGameplayAbility_FireGun : public UBSGameplayAbility
{
	GENERATED_BODY()
	
public:
	UBSGameplayAbility_FireGun();

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* FireHipMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TSubclassOf<UGameplayEffect> DamageGameplayEffect;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Damage = 666.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float ProjectileSpeed = 250000.f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float TraceDistance = 100000.f;

	UFUNCTION()
	virtual void OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	virtual void EventReceived(FGameplayTag EventTag, FGameplayEventData EventData);

	UFUNCTION()
	void OnReleased(float TimeHeld);

	void SpawnProjectile(ABSCharacter* ActorCharacter) const;

	void PlayMontage();
};
