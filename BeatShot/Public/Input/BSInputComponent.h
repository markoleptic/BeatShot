// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "Input/BSInputConfig.h"
#include "Containers/Array.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "HAL/Platform.h"
#include "InputTriggers.h"
#include "SaveLoadInterface.h"
#include "Misc/AssertionMacros.h"
#include "BSInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;

/**
 *	Component used to manage input mappings, bindings, and GameplayTags using an input config data asset.
 */
UCLASS()
class UBSInputComponent : public UEnhancedInputComponent, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	UBSInputComponent(const FObjectInitializer& ObjectInitializer);

	/** Adds player mapped keys to the InputSubsystem. */
	void AddInputMappings(const UBSInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	/** Removes player mapped keys to the InputSubsystem */
	void RemoveInputMappings(const UBSInputConfig* InputConfig,
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	
	/** Binds an (EnhancedInputAction, GameplayTag) pair to the specified PressedTriggerEvent. */
	template <class UserClass, typename FuncType>
	void BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object,
		FuncType Func, bool bLogIfNotFound);

	/** Binds an (EnhancedInputAction, GameplayTag) pair to the specified PressedTriggerEvent, along with
	 *  binding the PressedTriggerEvent and ETriggerEvent::Completed. */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object,
		PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, bool bLogIfNotFound);

	/** Binds an array of (EnhancedInputAction, GameplayTags) pair to the specified trigger events,
	 *  and associates a GameplayTag with it. */
	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UBSInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc,
		ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	/** Removes a native action binding by finding the input tag in the NativeActionBindings map */
	void RemoveBind(const FGameplayTag& InputTag);

	void RemoveBinds(TArray<uint32>& BindHandles);

protected:
	TMap<FGameplayTag, TArray<uint32>> NativeActionBindings;
};


template <class UserClass, typename FuncType>
void UBSInputComponent::BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag,
	UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	const FBSInputAction IA = InputConfig->FindBSInputActionForTag(InputTag, bLogIfNotFound);
	if (IA.InputAction)
	{
		NativeActionBindings.FindOrAdd(InputTag).Add(BindAction(IA.InputAction, IA.PressedTriggerEvent, Object, Func).GetHandle());
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBSInputComponent::BindNativeAction(const UBSInputConfig* InputConfig, const FGameplayTag& InputTag, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, bool bLogIfNotFound)
{
	check(InputConfig);
	const FBSInputAction IA = InputConfig->FindBSInputActionForTag(InputTag, bLogIfNotFound);
	if (IA.InputAction && IA.InputTag.IsValid())
	{
		if (PressedFunc)
		{
			NativeActionBindings.FindOrAdd(InputTag).Add(BindAction(IA.InputAction, IA.PressedTriggerEvent, Object, PressedFunc).GetHandle());
		}
		if (ReleasedFunc)
		{
			NativeActionBindings.FindOrAdd(InputTag).Add(BindAction(IA.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc).GetHandle());
		}
	}
}

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UBSInputComponent::BindAbilityActions(const UBSInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FBSInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, Action.PressedTriggerEvent, Object, PressedFunc,
					Action.InputTag).GetHandle());
			}

			if (ReleasedFunc)
			{
				BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,
					Action.InputTag).GetHandle());
			}
		}
	}
}
