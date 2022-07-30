// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

class USceneComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USoundBase;
class UAnimMontage;
class UAnimInstance;

UCLASS()
class BEATAIM_API ADefaultCharacter : public ACharacter
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

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USkeletalMeshComponent* HandsMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USkeletalMeshComponent* GunMesh;
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
		USceneComponent* MuzzleLocation;
	UPROPERTY(VisibleAnywhere, Category = "Camera", BlueprintReadOnly, meta = (AllowPrivateAccess = true))
		UCameraComponent* Camera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		FVector GunOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
		float Sensitivity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		FVector MuzzleOffset;

private:
	void Fire();
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void InteractPressed();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<class AProjectile> ProjectileClass;
	UPROPERTY(VisibleAnywhere, Category = "Projectile");
		USoundBase* FireSound;
	UPROPERTY(VisibleAnywhere, Category = "Projectile");
		UAnimMontage* FireAnim;
	UAnimInstance* AnimInstance;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Interaction");
		float TraceDistance;
	UFUNCTION(BlueprintNativeEvent)
		void TraceForward();
		void TraceForward_Implementation();

};
