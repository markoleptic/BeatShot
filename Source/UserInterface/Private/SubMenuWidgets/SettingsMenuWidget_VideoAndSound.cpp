// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/SettingsMenuWidget_VideoAndSound.h"
#include "BSWidgetInterface.h"
#include "StreamlineLibraryDLSSG.h"
#include "StreamlineLibraryReflex.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/BSButton.h"
#include "WidgetComponents/BSComboBoxString.h"
#include "WidgetComponents/BSComboBoxEntry.h"
#include "WidgetComponents/SavedTextWidget.h"

ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSSupport, UDLSSSupport::Supported, UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive);
ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSMode, UDLSSMode::Off, UDLSSMode::UltraPerformance);
ENUM_RANGE_BY_FIRST_AND_LAST(UNISMode, UNISMode::Off, UNISMode::Custom);
ENUM_RANGE_BY_FIRST_AND_LAST(UStreamlineReflexMode, UStreamlineReflexMode::Disabled, UStreamlineReflexMode::EnabledPlusBoost);
ENUM_RANGE_BY_FIRST_AND_LAST(UStreamlineDLSSGMode, UStreamlineDLSSGMode::Off, UStreamlineDLSSGMode::On);

void USettingsMenuWidget_VideoAndSound::NativeConstruct()
{
	Super::NativeConstruct();

	{
		AAQ0->SetDefaults(EVideoSettingType::AntiAliasing, 0, AAQ1);
		AAQ1->SetDefaults(EVideoSettingType::AntiAliasing, 1, AAQ2);
		AAQ2->SetDefaults(EVideoSettingType::AntiAliasing, 2, AAQ3);
		AAQ3->SetDefaults(EVideoSettingType::AntiAliasing, 3, AAQ0);
		
		AAQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		AAQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		AAQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		AAQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);

		GIQ0->SetDefaults(EVideoSettingType::GlobalIllumination, 0, GIQ1);
		GIQ1->SetDefaults(EVideoSettingType::GlobalIllumination, 1, GIQ2);
		GIQ2->SetDefaults(EVideoSettingType::GlobalIllumination, 2, GIQ3);
		GIQ3->SetDefaults(EVideoSettingType::GlobalIllumination, 3, GIQ0);
		
		GIQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		GIQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		GIQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		GIQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);

		PPQ0->SetDefaults(EVideoSettingType::PostProcessing, 0, PPQ1);
		PPQ1->SetDefaults(EVideoSettingType::PostProcessing, 1, PPQ2);
		PPQ2->SetDefaults(EVideoSettingType::PostProcessing, 2, PPQ3);
		PPQ3->SetDefaults(EVideoSettingType::PostProcessing, 3, PPQ0);
		
		PPQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		PPQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		PPQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		PPQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);

		RQ0->SetDefaults(EVideoSettingType::Reflection, 0, RQ1);
		RQ1->SetDefaults(EVideoSettingType::Reflection, 1, RQ2);
		RQ2->SetDefaults(EVideoSettingType::Reflection, 2, RQ3);
		RQ3->SetDefaults(EVideoSettingType::Reflection, 3, RQ0);
		
		RQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		RQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		RQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		RQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		
		SWQ0->SetDefaults(EVideoSettingType::Shadow, 0, SWQ1);
		SWQ1->SetDefaults(EVideoSettingType::Shadow, 1, SWQ2);
		SWQ2->SetDefaults(EVideoSettingType::Shadow, 2, SWQ3);
		SWQ3->SetDefaults(EVideoSettingType::Shadow, 3, SWQ0);

		SWQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SWQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SWQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SWQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		
		SGQ0->SetDefaults(EVideoSettingType::Shading, 0, SGQ1);
		SGQ1->SetDefaults(EVideoSettingType::Shading, 1, SGQ2);
		SGQ2->SetDefaults(EVideoSettingType::Shading, 2, SGQ3);
		SGQ3->SetDefaults(EVideoSettingType::Shading, 3, SGQ0);

		SGQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SGQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SGQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		SGQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		
		TQ0->SetDefaults(EVideoSettingType::Texture, 0, TQ1);
		TQ1->SetDefaults(EVideoSettingType::Texture, 1, TQ2);
		TQ2->SetDefaults(EVideoSettingType::Texture, 2, TQ3);
		TQ3->SetDefaults(EVideoSettingType::Texture, 3, TQ0);

		TQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		TQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		TQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		TQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		
		VDQ0->SetDefaults(EVideoSettingType::ViewDistance, 0, VDQ1);
		VDQ1->SetDefaults(EVideoSettingType::ViewDistance, 1, VDQ2);
		VDQ2->SetDefaults(EVideoSettingType::ViewDistance, 2, VDQ3);
		VDQ3->SetDefaults(EVideoSettingType::ViewDistance, 3, VDQ0);

		VDQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VDQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VDQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VDQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		
		VEQ0->SetDefaults(EVideoSettingType::VisualEffect, 0, VEQ1);
		VEQ1->SetDefaults(EVideoSettingType::VisualEffect, 1, VEQ2);
		VEQ2->SetDefaults(EVideoSettingType::VisualEffect, 2, VEQ3);
		VEQ3->SetDefaults(EVideoSettingType::VisualEffect, 3, VEQ0);

		VEQ0->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VEQ1->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VEQ2->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
		VEQ3->OnBSButtonPressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality);
	}
	
	SetupTooltip(QMark_DLSS, GetTooltipTextFromKey("DLSS"));
	SetupTooltip(QMark_FrameGeneration, GetTooltipTextFromKey("DLSS_FrameGeneration"));
	SetupTooltip(QMark_SuperResolution, GetTooltipTextFromKey("DLSS_SuperResolution"));
	SetupTooltip(QMark_NIS, GetTooltipTextFromKey("NIS"));
	SetupTooltip(QMark_Reflex, GetTooltipTextFromKey("Reflex"));
	
	Slider_GlobalSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_GlobalSound);
	Slider_MenuSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_MenuSound);
	Slider_MusicSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_MusicSound);
	Slider_DLSS_Sharpness->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_DLSS_Sharpness);
	Slider_NIS_Sharpness->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_NIS_Sharpness);
	Slider_ResolutionScale->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_ResolutionScale);

	Slider_GlobalSound->SetMinValue(MinValue_Volume);
	Slider_GlobalSound->SetMaxValue(MaxValue_Volume);
	Slider_GlobalSound->SetStepSize(SnapSize_Volume);
	
	Slider_MenuSound->SetMinValue(MinValue_Volume);
	Slider_MenuSound->SetMaxValue(MaxValue_Volume);
	Slider_MenuSound->SetStepSize(SnapSize_Volume);
	
	Slider_MusicSound->SetMinValue(MinValue_Volume);
	Slider_MusicSound->SetMaxValue(MaxValue_Volume);
	Slider_MusicSound->SetStepSize(SnapSize_Volume);
	
	Slider_DLSS_Sharpness->SetMinValue(MinValue_DLSSSharpness);
	Slider_DLSS_Sharpness->SetMaxValue(MaxValue_DLSSSharpness);
	Slider_DLSS_Sharpness->SetStepSize(SnapSize_DLSSSharpness);
	
	Slider_NIS_Sharpness->SetMinValue(MinValue_NISSharpness);
	Slider_NIS_Sharpness->SetMaxValue(MaxValue_NISSharpness);
	Slider_NIS_Sharpness->SetStepSize(SnapSize_NISSharpness);

	const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	float CurrentScaleNormalized;
	float CurrentScale;
	float MinScale;
	float MaxScale;
	Settings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScale, MinScale, MaxScale);
	Slider_ResolutionScale->SetMinValue(MinScale / 100.f);
	Slider_ResolutionScale->SetMaxValue(MaxScale / 100.f);
	
	Value_GlobalSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_GlobalSound);
	Value_MenuSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_MenuSound);
	Value_MusicSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_MusicSound);
	Value_DLSS_Sharpness->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_DLSS_Sharpness);
	Value_NIS_Sharpness->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_NIS_Sharpness);
	Value_ResolutionScale->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_ResolutionScale);
	
	ComboBox_WindowMode->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_WindowMode);
	ComboBox_Resolution->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Resolution);
	ComboBox_DLSS->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_DLSS);
	ComboBox_FrameGeneration->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_FrameGeneration);
	ComboBox_SuperResolution->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_SuperResolution);
	ComboBox_NIS->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS);
	ComboBox_NIS_Mode->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS_Mode);
	ComboBox_Reflex->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Reflex);

	ComboBox_DLSS->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_FrameGeneration->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_SuperResolution->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_NIS->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_NIS_Mode->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	ComboBox_Reflex->OnGenerateWidgetEventDelegate.BindDynamic(this, &ThisClass::OnGenerateWidgetEvent);
	
	ComboBox_DLSS->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_FrameGeneration->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_SuperResolution->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_NIS->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_NIS_Mode->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	ComboBox_Reflex->OnSelectionChanged_GenerateWidgetForMultiSelection.BindDynamic(this, &ThisClass::OnSelectionChanged_GenerateMultiSelectionItem);
	
	Value_FrameLimitMenu->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitMenu);
	CheckBox_VSyncEnabled->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_VSyncEnabled);
	
	Button_Reset->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnBSButtonPressed_SaveReset);
	Button_Save->OnBSButtonPressed.AddDynamic(this, &ThisClass::OnBSButtonPressed_SaveReset);
	
	Button_Reset->SetDefaults(static_cast<uint8>(ESettingButtonType::Reset));
	Button_Save->SetDefaults(static_cast<uint8>(ESettingButtonType::Save));

	// DLSS On/Off
	ComboBox_DLSS->AddOption(UEnum::GetDisplayValueAsText(EDLSSEnabledMode::Off).ToString());
	if (UDLSSLibrary::IsDLSSSupported())
	{
		ComboBox_DLSS->AddOption(UEnum::GetDisplayValueAsText(EDLSSEnabledMode::On).ToString());
	}
	// Frame Generation
	for (const UStreamlineDLSSGMode Mode : UStreamlineLibraryDLSSG::GetSupportedDLSSGModes())
	{
		ComboBox_FrameGeneration->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
	}
	// Super Resolution (DLSS Modes)
	ComboBox_SuperResolution->AddOption(UEnum::GetDisplayValueAsText(UDLSSMode::Off).ToString());
	ComboBox_SuperResolution->AddOption(UEnum::GetDisplayValueAsText(UDLSSMode::Auto).ToString());
	for (const UDLSSMode Mode : UDLSSLibrary::GetSupportedDLSSModes())
	{
		if (Mode != UDLSSMode::Off && Mode != UDLSSMode::Auto)
		{
			ComboBox_SuperResolution->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
		}
	}
	// Make DLAA the last option
	if (ComboBox_SuperResolution->GetIndexOfOption(UEnum::GetDisplayValueAsText(UDLSSMode::DLAA).ToString()) != INDEX_NONE)
	{
		ComboBox_SuperResolution->RemoveOption(UEnum::GetDisplayValueAsText(UDLSSMode::DLAA).ToString());
		ComboBox_SuperResolution->AddOption(UEnum::GetDisplayValueAsText(UDLSSMode::DLAA).ToString());
	}
	// NIS On/Off
	ComboBox_NIS->AddOption(UEnum::GetDisplayValueAsText(ENISEnabledMode::Off).ToString());
	if (UNISLibrary::IsNISSupported())
	{
		ComboBox_NIS->AddOption(UEnum::GetDisplayValueAsText(ENISEnabledMode::On).ToString());
	}
	// NIS Modes
	for (const UNISMode Mode : UNISLibrary::GetSupportedNISModes())
	{
		if (Mode != UNISMode::Custom)
		{
			ComboBox_NIS_Mode->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
		}
	}
	// Reflex
	ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(UStreamlineReflexMode::Disabled).ToString());
	if (UStreamlineLibraryReflex::IsReflexSupported())
	{
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(UStreamlineReflexMode::Enabled).ToString());
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(UStreamlineReflexMode::EnabledPlusBoost).ToString());
	}
	
	UpdateBrushColors();
	InitializeVideoAndSoundSettings(LoadPlayerSettings().VideoAndSound);
}

