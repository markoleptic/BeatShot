// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "GameplayAbility/BSGA_FireGun.h"
#include "AbilitySystemComponent.h"
#include "BSCharacter.h"
#include "BeatShot/BSGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameplayAbility/Tasks/BSAT_PlayMontageAndWaitForEvent.h"
#include "Kismet/KismetMathLibrary.h"

UBSGA_FireGun::UBSGA_FireGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	AbilityTags.AddTag(FBSGameplayTags::Get().Input_Fire);
	AbilityTags.AddTag(FBSGameplayTags::Get().Ability_Fire);
	ActivationOwnedTags.AddTag(FBSGameplayTags::Get().State_Firing);
	TargetBlockedTags.AddTag(FBSGameplayTags::Get().State_Firing);
}

void UBSGA_FireGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	UAnimMontage* MontageToPlay = FireHipMontage;
	// Play fire montage and wait for event telling us to spawn the projectile
	UBSAT_PlayMontageAndWaitForEvent* Task = UBSAT_PlayMontageAndWaitForEvent::PlayMontageAndWaitForEvent(this, NAME_None, MontageToPlay, FGameplayTagContainer(), 1.0f, NAME_None, false, 1.0f);
	//Task->OnBlendOut.AddDynamic(this, &UBSGA_FireGun::OnCompleted);
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
	// Montage told us to end the ability before the montage finished playing.
	// Montage was set to continue playing animation even after ability ends so this is okay.
	if (EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.EndAbility")))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	// Only spawn projectiles on the Server.
	// Predicting projectiles is an advanced topic not covered in this example.
	if (GetOwningActorFromActorInfo()->GetLocalRole() == ROLE_Authority && EventTag == FGameplayTag::RequestGameplayTag(FName("Event.Montage.SpawnProjectile")))
	{
		ABSCharacter* Character = Cast<ABSCharacter>(GetAvatarActorFromActorInfo());
		if (!Character)
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		const FVector MuzzleLoc = Character->Gun->MeshComp->GetSocketTransform("Muzzle").GetLocation();
		FVector StartTrace = Character->ShotDirection->GetComponentLocation();
		FVector ForwardVector = Character->ShotDirection->GetForwardVector();
		float AngleDeg = -Character->Gun->CurrentShotRecoilRotation.Pitch;
		FVector Axis = Character->ShotDirection->GetRightVector();
		FVector RotatedVector1 = UKismetMathLibrary::RotateAngleAxis(ForwardVector, AngleDeg, Axis);
		float AngleDeg2 = Character->Gun->CurrentShotRecoilRotation.Yaw;
		FVector Axis2 = Character->ShotDirection->GetUpVector();
		FVector RotatedVector2 = UKismetMathLibrary::RotateAngleAxis(RotatedVector1, AngleDeg2, Axis2);
		FVector EndTrace = StartTrace + RotatedVector2 * FVector(TraceDistance);
		
		FTransform Transform;
		if (FHitResult Hit; GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, ECC_Visibility, FCollisionQueryParams::DefaultQueryParam))
		{
			Transform = {UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, Hit.ImpactPoint), MuzzleLoc, FVector(1.f)};
			//DrawDebugLine(GetWorld(), StartTrace, Hit.ImpactPoint, FColor::Blue, false, 10.f);
			//DrawDebugLine(GetWorld(), MuzzleTransform.GetLocation(), Hit.ImpactPoint, FColor::Red, false, 10.f);
		}
		else
		{
			Transform = {UKismetMathLibrary::FindLookAtRotation(MuzzleLoc, EndTrace), MuzzleLoc, FVector(1.f)};
		}
		
		FGameplayEffectSpecHandle DamageEffectSpecHandle = MakeOutgoingGameplayEffectSpec(DamageGameplayEffect, GetAbilityLevel());
		
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
