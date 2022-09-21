// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gun_AK47.generated.h"

class ADefaultPlayerController;
class UNiagaraSystem;
class UDefaultGameInstance;
class ADefaultCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFired);

UCLASS()
class BEATSHOT_API AGun_AK47 : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGun_AK47();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* MuzzleLocation;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
		TSubclassOf<class AProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		ADefaultCharacter* Character;

	UPROPERTY(VisibleAnywhere, Category = "References")
		UDefaultGameInstance* GI;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
		ADefaultPlayerController* PlayerController;

	UPROPERTY(EditAnywhere, Category = "Animation", BlueprintReadWrite)
		UNiagaraSystem* NS_MuzzleFlash;

	UPROPERTY()
		FOnShotFired OnShotFired;

	UPROPERTY(VisibleAnywhere, Category = "Interaction");
	float TraceDistance;
};