TSharedRef<SWidget> USettingsMenuWidget_VideoAndSound::RebuildWidget()
{
	return Super::RebuildWidget();
}

void USettingsMenuWidget_VideoAndSound::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void USettingsMenuWidget_VideoAndSound::InitializeVideoAndSoundSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
	UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	
	Slider_GlobalSound->SetValue(InVideoAndSoundSettings.GlobalVolume);
	Slider_MenuSound->SetValue(InVideoAndSoundSettings.MenuVolume);
	Slider_MusicSound->SetValue(InVideoAndSoundSettings.MusicVolume);
	Slider_DLSS_Sharpness->SetValue(InVideoAndSoundSettings.DLSSSharpness);
	Slider_NIS_Sharpness->SetValue(InVideoAndSoundSettings.NISSharpness);
	Slider_ResolutionScale->SetValue(GameUserSettings->GetResolutionScaleNormalized());
	
	Value_GlobalSound->SetText(FText::AsNumber(InVideoAndSoundSettings.GlobalVolume));
	Value_MenuSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MenuVolume));
	Value_MusicSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MusicVolume));
	Value_FrameLimitGame->SetText(FText::AsNumber(InVideoAndSoundSettings.FrameRateLimitGame));
	Value_FrameLimitMenu->SetText(FText::AsNumber(InVideoAndSoundSettings.FrameRateLimitMenu));
	Value_DLSS_Sharpness->SetText(FText::AsNumber(InVideoAndSoundSettings.DLSSSharpness));
	Value_NIS_Sharpness->SetText(FText::AsNumber(InVideoAndSoundSettings.NISSharpness));
	Value_ResolutionScale->SetText(FText::AsNumber(GameUserSettings->GetResolutionScaleNormalized()));

	CheckBox_FPSCounter->SetIsChecked(InVideoAndSoundSettings.bShowFPSCounter);
	
	switch (UGameUserSettings::GetGameUserSettings()->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			ComboBox_WindowMode->SetSelectedOption("Fullscreen");
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			ComboBox_WindowMode->SetSelectedOption("Windowed Fullscreen");
			break;
		}
	case EWindowMode::Windowed:
		{
			ComboBox_WindowMode->SetSelectedOption("Windowed");
			break;
		}
	case EWindowMode::NumWindowModes:
		{
			break;
		}
	}

	PopulateResolutionComboBox();
	LastConfirmedResolution = GameUserSettings->GetScreenResolution();
	
	FindVideoSettingButton(GameUserSettings->GetAntiAliasingQuality(), EVideoSettingType::AntiAliasing)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetGlobalIlluminationQuality(), EVideoSettingType::GlobalIllumination)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetPostProcessingQuality(), EVideoSettingType::PostProcessing)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetReflectionQuality(), EVideoSettingType::Reflection)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetShadowQuality(), EVideoSettingType::Shadow)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetShadingQuality(), EVideoSettingType::Shading)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetTextureQuality(), EVideoSettingType::Texture)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetViewDistanceQuality(), EVideoSettingType::ViewDistance)->SetActive();
	FindVideoSettingButton(GameUserSettings->GetVisualEffectQuality(), EVideoSettingType::VisualEffect)->SetActive();

	ComboBox_DLSS->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.DLSSEnabledMode).ToString());
	ComboBox_FrameGeneration->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.FrameGenerationEnabledMode).ToString());
	ComboBox_SuperResolution->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.DLSSMode).ToString());
	ComboBox_NIS->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.NISEnabledMode).ToString());
	ComboBox_NIS_Mode->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.NISMode).ToString());
	ComboBox_Reflex->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.StreamlineReflexMode).ToString());

	CacheDLSSSettings(InVideoAndSoundSettings.DLSSEnabledMode == EDLSSEnabledMode::On);
	HandleDLSSEnabledChanged(InVideoAndSoundSettings.DLSSEnabledMode == EDLSSEnabledMode::On);
}

