// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "Engine/DataAsset.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayTagContainer.h"
#include "BSAbilitySet.generated.h"

class UBSGameplayAbility;
class UGameplayEffect;
class UAttributeSet;
class UBSAbilitySystemComponent;

/** Associate a BSGameplayAbility with a GameplayTag. Also includes Ability Level */
USTRUCT(BlueprintType)
struct FBSAbilitySet_GameplayAbility
{
	GENERATED_BODY()

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBSGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/** Contains the gameplay effect and the effect level */
USTRUCT(BlueprintType)
struct FBSAbilitySet_GameplayEffect
{
	GENERATED_BODY()

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	float EffectLevel = 1.0f;
};

/** AttributeSet Wrapper */
USTRUCT(BlueprintType)
struct FBSAbilitySet_AttributeSet
{
	GENERATED_BODY()

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAttributeSet> AttributeSet;
};

/** Data used to store handles to what has been granted by the ability set. */
USTRUCT(BlueprintType)
struct FBSAbilitySet_GrantedHandles
{
	GENERATED_BODY()
	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);
	void TakeFromAbilitySystem(UBSAbilitySystemComponent* ASC);
	FGameplayAbilitySpec* FindFirstAbilitySpecFromHandle(UBSAbilitySystemComponent* ASC);
	bool IsEmpty() const;

protected:
	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
	TArray<TObjectPtr<UAttributeSet>> GrantedAttributeSets;
};


/** A data asset that contains GameplayAbilities, GameplayEffects, and AttributeSets that should be granted to the owner */
UCLASS(BlueprintType, Const)
class UBSAbilitySet : public UDataAsset
{
	GENERATED_BODY()

public:
	UBSAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(UBSAbilitySystemComponent* ASC, FBSAbilitySet_GrantedHandles* OutGrantedHandles,
		UObject* SourceObject = nullptr) const;

protected:
	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta=(TitleProperty=Ability))
	TArray<FBSAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta=(TitleProperty=GameplayEffect))
	TArray<FBSAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta=(TitleProperty=AttributeSet))
	TArray<FBSAbilitySet_AttributeSet> GrantedAttributes;
};
