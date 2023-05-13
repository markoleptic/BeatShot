// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "BeatShot/Beatshot.h"
#include "SaveLoadInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "BSGun.generated.h"

class ASphereTarget;
class ABSPlayerController;
class UNiagaraSystem;
class UBSGameInstance;
class ABSCharacter;
class USkeletalMeshComponent;
class UAnimMontage;

/** The base gun used in this game */
UCLASS()
class BEATSHOT_API ABSGun : public AActor, public IGameplayTagAssetInterface, public ISaveLoadInterface
{
	GENERATED_BODY()

	/** Sets default values for this actor's properties */
	ABSGun();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** ~IGameplayTagAssetInterface begin */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/** ~IGameplayTagAssetInterface end */

	/** ~ISaveLoadInterface begin */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	/** ~ISaveLoadInterface end */

public:

	/** Increments ShotsFired, executes OnShotFired */
	UFUNCTION(BlueprintCallable)
	virtual void Fire();

	/** Stops the timer that allows for automatic fire */
	UFUNCTION(BlueprintCallable)
	virtual void StopFire();

	/** Returns the current spread rotation (Pitch and Yaw at the current time). Used by FireGun ability */
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentRecoilRotation() const;

	/** Returns the location of the muzzle */
	UFUNCTION(BlueprintCallable)
	virtual FVector GetMuzzleLocation() const;

	/** Returns whether or not the gun should recoil, based on GameplayTags */
	UFUNCTION(BlueprintCallable)
	bool ShouldRecoil() const;

	/** Returns whether or not to show bullet decals, based on GameplayTags */
	UFUNCTION(BlueprintCallable)
	bool ShouldShowDecals() const;

	/** Returns whether or not to show bullet tracers, based on GameplayTags */
	UFUNCTION(BlueprintCallable)
	bool ShouldShowTracers() const;

	/** Returns whether or not the gun is auto fire rate, based on GameplayTags */
	UFUNCTION(BlueprintCallable)
	bool IsAutoFireRate() const;

	/** Returns whether the weapon can fire or not */
	UFUNCTION(BlueprintCallable)
	bool CanFire() const { return bCanFire; }

	/** Returns whether or not the gun is currently firing (input is being held down) */
	UFUNCTION(BlueprintCallable)
	bool IsFiring() const { return bIsFiring; }

	/** Sets whether or not this gun can be fired */
	UFUNCTION(BlueprintCallable)
	void SetCanFire(const bool bNewFire) { bCanFire = bNewFire; }

	/** Sets the fire rate of this gun, updating its GameplayTags */
	UFUNCTION(BlueprintCallable)
	void SetFireRate(const bool bAutomatic);

	/** Sets whether or not the gun should recoil, updating its GameplayTags */
	UFUNCTION(BlueprintCallable)
	void SetShouldRecoil(const bool bRecoil);

	/** Sets whether or not the gun should show bullet decals, updating its GameplayTags */
	UFUNCTION(BlueprintCallable)
	void SetShowDecals(const bool bShowDecals);

	/** Sets whether or not the gun should show bullet tracers, updating its GameplayTags */
	UFUNCTION(BlueprintCallable)
	void SetShowTracers(const bool bShowTracers);

	/** Begins or resumes the recoil timeline, allowing UpdateRecoil to receive input from the timeline on tick */
	UFUNCTION(BlueprintCallable)
	void Recoil();
	
	/** GameMode binds to this delegate to keep track of number of shots fired */
	FOnShotFired OnShotFired;

protected:
	/** The skeletal mesh of the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	/** The location of the muzzle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* MuzzleLocationComp;

	/** Vector curve that implements vertical and horizontal recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	UCurveVector* RecoilCurve;

	/** Float curve that implements a screen kickback (camera shake) effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Recoil")
	UCurveFloat* KickbackCurve;

	UFUNCTION(BlueprintCallable)
	ABSCharacter* GetBSCharacter() const;
	
	/** Interpolates the current gun recoil, camera recoil, and kickback inside of OnTick
	 *  based on CurrentShotRecoilRotation, CurrentShotCameraRecoilRotation, and KickbackAngle */
	virtual void UpdateKickbackAndRecoil(float DeltaTime);

	/** Update the screen-shake-like camera recoil */
	virtual void UpdateKickback(float DeltaTime);

	/** Updates CurrentShotRecoilRotation and CurrentShotCameraRecoilRotation. Bound to RecoilTimeline, which corresponds to the RecoilCurve */
	UFUNCTION()
	void UpdateRecoil(FVector Output);

	/** The timeline corresponding to RecoilCurve */
	FTimeline RecoilTimeline;
	
	/** The current rotation representing the spread for the bullet, used when the gun is fired */
	FRotator CurrentShotRecoilRotation;

	/** Same as CurrentShotRecoilRotation but applied at half the scale */
	FRotator CurrentShotCameraRecoilRotation;

	/** Whether or not the player is holding down left click */
	UPROPERTY(BlueprintReadWrite)
	bool bIsFiring;
	
	/** Determines if the player can fire */
	UPROPERTY(BlueprintReadWrite)
	bool bCanFire;

	/** Whether or not to increment KickbackAngle, which is applied to the owning character's CameraRecoilComponent */
	bool bShouldKickback;

	/** Whether or not to fire the gun continuously */
	UPROPERTY(BlueprintReadWrite)
	bool bAutomaticFire;

	UPROPERTY(EditDefaultsOnly)
	float CameraRecoilInterpSpeed = 4.f;
	
	/** The accumulated delta seconds since the last camera kickback duration */
	float KickbackAlpha;

	/** The value pulled from the KickbackCurve at time KickbackAlpha divided by KickbackDuration */
	float KickbackAngle;

	/** The duration of each kickback animation */
	float KickbackDuration = 0.2f;

	/** Used in recoil to make sure the first shot has properly applied recoil */
	int32 ShotsFired;

private:
	FGameplayTagContainer GameplayTags;
};
