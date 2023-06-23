// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "AbilitySystem/Globals/BSAbilitySystemGlobals.h"
#include "GameplayEffect.h"

void UBSAbilitySystemGlobals::PushCurrentAppliedGE(const FGameplayEffectSpec* Spec, UAbilitySystemComponent* AbilitySystemComponent)
{
	check(IsInGameThread());
	CurrentGameplayEffectSpecStack.Push(*Spec);
}

void UBSAbilitySystemGlobals::SetCurrentAppliedGE(const FGameplayEffectSpec* Spec)
{
	check(IsInGameThread());
	check(CurrentGameplayEffectSpecStack.Num() > 0);

	FGameplayEffectSpec& CurrentSpec = CurrentGameplayEffectSpecStack.Top();
	CurrentSpec = *Spec;
}

void UBSAbilitySystemGlobals::PopCurrentAppliedGE()
{
	check(IsInGameThread());
	CurrentGameplayEffectSpecStack.Pop();
}

const FGameplayEffectSpec* UBSAbilitySystemGlobals::GetCurrentAppliedGE()
{
	check(IsInGameThread());

	FGameplayEffectSpec* Spec = nullptr;
	if (CurrentGameplayEffectSpecStack.Num() > 0)
	{
		Spec = &CurrentGameplayEffectSpecStack.Top();
	}

	return Spec;
}

UBSAbilitySystemGlobals& UBSAbilitySystemGlobals::GetTestGlobals()
{
	return *CastChecked<UBSAbilitySystemGlobals>(IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals());
}
