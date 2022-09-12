// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

class UNiagaraSystem;
class AProjectile;
class ADefaultPlayerController;
class USaveGamePlayerSettings;
class UDefaultGameInstance;
class USceneComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USoundBase;
class UAnimMontage;
class UAnimInstance;
class UPlayerHUD;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnShotFired);

UCLASS()
class BEATSHOT_API ADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ADefaultCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Cleanup pawn when game ends
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	USceneComponent* MuzzleLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	FVector MuzzleOffset;

	UPROPERTY(VisibleAnywhere, Category = "Projectile");
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* FireAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ADefaultPlayerController* PlayerController;

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float NewSensitivity);

	UPROPERTY()
	FOnShotFired OnShotFired;

	UPROPERTY(EditAnywhere, Category = "Animation", BlueprintReadWrite)
	UNiagaraSystem* NS_MuzzleFlash;

	FTimerHandle RecoilAnimDelay;

	UFUNCTION()
	void PlayRecoilAnim();

private:
	UPROPERTY(VisibleAnywhere, Category = "References")
	UDefaultGameInstance* GI;

	void Fire();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void InteractPressed();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float Sensitivity;

	UPROPERTY(VisibleAnywhere, Category = "Interaction");
	float TraceDistance;

	UPROPERTY(VisibleAnywhere, Category = "Target Properties");
	float TotalPossibleDamage;

protected:

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();
};