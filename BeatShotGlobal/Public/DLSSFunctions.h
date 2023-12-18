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

/** Replacement function to set the DLSSMode since UDLSSLibrary::SetDLSSMode is deprecated */
static void SetDLSSMode(const UDLSSMode InDLSSMode, const bool bRestoreFullResWhenDisabled = true)
{
	if (UDLSSLibrary::IsDLSSSupported())
	{
		const FVector2d ScreenResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
		bool bIsSupported;
		float OptimalScreenPercentage;
		bool bIsFixedScreenPercentage;
		float MinScreenPercentage;
		float MaxScreenPercentage;
		float OptimalSharpness;

		UDLSSLibrary::GetDLSSModeInformation(InDLSSMode, ScreenResolution, bIsSupported, OptimalScreenPercentage,
			bIsFixedScreenPercentage, MinScreenPercentage, MaxScreenPercentage, OptimalSharpness);

		bIsSupported = bIsSupported || InDLSSMode == UDLSSMode::Auto;
		const bool bIsDLAA = InDLSSMode == UDLSSMode::DLAA;
		const bool bShouldEnable = (InDLSSMode != UDLSSMode::Off || bIsDLAA) && bIsSupported;
		const bool bValidScreenPercentage = OptimalScreenPercentage > 0.f && bIsSupported;

		// Enable/Disable DLSS
		UDLSSLibrary::EnableDLSS(bShouldEnable);

		// Set Screen Percentage
		const float ScreenPercentage = bIsDLAA || !bValidScreenPercentage ? 100.f : OptimalScreenPercentage;
		if (bShouldEnable || bRestoreFullResWhenDisabled)
		{
			if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(
				TEXT("r.ScreenPercentage")))
			{
				CVarScreenPercentage->Set(ScreenPercentage);
			}
		}
	}
	else
	{
		if (bRestoreFullResWhenDisabled)
		{
			if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(
				TEXT("r.ScreenPercentage")))
			{
				CVarScreenPercentage->Set(100.f);
			}
		}
	}
}

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
			const FIntPoint ScreenResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
			bool bIsSupported;
			float OptimalScreenPercentage;
			bool bIsFixedScreenPercentage;
			float MinScreenPercentage;
			float MaxScreenPercentage;
			float OptimalSharpness;

			UDLSSLibrary::GetDLSSModeInformation(VideoSettings.DLSSMode,
				FVector2d(ScreenResolution.X, ScreenResolution.Y), bIsSupported, OptimalScreenPercentage,
				bIsFixedScreenPercentage, MinScreenPercentage, MaxScreenPercentage, OptimalSharpness);

			bIsSupported = bIsSupported || VideoSettings.DLSSMode == UDLSSMode::Auto;
			const bool bIsDLAA = VideoSettings.DLSSMode == UDLSSMode::DLAA;
			const bool bShouldEnable = (VideoSettings.DLSSMode != UDLSSMode::Off || bIsDLAA) && bIsSupported;
			const bool bValidScreenPercentage = OptimalScreenPercentage > 0.f && bIsSupported;

			// Enable/Disable DLSS
			UDLSSLibrary::EnableDLSS(bShouldEnable);

			// Set Screen Percentage using Console Command
			if (bShouldEnable)
			{
				UDLSSLibrary::SetDLSSSharpness(VideoSettings.DLSSSharpness);
				if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(
					TEXT("r.ScreenPercentage")))
				{
					const float ScreenPercentage = bIsDLAA || !bValidScreenPercentage ? 100.f : OptimalScreenPercentage;
					CVarScreenPercentage->Set(ScreenPercentage);
				}
			}

			// Update DLSS Settings
			VideoSettings.DLSSEnabledMode = bShouldEnable ? EDLSSEnabledMode::On : EDLSSEnabledMode::Off;
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
