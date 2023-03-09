#include "GameplayAbility/BSAbilitySet.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayAbility/BSAbilitySystemComponent.h"
#include "GameplayAbility/BSGameplayAbility.h"

void FBSAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		const int32 NumAdded = AbilitySpecHandles.Add(Handle);
		UE_LOG(LogTemp, Display, TEXT("Number of AbilitySpecHandles added: %d"), NumAdded);
	}
}

void FBSAbilitySet_GrantedHandles::AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle)
{
	if (Handle.IsValid())
	{
		const int32 NumAdded = GameplayEffectHandles.Add(Handle);
		UE_LOG(LogTemp, Display, TEXT("Number of GameplayEffectHandles added: %d"), NumAdded);
	}
}

void FBSAbilitySet_GrantedHandles::AddAttributeSet(UAttributeSet* Set)
{
	const int32 NumAdded = GrantedAttributeSets.Add(Set);
	UE_LOG(LogTemp, Display, TEXT("Number of AttributeSets added: %d"), NumAdded);
}

void FBSAbilitySet_GrantedHandles::TakeFromAbilitySystem(UBSAbilitySystemComponent* ASC)
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		// Must be authoritative to give or take ability sets.
		UE_LOG(LogTemp, Display, TEXT("Must be authoritative to give or take ability sets from %s"), *FString(__FUNCTION__));
		return;
	}

	for (const FGameplayAbilitySpecHandle& Handle : AbilitySpecHandles)
	{
		if (Handle.IsValid())
		{
			ASC->ClearAbility(Handle);
			UE_LOG(LogTemp, Display, TEXT("Cleared a valid FGameplayAbilitySpecHandle inside AbilitySpecHandles %s"), *FString(__FUNCTION__));
		}
	}

	for (const FActiveGameplayEffectHandle& Handle : GameplayEffectHandles)
	{
		if (Handle.IsValid())
		{
			ASC->RemoveActiveGameplayEffect(Handle);
			UE_LOG(LogTemp, Display, TEXT("Cleared a valid FActiveGameplayEffectHandle inside GameplayEffectHandles %s"), *FString(__FUNCTION__));
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

UBSAbilitySet::UBSAbilitySet(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
}

void UBSAbilitySet::GiveToAbilitySystem(UBSAbilitySystemComponent* ASC,
	FBSAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(ASC);

	if (!ASC->IsOwnerActorAuthoritative())
	{
		UE_LOG(LogTemp, Display, TEXT("Must be authoritative to give or take ability sets %s"), *FString(__FUNCTION__));
		// Must be authoritative to give or take ability sets.
		return;
	}

	// Grant the gameplay abilities.
	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FBSAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		UBSGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<UBSGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
		
		UE_LOG(LogTemp, Display, TEXT("Granted AbilitySpecHandle: %s %s"),*AbilitySpecHandle.ToString(), *FString(__FUNCTION__));
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAbilitySpecHandle(AbilitySpecHandle);
		}
	}

	// Grant the gameplay effects.
	for (int32 EffectIndex = 0; EffectIndex < GrantedGameplayEffects.Num(); ++EffectIndex)
	{
		const FBSAbilitySet_GameplayEffect& EffectToGrant = GrantedGameplayEffects[EffectIndex];

		if (!IsValid(EffectToGrant.GameplayEffect))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayEffects[%d] on ability set [%s] is not valid"), EffectIndex, *GetNameSafe(this));
			continue;
		}

		const UGameplayEffect* GameplayEffect = EffectToGrant.GameplayEffect->GetDefaultObject<UGameplayEffect>();
		const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffect, EffectToGrant.EffectLevel, ASC->MakeEffectContext());
		UE_LOG(LogTemp, Display, TEXT("Granted GameplayEffectHandle: %s %s"),*GameplayEffectHandle.ToString(), *FString(__FUNCTION__));
		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddGameplayEffectHandle(GameplayEffectHandle);
		}
	}

	// Grant the attribute sets.
	for (int32 SetIndex = 0; SetIndex < GrantedAttributes.Num(); ++SetIndex)
	{
		const FBSAbilitySet_AttributeSet& SetToGrant = GrantedAttributes[SetIndex];

		if (!IsValid(SetToGrant.AttributeSet))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedAttributes[%d] on ability set [%s] is not valid"), SetIndex, *GetNameSafe(this));
			continue;
		}

		UAttributeSet* NewSet = NewObject<UAttributeSet>(ASC->GetOwner(), SetToGrant.AttributeSet);
		ASC->AddAttributeSetSubobject(NewSet);
		NewSet->PrintDebug();

		if (OutGrantedHandles)
		{
			OutGrantedHandles->AddAttributeSet(NewSet);
		}
	}
}
