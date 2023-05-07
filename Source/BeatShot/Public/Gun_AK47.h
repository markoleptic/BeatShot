// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagAssetInterface.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Actor.h"
#include "Gun_AK47.generated.h"

class ASphereTarget;
class ABSPlayerController;
class UNiagaraSystem;
class UBSGameInstance;
class ABSCharacter;
class USkeletalMeshComponent;
class UAnimMontage;

DECLARE_DELEGATE(FOnShotFired);

/** The base gun used in this game */
UCLASS()
class BEATSHOT_API AGun_AK47 : public AActor, public IGameplayTagAssetInterface 
{
	GENERATED_BODY()

	/** Sets default values for this actor's properties */
	AGun_AK47();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Implement IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

public:

	/** Increments ShotsFired, executes OnShotFired */
	UFUNCTION(BlueprintCallable)
	void Fire();

	/** Stops the timer that allows for automatic fire */
	UFUNCTION(BlueprintCallable)
	void StopFire();

	/** Returns the current spread rotation (Pitch and Yaw at the current time). Used by FireGun ability */
	UFUNCTION(BlueprintCallable)
	FRotator GetCurrentRecoilRotation() const;

	/** Returns the location of the muzzle */
	UFUNCTION(BlueprintCallable)
	FVector GetMuzzleLocation() const;

	UFUNCTION(BlueprintCallable)
	void SetCanFire(const bool bNewFire) { bCanFire = bNewFire; }

	UFUNCTION(BlueprintCallable)
	void SetFireRate(const bool bAutomatic);

	UFUNCTION(BlueprintCallable)
	void SetShouldRecoil(const bool bRecoil);

	UFUNCTION(BlueprintCallable)
	void SetShowDecals(const bool bShowDecals);

	UFUNCTION(BlueprintCallable)
	void SetShowTracers(const bool bShowTracers);

	UFUNCTION(BlueprintCallable)
	void Recoil();

	/** Returns whether the weapon can fire or not */
	UFUNCTION(BlueprintCallable)
	bool CanFire() const { return bCanFire; }

	UFUNCTION(BlueprintCallable)
	bool IsFiring() const { return bIsFiring; }
	
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
	
	/** Controls gun recoil, camera recoil, and kickback inside of OnTick */
	void UpdateKickbackAndRecoil(float DeltaTime);

	/** Update the screen-shake-like camera recoil */
	void UpdateKickback(float DeltaTime);

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
