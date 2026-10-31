// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "Player/BSLocalPlayer.h"
#include "AudioMixerBlueprintLibrary.h"
#include "BSGameUserSettings.h"

DEFINE_LOG_CATEGORY(LogBSLocalPlayer);

UBSLocalPlayer::UBSLocalPlayer()
{
}

void UBSLocalPlayer::PostInitProperties()
{
	Super::PostInitProperties();

	if (UBSGameUserSettings* LocalSettings = GetLocalSettings())
	{
		LocalSettings->OnAudioOutputDeviceChanged.AddUObject(this, &UBSLocalPlayer::OnAudioOutputDeviceChanged);
	}
}

UBSGameUserSettings* UBSLocalPlayer::GetLocalSettings() const
{
	return UBSGameUserSettings::Get();
}

void UBSLocalPlayer::OnAudioOutputDeviceChanged(const FString& InAudioOutputDeviceId)
{
	FOnCompletedDeviceSwap DevicesSwappedCallback;
	DevicesSwappedCallback.BindUFunction(this, FName("OnCompletedAudioDeviceSwap"));
	UAudioMixerBlueprintLibrary::SwapAudioOutputDevice(GetWorld(), InAudioOutputDeviceId, DevicesSwappedCallback);
}

void UBSLocalPlayer::OnCompletedAudioDeviceSwap(const FSwapAudioOutputResult& SwapResult)
{
	if (SwapResult.Result == ESwapAudioOutputDeviceResultState::Failure)
	{
		UE_LOG(LogBSLocalPlayer, Warning, TEXT("Failed to swap audio device"));
	}
	else
	{
		UE_LOG(LogBSLocalPlayer, Warning, TEXT("Successfully swapped audio device"));
	}
}
