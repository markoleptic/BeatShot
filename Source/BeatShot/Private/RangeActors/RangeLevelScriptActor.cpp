// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "RangeActors/RangeLevelScriptActor.h"
#include "BSGameInstance.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/PostProcessVolume.h"

ARangeLevelScriptActor::ARangeLevelScriptActor()
{
}

void ARangeLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority())
	{
		return;
	}

	UBSGameInstance* GI = Cast<UBSGameInstance>(GetGameInstance());
	GI->GetPublicVideoAndSoundSettingsChangedDelegate().AddUniqueDynamic(this, &ThisClass::OnPlayerSettingsChanged_VideoAndSound);
	OnPlayerSettingsChanged_VideoAndSound(LoadPlayerSettings().VideoAndSound);
}

void ARangeLevelScriptActor::OnPlayerSettingsChanged_VideoAndSound(const FPlayerSettings_VideoAndSound& VideoAndSoundSettings)
{
	if (PostProcessVolume)
	{
		PostProcessVolume->Settings.bOverride_AutoExposureBias = true;
		PostProcessVolume->Settings.AutoExposureBias = VideoAndSoundSettings.GetPostProcessBiasFromBrightness();
	}
}
