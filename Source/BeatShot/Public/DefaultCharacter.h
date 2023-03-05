// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to whoisEllie on Github for some code used in this class

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Character.h"
#include "DefaultCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnInteractDelegate, const int32);
DECLARE_DELEGATE_OneParam(FOnShiftInteractDelegate, const int32);

struct FInputActionValue;
struct FInputActionInstance;
class AGun_AK47;
class UNiagaraSystem;
class AProjectile;
class ADefaultPlayerController;
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
enum class EMovementType : uint8
{
	Sprinting UMETA(DisplayName="Sprinting"),
	Walking UMETA(DisplayName="Walking"),
	Crouching UMETA(DisplayName="Crouching")
};

/** Used to store movement properties for different movement types */
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
class BEATSHOT_API ADefaultCharacter : public ACharacter, public ISaveLoadInterface
{
	GENERATED_BODY()

protected:
	/** Sets default values for this character's properties */
	ADefaultCharacter();

	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Sets the Mapping Context in Player Controller */
	virtual void PawnClientRestart() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called to bind functionality to input */
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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

	/** A map holding data for each movement state */
	UPROPERTY(EditDefaultsOnly, Category = "Movement | Data")
	TMap<EMovementType, FMovementTypeVariables> MovementDataMap;

	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* Curve_AimBotRotationSpeed;

public:
	/** Called when PlayerSettings are changed while Character is spawned */
	UFUNCTION()
	void OnUserSettingsChange(const FPlayerSettings& PlayerSettings);

	/** Bound to DefaultGameMode's OnTargetSpawned delegate, executes when a target has been spawned and adds the
	 *  spawned target to the ActiveTargetLocations_AimBot queue */
	UFUNCTION()
	void OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget);

	/** Sets the bEnabled_AimBot state */
	void SetEnabled_AimBot(const bool bEnable) { bEnabled_AimBot = bEnable; }

	/** Sets the speed of the timeline playback */
	void SetTimelinePlaybackRate_AimBot(const float TargetSpawnCD) { TimelinePlaybackRate_AimBot = 1.f / TargetSpawnCD; }

	/** Reference to direction of fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UArrowComponent* ShotDirection;

	/** Additional layer of rotation to use for more realistic recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USceneComponent* CameraRecoilComp;

	/** Reference to gun */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	AGun_AK47* Gun;

	/** Returns HandMesh **/
	USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }

	/** Returns Camera **/
	UCameraComponent* GetCamera() const { return Camera; }

	/** Simulates human lag by continuing to track towards provided direction */
	UFUNCTION()
	void OnBeatTrackDirectionChanged(const FVector Location);
	
	/** Executed when interact is pressed */
	FOnInteractDelegate OnInteractDelegate;

	/** Executed when shift interact is pressed */
	FOnShiftInteractDelegate OnShiftInteractDelegate;

private:
	/** Handles updates to the movement state and changes relevant values accordingly
	 *	@param NewMovementType The new movement state of the player
	 */
	void UpdateMovementValues(EMovementType NewMovementType);
	
	/** Begin firing gun */
	void StartFire();

	/** Stop firing gun, if automatic fire */
	UFUNCTION()
	void StopFire();

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

	/** Triggered on pressing E or Shift + E */
	void OnInteractStarted(const FInputActionInstance& Instance);

	/** Triggered on pressing E or Shift + E */
	void OnInteractCompleted(const FInputActionInstance& Instance);

	/** Triggered on pressing E or Shift + E */
	void OnShiftInteractStarted(const FInputActionInstance& Instance);

	/** Triggered on pressing E or Shift + E */
	void OnShiftInteractCompleted(const FInputActionInstance& Instance);

	/** Float interpolation function bound to AimBotTimeline, which sets the rotation of the camera */
	UFUNCTION()
	void OnTimelineTick_AimBot(const float Alpha);
	
	/** Executes when the AimBotTimeline completes. Shoots the target at the front of the queue and pops it, then calls
	 *  DestroyNextTarget_AimBot() */
	UFUNCTION()
	void OnTimelineCompleted_AimBot();

	/** Sets new values for StartRotation_AimBot and TargetRotation_AimBot based on next target location in ActiveTargetLocations_AimBot,
	 *  and plays AimBotTimeline */
	void DestroyNextTarget_AimBot();

	/** Whether the player is holding the crouch button */
	bool bHoldingCrouch;

	/** Whether the player is holding the walk key */
	bool bHoldingWalk;

	/** Whether the character is sprinting */
	bool bIsWalking;

	/** Whether the character is crouching */
	bool bIsCrouching;

	/** Whether the AimBot is active */
	bool bEnabled_AimBot;

	/** Multiplier to controller pitch and yaw */
	float Sensitivity;

	/** Playback rate for AimBotTimeline */
	float TimelinePlaybackRate_AimBot;

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

	/** Timeline for interpolating the rotation to aim at the target to destroy */
	FTimeline AimBotTimeline;

	/** A queue of target locations that have not yet been destroyed */
	TQueue<FVector> ActiveTargetLocations_AimBot;

	/** A queue of target locations that have not yet been destroyed */
	TQueue<ASphereTarget*> ActiveTargets_AimBot;

	/** The rotation at the start of the rotation interpolation */
	FRotator StartRotation_AimBot;

	/** The rotation to interpolate over the course of the timeline */
	FRotator TargetRotation_AimBot;

	/** Delegate to call StopFire() shortly after AimBot has called StartFire() */
	FTimerDelegate StopFire_AimBot;

	/** Whether or not the player is simulating human lag */
	bool bIsLagging = false;

	/** Location for Character to keep tracking towards to simulate human lag */
	FVector LagLocation;

	/** Timer Delegate to switch bIsLagging to false */
	FTimerDelegate LagDelegate;

	/** Timer for how long the character should track after the target has changed directions */
	FTimerHandle LagHandle;

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
	UInputAction* ShiftModifierAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* ShiftInteractAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* ScrollAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Actions")
	UInputAction* PauseAction;

	/** Input Mappings */

	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
	// ReSharper disable once UnrealHeaderToolError
	UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input | Mappings")
	int32 BaseMappingPriority = 0;
};
