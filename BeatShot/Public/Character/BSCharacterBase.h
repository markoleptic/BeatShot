﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "Input/BSInputConfig.h"
#include "GameplayTagAssetInterface.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Globals/BSAbilitySet.h"
#include "BSCharacterBase.generated.h"

class UBSCharacterMovementComponent;
class ABSPlayerState;
class UBSMoveStepSound;
class UBSInventoryManagerComponent;
class UBSInventoryItemDefinition;
class UBSEquipmentManagerComponent;
class ABSPlayerController;
class USceneComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UBSAttributeSetBase;
class UBSAbilitySystemComponent;
struct FInputActionValue;

DECLARE_DELEGATE_OneParam(FOnInteractDelegate, const int32);
DECLARE_DELEGATE_OneParam(FOnShiftInteractDelegate, const int32);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTargetAddedToQueue);

inline float SimpleSpline(const float Value)
{
	const float ValueSquared = Value * Value;
	return (3.0f * ValueSquared - 2.0f * ValueSquared * Value);
}

/** Base Character for this game */
UCLASS()
class BEATSHOT_API ABSCharacterBase : public ACharacter, public ISaveLoadInterface, public IAbilitySystemInterface,
                                  public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	ABSCharacterBase(const FObjectInitializer& ObjectInitializer);

protected:
	/** The spring arm component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	/** Camera component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<UCameraComponent> CameraComponent;

	/** The Equipment Manager component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<UBSEquipmentManagerComponent> EquipmentManagerComponent;
	
	/** The Equipment Manager component */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "BeatShot|Components")
	TObjectPtr<UBSInventoryManagerComponent> InventoryManagerComponent;

	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Input")
	UInputMappingContext* BaseMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Input")
	int32 BaseMappingPriority = 0;
	
	/** Input configuration used by player controlled pawns to create input mappings and bind input actions. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BeatShot|Input")
	TObjectPtr<UBSInputConfig> InputConfig;

	/** Default abilities, attributes, and effects */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Abilities")
	TArray<TObjectPtr<UBSAbilitySet>> AbilitySets;

	/** Default inventory items. Each entry should contain an inventory item definition composed of inventory fragments */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BeatShot|Inventory")
	TArray<TSubclassOf<UBSInventoryItemDefinition>> InitialInventoryItems;

	/** Move step sounds by physical surface */
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "BeatShot|Sounds")
	TMap<TEnumAsByte<EPhysicalSurface>, TSubclassOf<UBSMoveStepSound>> MoveStepSounds;

	/** Ability system component that lives on the PlayerState. */
	TWeakObjectPtr<UBSAbilitySystemComponent> AbilitySystemComponent;

	/** Attribute set that lives on the PlayerState. */
	TWeakObjectPtr<const UBSAttributeSetBase> AttributeSetBase;

	/** Handles returned when granting abilities. */
	FBSAbilitySet_GrantedHandles AbilitySet_GrantedHandles;

public:
	/** Implement IAbilitySystemInterface */
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	ABSPlayerController* GetBSPlayerController() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	ABSPlayerState* GetBSPlayerState() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponent() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	UBSEquipmentManagerComponent* GetEquipmentManager() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	UBSInventoryManagerComponent* GetInventoryManager() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	UBSCharacterMovementComponent* GetBSCharacterMovement() const;

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	UCameraComponent* GetCamera() const { return CameraComponent.Get(); };

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	float GetMinSpeedForFallDamage() const { return MinSpeedForFallDamage; };

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	float GetMinLandBounceSpeed() const { return MinLandBounceSpeed; }

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	bool DoesWantToWalk() const { return bWantsToWalk; }

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	float GetLastJumpTime() const { return LastJumpTime; }

	UFUNCTION(BlueprintPure, Category = "BeatShot|Character")
	FORCEINLINE bool GetAutoBunnyHop() const { return bAutoBunnyHop; }

	UFUNCTION(Category = "BeatShot|Character", BlueprintCallable)
	void SetAutoBunnyHop(bool Value) { bAutoBunnyHop = Value; }

	FORCEINLINE TSubclassOf<UBSMoveStepSound>* GetMoveStepSound(const TEnumAsByte<EPhysicalSurface> Surface)
	{
		return MoveStepSounds.Find(Surface);
	}
	
	/** Implement IGameplayTagAssetInterface */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	/** End Implement IGameplayTagAssetInterface */
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;
	virtual void OnRep_PlayerState() override;
	virtual void ApplyDamageMomentum(float DamageTaken, FDamageEvent const& DamageEvent, APawn* PawnInstigator,
		AActor* DamageCauser) override;
	virtual void ClearJumpInput(float DeltaTime) override;
	virtual void Jump() override;
	virtual void StopJumping() override;
	virtual void OnJumped_Implementation() override;
	virtual bool CanJumpInternal_Implementation() const override;
	virtual void RecalculateBaseEyeHeight() override;
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;
	virtual bool CanCrouch() const override;
	
	/** Crouches or un-crouches based on current state */
	void ToggleCrouch();
	
