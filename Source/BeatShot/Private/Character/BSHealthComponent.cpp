// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Character/BSHealthComponent.h"
#include "Target/Target.h"
#include "AbilitySystem/Globals/BSAttributeSetBase.h"
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

void UBSHealthComponent::OnHealthAttributeChanged(AActor* EffectInstigator, AActor* EffectCauser, const FGameplayEffectSpec* EffectSpec, float EffectMagnitude, float OldValue, float NewValue)
{
	OnHealthChanged.Broadcast(EffectInstigator, OldValue, NewValue);
}

void UBSHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBSHealthComponent::InitializeWithAbilitySystem(UBSAbilitySystemComponent* InASC)
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
	AttributeSetBase->OnHealthChanged.AddUObject(this, &ThisClass::OnHealthAttributeChanged);
}
