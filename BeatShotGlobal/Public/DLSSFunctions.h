// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "InputCoreTypes.h"
#include "SaveGamePlayerSettings.h"
#include "StreamlineLibraryDLSSG.h"
#include "StreamlineLibraryReflex.h"
#include "GameFramework/GameUserSettings.h"

/** Sets display settings using console variables according to the DLSSMode.
 *
 * @param InDLSSMode the mode adjust settings based off of
 * @param bRestoreFullResWhenDisabled whether or not to set primary screen percentage to 100 if DLSS is disabled
 * 
 * @returns Whether or not DLSS was enabled
 */
static bool SetDLSSMode(const UDLSSMode InDLSSMode, const bool bRestoreFullResWhenDisabled = true)
{
	bool bShouldEnable =  false;
	float ScreenPercentage = 100.f;
	
	if (UDLSSLibrary::IsDLSSSupported())
	{
		const FIntPoint ScreenResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
		bool bIsSupported;
		float OptimalScreenPercentage;
		bool bIsFixedScreenPercentage;
		float MinScreenPercentage;
		float MaxScreenPercentage;
		float OptimalSharpness;

		UDLSSLibrary::GetDLSSModeInformation(InDLSSMode, FVector2D(ScreenResolution.X, ScreenResolution.Y),
			bIsSupported, OptimalScreenPercentage, bIsFixedScreenPercentage, MinScreenPercentage,
			MaxScreenPercentage, OptimalSharpness);

		bIsSupported = bIsSupported || InDLSSMode == UDLSSMode::Auto;
		const bool bIsDLAA = InDLSSMode == UDLSSMode::DLAA;
		bShouldEnable = (InDLSSMode != UDLSSMode::Off || bIsDLAA) && bIsSupported;
		const bool bValidScreenPercentage = OptimalScreenPercentage > 0.f && bIsSupported;

		// Enable/Disable DLSS
		UDLSSLibrary::EnableDLSS(bShouldEnable);

		// Set screen percentage to 100 if DLAA mode or invalid screen percentage
		ScreenPercentage = bIsDLAA || !bValidScreenPercentage ? 100.f : OptimalScreenPercentage;
	}

	if (bShouldEnable || bRestoreFullResWhenDisabled)
	{
		if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(
			TEXT("r.ScreenPercentage")))
		{
			CVarScreenPercentage->Set(ScreenPercentage);
		}
	}
	
	return bShouldEnable;
}

/** Initializes DLSS settings for a FPlayerSettings_VideoAndSound struct, if DLSS is supported.
 *
 *  @param VideoSettings the settings to initialize
 */
static void InitDLSSSettings(FPlayerSettings_VideoAndSound& VideoSettings)
{
	// DLSS
	if (UDLSSLibrary::IsDLSSSupported())
	{
		// Fallback to auto if mode is invalid for whatever reason
		if (!UDLSSLibrary::IsDLSSModeSupported(VideoSettings.DLSSMode) && VideoSettings.DLSSMode != UDLSSMode::Auto)
		{
			VideoSettings.DLSSMode = UDLSSMode::Auto;
		}

		if (UDLSSLibrary::IsDLSSModeSupported(VideoSettings.DLSSMode) || VideoSettings.DLSSMode == UDLSSMode::Auto)
		{
			const bool bWasEnabled = SetDLSSMode(VideoSettings.DLSSMode);

			// Update DLSS Settings
			VideoSettings.DLSSEnabledMode = bWasEnabled ? EDLSSEnabledMode::On : EDLSSEnabledMode::Off;
		}
		else
		{
			// Update DLSS Settings
			VideoSettings.DLSSEnabledMode = EDLSSEnabledMode::Off;
			VideoSettings.DLSSMode = UDLSSMode::Off;
		}
	}
	else
	{
		// Update DLSS Settings
		VideoSettings.DLSSEnabledMode = EDLSSEnabledMode::Off;
		VideoSettings.DLSSMode = UDLSSMode::Off;
	}

	// Frame Generation
	if (UStreamlineLibraryDLSSG::IsDLSSGSupported() && UStreamlineLibraryDLSSG::IsDLSSGModeSupported(
		VideoSettings.FrameGenerationEnabledMode))
	{
		UStreamlineLibraryDLSSG::SetDLSSGMode(VideoSettings.FrameGenerationEnabledMode);
	}
	else
	{
		UStreamlineLibraryDLSSG::SetDLSSGMode(UStreamlineDLSSGMode::Off);
		VideoSettings.FrameGenerationEnabledMode = UStreamlineDLSSGMode::Off;
	}

	// NIS
	if (UNISLibrary::IsNISSupported() && UNISLibrary::IsNISModeSupported(VideoSettings.NISMode) && VideoSettings.
		DLSSEnabledMode == EDLSSEnabledMode::Off)
	{
		UNISLibrary::SetNISMode(VideoSettings.NISMode);
		UNISLibrary::SetNISSharpness(VideoSettings.NISSharpness);
	}
	else
	{
		UNISLibrary::SetNISMode(UNISMode::Off);
		VideoSettings.NISMode = UNISMode::Off;
	}

	// Reflex
	if (UStreamlineLibraryReflex::IsReflexSupported())
	{
		UStreamlineLibraryReflex::SetReflexMode(VideoSettings.StreamlineReflexMode);
	}
	else
	{
		UStreamlineLibraryReflex::SetReflexMode(UStreamlineReflexMode::Disabled);
		VideoSettings.StreamlineReflexMode = UStreamlineReflexMode::Disabled;
	}
}