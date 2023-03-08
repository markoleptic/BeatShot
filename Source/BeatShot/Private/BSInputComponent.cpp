// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "BSInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

	// Add any registered input mappings from the settings!
	/*if (UGameUserSettings* LocalSettings = UGameUserSettings::GetGameUserSettings())
	{	
		// Tell enhanced input about any custom keymappings that the player may have customized
		for (const TPair<FName, FKey>& Pair : LocalSettings)
		{
			if (Pair.Key != NAME_None && Pair.Value.IsValid())
			{
				InputSubsystem->AddPlayerMappedKey(Pair.Key, Pair.Value);
			}
		}
	}*/
}

void UBSInputComponent::RemoveInputMappings(const UBSInputConfig* InputConfig,
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	ULocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer();
	check(LocalPlayer);
	
	/*if (UGameUserSettings* LocalSettings = UGameUserSettings::GetGameUserSettings())
	{
		// Remove any registered input contexts
		const TArray<FLoadedMappableConfigPair>& Configs = LocalSettings->GetAllRegisteredInputConfigs();
		for (const FLoadedMappableConfigPair& Pair : Configs)
		{
			InputSubsystem->RemovePlayerMappableConfig(Pair.Config);
		}
		
		// Clear any player mapped keys from enhanced input
		for (const TPair<FName, FKey>& Pair : LocalSettings->GetCustomPlayerInputConfig())
		{
			InputSubsystem->RemovePlayerMappedKey(Pair.Key);
		}
	}*/
}

void UBSInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
