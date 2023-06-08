// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Character/BSHealthComponent.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/BSAbilitySystemGlobals.h"
#include "Target/SphereTarget.h"
#include "BeatShot/BSGameplayTags.h"
#include "AbilitySystem/AttributeSets/BSAttributeSetBase.h"
#include "GameFramework/Actor.h"

UBSHealthComponent::UBSHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	AbilitySystemComponent = nullptr;
	AttributeSetBase = nullptr;
}

void UBSHealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBSHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	AActor* Instigator = nullptr;
	if (const FGameplayEffectSpec* Spec = ChangeData.GEModData ? &ChangeData.GEModData->EffectSpec : UBSAbilitySystemGlobals::GetTestGlobals().GetCurrentAppliedGE())
	{
		const FGameplayEffectContextHandle& EffectContext = Spec->GetEffectContext();
		Instigator = EffectContext.GetOriginalInstigator();
	}
	OnHealthChanged.Broadcast(Instigator, ChangeData.OldValue, ChangeData.NewValue);
}

void UBSHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
}

void UBSHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{
	OnOutOfHealth.Broadcast();
}

void UBSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBSHealthComponent::InitializeWithAbilitySystem(UBSAbilitySystemComponent* InASC, const FGameplayTagContainer& GameplayTagContainer)
{
	const AActor* Owner = GetOwner();
	check(Owner);

	if (AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Health component for owner [%s] has already been initialized with an ability system."), *GetNameSafe(Owner));
		return;
	}

	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Cannot initialize health component for owner [%s] with NULL ability system."), *GetNameSafe(Owner));
		return;
	}

	AttributeSetBase = AbilitySystemComponent->GetSet<UBSAttributeSetBase>();
	if (!AttributeSetBase)
	{
		UE_LOG(LogTemp, Error, TEXT("BSHealthComponent: Cannot initialize health component for owner [%s] with NULL health set on the ability system."), *GetNameSafe(Owner));
		return;
	}

	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UBSAttributeSetBase::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AttributeSetBase->OnHealthReachZero.AddUObject(this, &ThisClass::HandleOutOfHealth);
}
