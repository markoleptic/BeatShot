// Fill out your copyright notice in the Description page of Project Settings.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Gun_AK47.generated.h"

class UCapsuleComponent;
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
		USceneComponent* MuzzleLocationComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
		UCurveVector* RecoilVectorCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
		UCurveFloat* KickbackCurve;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInterface* BulletDecalMaterial;

	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
		UMaterialInstanceDynamic* BulletDecalInstance;

	/** Whether or not to recoil the gun */
	bool bShouldRecoil;

	/** Whether or not to fire the gun continuously */
	bool bAutomaticFire;

private:

	/** Update the screen-shake-like camera recoil */
	void UpdateCameraKickback(float DeltaTime);

	/** Update CurrentShotRecoils to vector at the current time into the spray */
	void UpdateRecoilPattern(FVector Output);

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	/** The timeline for recover (set from the curve) */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimeline RecoilTimeline;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
		float TraceDistance;

	/** The timer that handles automatic fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gun State")
		FTimerHandle ShotDelay;

	/** The timer that is used when we need to wait for an animation to finish before being able to fire again */
	UPROPERTY(EditDefaultsOnly, Category = "Gun State")
		FTimerHandle AnimationWaitDelay;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UNiagaraSystem* NS_MuzzleFlash;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
		UAnimMontage* FireAnimation;

	/** Used in recoil to make sure the first shot has properly applied recoil */
	UPROPERTY(VisibleAnywhere, Category = "Gun State")
		int ShotsFired;

	UPROPERTY(VisibleAnywhere, Category = "Gun State")
		bool bIsFiring;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
		FRotator StartRotation;

	UPROPERTY(VisibleAnywhere, Category = "Recoil")
		FRotator EndRotation;

	/** The current rotation value pulled from the vector curve, added to control rotation during fire */
	UPROPERTY(VisibleAnywhere, Category = "Recoil")
		FRotator CurrentShotRecoilRotation;

	/** Same as CurrentShotRecoilRotation but applied at half the scale */
	UPROPERTY(VisibleAnywhere, Category = "Recoil")
		FRotator CurrentShotCameraRecoilRotation;

	/** Offset the muzzle flash a fixed amount */
	FVector MuzzleFlashOffset = FVector(5, 0, 0);

	UPROPERTY(VisibleAnywhere, Category = "References")
		ADefaultCharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = "References")
		UDefaultGameInstance* GI;

	UPROPERTY(VisibleAnywhere, Category = "References")
		ADefaultPlayerController* PlayerController;

	/** Determines if the player can fire */
	bool bCanFire = true;

	/** Whether or not to trace on tick for BeatTrack */
	bool bShouldTrace;

	FActorSpawnParameters ProjectileSpawnParams;

	float RecoilAlpha;

	float RecoilAngle;

	bool Recoil;

	float RecoilDuration = 0.2f;
};