FPlayerSettings_VideoAndSound USettingsMenuWidget_VideoAndSound::GetVideoAndSoundSettings() const
{
	FPlayerSettings_VideoAndSound ReturnSettings;
	
	ReturnSettings.GlobalVolume = FMath::GridSnap(FMath::Clamp(Slider_GlobalSound->GetValue(), MinValue_Volume, MaxValue_Volume), SnapSize_Volume);
	ReturnSettings.MenuVolume = FMath::GridSnap(FMath::Clamp(Slider_MenuSound->GetValue(), MinValue_Volume, MaxValue_Volume), SnapSize_Volume);
	ReturnSettings.MusicVolume = FMath::GridSnap(FMath::Clamp(Slider_MusicSound->GetValue(), MinValue_Volume, MaxValue_Volume), SnapSize_Volume);
	ReturnSettings.DLSSEnabledMode = GetEnumFromString<EDLSSEnabledMode>(ComboBox_DLSS->GetSelectedOption(), EDLSSEnabledMode::Off);
	ReturnSettings.FrameGenerationEnabledMode = GetEnumFromString<UStreamlineDLSSGMode>(ComboBox_FrameGeneration->GetSelectedOption(), UStreamlineDLSSGMode::Off);
	ReturnSettings.DLSSMode = GetEnumFromString<UDLSSMode>(ComboBox_SuperResolution->GetSelectedOption(), UDLSSMode::Off);
	ReturnSettings.DLSSSharpness = FMath::GridSnap(FMath::Clamp(Slider_DLSS_Sharpness->GetValue(), MinValue_DLSSSharpness, MaxValue_DLSSSharpness), SnapSize_DLSSSharpness);
	ReturnSettings.NISEnabledMode = GetEnumFromString<ENISEnabledMode>(ComboBox_NIS->GetSelectedOption(), ENISEnabledMode::Off);
	ReturnSettings.NISMode = GetEnumFromString<UNISMode>(ComboBox_NIS_Mode->GetSelectedOption(), UNISMode::Off);
	ReturnSettings.NISSharpness = FMath::GridSnap(FMath::Clamp(Slider_NIS_Sharpness->GetValue(), MinValue_NISSharpness, MaxValue_NISSharpness), SnapSize_NISSharpness);
	ReturnSettings.StreamlineReflexMode = GetEnumFromString<UStreamlineReflexMode>(ComboBox_Reflex->GetSelectedOption(), UStreamlineReflexMode::Disabled);
	ReturnSettings.bShowFPSCounter = CheckBox_FPSCounter->IsChecked();
	ReturnSettings.FrameRateLimitGame = FMath::GridSnap(FMath::Clamp(FCString::Atof(*Value_FrameLimitGame->GetText().ToString()), MinValue_FrameRateLimit, MaxValue_FrameRateLimit), SnapSize_FrameRateLimit);
	ReturnSettings.FrameRateLimitMenu = FMath::GridSnap(FMath::Clamp(FCString::Atof(*Value_FrameLimitMenu->GetText().ToString()), MinValue_FrameRateLimit, MaxValue_FrameRateLimit), SnapSize_FrameRateLimit);
	return ReturnSettings;
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_GlobalSound(const float NewGlobalSound)
{
	const float Value = OnSliderChanged(NewGlobalSound, Value_GlobalSound, SnapSize_Volume);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / static_cast<float>(MaxValue_Volume), 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_MenuSound(const float NewMenuSound)
{
	const float Value = OnSliderChanged(NewMenuSound, Value_MenuSound, SnapSize_Volume);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / static_cast<float>(MaxValue_Volume), 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_MusicSound(const float NewMusicSound)
{
	OnSliderChanged(NewMusicSound, Value_MusicSound, SnapSize_Volume);
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_DLSS_Sharpness(const float NewValue)
{
	const float Value = OnSliderChanged(NewValue, Value_DLSS_Sharpness, SnapSize_DLSSSharpness);
	if (UDLSSLibrary::GetDLSSMode() != UDLSSMode::Off)
	{
		UDLSSLibrary::SetDLSSSharpness(Value);
	}
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_NIS_Sharpness(const float NewValue)
{
	const float Value = OnSliderChanged(NewValue, Value_NIS_Sharpness, SnapSize_NISSharpness);
	if (UDLSSLibrary::GetDLSSMode() == UDLSSMode::Off)
	{
		UNISLibrary::SetNISSharpness(Value);
	}
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_ResolutionScale(const float NewValue)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	const float Value = OnSliderChanged(NewValue, Value_ResolutionScale, SnapSize_ResolutionScale);

	if (UDLSSLibrary::GetDLSSMode() != UDLSSMode::Off)
	{
		return;
	}
	
	Settings->SetResolutionScaleNormalized(Value);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_DLSS_Sharpness(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewValue, Value_DLSS_Sharpness, Slider_DLSS_Sharpness, SnapSize_DLSSSharpness, MinValue_DLSSSharpness, MaxValue_DLSSSharpness);
	if (UDLSSLibrary::GetDLSSMode() != UDLSSMode::Off)
	{
		UDLSSLibrary::SetDLSSSharpness(Value);
	}
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_NIS_Sharpness(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewValue, Value_NIS_Sharpness, Slider_NIS_Sharpness, SnapSize_NISSharpness, MinValue_NISSharpness, MaxValue_NISSharpness);
	if (UDLSSLibrary::GetDLSSMode() == UDLSSMode::Off)
	{
		UNISLibrary::SetNISSharpness(Value);
	}
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_ResolutionScale(const FText& NewValue, ETextCommit::Type CommitType)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	float CurrentScaleNormalized;
	float CurrentScale;
	float MinScale;
	float MaxScale;
	Settings->GetResolutionScaleInformationEx(CurrentScaleNormalized, CurrentScale, MinScale, MaxScale);
	const float Value = OnEditableTextBoxChanged(NewValue, Value_ResolutionScale, Slider_ResolutionScale, SnapSize_ResolutionScale, MinScale, MaxScale);

	if (UDLSSLibrary::GetDLSSMode() != UDLSSMode::Off)
	{
		return;
	}
	
	Settings->SetResolutionScaleNormalized(Value);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewGlobalSound, Value_GlobalSound, Slider_GlobalSound, SnapSize_Volume, MinValue_Volume, MaxValue_Volume);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / static_cast<float>(MaxValue_Volume), 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewMenuSound, Value_MenuSound, Slider_MenuSound, SnapSize_Volume, MinValue_Volume, MaxValue_Volume);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / static_cast<float>(MaxValue_Volume), 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMusicSound, Value_MusicSound, Slider_MusicSound, SnapSize_Volume, MinValue_Volume, MaxValue_Volume);
}

void USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_VideoQuality(const UBSButton* Button)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	const UVideoSettingButton* ClickedButton = Cast<UVideoSettingButton>(Button);
	if (!ClickedButton)
	{
		return;
	}
	switch (ClickedButton->GetVideoSettingType())
	{
	case EVideoSettingType::AntiAliasing:
		{
			Settings->SetAntiAliasingQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::GlobalIllumination:
		{
			Settings->SetGlobalIlluminationQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::PostProcessing:
		{
			Settings->SetPostProcessingQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::Reflection:
		{
			Settings->SetReflectionQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::Shadow:
		{
			Settings->SetShadowQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::Shading:
		{
			Settings->SetShadingQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::Texture:
		{
			Settings->SetTextureQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::ViewDistance:
		{
			Settings->SetViewDistanceQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	case EVideoSettingType::VisualEffect:
		{
			Settings->SetVisualEffectQuality(ClickedButton->GetVideoSettingQuality());
			break;
		}
	}
}

UVideoSettingButton* USettingsMenuWidget_VideoAndSound::FindVideoSettingButton(const int32 Quality, const EVideoSettingType& SettingType) const
{
	UVideoSettingButton* Head = nullptr;
	switch (SettingType)
	{
	case EVideoSettingType::AntiAliasing:
		{
			Head = AAQ0;
			break;
		}
	case EVideoSettingType::GlobalIllumination:
		{
			Head = GIQ0;
			break;
		}
	case EVideoSettingType::PostProcessing:
		{
			Head = PPQ0;
			break;
		}
	case EVideoSettingType::Reflection:
		{
			Head = RQ0;
			break;
		}
	case EVideoSettingType::Shadow:
		{
			Head = SWQ0;
			break;
		}
	case EVideoSettingType::Shading:
		{
			Head = SGQ0;
			break;
		}
	case EVideoSettingType::Texture:
		{
			Head = TQ0;
			break;
		}
	case EVideoSettingType::ViewDistance:
		{
			Head = VDQ0;
			break;
		}
	case EVideoSettingType::VisualEffect:
		{
			Head = VEQ0;
			break;
		}
	}
	if (Head && Quality >= 0 && Quality <= 4 )
	{
		while (Head->GetVideoSettingQuality() != Quality)
		{
			Head = Head->GetNext();
		}
	}
	return Head;
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitMenu(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), SnapSize_FrameRateLimit);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(FrameLimit);
}

void USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_VSyncEnabled(const bool bIsChecked)
{
	UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(bIsChecked);
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_WindowMode(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (SelectedOption.Equals("Fullscreen"))
	{
		Settings->SetFullscreenMode(EWindowMode::Fullscreen);
	}
	else if (SelectedOption.Equals("Windowed Fullscreen"))
	{
		Settings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	}
	else if (SelectedOption.Equals("Windowed"))
	{
		Settings->SetFullscreenMode(EWindowMode::Windowed);
	}
	Settings->ApplyResolutionSettings(false);
	ShowConfirmVideoSettingsMessage();
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Resolution(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	FString LeftS;
	FString RightS;
	SelectedOption.Split("x", &LeftS, &RightS);
	LeftS = UKismetStringLibrary::Replace(LeftS, ",", "");
	RightS = UKismetStringLibrary::Replace(RightS, ",", "");
	const FIntPoint NewResolution = FIntPoint(FCString::Atoi(*LeftS), FCString::Atoi(*RightS));
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	LastConfirmedResolution = Settings->GetScreenResolution();
	Settings->SetScreenResolution(NewResolution);
	Settings->ApplyResolutionSettings(false);
	if (SelectionType != ESelectInfo::Direct)
	{
		ShowConfirmVideoSettingsMessage();
	}
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_DLSS(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	switch (GetEnumFromString<EDLSSEnabledMode>(SelectedOptions.Top(), EDLSSEnabledMode::Off)) {
	case EDLSSEnabledMode::Off:
		CacheDLSSSettings(true);
		HandleDLSSEnabledChanged(false);
		HandleDLSSDependencies(false);
		break;
	case EDLSSEnabledMode::On:
		CacheDLSSSettings(false);
		HandleDLSSEnabledChanged(true);
		HandleDLSSDependencies(true);
		break;
	}
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_FrameGeneration(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (UStreamlineLibraryDLSSG::IsDLSSGSupported())
	{
		UStreamlineLibraryDLSSG::SetDLSSGMode(GetEnumFromString<UStreamlineDLSSGMode>(SelectedOptions.Top(), UStreamlineDLSSGMode::Off));
	}
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_SuperResolution(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (UDLSSLibrary::IsDLSSSupported())
	{
		SetDLSSMode(GetEnumFromString<UDLSSMode>(SelectedOptions.Top(), UDLSSMode::Off));
	}
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	
	const ENISEnabledMode NISEnabledMode = GetEnumFromString<ENISEnabledMode>(SelectedOptions.Top(), ENISEnabledMode::Off);
	const EDLSSEnabledMode DLSSEnabledMode = GetSelectedDLSSEnabledMode();
	if (NISEnabledMode == ENISEnabledMode::On || DLSSEnabledMode == EDLSSEnabledMode::On)
	{
		Slider_ResolutionScale->SetLocked(true);
		Value_ResolutionScale->SetIsReadOnly(true);
	}
	else
	{
		Slider_ResolutionScale->SetLocked(false);
		Value_ResolutionScale->SetIsReadOnly(false);
	}
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS_Mode(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}

	const UNISMode SelectedNISMode = GetEnumFromString<UNISMode>(SelectedOptions.Top(), UNISMode::Off);
	const EDLSSEnabledMode DLSSEnabledMode = GetSelectedDLSSEnabledMode();
	if (!UNISLibrary::IsNISSupported() || DLSSEnabledMode == EDLSSEnabledMode::On)
	{
		return;
	}
	UNISLibrary::SetNISMode(SelectedNISMode);
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Reflex(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType)
{
	if (SelectedOptions.Num() != 1 || SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (UStreamlineLibraryReflex::IsReflexSupported())
	{
		UStreamlineLibraryReflex::SetReflexMode(GetEnumFromString<UStreamlineReflexMode>(SelectedOptions.Top(), UStreamlineReflexMode::Disabled));
	}
}

void USettingsMenuWidget_VideoAndSound::RevertVideoSettingsTimerCallback()
{
	const float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(RevertVideoSettingsTimer);
	if (Elapsed >= VideoSettingsTimeoutLength || Elapsed == -1.f)
	{
		OnButtonPressed_CancelVideoSettings();
		return;
	}
	TArray<FString> Out;
	FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupMessage").ToString().ParseIntoArray(Out, TEXT(" "));
	int32 Index = INDEX_NONE;
	for (int i = 0; i< Out.Num(); i++)
	{
		if (UKismetStringLibrary::IsNumeric(Out[i]))
		{
			Index = i;
			break;
		}
	}
	if (Index != INDEX_NONE && PopupMessageWidget)
	{
		Out[Index] = FString::FromInt(roundf(VideoSettingsTimeoutLength - Elapsed));
		PopupMessageWidget->ChangeMessageText(FText::FromString(UKismetStringLibrary::JoinStringArray(Out)));
	}
}

void USettingsMenuWidget_VideoAndSound::PopulateResolutionComboBox()
{
	const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	TArray<FIntPoint> Resolutions;
	FIntPoint MaxResolution = FIntPoint(0, 0);
	bool bIsWindowedFullscreen = false;
	ComboBox_Resolution->ClearOptions();
	LastConfirmedResolution = Settings->GetScreenResolution();
	switch (Settings->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
			bIsWindowedFullscreen = true;
			break;
		}
	case EWindowMode::Windowed:
		{
			UKismetSystemLibrary::GetConvenientWindowedResolutions(Resolutions);
			break;
		}
	case EWindowMode::NumWindowModes:
		{
			UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
			break;
		}
	}
	for (const FIntPoint Resolution : Resolutions)
	{
		if (!bIsWindowedFullscreen)
		{
			ComboBox_Resolution->AddOption(FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
			if (Resolution == LastConfirmedResolution)
			{
				ComboBox_Resolution->SetSelectedOption(FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
			}
		}
		else
		{
			if (Resolution.X > MaxResolution.X)
			{
				MaxResolution = Resolution;
			}
		}
	}
	if (bIsWindowedFullscreen)
	{
		ComboBox_Resolution->AddOption(FString::FormatAsNumber(MaxResolution.X) + "x" + FString::FormatAsNumber(MaxResolution.Y));
		ComboBox_Resolution->SetSelectedOption(FString::FormatAsNumber(MaxResolution.X) + "x" + FString::FormatAsNumber(MaxResolution.Y));
	}
}

void USettingsMenuWidget_VideoAndSound::CacheDLSSSettings(const bool bDLSSWasEnabled)
{
	if (bDLSSWasEnabled)
	{
		PreviousDLSSOnSettings.DLSSEnabledMode = EDLSSEnabledMode::On;
		PreviousDLSSOnSettings.FrameGenerationEnabledMode = GetEnumFromString<UStreamlineDLSSGMode>(ComboBox_FrameGeneration->GetSelectedOption(), UStreamlineDLSSGMode::Off);
		PreviousDLSSOnSettings.DLSSMode = GetEnumFromString<UDLSSMode>(ComboBox_SuperResolution->GetSelectedOption(), UDLSSMode::Off);
		PreviousDLSSOnSettings.NISEnabledMode = GetEnumFromString<ENISEnabledMode>(ComboBox_NIS->GetSelectedOption(), ENISEnabledMode::Off);
		PreviousDLSSOnSettings.NISMode = GetEnumFromString<UNISMode>(ComboBox_NIS_Mode->GetSelectedOption(), UNISMode::Off);
		PreviousDLSSOnSettings.StreamlineReflexMode = GetEnumFromString<UStreamlineReflexMode>(ComboBox_Reflex->GetSelectedOption(), UStreamlineReflexMode::Disabled);
		PreviousDLSSOnSettings.DLSSSharpness = FMath::GridSnap(FMath::Clamp(Slider_DLSS_Sharpness->GetValue(), MinValue_DLSSSharpness, MaxValue_DLSSSharpness), SnapSize_DLSSSharpness);
		PreviousDLSSOnSettings.NISSharpness = FMath::GridSnap(FMath::Clamp(Slider_NIS_Sharpness->GetValue(), MinValue_NISSharpness, MaxValue_NISSharpness), SnapSize_NISSharpness);
		PreviousDLSSOnSettings.bHasBeenInitialized = true;
	}
	else
	{
		PreviousDLSSOffSettings.DLSSEnabledMode = EDLSSEnabledMode::Off;
		PreviousDLSSOffSettings.FrameGenerationEnabledMode = GetEnumFromString<UStreamlineDLSSGMode>(ComboBox_FrameGeneration->GetSelectedOption(), UStreamlineDLSSGMode::Off);
		PreviousDLSSOffSettings.DLSSMode = GetEnumFromString<UDLSSMode>(ComboBox_SuperResolution->GetSelectedOption(), UDLSSMode::Off);
		PreviousDLSSOffSettings.NISEnabledMode = GetEnumFromString<ENISEnabledMode>(ComboBox_NIS->GetSelectedOption(), ENISEnabledMode::Off);
		PreviousDLSSOffSettings.NISMode = GetEnumFromString<UNISMode>(ComboBox_NIS_Mode->GetSelectedOption(), UNISMode::Off);
		PreviousDLSSOffSettings.StreamlineReflexMode = GetEnumFromString<UStreamlineReflexMode>(ComboBox_Reflex->GetSelectedOption(), UStreamlineReflexMode::Disabled);
		PreviousDLSSOffSettings.DLSSSharpness = FMath::GridSnap(FMath::Clamp(Slider_DLSS_Sharpness->GetValue(), MinValue_DLSSSharpness, MaxValue_DLSSSharpness), SnapSize_DLSSSharpness);
		PreviousDLSSOffSettings.NISSharpness = FMath::GridSnap(FMath::Clamp(Slider_NIS_Sharpness->GetValue(), MinValue_NISSharpness, MaxValue_NISSharpness), SnapSize_NISSharpness);
		PreviousDLSSOffSettings.bHasBeenInitialized = true;
	}
}

void USettingsMenuWidget_VideoAndSound::HandleDLSSEnabledChanged(const bool bIsDLSSEnabled)
{
	// Enabling/disabling
	if (bIsDLSSEnabled)
	{
		// Enable Settings that require DLSS to be on
		ComboBox_FrameGeneration->SetIsEnabled(true);
		ComboBox_SuperResolution->SetIsEnabled(true);
		Value_DLSS_Sharpness->SetIsReadOnly(false);
		Slider_DLSS_Sharpness->SetLocked(false);

		// Disable Settings that require DLSS to be off, or are forced to be on
		ComboBox_NIS->SetIsEnabled(false);
		ComboBox_NIS_Mode->SetIsEnabled(false);
		ComboBox_Reflex->SetIsEnabled(false);
		
		Slider_NIS_Sharpness->SetLocked(true);
		Value_NIS_Sharpness->SetIsReadOnly(true);

		Slider_ResolutionScale->SetLocked(true);
		Value_ResolutionScale->SetIsReadOnly(true);

		// Force V Sync disabled
		CheckBox_VSyncEnabled->SetIsEnabled(false);
		CheckBox_VSyncEnabled->SetIsChecked(false);
	}
	else
	{
		// Disable Settings that require DLSS to be on
		ComboBox_FrameGeneration->SetIsEnabled(false);
		ComboBox_SuperResolution->SetIsEnabled(false);
		Value_DLSS_Sharpness->SetIsReadOnly(true);
		Slider_DLSS_Sharpness->SetLocked(true);
		
		// Enable Settings that are don't require DLSS to be on
		ComboBox_NIS->SetIsEnabled(true);
		ComboBox_NIS_Mode->SetIsEnabled(true);
		ComboBox_Reflex->SetIsEnabled(true);
		
		// Enable Resolution Scale if NIS is off
		if (GetSelectedNISMode() == UNISMode::Off)
		{
			Slider_ResolutionScale->SetLocked(false);
			Value_ResolutionScale->SetIsReadOnly(false);
		}
		else
		{
			Slider_ResolutionScale->SetLocked(true);
			Value_ResolutionScale->SetIsReadOnly(true);
		}
		
		Slider_NIS_Sharpness->SetLocked(false);
		Value_NIS_Sharpness->SetIsReadOnly(false);
		
		CheckBox_VSyncEnabled->SetIsEnabled(true);
	}
}

void USettingsMenuWidget_VideoAndSound::HandleDLSSDependencies(const bool bIsDLSSEnabled)
{
	const EDLSSEnabledMode DLSSEnabledMode = bIsDLSSEnabled ? EDLSSEnabledMode::On : EDLSSEnabledMode::Off;
	float DLSSSharpness = FMath::GridSnap(FMath::Clamp(Slider_DLSS_Sharpness->GetValue(), MinValue_DLSSSharpness, MaxValue_DLSSSharpness), SnapSize_DLSSSharpness);
	float NISSharpness = FMath::GridSnap(FMath::Clamp(Slider_NIS_Sharpness->GetValue(), MinValue_NISSharpness, MaxValue_NISSharpness), SnapSize_NISSharpness);
	UStreamlineDLSSGMode FrameGenMode = GetSelectedFrameGenerationMode();
	UDLSSMode DLSSMode = GetSelectedDLSSMode();
	ENISEnabledMode NISEnabledMode = GetSelectedNISEnabledMode();
	UNISMode NISMode = GetSelectedNISMode();
	UStreamlineReflexMode ReflexMode = GetSelectedReflexMode();

	// Use cached settings if they've been populated
	if (bIsDLSSEnabled)
	{
		if (PreviousDLSSOnSettings.bHasBeenInitialized)
		{
			DLSSSharpness = PreviousDLSSOnSettings.DLSSSharpness;
			NISSharpness = PreviousDLSSOnSettings.NISSharpness;
			FrameGenMode = PreviousDLSSOnSettings.FrameGenerationEnabledMode;
			DLSSMode = PreviousDLSSOnSettings.DLSSMode;
			NISEnabledMode = PreviousDLSSOnSettings.NISEnabledMode;
			NISMode = PreviousDLSSOnSettings.NISMode;
			ReflexMode = PreviousDLSSOnSettings.StreamlineReflexMode;
		}
	}
	else
	{
		if (PreviousDLSSOffSettings.bHasBeenInitialized)
		{
			DLSSSharpness = PreviousDLSSOffSettings.DLSSSharpness;
			NISSharpness = PreviousDLSSOffSettings.NISSharpness;
			FrameGenMode = PreviousDLSSOffSettings.FrameGenerationEnabledMode;
			DLSSMode = PreviousDLSSOffSettings.DLSSMode;
			NISEnabledMode = PreviousDLSSOffSettings.NISEnabledMode;
			NISMode = PreviousDLSSOffSettings.NISMode;
			ReflexMode = PreviousDLSSOffSettings.StreamlineReflexMode;
		}
	}
	
	if (bIsDLSSEnabled)
	{
		// Force disable NIS
		NISEnabledMode = ENISEnabledMode::Off;
		// Force disable NIS
		NISMode = UNISMode::Off;
		// Force Reflex enabled
		ReflexMode = UStreamlineReflexMode::Enabled;
	}
	else
	{
		// Force disable Frame Generation
		FrameGenMode = UStreamlineDLSSGMode::Off;
		// Force disable Super Resolution
		DLSSMode = UDLSSMode::Off;
	}
	
	ComboBox_FrameGeneration->SetSelectedOption(UEnum::GetDisplayValueAsText(FrameGenMode).ToString());
	ComboBox_SuperResolution->SetSelectedOption(UEnum::GetDisplayValueAsText(DLSSMode).ToString());
	ComboBox_NIS->SetSelectedOption(UEnum::GetDisplayValueAsText(NISEnabledMode).ToString());
	ComboBox_NIS_Mode->SetSelectedOption(UEnum::GetDisplayValueAsText(NISMode).ToString());
	ComboBox_Reflex->SetSelectedOption(UEnum::GetDisplayValueAsText(ReflexMode).ToString());

	UStreamlineLibraryDLSSG::SetDLSSGMode(FrameGenMode);
	SetDLSSMode(DLSSMode);
	UNISLibrary::SetNISMode(NISMode);
	UStreamlineLibraryReflex::SetReflexMode(ReflexMode);
	UDLSSLibrary::SetDLSSSharpness(DLSSSharpness);
	UNISLibrary::SetNISSharpness(NISSharpness);
	
	// V-Sync
	if (DLSSEnabledMode == EDLSSEnabledMode::On)
	{
		CheckBox_VSyncEnabled->SetIsChecked(false);
		UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(false);
	}
	else
	{
		CheckBox_VSyncEnabled->SetIsChecked(UGameUserSettings::GetGameUserSettings()->IsVSyncEnabled());
	}
}

void USettingsMenuWidget_VideoAndSound::OnButtonPressed_Save()
{
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	SavePlayerSettings(GetVideoAndSoundSettings());
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_VideoAndSound::OnButtonPressed_Reset()
{
	UGameUserSettings::GetGameUserSettings()->SetToDefaults();
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	InitializeVideoAndSoundSettings(FPlayerSettings_VideoAndSound());
}

void USettingsMenuWidget_VideoAndSound::OnBSButtonPressed_SaveReset(const UBSButton* Button)
{
	switch (static_cast<ESettingButtonType>(Button->GetEnumValue()))
	{
	case ESettingButtonType::Save:
		OnButtonPressed_Save();
		break;
	case ESettingButtonType::Reset:
		OnButtonPressed_Reset();
		break;
	default:
		break;
	}
}

void USettingsMenuWidget_VideoAndSound::ShowConfirmVideoSettingsMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (PopupMessageWidget)
	{
		PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupTitle"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupMessage"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupButton1"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupButton2"));
		PopupMessageWidget->OnButton1Pressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonPressed_ConfirmVideoSettings);
		PopupMessageWidget->OnButton2Pressed.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonPressed_CancelVideoSettings);
		GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer_UpdateSecond, this, &USettingsMenuWidget_VideoAndSound::RevertVideoSettingsTimerCallback, 1.f, true);
		GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer, Constants::VideoSettingsTimeoutLength, false);
	}
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void USettingsMenuWidget_VideoAndSound::OnButtonPressed_ConfirmVideoSettings()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer_UpdateSecond);
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->ConfirmVideoMode();
	LastConfirmedResolution = Settings->GetScreenResolution();
	PopulateResolutionComboBox();
	
	PopupMessageWidget->FadeOut();
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_VideoAndSound::OnButtonPressed_CancelVideoSettings()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer_UpdateSecond);
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->RevertVideoMode();
	Settings->SetScreenResolution(LastConfirmedResolution);
	Settings->ApplyResolutionSettings(false);
	switch (UGameUserSettings::GetGameUserSettings()->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			ComboBox_WindowMode->SetSelectedOption("Fullscreen");
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			ComboBox_WindowMode->SetSelectedOption("Windowed Fullscreen");
			break;
		}
	case EWindowMode::Windowed:
		{
			ComboBox_WindowMode->SetSelectedOption("Windowed");
			break;
		}
	case EWindowMode::NumWindowModes:
		{
			break;
		}
	}
	PopulateResolutionComboBox();
	
	PopupMessageWidget->FadeOut();
}

UWidget* USettingsMenuWidget_VideoAndSound::OnGenerateWidgetEvent(const UBSComboBoxString* ComboBoxString, FString Method)
{
	const FText EntryText = Method.IsEmpty() ? FText::FromString("None Selected") : FText::FromString(Method);
	FText TooltipText = FText::GetEmpty();
	if (const FString Key = GetStringTableKeyFromComboBox(ComboBoxString, Method); !Key.IsEmpty())
	{
		TooltipText = GetTooltipTextFromKey(Key);
	}
	const bool bShowTooltipImage = !TooltipText.IsEmpty();

	if (UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(this, ComboBoxString->GetComboboxEntryWidget()))
	{
		ComboBoxString->InitializeComboBoxEntry(Entry, EntryText, bShowTooltipImage, TooltipText);
		return Entry;
	}
	return nullptr;
}

UWidget* USettingsMenuWidget_VideoAndSound::OnSelectionChanged_GenerateMultiSelectionItem(const UBSComboBoxString* ComboBoxString, const TArray<FString>& SelectedOptions)
{
	FString EntryString = FString();

	if (!SelectedOptions.IsEmpty())
	{
		for (int i = 0; i < SelectedOptions.Num(); i++)
		{
			if (!SelectedOptions[i].IsEmpty())
			{
				EntryString.Append(SelectedOptions[i]);
				if (i < SelectedOptions.Num() - 1)
				{
					EntryString.Append(", ");
				}
			}
		}
	}
	FText TooltipText = FText::GetEmpty();
	if (SelectedOptions.Num() == 1)
	{
		if (const FString Key = GetStringTableKeyFromComboBox(ComboBoxString, SelectedOptions[0]); !Key.IsEmpty())
		{
			TooltipText = GetTooltipTextFromKey(Key);
		}
	}

	const FText EntryText = FText::FromString(EntryString);
	const bool bShowTooltipImage = !TooltipText.IsEmpty();
	
	if (UBSComboBoxEntry* Entry = CreateWidget<UBSComboBoxEntry>(this, ComboBoxString->GetComboboxEntryWidget()))
	{
		ComboBoxString->InitializeComboBoxEntry(Entry, EntryText, bShowTooltipImage, TooltipText);
		return Entry;
	}
	
	return nullptr;
}

FString USettingsMenuWidget_VideoAndSound::GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString)
{
	if (ComboBoxString == ComboBox_DLSS)
	{
		return FString();
	}
	if (ComboBoxString == ComboBox_FrameGeneration)
	{
		return GetStringTableKeyNameFromEnum<UStreamlineDLSSGMode>(GetEnumFromString<UStreamlineDLSSGMode>(EnumString, UStreamlineDLSSGMode::Off));
	}
	if (ComboBoxString == ComboBox_SuperResolution)
	{
		return GetStringTableKeyNameFromEnum<UDLSSMode>(GetEnumFromString<UDLSSMode>(EnumString, UDLSSMode::Off));
	}
	if (ComboBoxString == ComboBox_NIS)
	{
		return FString();
	}
	if (ComboBoxString == ComboBox_NIS_Mode)
	{
		return FString();
	}
	if (ComboBoxString == ComboBox_Reflex)
	{
		return GetStringTableKeyNameFromEnum<UStreamlineReflexMode>(GetEnumFromString<UStreamlineReflexMode>(EnumString, UStreamlineReflexMode::Disabled));
	}
	
	UE_LOG(LogTemp, Display, TEXT("Couldn't find matching value for %s in UGameModesWidget_TargetConfig"), *ComboBoxString->GetName());
	return FString();
}

void USettingsMenuWidget_VideoAndSound::SetDLSSMode(const UDLSSMode InDLSSMode, const bool bRestoreFullResWhenDisabled)
{
	if (UDLSSLibrary::IsDLSSModeSupported(InDLSSMode))
	{
		const FIntPoint ScreenResolution = UGameUserSettings::GetGameUserSettings()->GetScreenResolution();
	
		bool bIsSupported;
		float OptimalScreenPercentage;
		bool bIsFixedScreenPercentage;
		float MinScreenPercentage;
		float MaxScreenPercentage;
		float OptimalSharpness;
		
		UDLSSLibrary::GetDLSSModeInformation(InDLSSMode, FVector2d(ScreenResolution.X, ScreenResolution.Y), bIsSupported, OptimalScreenPercentage,
			bIsFixedScreenPercentage, MinScreenPercentage, MaxScreenPercentage, OptimalSharpness);

		const bool bIsDLAA = InDLSSMode == UDLSSMode::DLAA;
		const bool bShouldEnable = (InDLSSMode != UDLSSMode::Off || bIsDLAA) && bIsSupported;
		const bool bValidScreenPercentage = OptimalScreenPercentage > 0.f && bIsSupported;

		// Enable/Disable DLSS
		UDLSSLibrary::EnableDLSS(bShouldEnable);
	
		// Set Screen Percentage
		float SelectedScreenPercentage;
		if (!bValidScreenPercentage || bIsDLAA)
		{
			// DLAA overrides DLSS mode if both are enabled
			SelectedScreenPercentage = 100.f;
		}
		else
		{
			SelectedScreenPercentage = OptimalScreenPercentage;
		}
		// Execute Screen Percentage Console Command
		if (bShouldEnable || bRestoreFullResWhenDisabled)
		{
			if (static IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage")))
			{
				CVarScreenPercentage->Set(SelectedScreenPercentage);
			}
		}
	}
}

EDLSSEnabledMode USettingsMenuWidget_VideoAndSound::GetSelectedDLSSEnabledMode() const
{
	return GetEnumFromString<EDLSSEnabledMode>(ComboBox_DLSS->GetSelectedOption(), EDLSSEnabledMode::Off);
}

UStreamlineDLSSGMode USettingsMenuWidget_VideoAndSound::GetSelectedFrameGenerationMode() const
{
	return GetEnumFromString<UStreamlineDLSSGMode>(ComboBox_FrameGeneration->GetSelectedOption(), UStreamlineDLSSGMode::Off);
}

UDLSSMode USettingsMenuWidget_VideoAndSound::GetSelectedDLSSMode() const
{
	return GetEnumFromString<UDLSSMode>(ComboBox_SuperResolution->GetSelectedOption(), UDLSSMode::Off);
}

ENISEnabledMode USettingsMenuWidget_VideoAndSound::GetSelectedNISEnabledMode() const
{
	return GetEnumFromString<ENISEnabledMode>(ComboBox_NIS->GetSelectedOption(), ENISEnabledMode::Off);
}

UNISMode USettingsMenuWidget_VideoAndSound::GetSelectedNISMode() const
{
	return GetEnumFromString<UNISMode>(ComboBox_NIS_Mode->GetSelectedOption(), UNISMode::Off);
}

UStreamlineReflexMode USettingsMenuWidget_VideoAndSound::GetSelectedReflexMode() const
{
	return GetEnumFromString<UStreamlineReflexMode>(ComboBox_Reflex->GetSelectedOption(), UStreamlineReflexMode::Disabled);
}