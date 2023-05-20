// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SubMenuWidgets/SettingsMenuWidget_VideoAndSound.h"
#include "BSWidgetInterface.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/BSHorizontalBox.h"
#include "WidgetComponents/SavedTextWidget.h"

ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSSupport, UDLSSSupport::Supported, UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive);
ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSMode, UDLSSMode::Off, UDLSSMode::UltraPerformance);
ENUM_RANGE_BY_FIRST_AND_LAST(UNISMode, UNISMode::Off, UNISMode::Custom);

void USettingsMenuWidget_VideoAndSound::NativeConstruct()
{
	Super::NativeConstruct();

	{
		AAQ0->InitVideoSettingButton(EVideoSettingType::AntiAliasing, 0, AAQ1);
		AAQ1->InitVideoSettingButton(EVideoSettingType::AntiAliasing, 1, AAQ2);
		AAQ2->InitVideoSettingButton(EVideoSettingType::AntiAliasing, 2, AAQ3);
		AAQ3->InitVideoSettingButton(EVideoSettingType::AntiAliasing, 3, AAQ0);
		
		AAQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		AAQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		AAQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		AAQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);

		GIQ0->InitVideoSettingButton(EVideoSettingType::GlobalIllumination, 0, GIQ1);
		GIQ1->InitVideoSettingButton(EVideoSettingType::GlobalIllumination, 1, GIQ2);
		GIQ2->InitVideoSettingButton(EVideoSettingType::GlobalIllumination, 2, GIQ3);
		GIQ3->InitVideoSettingButton(EVideoSettingType::GlobalIllumination, 3, GIQ0);
		
		GIQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		GIQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		GIQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		GIQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);

		PPQ0->InitVideoSettingButton(EVideoSettingType::PostProcessing, 0, PPQ1);
		PPQ1->InitVideoSettingButton(EVideoSettingType::PostProcessing, 1, PPQ2);
		PPQ2->InitVideoSettingButton(EVideoSettingType::PostProcessing, 2, PPQ3);
		PPQ3->InitVideoSettingButton(EVideoSettingType::PostProcessing, 3, PPQ0);
		
		PPQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		PPQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		PPQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		PPQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);

		RQ0->InitVideoSettingButton(EVideoSettingType::Reflection, 0, RQ1);
		RQ1->InitVideoSettingButton(EVideoSettingType::Reflection, 1, RQ2);
		RQ2->InitVideoSettingButton(EVideoSettingType::Reflection, 2, RQ3);
		RQ3->InitVideoSettingButton(EVideoSettingType::Reflection, 3, RQ0);
		
		RQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		RQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		RQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		RQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		
		SWQ0->InitVideoSettingButton(EVideoSettingType::Shadow, 0, SWQ1);
		SWQ1->InitVideoSettingButton(EVideoSettingType::Shadow, 1, SWQ2);
		SWQ2->InitVideoSettingButton(EVideoSettingType::Shadow, 2, SWQ3);
		SWQ3->InitVideoSettingButton(EVideoSettingType::Shadow, 3, SWQ0);

		SWQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SWQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SWQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SWQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		
		SGQ0->InitVideoSettingButton(EVideoSettingType::Shading, 0, SGQ1);
		SGQ1->InitVideoSettingButton(EVideoSettingType::Shading, 1, SGQ2);
		SGQ2->InitVideoSettingButton(EVideoSettingType::Shading, 2, SGQ3);
		SGQ3->InitVideoSettingButton(EVideoSettingType::Shading, 3, SGQ0);

		SGQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SGQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SGQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		SGQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		
		TQ0->InitVideoSettingButton(EVideoSettingType::Texture, 0, TQ1);
		TQ1->InitVideoSettingButton(EVideoSettingType::Texture, 1, TQ2);
		TQ2->InitVideoSettingButton(EVideoSettingType::Texture, 2, TQ3);
		TQ3->InitVideoSettingButton(EVideoSettingType::Texture, 3, TQ0);

		TQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		TQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		TQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		TQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		
		VDQ0->InitVideoSettingButton(EVideoSettingType::ViewDistance, 0, VDQ1);
		VDQ1->InitVideoSettingButton(EVideoSettingType::ViewDistance, 1, VDQ2);
		VDQ2->InitVideoSettingButton(EVideoSettingType::ViewDistance, 2, VDQ3);
		VDQ3->InitVideoSettingButton(EVideoSettingType::ViewDistance, 3, VDQ0);

		VDQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VDQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VDQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VDQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		
		VEQ0->InitVideoSettingButton(EVideoSettingType::VisualEffect, 0, VEQ1);
		VEQ1->InitVideoSettingButton(EVideoSettingType::VisualEffect, 1, VEQ2);
		VEQ2->InitVideoSettingButton(EVideoSettingType::VisualEffect, 2, VEQ3);
		VEQ3->InitVideoSettingButton(EVideoSettingType::VisualEffect, 3, VEQ0);

		VEQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VEQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VEQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
		VEQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality);
	}
	
	Slider_GlobalSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_GlobalSound);
	Slider_MenuSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_MenuSound);
	Slider_MusicSound->OnValueChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSliderChanged_MusicSound);
	Value_GlobalSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_GlobalSound);
	Value_MenuSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_MenuSound);
	Value_MusicSound->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_MusicSound);

	ComboBox_WindowMode->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_WindowMode);
	ComboBox_Resolution->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Resolution);
	Value_FrameLimitMenu->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitMenu);
	Value_FrameLimitGame->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitGame);
	CheckBox_VSyncEnabled->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_VSyncEnabled);
	CheckBox_FPSCounter->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_FPSCounter);
	
	Button_Reset->OnClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_Reset);
	Button_Save->OnClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_Save);
	ComboBox_Reflex->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Reflex);
	ComboBox_DLSS->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_DLSS);
	ComboBox_NIS->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS);
	
	/* Reflex */
	ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Disabled).ToString());
	if (GetReflexAvailable())
	{
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Enabled).ToString());
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::EnabledPlusBoost).ToString());
	}
	
	/* DLSS & NIS */
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported)
	{
		for (const UDLSSMode Mode : UDLSSLibrary::GetSupportedDLSSModes())
		{
			ComboBox_DLSS->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
		}
		BSBox_NIS->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		BSBox_DLSS->SetVisibility(ESlateVisibility::Collapsed);
		if (UNISLibrary::IsNISSupported())
		{
			for (const UNISMode Mode : UNISLibrary::GetSupportedNISModes())
			{
				if (Mode != UNISMode::Custom)
				{
					ComboBox_NIS->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
				}
			}
		}
		else
		{
			ComboBox_NIS->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	
	UpdateBrushColors();
	InitializeVideoAndSoundSettings(LoadPlayerSettings().VideoAndSound);
}

void USettingsMenuWidget_VideoAndSound::InitSettingCategoryWidget()
{
	TArray<TObjectPtr<UBSVerticalBox>> InContainers;
	InContainers.Add(BSBox_Video);
	InContainers.Add(BSBox_Sound);
	AddAdditionalContainers(InContainers);
	Super::InitSettingCategoryWidget();
}

void USettingsMenuWidget_VideoAndSound::InitializeVideoAndSoundSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
	Slider_GlobalSound->SetValue(InVideoAndSoundSettings.GlobalVolume);
	Slider_MenuSound->SetValue(InVideoAndSoundSettings.MenuVolume);
	Slider_MusicSound->SetValue(InVideoAndSoundSettings.MusicVolume);
	Value_GlobalSound->SetText(FText::AsNumber(InVideoAndSoundSettings.GlobalVolume));
	Value_MenuSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MenuVolume));
	Value_MusicSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MusicVolume));

	CheckBox_FPSCounter->SetIsChecked(InVideoAndSoundSettings.bShowFPSCounter);
	Value_FrameLimitGame->SetText(FText::AsNumber(InVideoAndSoundSettings.FrameRateLimitGame));
	Value_FrameLimitMenu->SetText(FText::AsNumber(InVideoAndSoundSettings.FrameRateLimitMenu));

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

	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
	LastConfirmedResolution = GameUserSettings->GetScreenResolution();
	FindVideoSettingButton(GameUserSettings->GetAntiAliasingQuality(), EVideoSettingType::AntiAliasing)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetGlobalIlluminationQuality(), EVideoSettingType::GlobalIllumination)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetPostProcessingQuality(), EVideoSettingType::PostProcessing)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetReflectionQuality(), EVideoSettingType::Reflection)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetShadowQuality(), EVideoSettingType::Shadow)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetShadingQuality(), EVideoSettingType::Shading)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetTextureQuality(), EVideoSettingType::Texture)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetViewDistanceQuality(), EVideoSettingType::ViewDistance)->SetButtonSettingCategoryBackgroundColors();
	FindVideoSettingButton(GameUserSettings->GetVisualEffectQuality(), EVideoSettingType::VisualEffect)->SetButtonSettingCategoryBackgroundColors();
	
	if (GetReflexAvailable())
	{
		ComboBox_Reflex->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.ReflexMode).ToString());
		SetReflexMode(InVideoAndSoundSettings.ReflexMode);
	}
	else
	{
		ComboBox_Reflex->SetSelectedOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Disabled).ToString());
	}
	
	
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported)
	{
		/* Should only be the case if they haven't changed the default setting */
		if (InVideoAndSoundSettings.DLSSMode == UDLSSMode::Auto)
		{
			const UDLSSMode DefaultDLSSMode = UDLSSLibrary::GetDefaultDLSSMode();
			UDLSSLibrary::SetDLSSMode(GetWorld(), DefaultDLSSMode);
			ComboBox_DLSS->SetSelectedOption(UEnum::GetDisplayValueAsText(DefaultDLSSMode).ToString());
			FPlayerSettings_VideoAndSound Settings = LoadPlayerSettings().VideoAndSound;
			Settings.DLSSMode = DefaultDLSSMode;
			SavePlayerSettings(Settings);
		}
		else
		{
			UDLSSLibrary::SetDLSSMode(GetWorld(), InVideoAndSoundSettings.DLSSMode);
			ComboBox_DLSS->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.DLSSMode).ToString());
		}
	}
	else if (UDLSSLibrary::QueryDLSSSupport() != UDLSSSupport::Supported && UNISLibrary::IsNISSupported())
	{
		/* Should only be the case if they haven't changed the default setting */
		if (InVideoAndSoundSettings.NISMode == UNISMode::Custom)
		{
			const UNISMode DefaultNISMode = UNISLibrary::GetDefaultNISMode();
			UNISLibrary::SetNISMode(DefaultNISMode);
			ComboBox_NIS->SetSelectedOption(UEnum::GetDisplayValueAsText(DefaultNISMode).ToString());
			FPlayerSettings_VideoAndSound Settings = LoadPlayerSettings().VideoAndSound;
			Settings.NISMode = DefaultNISMode;
			SavePlayerSettings(Settings);
		}
		else
		{
			ComboBox_NIS->SetSelectedOption(UEnum::GetDisplayValueAsText(InVideoAndSoundSettings.NISMode).ToString());
		}
	}
}

