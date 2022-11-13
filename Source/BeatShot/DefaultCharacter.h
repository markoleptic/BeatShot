// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "InputMappingContext.h" // Rider may mark this as unused, but this is incorrect and removal will cause issues
#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Marketplace/FPSCore/Source/FPSCore/Public/FPSCharacter.h"
#include "C:/Program Files/Epic Games/UE_5.0/Engine/Plugins/Experimental/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputLibrary.h"
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
class UCurveFloat;
class UBlendSpace;

UCLASS()
class BEATSHOT_API ADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** Sets default values for this character's properties */
	ADefaultCharacter();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	virtual void PawnClientRestart() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Returns the character's forward movement (from 0 to 1) */
	UFUNCTION(BlueprintCallable, Category = "FPS Character")
		float GetForwardMovement() const { return ForwardMovement; }

	/** Returns the character's sideways movement (from 0 to 1) */
	UFUNCTION(BlueprintCallable, Category = "FPS Character")
		float GetRightMovement() const { return RightMovement; }

	/** Returns the character's vertical mouse position (from 0 to 1) */
	UFUNCTION(BlueprintCallable, Category = "FPS Character")
		float GetMouseY() const { return MouseY; }

	/** Returns the character's horizontal mouse position (from 0 to 1) */
	UFUNCTION(BlueprintCallable, Category = "FPS Character")
		float GetMouseX() const { return MouseX; }

	/** Returns the character's current movement state */
	UFUNCTION(BlueprintPure, Category = "FPS Character")
		EMovementState GetMovementState() const { return MovementState; }

	/** A global system that handles updates to the movement state and changes relevant values accordingly
	*	@param NewMovementState The new movement state of the player
	*/
	void UpdateMovementValues(EMovementState NewMovementState);

	/** A map holding data for each movement state */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Data")
		TMap<EMovementState, FMovementVariables> MovementDataMap;

	void Fire() const;

	void StartFire() const;

	void StopFire() const;

	void InteractPressed();

	UFUNCTION(BlueprintCallable)
		void SetSensitivity(float NewSensitivity);

	/** The spring arm component, which is required to enable 'use control rotation' */
	UPROPERTY(EditDefaultsOnly, Category = "Components")
		USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* HandsMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, Category = "Mesh")
		TSubclassOf<AGun_AK47> GunClass;

	UPROPERTY(BlueprintReadWrite, Category = "Mesh")
		AGun_AK47* Gun;

	/** Returns HandMesh **/
	USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }

	/** Returns a reference to the player's camera component */
	UCameraComponent* GetCameraComponent() const { return Camera; }

protected:

	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();
	void TraceForward_Implementation();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		UDefaultGameInstance* GI;

private:

	/** Move the character left/right and forward/back
*	@param Value The value passed in by the Input Component
*/
	void Move(const FInputActionValue& Value);

	/** Look left/right and up/down
	 *	@param Value The value passed in by the Input Component
	 */
	void Look(const FInputActionValue& Value);
	 /** Alternative to the built in Crouch function
	  *  Handles crouch input and decides what action to perform based on the character's current state
	  */
	void ToggleCrouch();

	/** Transitions the character out of the crouched state
	 *	@param bToSprint Whether to transition into a sprint state
	 */
	void StopCrouch(bool bToSprint);

	/** Toggle for crouching */
	void StartCrouch();

	/** Exits the character from the slide state if they are sliding and updates bHoldingCrouch */
	void ReleaseCrouch();

	/** Starting to sprint */
	void StartWalk();

	/** Stopping to sprint */
	void StopWalk();

	/** The forward movement value (used to drive animations) */
	float ForwardMovement;

	/** The right movement value (used to drive animations) */
	float RightMovement;

	/** The look up value (used to drive procedural weapon sway) */
	float MouseY;

	/** The right look value (used to drive procedural weapon sway) */
	float MouseX;

	/** Whether the player is holding the crouch button */
	bool bHoldingCrouch;

	/** Whether the player is holding the sprint key */
	bool bHoldingWalk;

	/** Whether the character is sprinting */
	bool bIsWalking;

	/** Whether the character is crouching */
	bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
		float Sensitivity;

	/** Enumerator holding the 5 possible movement states defined by EMovementState */
	UPROPERTY()
		EMovementState MovementState;

	/** Sets the height of the player's capsule component when crouched */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float DefaultCapsuleHalfHeight = 96.f;

	/** Sets the height of the player's capsule component when crouched */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float CrouchedCapsuleHalfHeight = 58.0f;

	/** The rate at which the character crouches */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float CrouchSpeed = 10.0f;

	/** Input actions */

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* MovementAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* LookAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* JumpAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* SprintAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* CrouchAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* FiringAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* PrimaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* SecondaryWeaponAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* ReloadAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* AimAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* InteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* ScrollAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
		UInputAction* PauseAction;

	/** Input Mappings */

	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
		UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
		int32 BaseMappingPriority = 0;
};