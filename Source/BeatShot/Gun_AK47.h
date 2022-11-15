// Fill out your copyright notice in the Description page of Project Settings.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
//#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Marketplace/FPSCore/Source/FPSCore/Public/WeaponBase.h"
#include "GameFramework/Actor.h"
#include "Gun_AK47.generated.h"

class ADefaultPlayerController;
class UNiagaraSystem;
class UDefaultGameInstance;
class ADefaultCharacter;
class AWeaponBase;
class USkeletalMeshComponent;
class USkeletalMesh;
class UStaticMesh;
class UAnimMontage;
class UAnimationAsset;
class UAnimSequence;
class UNiagaraSystem;
class UBlendSpace;
class USoundCue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFired);

UCLASS()
class BEATSHOT_API AGun_AK47 : public AActor
{
	GENERATED_BODY()

	// Sets default values for this actor's properties
	AGun_AK47();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:	

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Fire();

	/** Starts firing the gun (sets the timer for automatic fire) */
	void StartFire();

	/** Stops the timer that allows for automatic fire */
	void StopFire();

	/** Allows the player to fire again */
	void EnableFire();

	/** Whether the weapon can fire or not */
	bool CanFire() const { return bCanFire; }

	/** Update the weapon's ability to fire
	 *	@param bNewFire The new state of the weapon's ability to fire
	 */
	void SetCanFire(const bool bNewFire) { bCanFire = bNewFire; }

	UPROPERTY(BlueprintCallable)
		FOnShotFired OnShotFired;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* MeshComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USceneComponent* MuzzleLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCurveVector* RecoilVectorCurve;

private:

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	/** The timeline for recover (set from the curve) */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimeline RecoilTimeline;

	/** The timer that handles automatic fire */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimerHandle ShotDelay;

	/** The timer that is used when we need to wait for an animation to finish before being able to fire again */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimerHandle AnimationWaitDelay;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* FireSound;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		ADefaultPlayerController* PlayerController;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UNiagaraSystem* NS_MuzzleFlash;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, Category = "References")
		ADefaultCharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = "References")
		UDefaultGameInstance* GI;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		float TraceDistance;

	/** Determines if the player can fire */
	bool bCanFire = true;

	/** Keeping track of whether we should do a recoil recovery after finishing firing or not */
	bool bShouldRecover;

	/** Used in recoil to make sure the first shot has properly applied recoil */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		int ShotsFired;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator RecoverRotation;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		bool bIsRecovering;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		bool bIsFiring;

	UFUNCTION(BlueprintCallable, Category = "Recoil")
		void UpdateKickback(FVector Output);

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator StartRotation;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator EndRotation;

	/** The total recoil rotation being accumulated during the time that bIsFiring is true */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator TotalRotationFromRecoil;

	/** The current amount recoil rotation recovered during the time that bIsRecovering was true */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator TotalRecoilRecovered;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator LastRecoilRotation;

	/** The sum of the recoil rotation accumulated during the time that bIsFiring was true */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator RecoilToRecover;

	/** The current rotation value pulled from the vector curve, added to control rotation during fire */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator CurrentShotRecoilRotation;

	bool AutoFiring;

	/** Whether or not to trace on tick for BeatTrack */
		//bool ShouldTrace;
};
