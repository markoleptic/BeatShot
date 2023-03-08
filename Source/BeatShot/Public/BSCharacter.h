// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to whoisEllie on Github for some code used in this class
// Credit to Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Character.h"
#include "BeatShot/BeatShot.h"
#include "AbilitySystemInterface.h"
#include "BSInputConfig.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "BSCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnInteractDelegate, const int32);
DECLARE_DELEGATE_OneParam(FOnShiftInteractDelegate, const int32);

UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);

struct FInputActionValue;
struct FInputActionInstance;
class AGun_AK47;
class UNiagaraSystem;
class AProjectile;
class ABSPlayerController;
class UBSGameInstance;
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

UCLASS()
class BEATSHOT_API ABSCharacter : public ACharacter, public ISaveLoadInterface, public IAbilitySystemInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

protected:
	
	/** Sets default values for this character's properties */
	ABSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

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

#pragma region AbilitySystem
	
 	virtual void PossessedBy(AController* NewController) override;

	virtual void OnRep_PlayerState() override;
	
	// Implement IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	// Switch on AbilityID to return individual ability levels. Hardcoded to 1 for every ability in this project.
	UFUNCTION(BlueprintCallable)
	virtual int32 GetAbilityLevel(EBSAbilityInputID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();
	
	/**
	* Getters for attributes from GDAttributeSetBase
	**/
	
	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetMoveSpeedBaseValue() const;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	TObjectPtr<UBSInputConfig> InputConfig;

	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

protected:

	TWeakObjectPtr<class UBSAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<class UBSAttributeSetBase> AttributeSetBase;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FText CharacterName;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UBSGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayEffect>> StartupEffects;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

#pragma endregion

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

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

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


#pragma region Aimbot
	
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

#pragma endregion

#pragma region InputActions
	
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

#pragma endregion
};
