// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

class UDefaultGameInstance;
class USceneComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class USoundBase;
class UAnimMontage;
class UAnimInstance;
class UPlayerHUD;

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

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* HandsMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMesh;

	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, Category = "Camera", BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector GunOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FVector MuzzleOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<class ADefaultPlayerController> PlayerControllerClass;

	UPROPERTY()
	ADefaultPlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPlayerHUD> PlayerHUDClass;

	UPROPERTY()
	UPlayerHUD* PlayerHUD;

	UFUNCTION(BlueprintCallable, Category = "Player Stats")
	void ShowPlayerHUD(bool ShouldShow);

	bool HUDActive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float Sensitivity;

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float InputSensitivity);

	UFUNCTION(BlueprintCallable)
	float GetSensitivity();

	UDefaultGameInstance* GI;

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
