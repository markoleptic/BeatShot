// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Input/BSInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

UBSInputComponent::UBSInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UBSInputComponent::AddInputMappings(const UBSInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	check(LocalPlayer);
	
	FPlayerSettings_User UserSettings = LoadPlayerSettings().User;

	UEnhancedInputUserSettings* EnhancedInputUserSettings = InputSubsystem->GetUserSettings();
	
	for (const TPair<FName, FKey>& Pair : UserSettings.Keybindings)
	{
		if (Pair.Key != NAME_None && Pair.Value.IsValid())
		{
			FGameplayTagContainer Failure;
			FMapPlayerKeyArgs Args;
			Args.MappingName = Pair.Key;
			Args.NewKey = Pair.Value;
			Args.Slot = EPlayerMappableKeySlot::First;
			EnhancedInputUserSettings->MapPlayerKey(Args, Failure);
		}
	}
}

void UBSInputComponent::RemoveInputMappings(const UBSInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	check(LocalPlayer);

	FPlayerSettings_User Settings = LoadPlayerSettings().User;
	UEnhancedInputUserSettings* EnhancedInputUserSettings = InputSubsystem->GetUserSettings();

	// Clear any player mapped keys from enhanced input
	for (const TPair<FName, FKey>& Pair : Settings.Keybindings)
	{
		FGameplayTagContainer Failure;
		FMapPlayerKeyArgs Args;
		Args.MappingName = Pair.Key;
		Args.NewKey = Pair.Value;
		Args.Slot = EPlayerMappableKeySlot::First;
		EnhancedInputUserSettings->UnMapPlayerKey(Args, Failure);
	}
}

void UBSInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
