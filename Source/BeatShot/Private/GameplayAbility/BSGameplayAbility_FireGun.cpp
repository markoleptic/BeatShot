// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGameplayAbility_FireGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "GameplayCueNotifyTypes.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "BeatShot/BSGameplayTags.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayAbility/Tasks/BSAbilityTask_MontageEventWait.h"

UBSGameplayAbility_FireGun::UBSGameplayAbility_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBSGameplayAbility_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}
	
	PlayMontage();

	if (GetBSCharacterFromActorInfo())
	{
		GetBSCharacterFromActorInfo()->GetGun()->StartFire();
	}

	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority)
	{
		// Spawn the bullet projectile
		SpawnProjectile(GetBSCharacterFromActorInfo());

		// Don't really know where to properly put this but this executes the muzzle flash Niagara effect (GC_MuzzleFlash)
		K2_ExecuteGameplayCue(FBSGameplayTags::Get().GameplayCue_MuzzleFlash, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	}
	
	// Currently doesn't work or just gets overriden by task repeating over and over
	UAbilityTask_WaitInputRelease* ReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
	ReleaseTask->OnRelease.AddDynamic(this, &UBSGameplayAbility_FireGun::OnReleased);
	ReleaseTask->ReadyForActivation();
}

void UBSGameplayAbility_FireGun::OnCancelled(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBSGameplayAbility_FireGun::OnCompleted(FGameplayTag EventTag, FGameplayEventData EventData)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UBSGameplayAbility_FireGun::EventReceived(FGameplayTag EventTag, FGameplayEventData EventData)
{
	// Only spawn projectiles on the Server.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FBSGameplayTags::Get().Event_Montage_SpawnProjectile)
	{
		// // Spawn the bullet projectile
		// SpawnProjectile(GetBSCharacterFromActorInfo());
		//
		// // Don't really know where to properly put this but this executes the muzzle flash Niagara effect (GC_MuzzleFlash)
		// K2_ExecuteGameplayCue(FBSGameplayTags::Get().GameplayCue_MuzzleFlash, MakeEffectContext(CurrentSpecHandle, CurrentActorInfo));
	}
}

void UBSGameplayAbility_FireGun::OnReleased(float TimeHeld)
{
	UE_LOG(LogTemp, Display, TEXT("Released: %f"), FPlatformTime::Seconds());
	if (const ABSCharacter* Character = GetBSCharacterFromActorInfo())
	{
		Character->GetGun()->StopFire();
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UBSGameplayAbility_FireGun::SpawnProjectile(ABSCharacter* ActorCharacter) const
{
	const FTransform Transform = ActorCharacter->GetGun()->GetTraceTransform();
	const FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
	
	// Pass the damage to the Damage Execution Calculation through a SetByCaller value on the GameplayEffectSpec
	DamageEffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FBSGameplayTags::Get().Data_Damage, Damage);
		
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		
	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(ProjectileClass, Transform, GetOwningActorFromActorInfo(),
		ActorCharacter, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	Projectile->DamageEffectSpecHandle = DamageEffectSpecHandle;
	Projectile->ProjectileMovement->InitialSpeed = ProjectileSpeed;
	Projectile->ProjectileMovement->MaxSpeed = ProjectileSpeed;
	Projectile->bSpawnDecalOnHit = ActorCharacter->LoadPlayerSettings().Game.bShowBulletDecals;
	Projectile->FinishSpawning(Transform, true);
}

void UBSGameplayAbility_FireGun::PlayMontage()
{
	UBSAbilityTask_MontageEventWait* Task = UBSAbilityTask_MontageEventWait::PlayMontageAndWaitForEvent(this, NAME_None, FireHipMontage, FGameplayTagContainer(), 1.0f,
	NAME_None, false, 1.0f);
	Task->OnBlendOut.AddDynamic(this, &UBSGameplayAbility_FireGun::OnCompleted);
	Task->OnCompleted.AddDynamic(this, &UBSGameplayAbility_FireGun::OnCompleted);
	Task->OnInterrupted.AddDynamic(this, &UBSGameplayAbility_FireGun::OnCancelled);
	Task->OnCancelled.AddDynamic(this, &UBSGameplayAbility_FireGun::OnCancelled);
	Task->EventReceived.AddDynamic(this, &UBSGameplayAbility_FireGun::EventReceived);
	// Activate the AbilityTask
	Task->ReadyForActivation();
}
