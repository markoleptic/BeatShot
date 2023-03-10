// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.
// Credit to Dan Kestranek.

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "BSInputConfig.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GameplayAbility/BSAbilitySet.h"
#include "BSCharacter.generated.h"

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
class UBSAttributeSetBase;
class UBSAbilitySystemComponent;

DECLARE_DELEGATE_OneParam(FOnInteractDelegate, const int32);
DECLARE_DELEGATE_OneParam(FOnShiftInteractDelegate, const int32);

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

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client. Called from inside PossessedBy().
	virtual void AddCharacterAbilities();

	/** Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too. */
	virtual void RemoveCharacterAbilities();

	/** The spring arm component, which is required to enable 'use control rotation' */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	USpringArmComponent* SpringArmComponent;

	/** The skeletal mesh for hands that hold the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	USkeletalMeshComponent* HandsMesh;

	/** Camera component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	UCameraComponent* Camera;

	/** The blueprint class associated with the gun to spawn */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TSubclassOf<AGun_AK47> GunClass;

	/** Reference to child gun actor component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BeatShot|Components")
	UChildActorComponent* GunActorComp;

	/** Sets the height of the player's capsule component when standing */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Input")
	float DefaultCapsuleHalfHeight = 96.f;

	/** Input configuration used by player controlled pawns to create input mappings and bind input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BeatShot|Input")
	TObjectPtr<UBSInputConfig> InputConfig;

	/** Default abilities, attributes, and effects */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TArray<TObjectPtr<UBSAbilitySet>> AbilitySets;

	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UBSAttributeSetBase> AttributeSetBase;
	FBSAbilitySet_GrantedHandles AbilitySet_GrantedHandles;

public:
	/** Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/** Implement IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	/** Called when PlayerSettings are changed while Character is spawned */
	UFUNCTION()
	void OnUserSettingsChange(const FPlayerSettings& PlayerSettings);

	/** Bound to DefaultGameMode's OnTargetSpawned delegate, executes when a target has been spawned and adds the spawned target to the ActiveTargetLocations_AimBot queue. */
	UFUNCTION()
	void OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget);

	/** Sets the bEnabled_AimBot state */
	void SetEnabled_AimBot(const bool bEnable) { bEnabled_AimBot = bEnable; }

	/** Returns the bEnabled_AimBot state */
	bool IsEnabled_AimBot() const { return bEnabled_AimBot; }

	/** Sets the speed of the timeline playback */
	void SetTimelinePlaybackRate_AimBot(const float TargetSpawnCD) { TimelinePlaybackRate_AimBot = 1.f / TargetSpawnCD; }

	/** Returns HandMesh **/
	USkeletalMeshComponent* GetHandsMesh() const { return HandsMesh; }

	/** Returns Camera **/
	UCameraComponent* GetCamera() const { return Camera; }

	/** Simulates human lag by continuing to track towards provided direction */
	UFUNCTION()
	void OnBeatTrackDirectionChanged(const FVector Location);

	/** Reference to direction of fire */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BeatShot|Components")
	UArrowComponent* ShotDirection;

	/** Additional layer of rotation to use for more realistic recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	USceneComponent* CameraRecoilComp;

	/** Reference to gun */
	UPROPERTY()
	AGun_AK47* Gun;

	/** Executed when interact is pressed */
	FOnInteractDelegate OnInteractDelegate;

	/** Executed when shift interact is pressed */
	FOnShiftInteractDelegate OnShiftInteractDelegate;

#pragma region Aimbot

protected:
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* Curve_AimBotRotationSpeed;

private:
	/** Float interpolation function bound to AimBotTimeline, which sets the rotation of the camera */
	UFUNCTION()
	void OnTimelineTick_AimBot(const float Alpha);

	/** Executes when the AimBotTimeline completes. Shoots the target at the front of the queue and pops it, then calls DestroyNextTarget_AimBot() */
	UFUNCTION()
	void OnTimelineCompleted_AimBot();

	/** Sets new values for StartRotation_AimBot and TargetRotation_AimBot based on next target location in ActiveTargetLocations_AimBot, and plays AimBotTimeline */
	void DestroyNextTarget_AimBot();

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

	/** Delegate to call StopFire() shortly after AimBot has called Input_StartFire() */
	FTimerDelegate StopFire_AimBot;

	/** Whether the AimBot is active */
	bool bEnabled_AimBot;

	/** Whether or not the player is simulating human lag */
	bool bIsLagging = false;

	/** Playback rate for AimBotTimeline */
	float TimelinePlaybackRate_AimBot;

	/** Location for Character to keep tracking towards to simulate human lag */
	FVector LagLocation;

	/** Timer Delegate to switch bIsLagging to false */
	FTimerDelegate LagDelegate;

	/** Timer for how long the character should track after the target has changed directions */
	FTimerHandle LagHandle;

#pragma endregion

#pragma region Input

	/** Move the character left/right and forward/back */
	void Input_Move(const FInputActionValue& Value);

	/** Look left/right and up/down */
	void Input_Look(const FInputActionValue& Value);

	/** Toggles crouching */
	void Input_Crouch(const FInputActionInstance& Instance);

	/** Begin firing gun */
	void Input_StartFire();

	/** Stop firing gun, if automatic fire */
	UFUNCTION()
	void Input_StopFire();

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	/** Crouches or un-crouches based on current state */
	void ToggleCrouch();

	/** Triggered on pressing E */
	void OnInteractStarted(const FInputActionInstance& Instance);

	/** Triggered on releasing E */
	void OnInteractCompleted(const FInputActionInstance& Instance);

	/** Triggered on pressing Shift + E */
	void OnShiftInteractStarted(const FInputActionInstance& Instance);

	/** Triggered on releasing Shift + E */
	void OnShiftInteractCompleted(const FInputActionInstance& Instance);

	/** Let ASC know an ability bound to an input was pressed. */
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	/** Let ASC know an ability bound to an input was released. */
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	/** Multiplier to controller pitch and yaw */
	float Sensitivity;

	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Input")
	// ReSharper disable once UnrealHeaderToolError
	UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Input")
	int32 BaseMappingPriority = 0;

	const float SensitivityMultiplier = 14.2789148024750118991f;

#pragma endregion
};
