// Fill out your copyright notice in the Description page of Project Settings.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Gun_AK47.generated.h"

class ASphereTarget;
class ADefaultPlayerController;
class UNiagaraSystem;
class UDefaultGameInstance;
class ADefaultCharacter;
class USkeletalMeshComponent;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFired);

UCLASS()
class BEATSHOT_API AGun_AK47 : public AActor
{
	GENERATED_BODY()

	/** Sets default values for this actor's properties */
	AGun_AK47();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
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

	/** Returns whether the weapon can fire or not */
	bool CanFire() const { return bCanFire; }

	/** Update the weapon's ability to fire
	 *	@param bNewFire The new state of the weapon's ability to fire
	 */
	void SetCanFire(const bool bNewFire) { bCanFire = bNewFire; }

	/** GameModeActorBase binds to this delegate to keep track of number of shots fired */
	UPROPERTY()
	FOnShotFired OnShotFired;

	/** The skeletal mesh of the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	/** The location of the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* MuzzleLocationComp;

	/** Vector curve that implements vertical and horizontal recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	UCurveVector* RecoilVectorCurve;

	/** Float curve that implements a screen kickback effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	UCurveFloat* KickbackCurve;

	/** The material interface for the bullet decal */
	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInterface* BulletDecalMaterial;

	/** The MID for the bullet decal */
	UPROPERTY(EditAnywhere, Category = "Materials", BlueprintReadWrite)
	UMaterialInstanceDynamic* BulletDecalInstance;

	/** Reference to the tracking target that was obtained from FOnBeatTrackTargetSpawned,
	*   so we can change color of target based on the line trace */
	UPROPERTY()
	ASphereTarget* TrackingTarget;

	/** Whether or not to recoil the gun */
	bool bShouldRecoil;

	/** Whether or not to fire the gun continuously */
	bool bAutomaticFire;

	/** Whether or not to trace on tick for BeatTrack */
	bool bShouldTrace;

	/** Whether or not to trace on tick for BeatTrack */
	bool bShowBulletDecals;

private:
	/** Controls gun recoil, camera recoil, and kickback inside of OnTick */
	void UpdateRecoilAndKickback(float DeltaTime);

	/** Update the screen-shake-like camera recoil */
	void UpdateCameraKickback(float DeltaTime);

	/** Changes the color of the target based on a line trace from the gun to player's crosshair */
	void TraceForward() const;

	/** Update CurrentShotRecoils to vector at the current time into the spray.
	 *  Bound to RecoilTimeline, which corresponds to the RecoilVectorCurve */
	UFUNCTION()
	void UpdateRecoilPattern(FVector Output);

	/** Plays the muzzle flash Niagara effect */
	void ShowMuzzleFlash(const FRotator& MuzzleRotation) const;
	/** Plays the gunshot sound (no gunshot sound atm) */
	void PlayGunshotSound() const;
	/** Spawns a bullet decal at the Hit result location */
	void ShotBulletDecal(const FHitResult& Hit) const;
	/** Plays the recoil animation located in DefaultCharacter */
	void PlayRecoilAnimation() const;

	/** The timeline set from RecoilVectorCurve */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
	FTimeline RecoilTimeline;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<class AProjectile> ProjectileClass;

	/** How far to trace forward to look for a collision */
	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	float TraceDistance;

	/** The timer that handles automatic fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gun State")
	FTimerHandle ShotDelay;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* FireSound;

	/** Muzzle flash effect */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UNiagaraSystem* NS_MuzzleFlash;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* FireAnimation;

	/** Used in recoil to make sure the first shot has properly applied recoil */
	UPROPERTY(VisibleAnywhere, Category = "Gun State")
	int32 ShotsFired;

	/** Whether or not the player is holding down left click */
	UPROPERTY(VisibleAnywhere, Category = "Gun State")
	bool bIsFiring;

	/** The current rotation value pulled from the vector curve (only the x and y values are used),
	 *  added to control rotation during fire */
	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FRotator CurrentShotRecoilRotation;

	/** Same as CurrentShotRecoilRotation but applied at half the scale */
	UPROPERTY(VisibleAnywhere, Category = "Recoil")
	FRotator CurrentShotCameraRecoilRotation;

	/** Offset the muzzle flash a fixed amount */
	FVector MuzzleFlashOffset = FVector(5, 0, 0);

	/** Reference to owning character */
	UPROPERTY(VisibleAnywhere, Category = "References")
	ADefaultCharacter* Character;

	/** Reference to player controller */
	UPROPERTY(VisibleAnywhere, Category = "References")
	ADefaultPlayerController* PlayerController;

	/** The spawn parameters for the bullet */
	FActorSpawnParameters ProjectileSpawnParams;

	/** Determines if the player can fire */
	bool bCanFire;

	/** The accumulated delta seconds since the last camera kickback duration */
	float KickbackAlpha;

	/** The value pulled from the KickbackCurve at time KickbackAlpha divided by KickbackDuration */
	float KickbackAngle;

	/** Whether or not to play the kickback animation */
	bool bShouldKickback;

	/** The duration of each kickback animation */
	float KickbackDuration = 0.2f;
};