FPlayerSettings_VideoAndSound USettingsMenuWidget_VideoAndSound::GetVideoAndSoundSettings() const
{
	FPlayerSettings_VideoAndSound ReturnSettings;
	ReturnSettings.GlobalVolume = roundf(Slider_GlobalSound->GetValue());
	ReturnSettings.MenuVolume = roundf(Slider_MenuSound->GetValue());
	ReturnSettings.MusicVolume = roundf(Slider_MusicSound->GetValue());
	ReturnSettings.DLSSMode = GetDLSSMode();
	ReturnSettings.NISMode = GetNISMode();
	ReturnSettings.ReflexMode = GetReflexMode();
	ReturnSettings.bShowFPSCounter = CheckBox_FPSCounter->IsChecked();
	ReturnSettings.FrameRateLimitGame = roundf(FCString::Atof(*Value_FrameLimitGame->GetText().ToString()));
	ReturnSettings.FrameRateLimitMenu = roundf(FCString::Atof(*Value_FrameLimitMenu->GetText().ToString()));
	return ReturnSettings;
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_GlobalSound(const float NewGlobalSound)
{
	const float Value = OnSliderChanged(NewGlobalSound, Value_GlobalSound, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_MenuSound(const float NewMenuSound)
{
	const float Value = OnSliderChanged(NewMenuSound, Value_MenuSound, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnSliderChanged_MusicSound(const float NewMusicSound)
{
	OnSliderChanged(NewMusicSound, Value_MusicSound, 1);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewGlobalSound, Value_GlobalSound, Slider_GlobalSound, 1, 0, 100);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType)
{
	const float Value = OnEditableTextBoxChanged(NewMenuSound, Value_MenuSound, Slider_MenuSound, 1, 0, 100);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / 100.f, 1, 0.0f);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType)
{
	OnEditableTextBoxChanged(NewMusicSound, Value_MusicSound, Slider_MusicSound, 1, 0, 100);
}

void USettingsMenuWidget_VideoAndSound::OnButtonClicked_VideoQuality(const UVideoSettingButton* ClickedButton)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();

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
	ClickedButton->SetButtonSettingCategoryBackgroundColors();
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
	return Head->GetVideoSettingButtonFromQuality(Quality);
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

void USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitMenu(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(FrameLimit);
}

void USettingsMenuWidget_VideoAndSound::OnValueChanged_FrameLimitGame(const FText& NewValue, ETextCommit::Type CommitType)
{
}

void USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_VSyncEnabled(const bool bIsChecked)
{
	UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(bIsChecked);
}

void USettingsMenuWidget_VideoAndSound::OnCheckStateChanged_FPSCounter(const bool bIsChecked)
{
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_Reflex(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	SetReflexMode(GetEnumFromString<EBudgetReflexMode>(SelectedOption, EBudgetReflexMode::Disabled));
}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_DLSS(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (UDLSSLibrary::QueryDLSSSupport() != UDLSSSupport::Supported)
	{
		return;
	}
	
	const UDLSSMode Mode = GetEnumFromString<UDLSSMode>(SelectedOption, UDLSSMode::Off);
	
	if (Mode == UDLSSMode::Auto)
	{
		UDLSSLibrary::SetDLSSMode(GetWorld(), UDLSSLibrary::GetDefaultDLSSMode());
	}
	else
	{
		UDLSSLibrary::SetDLSSMode(GetWorld(), Mode);
	}

}

void USettingsMenuWidget_VideoAndSound::OnSelectionChanged_NIS(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported && !UNISLibrary::IsNISSupported())
	{
		return;
	}
	UNISLibrary::SetNISMode(GetEnumFromString<UNISMode>(SelectedOption, UNISMode::Off));
}

void USettingsMenuWidget_VideoAndSound::RevertVideoSettingsTimerCallback()
{
	const float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(RevertVideoSettingsTimer);
	if (Elapsed >= VideoSettingsTimeoutLength || Elapsed == -1.f)
	{
		OnButtonClicked_CancelVideoSettings();
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

UDLSSMode USettingsMenuWidget_VideoAndSound::GetDLSSMode() const
{
	if (UDLSSLibrary::QueryDLSSSupport() != UDLSSSupport::Supported)
	{
		return UDLSSMode::Off;
	}
	const FString SelectedOption = ComboBox_DLSS->GetSelectedOption();
	const UDLSSMode Mode = GetEnumFromString<UDLSSMode>(SelectedOption, UDLSSMode::Off);
	if (Mode == UDLSSMode::Auto)
	{
		return UDLSSLibrary::GetDefaultDLSSMode();
	}
	return Mode;
}

UNISMode USettingsMenuWidget_VideoAndSound::GetNISMode() const
{
	const FString SelectedOption = ComboBox_DLSS->GetSelectedOption();
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported || !UNISLibrary::IsNISSupported())
	{
		return UNISMode::Off;
	}
	for (const UNISMode Mode : TEnumRange<UNISMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			return Mode;
		}
	}
	return UNISMode::Off;
}

EBudgetReflexMode USettingsMenuWidget_VideoAndSound::GetReflexMode() const
{
	const FString SelectedOption = ComboBox_Reflex->GetSelectedOption();
	for (const EBudgetReflexMode Mode : TEnumRange<EBudgetReflexMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			return Mode;
		}
	}
	return EBudgetReflexMode::Disabled;
}

void USettingsMenuWidget_VideoAndSound::OnButtonClicked_Save()
{
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	SavePlayerSettings(GetVideoAndSoundSettings());
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void USettingsMenuWidget_VideoAndSound::OnButtonClicked_Reset()
{
	UGameUserSettings::GetGameUserSettings()->SetToDefaults();
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	InitializeVideoAndSoundSettings(FPlayerSettings_VideoAndSound());
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
		PopupMessageWidget->Button_1->OnClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_ConfirmVideoSettings);
		PopupMessageWidget->Button_2->OnClicked.AddDynamic(this, &USettingsMenuWidget_VideoAndSound::OnButtonClicked_CancelVideoSettings);
		GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer_UpdateSecond, this, &USettingsMenuWidget_VideoAndSound::RevertVideoSettingsTimerCallback, 1.f, true);
		GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer, Constants::VideoSettingsTimeoutLength, false);
	}
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void USettingsMenuWidget_VideoAndSound::OnButtonClicked_ConfirmVideoSettings()
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

void USettingsMenuWidget_VideoAndSound::OnButtonClicked_CancelVideoSettings()
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
