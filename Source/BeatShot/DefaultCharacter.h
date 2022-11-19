// Fill out your copyright notice in the Description page of Project Settings.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "GameFramework/Character.h"
#include "SaveGamePlayerSettings.h"
#include "InputActionValue.h"
// ReSharper disable once CppUnusedIncludeDirective
#include "InputMappingContext.h" // Rider may mark this as unused, but this is incorrect and removal will cause issues
#include "U:/Epic Games/UE_5.0/Engine/Plugins/Marketplace/FPSCore/Source/FPSCore/Public/FPSCharacter.h"
#include "U:/Epic Games/UE_5.0/Engine/Plugins/Experimental/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputLibrary.h"
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

	/** Called on Pawn restart */
	virtual void PawnClientRestart() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Handles updates to the movement state and changes relevant values accordingly
	*	@param NewMovementState The new movement state of the player
	*/
	void UpdateMovementValues(EMovementState NewMovementState);

	/** Returns HandMesh **/
	USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }

	/** Returns Camera **/
	UCameraComponent* GetCamera() const { return Camera; }

	/** Called when PlayerSettings are changed while Character is spawned */
	UFUNCTION(BlueprintCallable)
		void OnUserSettingsChange(FPlayerSettings PlayerSettings);

	/** The spring arm component, which is required to enable 'use control rotation' */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArmComponent;

	/** The skeletal mesh for hands that hold the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USkeletalMeshComponent* HandsMesh;

	/** Camera component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		UCameraComponent* Camera;

	/** The blueprint class associated with the gun to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		TSubclassOf<AGun_AK47> GunClass;

	/** Reference to child gun actor component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UChildActorComponent* GunActorComp;

	/** Reference to gun */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		AGun_AK47* Gun;

	/** Reference to direction of fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UArrowComponent* ShotDirection;

	/** Additional layer of rotation to use for more realistic recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
		USceneComponent* CameraRecoilComp;

	/** A map holding data for each movement state */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Data")
		TMap<EMovementState, FMovementVariables> MovementDataMap;

protected:

	/** Debug for tracing a line where the character is facing */
	UFUNCTION(BlueprintNativeEvent)
	void TraceForward();

	/** Blueprint version of Debug for tracing a line where the character is facing */
	void TraceForward_Implementation();

private:

	/** Begin firing gun */
	void StartFire() const;

	/** Stop firing gun, if automatic fire */
	void StopFire() const;

	/** Stop firing gun, if automatic fire */
	void InteractPressed();

	/** Move the character left/right and forward/back
	*	@param Value The value passed in by the Input Component
	*/
	void Move(const FInputActionValue& Value);

	/** Look left/right and up/down
	 *	@param Value The value passed in by the Input Component
	 */
	void Look(const FInputActionValue& Value);

	/** Change movement state to crouching */
	void StartCrouch();

	/** Change movement state from crouching */
	void ReleaseCrouch();

	/** Change movement state to walk */
	void StartWalk();

	/** Change movement state from walk */
	void StopWalk();

	/** Whether the player is holding the crouch button */
	bool bHoldingCrouch;

	/** Whether the player is holding the walk key */
	bool bHoldingWalk;

	/** Whether the character is sprinting */
	bool bIsWalking;

	/** Whether the character is crouching */
	bool bIsCrouching;

	/** Multiplier to controller pitch and yaw */
	float Sensitivity;

	/** Enumerator holding the 5 possible movement states defined by EMovementState */
	UPROPERTY()
		EMovementState MovementState;

	/** Sets the height of the player's capsule component when standing */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float DefaultCapsuleHalfHeight = 96.f;

	/** Sets the height of the player's capsule component when crouched */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float CrouchedCapsuleHalfHeight = 58.0f;

	/** The rate at which the character crouches */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Crouch")
		float CrouchSpeed = 10.0f;

	/** Input actions bound inside of the the blueprint for this class */

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

#pragma region NOTINUSE

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

	/** The forward movement value (used to drive animations) */
	float ForwardMovement;

	/** The right movement value (used to drive animations) */
	float RightMovement;

	/** The look up value (used to drive procedural weapon sway) */
	float MouseY;

	/** The right look value (used to drive procedural weapon sway) */
	float MouseX;

#pragma endregion
};