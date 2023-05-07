// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gun_AK47.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "BSInputConfig.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "Character/PBPlayerCharacter.h"
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
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetAddedToQueue);

/** Base Character for this game */
UCLASS()
class BEATSHOT_API ABSCharacter : public APBPlayerCharacter, public ISaveLoadInterface, public IAbilitySystemInterface, public IGameplayTagAssetInterface
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

	UFUNCTION(BlueprintCallable)
	AGun_AK47* GetGun() const;

	UCameraComponent* GetCamera() const;

	USceneComponent* GetCameraRecoilComponent() const;

	UFUNCTION(BlueprintCallable)
	ASphereTarget* PeekActiveTargets();

	UFUNCTION(BlueprintCallable)
	void PopActiveTargets();

	UFUNCTION(BlueprintCallable)
	float GetAimBotPlaybackSpeed() const;

	UPROPERTY(BlueprintAssignable)
	FOnTargetAddedToQueue OnTargetAddedToQueue;
	
	void SetEnabled_AimBot(const bool bEnable) { bEnabled_AimBot = bEnable; }
	bool IsEnabled_AimBot() const { return bEnabled_AimBot; }
	
	/** Bound to DefaultGameMode's OnTargetSpawned delegate, executes when a target has been spawned and adds the spawned target to the ActiveTargetLocations_AimBot queue. */
	UFUNCTION()
	void OnTargetSpawned_AimBot(ASphereTarget* SpawnedTarget);

	/** Let ASC know an ability bound to an input was pressed. */
	UFUNCTION(BlueprintCallable)
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);

	UFUNCTION(BlueprintCallable)
	/** Let ASC know an ability bound to an input was released. */
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);

	/** Executed when interact is pressed */
	FOnInteractDelegate OnInteractDelegate;

	/** Executed when shift interact is pressed */
	FOnShiftInteractDelegate OnShiftInteractDelegate;

	/** Implement IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/** End Implement IGameplayTagAssetInterface */

	/** Implement ISaveLoadInterface */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
	/** End Implement ISaveLoadInterface */
	
protected:
	
	virtual void BeginPlay() override;
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	virtual bool IsSprinting() const override;

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

private:

	/** A queue of target locations that have not yet been destroyed */
	TQueue<ASphereTarget*> ActiveTargets_AimBot;

	/** Whether the AimBot is active */
	bool bEnabled_AimBot;

#pragma endregion

#pragma region Input

	/** Move the character left/right and forward/back */
	void Input_Move(const FInputActionValue& Value);

	/** Look left/right and up/down */
	void Input_Look(const FInputActionValue& Value);

	/** Toggles crouching */
	void Input_Crouch(const FInputActionValue& Value);

	/** Walk instead of default sprint */
	void Input_WalkStart(const FInputActionValue& Value);

	/** Walk instead of default sprint */
	void Input_WalkEnd(const FInputActionValue& Value);

	/** Crouches or un-crouches based on current state */
	void ToggleCrouch();

	/** Triggered on pressing E */
	void OnInteractStarted(const FInputActionValue& Value);

	/** Triggered on releasing E */
	void OnInteractCompleted(const FInputActionValue& Value);

	/** Triggered on pressing Shift + E */
	void OnShiftInteractStarted(const FInputActionValue& Value);

	/** Triggered on releasing Shift + E */
	void OnShiftInteractCompleted(const FInputActionValue& Value);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	
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
