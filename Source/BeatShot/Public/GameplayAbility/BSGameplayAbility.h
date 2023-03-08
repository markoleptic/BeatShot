// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "BeatShot/Beatshot.h"
#include "BSGameplayAbility.generated.h"

UENUM(BlueprintType)
enum class EBSAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};

/**
 *	Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EBSAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

UCLASS()
class BEATSHOT_API UBSGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	friend class UBSAbilitySystemComponent;
	
public:
	UBSGameplayAbility();

	UFUNCTION(BlueprintCallable, Category = "BS|Ability")
	UBSAbilitySystemComponent* GetBSAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BS|Ability")
	ABSPlayerController* GetBSPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BS|Ability")
	AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "BS|Ability")
	ABSCharacter* GetBSCharacterFromActorInfo() const;
	
	// Abilities with this set will automatically activate when the input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BS|Ability")
	EBSAbilityInputID AbilityInputID = EBSAbilityInputID::None;

	// Value to associate an ability with an slot without tying it to an automatically activated input.
	// Passive abilities won't be tied to an input so we need a way to generically associate abilities with slots.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "BS|Ability")
	EBSAbilityInputID AbilityID = EBSAbilityInputID::None;

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BS|Ability Activation")
	EBSAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BS|Ability Activation")
	EBSAbilityActivationGroup ActivationGroup;

	// Tells an ability to activate immediately when its granted. Used for passive abilities and abilities forced on others.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "BS|Ability")
	bool ActivateAbilityOnGranted = false;

	// If an ability is marked as 'ActivateAbilityOnGranted', activate them immediately when given here
	// Epic's comment: Projects may want to initiate passives or do other "BeginPlay" type of logic here.
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	EBSAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EBSAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }
};
