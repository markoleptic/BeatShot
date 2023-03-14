// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGA_FireGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayAbility/Tasks/BSAT_PlayMontageAndWaitForEvent.h"

UBSGA_FireGun::UBSGA_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBSGA_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = FireHipMontage;
	// Play fire montage and wait for event telling us to spawn the projectile.
	// Event.Montage.SpawnProjectile is broadcast in the first frame of the montage.
	UBSAT_PlayMontageAndWaitForEvent* Task = UBSAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UBSGA_FireGun::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UBSGA_FireGun::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UBSGA_FireGun::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UBSGA_FireGun::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UBSGA_FireGun::EventReceived);
	// ReadyForActivation() is how you activate the AbilityTask in C++. Blueprint has magic from K2Node_LatentGameplayTaskCall that will automatically call ReadyForActivation().
	Task->ReadyForActivation();
}

void UBSGA_FireGun::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBSGA_FireGun::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Display, TEXT("EVENT Compltetd"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBSGA_FireGun::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	UE_LOG(LogTemp, Display, TEXT("Event Tag: %s"), *EventTag.ToString());
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		ABSCharacter* Character = Cast<ABSCharacter>(GetAvatarActorFromActorInfo());
		if (!Character)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		const FTransform Transform = Character->GetGun()->GetTraceTransform();
		const FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
		
		// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
		DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FBSGameplayTags::Get().Data_Damage, Damage);
		
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
		AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform, GetOwningActorFromActorInfo(),
			Character, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
		Projectile->ProjectileMovement->InitialSpeed = ProjectileSpeed;
		Projectile->ProjectileMovement->MaxSpeed = ProjectileSpeed;
		Projectile->FinishSpawning(Transform, true);
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("ROLE_Authority or !Event.Montage.SpawnProjectile"));
	}
}
