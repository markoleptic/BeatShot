// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

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
class ABSPlayerController;
class USceneComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;
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
	
public:
	
	ABSCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|Character")
	ABSPlayerController* GetBSPlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category = "BeatShot|Character")
	ABSPlayerState* GetBSPlayerState() const;
	
	UFUNCTION(BlueprintCallable, Category = "BeatShot|Character")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;

	UFUNCTION(BlueprintCallable, Category = "BeatShot|Character")
	USkeletalMeshComponent* GetHandsMesh() const;

	/** Implement IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	void SetTimelinePlaybackRate_AimBot(const float TargetSpawnCD);
	UCameraComponent* GetCamera() const;
	USceneComponent* GetCameraRecoilComponent() const;
	void SetEnabled_AimBot(const bool bEnable) { bEnabled_AimBot = bEnable; }
	bool IsEnabled_AimBot() const { return bEnabled_AimBot; }
	
	UFUNCTION(BlueprintCallable)
	AGun_AK47* GetGun() const;
	
	/** Called when PlayerSettings are changed while Character is spawned */
	UFUNCTION()
	void OnUserSettingsChange(const FPlayerSettings& PlayerSettings);

	/** Bound to DefaultGameMode's OnTargetSpawned delegate, executes when a target has been spawned and adds the spawned target to the ActiveTargetLocations_AimBot queue. */
	UFUNCTION()
	void OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget);

	/** Simulates human lag by continuing to track towards provided direction */
	UFUNCTION()
	void OnBeatTrackDirectionChanged(const FVector Location);

	/** Executed when interact is pressed */
	FOnInteractDelegate OnInteractDelegate;

	/** Executed when shift interact is pressed */
	FOnShiftInteractDelegate OnShiftInteractDelegate;
	
protected:
	
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	/** Grant abilities on the Server. The Ability Specs will be replicated to the owning client. Called from inside PossessedBy(). */
	virtual void AddCharacterAbilities();

	/** Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too. */
	virtual void RemoveCharacterAbilities();

	/** The spring arm component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** The skeletal mesh for hands that hold the gun */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<USkeletalMeshComponent> HandsMesh;

	/** Camera component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	/** Reference to child gun actor component */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<UChildActorComponent> GunComponent;

	/** Additional layer of rotation to use for more realistic recoil */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<USceneComponent> CameraRecoilComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TSubclassOf<UAnimInstance> AnimClassLayers;

	/** Input configuration used by player controlled pawns to create input mappings and bind input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BeatShot|Input")
	TObjectPtr<UBSInputConfig> InputConfig;

	/** Default abilities, attributes, and effects */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TArray<TObjectPtr<UBSAbilitySet>> AbilitySets;

	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;
	TWeakObjectPtr<UBSAttributeSetBase> AttributeSetBase;
	FBSAbilitySet_GrantedHandles AbilitySet_GrantedHandles;

#pragma region Aimbot
	
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
