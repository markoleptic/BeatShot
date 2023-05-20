// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSEquipmentActor.h"
#include "BeatShot/Beatshot.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Actor.h"
#include "BSGun.generated.h"

class USkeletalMeshComponent;

/** The base gun used in this game */
UCLASS()
class BEATSHOT_API ABSGun : public ABSEquipmentActor, public ISaveLoadInterface
{
	GENERATED_BODY()

	/** Sets default values for this actor's properties */
	ABSGun();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** ~ISaveLoadInterface begin */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	/** ~ISaveLoadInterface end */

public:

	/** Increments ShotsFired, executes OnShotFired */
	UFUNCTION(BlueprintCallable)
	virtual void Fire();

	/** Sets bIsFiring to false */
	UFUNCTION(BlueprintCallable)
	virtual void StopFire();

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
	
	/** GameMode binds to this delegate to keep track of number of shots fired */
	FOnShotFired OnShotFired;

protected:
	/** The skeletal mesh of the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* MeshComp;

	/** The location of the muzzle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* MuzzleLocationComp;

	/** Whether or not the player is holding down left click */
	UPROPERTY(BlueprintReadWrite)
	bool bIsFiring;
	
	/** Determines if the player can fire */
	UPROPERTY(BlueprintReadWrite)
	bool bCanFire;
};