public:
	virtual bool IsSprinting() const;
	
protected:
	/** Grant abilities on the Server. The Ability Specs will be replicated to the owning client. Called from inside PossessedBy(). */
	virtual void AddCharacterAbilities();

	/** Grant equipment on the Server. Called from inside PossessedBy() */
	virtual void AddCharacterInventoryItems();

	/** Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too. */
	virtual void RemoveCharacterAbilities();

	/** Removes all equipment on the Server. Called from inside PossessedBy() */
	virtual void RemoveCharacterInventoryItems();

public:
	/** Executed when interact is pressed */
	FOnInteractDelegate OnInteractDelegate;

	/** Executed when shift interact is pressed */
	FOnShiftInteractDelegate OnShiftInteractDelegate;

	/** Binds IA_LeftClick to Input_OnLeftClick. */
	void BindLeftClick();

	/** Unbinds IA_LeftClick to Input_OnLeftClick. */
	void UnbindLeftClick();

protected:
	void Input_Move(const FInputActionValue& Value);
	void Input_Look(const FInputActionValue& Value);
	void Input_Crouch(const FInputActionValue& Value);
	void Input_WalkStart(const FInputActionValue& Value);
	void Input_WalkEnd(const FInputActionValue& Value);
	void Input_OnInteractStarted(const FInputActionValue& Value);
	void Input_OnInteractCompleted(const FInputActionValue& Value);
	void Input_OnShiftInteractStarted(const FInputActionValue& Value);
	void Input_OnShiftInteractCompleted(const FInputActionValue& Value);
	void Input_OnEquipmentSlot1Started(const FInputActionValue& Value);
	void Input_OnEquipmentSlot2Started(const FInputActionValue& Value);
	void Input_OnEquipmentSlot3Started(const FInputActionValue& Value);
	void Input_OnEquipmentSlotLastEquippedStarted(const FInputActionValue& Value);
	void Input_OnPause(const FInputActionValue& Value);
	void Input_OnLeftClick(const FInputActionValue& Value);
	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	/** Implement ISaveLoadInterface */
	virtual void OnPlayerSettingsChanged_Game(const FPlayerSettings_Game& GameSettings) override;
	virtual void OnPlayerSettingsChanged_User(const FPlayerSettings_User& UserSettings) override;
	/** End Implement ISaveLoadInterface */
	
	/** Multiplier to controller pitch and yaw */
	float Sensitivity;
	
	/** Automatic bunny-hopping */
	UPROPERTY(EditAnywhere, Category = "BeatShot|Character")
	bool bAutoBunnyHop;

	/** Minimum speed to play the camera shake for landing */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Character")
	float MinLandBounceSpeed;

	/** Don't take damage below this speed - so jumping doesn't damage */
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Character")
	float MinSpeedForFallDamage;

	// In HL2, the player has the Z component for applying momentum to the capsule capped
	UPROPERTY(EditDefaultsOnly, Category = "BeatShot|Character")
	float CapDamageMomentumZ = 0.f;

	/** Changed when pressing and releasing IA_Walk (shift key) */
	bool bWantsToWalk;

	/** defer the jump stop for a frame (for early jumps) */
	bool bDeferJumpStop;

	/** when we last jumped */
	float LastJumpTime;

	/** throttle jump boost when going up a ramp, so we don't spam it */
	float LastJumpBoostTime;

	/** maximum time it takes to jump */
	float MaxJumpTime;
};