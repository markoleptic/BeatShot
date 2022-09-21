// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

class AGun_AK47;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	UCameraComponent* Camera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimInstance* AnimInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	ADefaultPlayerController* PlayerController;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	TSubclassOf<AGun_AK47> GunClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	AGun_AK47* Gun;

	UFUNCTION(BlueprintCallable)
	void SetSensitivity(float NewSensitivity);

	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }

private:
	UPROPERTY(VisibleAnywhere, Category = "References")
	UDefaultGameInstance* GI;

	void Fire();

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Turn(float Value);

	void LookUp(float Value);

	void InteractPressed();

	UPROPERTY(EditAnywhere, Category = "Camera")
	float Sensitivity;

protected:

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();
};
