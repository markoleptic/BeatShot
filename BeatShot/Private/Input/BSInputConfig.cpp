﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#include "Input/BSInputConfig.h"

UBSInputConfig::UBSInputConfig()
{
}

const UInputAction* UBSInputConfig::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FBSInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."),
			*InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UBSInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FBSInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."),
			*InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

FBSInputAction UBSInputConfig::FindBSInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FBSInputAction& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find BSInputAction for InputTag [%s] on InputConfig [%s]."),
			*InputTag.ToString(), *GetNameSafe(this));
	}

	return FBSInputAction();
}
