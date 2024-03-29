﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "AbilitySystem/Globals/BSAbilitySet.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "AbilitySystem/BSAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/BSGameplayAbility.h"

/* ---------------------------- */
/* -- BS Granted Ability Set -- */
/* ---------------------------- */

void FBSGrantedAbilitySet::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle);
	}
}

void FBSGrantedAbilitySet::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		GameplayEffectHandles.Add(Handle);
	}
}

void FBSGrantedAbilitySet::AddAttributeSet(UAttributeSet* Set)
{
	GrantedAttributeSets.Add(Set);
}

void FBSGrantedAbilitySet::TakeFromAbilitySystem(UBSAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	for (UAttributeSet* Set : GrantedAttributeSets)
	{
		ASC->RemoveSpawnedAttribute(Set);
	}

	AbilitySpecHandles.Reset();
	GameplayEffectHandles.Reset();
	GrantedAttributeSets.Reset();
}

FGameplayAbilitySpec* FBSGrantedAbilitySet::FindAbilitySpecFromHandle(UBSAbilitySystemComponent* ASC)
{
	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			return ASC->FindAbilitySpecFromHandle(Handle);
		}
	}
	return nullptr;
}

bool FBSGrantedAbilitySet::IsEmpty() const
{
	return AbilitySpecHandles.IsEmpty() && GameplayEffectHandles.IsEmpty() && GrantedAttributeSets.IsEmpty();
}

/* -------------------- */
/* -- BS Ability Set -- */
/* -------------------- */

UBSAbilitySet::UBSAbilitySet(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UBSAbilitySet::GiveToAbilitySystem(UBSAbilitySystemComponent* ASC, FBSGrantedAbilitySet* OutGrantedHandles,
	UObject* SourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBSAbilitySetGameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex,
				*GetNameSafe(this));
			continue;
		}

		UBSGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBSGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);
		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);

		if (OutGrantedHandles && AbilitySpecHandle.IsValid())
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FBSAbilitySetGameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex,
				*GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect,
			EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		if (OutGrantedHandles && GameplayEffectHandle.IsValid())
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FBSAbilitySetAttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex,
				*GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);

		if (OutGrantedHandles && IsValid(NewSet))
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
