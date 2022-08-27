// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

class AProjectile;
class ADefaultPlayerController;
class UDefaultStatSaveGame;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ADefaultPlayerController* PlayerController;

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float NewSensitivity);

	UPROPERTY()
	FOnShotFired OnShotFired;

private:
	UDefaultGameInstance* GI;

	void Fire();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void InteractPressed();

	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	TSubclassOf<AProjectile> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Projectile");
	USoundBase* FireSound;

	UPROPERTY(VisibleAnywhere, Category = "Projectile");
	UAnimMontage* FireAnim;

	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, Category = "Camera")
	float Sensitivity;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Interaction");
	float TraceDistance;

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();
};
