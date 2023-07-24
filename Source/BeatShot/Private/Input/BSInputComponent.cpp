// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Input/BSInputComponent.h"
#include "EnhancedInputSubsystems.h"

UBSInputComponent::UBSInputComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UBSInputComponent::AddInputMappings(const UBSInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	check(LocalPlayer);

	// Add any registered input mappings from the settings!
	FPlayerSettings_User Settings = LoadPlayerSettings().User;
	
	InputSubsystem->AddPlayerMappableConfig(InputConfig->PlayerMappableInputConfig);
	
	// Tell enhanced input about any custom keymappings that the player may have customized
	for (const TPair<FName, FKey>& Pair : Settings.Keybindings)
	{
		if (Pair.Key != NAME_None && Pair.Value.IsValid())
		{
			InputSubsystem->AddPlayerMappedKeyInSlot(Pair.Key, Pair.Value);
		}
	}
}

void UBSInputComponent::RemoveInputMappings(const UBSInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	check(LocalPlayer);

	FPlayerSettings_User Settings = LoadPlayerSettings().User;

	// Clear any player mapped keys from enhanced input
	for (const TPair<FName, FKey>& Pair : Settings.Keybindings)
	{
		InputSubsystem->RemovePlayerMappedKeyInSlot(Pair.Key);
	}

	InputSubsystem->RemovePlayerMappableConfig(InputConfig->PlayerMappableInputConfig);
}

void UBSInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
