// Fill out your copyright notice in the Description page of Project Settings.

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Sets default values for this actor's properties
	AGun_AK47();

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

	/** Start weapon recoil */
	UFUNCTION(BlueprintCallable, Category = "Recoil")
		void Recoil();

	/** Allows the player to fire again */
	void EnableFire();

	/** Begins applying recoil to the weapon */
	void StartRecoil();

	/** Initiates the recoil function */
	void RecoilRecovery();

	/** Interpolates the player back to their initial view vector */
	UFUNCTION()
		void HandleRecoveryProgress(float Value);

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

private:

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;

	/** The curve for vertical recoil  */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		UCurveFloat* VerticalRecoilCurve;

	/** The curve for horizontal recoil */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		UCurveFloat* HorizontalRecoilCurve;

	/** The curve for recovery */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		UCurveFloat* RecoveryCurve;

	/** The timeline for vertical recoil */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimeline VerticalRecoilTimeline;

	/** The timeline for horizontal recoil (generated from the curve) */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimeline HorizontalRecoilTimeline;

	/** The timeline for recover (set from the curve) */
	UPROPERTY(EditDefaultsOnly, Category = "Recoil")
		FTimeline RecoilRecoveryTimeline;

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
	int ShotsFired;

	/** A value to temporarily cache the player's control rotation so that we can return to it */
	FRotator ControlRotation;

	/** The offset to apply */
	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator PunchAngle;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator RecoverRotation;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		bool isRecovering;

	void InterpFinalRecoil(float DeltaSeconds);

	void InterpRecoil(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator RecoilRotation;

	UPROPERTY(VisibleAnywhere, Category = "Interaction")
		FRotator FinalRecoilRotation;

	/** Whether or not to trace on tick for BeatTrack */
		//bool ShouldTrace;
};
