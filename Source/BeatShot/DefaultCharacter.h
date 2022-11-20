// Fill out your copyright notice in the Description page of Project Settings.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "GameFramework/Character.h"
#include "SaveGamePlayerSettings.h"
#include "DefaultCharacter.generated.h"

struct FInputActionValue;
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
class UCurveFloat;
class UInputAction;
class UInputMappingContext;

UENUM(BlueprintType)
enum class EMovementType : uint8 {
	Sprinting						UMETA(DisplayName, "Sprinting"),
	Walking							UMETA(DisplayName, "Walking"),
	Crouching						UMETA(DisplayName, "Crouching")
};

// Used to store movement properties for different movement types
USTRUCT(BlueprintType)
struct FMovementTypeVariables
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
		float MaxAcceleration;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
		float BreakingDecelerationWalking;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
		float GroundFriction;

	UPROPERTY(EditDefaultsOnly, Category = "Movement Variables")
		float MaxWalkSpeed;

	FMovementTypeVariables()
	{
		MaxAcceleration = 0.f;
		BreakingDecelerationWalking = 0.f;
		GroundFriction = 0.f;
		MaxWalkSpeed = 0.f;
	}
};

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
	*	@param NewMovementType The new movement state of the player
	*/
	void UpdateMovementValues(EMovementType NewMovementType);

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
		TMap<EMovementType, FMovementTypeVariables> MovementDataMap;

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

	/** Enumerator holding the 3 possible movement states defined by EMovementType */
	UPROPERTY()
		EMovementType MovementState;

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
};