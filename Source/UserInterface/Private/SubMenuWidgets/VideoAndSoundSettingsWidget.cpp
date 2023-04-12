// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/VideoAndSoundSettingsWidget.h"
#include "DLSSLibrary.h"
#include "NISLibrary.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/Slider.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSSupport, UDLSSSupport::Supported, UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive);
ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSMode, UDLSSMode::Off, UDLSSMode::UltraPerformance);
ENUM_RANGE_BY_FIRST_AND_LAST(UNISMode, UNISMode::Off, UNISMode::Custom);

void UVideoAndSoundSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	{
		AAQ0->SettingType = ESettingType::AntiAliasing;
		AAQ1->SettingType = ESettingType::AntiAliasing;
		AAQ2->SettingType = ESettingType::AntiAliasing;
		AAQ3->SettingType = ESettingType::AntiAliasing;
		AAQ0->Quality = 0;
		AAQ1->Quality = 1;
		AAQ2->Quality = 2;
		AAQ3->Quality = 3;
		AAQ0->Next = AAQ1;
		AAQ1->Next = AAQ2;
		AAQ2->Next = AAQ3;
		AAQ3->Next = AAQ0;
		AAQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		AAQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		AAQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		AAQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
	

		GIQ0->SettingType = ESettingType::GlobalIllumination;
		GIQ1->SettingType = ESettingType::GlobalIllumination;
		GIQ2->SettingType = ESettingType::GlobalIllumination;
		GIQ3->SettingType = ESettingType::GlobalIllumination;
		GIQ0->Quality = 0;
		GIQ1->Quality = 1;
		GIQ2->Quality = 2;
		GIQ3->Quality = 3;
		GIQ0->Next = GIQ1;
		GIQ1->Next = GIQ2;
		GIQ2->Next = GIQ3;
		GIQ3->Next = GIQ0;
		GIQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		GIQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		GIQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		GIQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		PPQ0->SettingType = ESettingType::PostProcessing;
		PPQ1->SettingType = ESettingType::PostProcessing;
		PPQ2->SettingType = ESettingType::PostProcessing;
		PPQ3->SettingType = ESettingType::PostProcessing;
		PPQ0->Quality = 0;
		PPQ1->Quality = 1;
		PPQ2->Quality = 2;
		PPQ3->Quality = 3;
		PPQ0->Next = PPQ1;
		PPQ1->Next = PPQ2;
		PPQ2->Next = PPQ3;
		PPQ3->Next = PPQ0;
		PPQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		PPQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		PPQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		PPQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		RQ0->SettingType = ESettingType::Reflection;
		RQ1->SettingType = ESettingType::Reflection;
		RQ2->SettingType = ESettingType::Reflection;
		RQ3->SettingType = ESettingType::Reflection;
		RQ0->Quality = 0;
		RQ1->Quality = 1;
		RQ2->Quality = 2;
		RQ3->Quality = 3;
		RQ0->Next = RQ1;
		RQ1->Next = RQ2;
		RQ2->Next = RQ3;
		RQ3->Next = RQ0;
		RQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		RQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		RQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		RQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		SWQ0->SettingType = ESettingType::Shadow;
		SWQ1->SettingType = ESettingType::Shadow;
		SWQ2->SettingType = ESettingType::Shadow;
		SWQ3->SettingType = ESettingType::Shadow;
		SWQ0->Quality = 0;
		SWQ1->Quality = 1;
		SWQ2->Quality = 2;
		SWQ3->Quality = 3;
		SWQ0->Next = SWQ1;
		SWQ1->Next = SWQ2;
		SWQ2->Next = SWQ3;
		SWQ3->Next = SWQ0;
		SWQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SWQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SWQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SWQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		SGQ0->SettingType = ESettingType::Shading;
		SGQ1->SettingType = ESettingType::Shading;
		SGQ2->SettingType = ESettingType::Shading;
		SGQ3->SettingType = ESettingType::Shading;
		SGQ0->Quality = 0;
		SGQ1->Quality = 1;
		SGQ2->Quality = 2;
		SGQ3->Quality = 3;
		SGQ0->Next = SGQ1;
		SGQ1->Next = SGQ2;
		SGQ2->Next = SGQ3;
		SGQ3->Next = SGQ0;
		SGQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SGQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SGQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		SGQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		TQ0->SettingType = ESettingType::Texture;
		TQ1->SettingType = ESettingType::Texture;
		TQ2->SettingType = ESettingType::Texture;
		TQ3->SettingType = ESettingType::Texture;
		TQ0->Quality = 0;
		TQ1->Quality = 1;
		TQ2->Quality = 2;
		TQ3->Quality = 3;
		TQ0->Next = TQ1;
		TQ1->Next = TQ2;
		TQ2->Next = TQ3;
		TQ3->Next = TQ0;
		TQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		TQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		TQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		TQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		VDQ0->SettingType = ESettingType::ViewDistance;
		VDQ1->SettingType = ESettingType::ViewDistance;
		VDQ2->SettingType = ESettingType::ViewDistance;
		VDQ3->SettingType = ESettingType::ViewDistance;
		VDQ0->Quality = 0;
		VDQ1->Quality = 1;
		VDQ2->Quality = 2;
		VDQ3->Quality = 3;
		VDQ0->Next = VDQ1;
		VDQ1->Next = VDQ2;
		VDQ2->Next = VDQ3;
		VDQ3->Next = VDQ0;
		VDQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VDQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VDQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VDQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);

		VEQ0->SettingType = ESettingType::VisualEffect;
		VEQ1->SettingType = ESettingType::VisualEffect;
		VEQ2->SettingType = ESettingType::VisualEffect;
		VEQ3->SettingType = ESettingType::VisualEffect;
		VEQ0->Quality = 0;
		VEQ1->Quality = 1;
		VEQ2->Quality = 2;
		VEQ3->Quality = 3;
		VEQ0->Next = VEQ1;
		VEQ1->Next = VEQ2;
		VEQ2->Next = VEQ3;
		VEQ3->Next = VEQ0;
		VEQ0->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VEQ1->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VEQ2->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
		VEQ3->OnButtonClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked);
	}
	
	/* Sound */

	GlobalSoundSlider->OnValueChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnGlobalSoundSliderChanged);
	MenuSoundSlider->OnValueChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnMenuSoundSliderChanged);
	MusicSoundSlider->OnValueChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnMusicSoundSliderChanged);
	GlobalSoundValue->OnTextCommitted.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnGlobalSoundValueChanged);
	MenuSoundValue->OnTextCommitted.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnMenuSoundValueChanged);
	MusicSoundValue->OnTextCommitted.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnMusicSoundValueChanged);

	/* Video */

	WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnWindowModeSelectionChanged);
	ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnResolutionSelectionChanged);
	FrameLimitMenuValue->OnTextCommitted.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnFrameLimitMenuValueChanged);
	FrameLimitGameValue->OnTextCommitted.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnFrameLimitGameValueChanged);
	VSyncEnabledCheckBox->OnCheckStateChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnVSyncEnabledCheckStateChanged);
	FPSCounterCheckBox->OnCheckStateChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnFPSCounterCheckStateChanged);


	/* Reflex */
	ReflexComboBox->OnSelectionChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnReflexSelectionChanged);
	ReflexComboBox->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Disabled).ToString());
	if (GetReflexAvailable())
	{
		ReflexComboBox->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Enabled).ToString());
		ReflexComboBox->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::EnabledPlusBoost).ToString());
	}
	
	/* DLSS & NIS */
	DLSSComboBox->OnSelectionChanged.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnDLSSSelectionChanged);
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported)
	{
		bDLSSSupported = true;
		DLSSText->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Video_DLSS"));
		for (const UDLSSMode Mode : UDLSSLibrary::GetSupportedDLSSModes())
		{
			DLSSComboBox->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
		}
	}
	else
	{
		if (UNISLibrary::IsNISSupported())
		{
			bNISSupported = true;
			DLSSText->SetText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "Video_NIS"));
			for (const UNISMode Mode : UNISLibrary::GetSupportedNISModes())
			{
				if (Mode != UNISMode::Custom)
				{
					DLSSComboBox->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
				}
			}
		}
		else
		{
			DLSSComboBox->AddOption(UEnum::GetDisplayValueAsText(UDLSSLibrary::QueryDLSSSupport()).ToString());
		}
	}
	
	/* Loading and Saving */

	ResetButton_VideoAndSound->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnResetButtonClicked_VideoAndSound);
	SaveButton_VideoAndSound->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnSaveButtonClicked_VideoAndSound);

	NewSettings = LoadPlayerSettings().VideoAndSound;
	PopulateSettings();
}

void UVideoAndSoundSettingsWidget::OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton)
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	switch (ClickedButton->SettingType)
	{
	case ESettingType::AntiAliasing:
		{
			Settings->SetAntiAliasingQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::GlobalIllumination:
		{
			Settings->SetGlobalIlluminationQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::PostProcessing:
		{
			Settings->SetPostProcessingQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::Reflection:
		{
			Settings->SetReflectionQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::Shadow:
		{
			Settings->SetShadowQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::Shading:
		{
			Settings->SetShadingQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::Texture:
		{
			Settings->SetTextureQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::ViewDistance:
		{
			Settings->SetViewDistanceQuality(ClickedButton->Quality);
			break;
		}
	case ESettingType::VisualEffect:
		{
			Settings->SetVisualEffectQuality(ClickedButton->Quality);
			break;
		}
	}
	SetVideoSettingButtonBackgroundColor(ClickedButton);
}

void UVideoAndSoundSettingsWidget::SetVideoSettingButtonBackgroundColor(UVideoSettingButton* ClickedButton)
{
	ClickedButton->Button->SetBackgroundColor(BeatShotBlue);
	const int32 ClickedButtonQuality = ClickedButton->Quality;
	UVideoSettingButton* Head = ClickedButton->Next;

	while (Head->Quality != ClickedButtonQuality)
	{
		Head->Button->SetBackgroundColor(White);
		Head = Head->Next;
	}
}

UVideoSettingButton* UVideoAndSoundSettingsWidget::FindVideoSettingButtonFromQuality(const int32 Quality, ESettingType SettingType)
{
	UVideoSettingButton* Head = nullptr;
	switch (SettingType)
	{
	case ESettingType::AntiAliasing:
		{
			Head = AAQ0;
			break;
		}
	case ESettingType::GlobalIllumination:
		{
			Head = GIQ0;
			break;
		}
	case ESettingType::PostProcessing:
		{
			Head = PPQ0;
			break;
		}
	case ESettingType::Reflection:
		{
			Head = RQ0;
			break;
		}
	case ESettingType::Shadow:
		{
			Head = SWQ0;
			break;
		}
	case ESettingType::Shading:
		{
			Head = SGQ0;
			break;
		}
	case ESettingType::Texture:
		{
			Head = TQ0;
			break;
		}
	case ESettingType::ViewDistance:
		{
			Head = VDQ0;
			break;
		}
	case ESettingType::VisualEffect:
		{
			Head = VEQ0;
			break;
		}
	}
	while (Head->Quality != Quality)
	{
		Head = Head->Next;
	}
	return Head;
}

void UVideoAndSoundSettingsWidget::OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
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

void UVideoAndSoundSettingsWidget::OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
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

void UVideoAndSoundSettingsWidget::OnFrameLimitMenuValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(FrameLimit);
	NewSettings.FrameRateLimitMenu = FrameLimit;
}

void UVideoAndSoundSettingsWidget::OnFrameLimitGameValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	NewSettings.FrameRateLimitGame = FrameLimit;
}

void UVideoAndSoundSettingsWidget::OnVSyncEnabledCheckStateChanged(const bool bIsChecked)
{
	UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(bIsChecked);
}

void UVideoAndSoundSettingsWidget::OnFPSCounterCheckStateChanged(const bool bIsChecked)
{
	NewSettings.bShowFPSCounter = bIsChecked;
}

void UVideoAndSoundSettingsWidget::PopulateResolutionComboBox()
{
	const UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	TArray<FIntPoint> Resolutions;
	FIntPoint MaxResolution = FIntPoint(0, 0);
	bool bIsWindowedFullscreen = false;
	ResolutionComboBox->ClearOptions();
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
			ResolutionComboBox->AddOption(FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
			if (Resolution == LastConfirmedResolution)
			{
				ResolutionComboBox->SetSelectedOption(FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
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
		ResolutionComboBox->AddOption(FString::FormatAsNumber(MaxResolution.X) + "x" + FString::FormatAsNumber(MaxResolution.Y));
		ResolutionComboBox->SetSelectedOption(FString::FormatAsNumber(MaxResolution.X) + "x" + FString::FormatAsNumber(MaxResolution.Y));
	}
}

void UVideoAndSoundSettingsWidget::OnReflexSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	for (const EBudgetReflexMode Mode : TEnumRange<EBudgetReflexMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			NewSettings.ReflexMode = Mode;
			SetReflexMode(Mode);
		}
	}
}

void UVideoAndSoundSettingsWidget::OnDLSSSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (bDLSSSupported)
	{
		for (const UDLSSMode Mode : TEnumRange<UDLSSMode>())
        {
        	if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
        	{
        		NewSettings.DLSSMode = Mode;
        		if (Mode == UDLSSMode::Auto)
        		{
        			UDLSSLibrary::SetDLSSMode(GetWorld(), UDLSSLibrary::GetDefaultDLSSMode());
        		}
        		else
        		{
        			UDLSSLibrary::SetDLSSMode(GetWorld(), Mode);
        		}
        	}
        }
	}
	if (!bDLSSSupported && bNISSupported)
	{
		for (const UNISMode Mode : TEnumRange<UNISMode>())
		{
			if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
			{
				NewSettings.NISMode = Mode;
				UNISLibrary::SetNISMode(Mode);
			}
		}
	}
}

void UVideoAndSoundSettingsWidget::OnGlobalSoundSliderChanged(const float NewValue)
{
	const float DecimalVolume = roundf(NewValue * 100) / 10000;
	ChangeValueOnSliderChange(NewValue, GlobalSoundValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, DecimalVolume, 1, 0.1f);
	NewSettings.GlobalVolume = roundf(NewValue);
}

void UVideoAndSoundSettingsWidget::OnMenuSoundSliderChanged(const float NewValue)
{
	const float DecimalVolume = roundf(NewValue * 100) / 10000;
	ChangeValueOnSliderChange(NewValue, MenuSoundValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, DecimalVolume, 1, 0.1f);
	NewSettings.MenuVolume = roundf(NewValue);
}

void UVideoAndSoundSettingsWidget::OnMusicSoundSliderChanged(const float NewValue)
{
	ChangeValueOnSliderChange(NewValue, MusicSoundValue, 1);
	NewSettings.MusicVolume = roundf(NewValue);
}

void UVideoAndSoundSettingsWidget::OnGlobalSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float DecimalVolume = roundf(FCString::Atof(*NewValue.ToString()) * 100) / 10000;
	ChangeSliderOnValueChange(NewValue, GlobalSoundSlider, 1);
	NewSettings.GlobalVolume = roundf(FCString::Atof(*NewValue.ToString()));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, DecimalVolume, 1, 0.1f);
}

void UVideoAndSoundSettingsWidget::OnMenuSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float DecimalVolume = roundf(FCString::Atof(*NewValue.ToString()) * 100) / 10000;
	ChangeSliderOnValueChange(NewValue, MenuSoundSlider, 1);
	NewSettings.MenuVolume = roundf(FCString::Atof(*NewValue.ToString()));
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, DecimalVolume, 1, 0.0f);
}

void UVideoAndSoundSettingsWidget::OnMusicSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	ChangeSliderOnValueChange(NewValue, MusicSoundSlider, 1);
	NewSettings.MusicVolume = roundf(FCString::Atof(*NewValue.ToString()));
}

void UVideoAndSoundSettingsWidget::OnSaveButtonClicked_VideoAndSound()
{
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	SavePlayerSettings(NewSettings);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UVideoAndSoundSettingsWidget::OnResetButtonClicked_VideoAndSound()
{
	UGameUserSettings::GetGameUserSettings()->SetToDefaults();
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	NewSettings = FPlayerSettings_VideoAndSound();
	PopulateSettings();
}

void UVideoAndSoundSettingsWidget::PopulateSettings()
{
	GlobalSoundSlider->SetValue(NewSettings.GlobalVolume);
	MenuSoundSlider->SetValue(NewSettings.MenuVolume);
	MusicSoundSlider->SetValue(NewSettings.MusicVolume);
	GlobalSoundValue->SetText(FText::AsNumber(NewSettings.GlobalVolume));
	MenuSoundValue->SetText(FText::AsNumber(NewSettings.MenuVolume));
	MusicSoundValue->SetText(FText::AsNumber(NewSettings.MusicVolume));

	FPSCounterCheckBox->SetIsChecked(NewSettings.bShowFPSCounter);
	FrameLimitGameValue->SetText(FText::AsNumber(NewSettings.FrameRateLimitGame));
	FrameLimitMenuValue->SetText(FText::AsNumber(NewSettings.FrameRateLimitMenu));

	switch (UGameUserSettings::GetGameUserSettings()->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			WindowModeComboBox->SetSelectedOption("Fullscreen");
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			WindowModeComboBox->SetSelectedOption("Windowed Fullscreen");
			break;
		}
	case EWindowMode::Windowed:
		{
			WindowModeComboBox->SetSelectedOption("Windowed");
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
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetAntiAliasingQuality(), ESettingType::AntiAliasing));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetGlobalIlluminationQuality(), ESettingType::GlobalIllumination));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetPostProcessingQuality(), ESettingType::PostProcessing));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetReflectionQuality(), ESettingType::Reflection));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetShadowQuality(), ESettingType::Shadow));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetShadingQuality(), ESettingType::Shading));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetTextureQuality(), ESettingType::Texture));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetViewDistanceQuality(), ESettingType::ViewDistance));
	OnVideoQualityButtonClicked(FindVideoSettingButtonFromQuality(GameUserSettings->GetVisualEffectQuality(), ESettingType::VisualEffect));
	
	if (GetReflexAvailable())
	{
		ReflexComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(NewSettings.ReflexMode).ToString());
		SetReflexMode(NewSettings.ReflexMode);
	}
	else
	{
		ReflexComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Disabled).ToString());
	}

	
	if (bDLSSSupported)
	{
		/* Should only be the case if they haven't changed the default setting */
		if (NewSettings.DLSSMode == UDLSSMode::Auto)
		{
			const UDLSSMode DefaultDLSSMode = UDLSSLibrary::GetDefaultDLSSMode();
			UDLSSLibrary::SetDLSSMode(GetWorld(), DefaultDLSSMode);
			DLSSComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(DefaultDLSSMode).ToString());
			NewSettings.DLSSMode = DefaultDLSSMode;
			SavePlayerSettings(NewSettings);
		}
		else
		{
			UDLSSLibrary::SetDLSSMode(GetWorld(), NewSettings.DLSSMode);
			DLSSComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(NewSettings.DLSSMode).ToString());
		}
	}
	else if (!bDLSSSupported && bNISSupported)
	{
		/* Should only be the case if they haven't changed the default setting */
		if (NewSettings.NISMode == UNISMode::Custom)
		{
			const UNISMode DefaultNISMode = UNISLibrary::GetDefaultNISMode();
			UNISLibrary::SetNISMode(DefaultNISMode);
			DLSSComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(DefaultNISMode).ToString());
			NewSettings.NISMode = DefaultNISMode;
			SavePlayerSettings(NewSettings);
		}
		else
		{
			DLSSComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(NewSettings.NISMode).ToString());
		}
	}
	else
	{
		DLSSComboBox->SetSelectedOption(UEnum::GetDisplayValueAsText(UDLSSLibrary::QueryDLSSSupport()).ToString());
	}
}

void UVideoAndSoundSettingsWidget::ShowConfirmVideoSettingsMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(this, PopupMessageClass);
	if (PopupMessageWidget)
	{
		PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupTitle"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupMessage"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupButton1"),
		                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ConfirmVideoSettingsPopupButton2"));
		PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnConfirmVideoSettingsButtonClicked);
		PopupMessageWidget->Button2->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnCancelVideoSettingsButtonClicked);
		GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer, this, &UVideoAndSoundSettingsWidget::OnCancelVideoSettingsButtonClicked, 10.f, false);
	}
}

void UVideoAndSoundSettingsWidget::OnConfirmVideoSettingsButtonClicked()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->ConfirmVideoMode();
	LastConfirmedResolution = Settings->GetScreenResolution();
	PopulateResolutionComboBox();
	PopupMessageWidget->FadeOut();
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UVideoAndSoundSettingsWidget::OnCancelVideoSettingsButtonClicked()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->RevertVideoMode();
	Settings->SetScreenResolution(LastConfirmedResolution);
	Settings->ApplyResolutionSettings(false);
	switch (UGameUserSettings::GetGameUserSettings()->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			WindowModeComboBox->SetSelectedOption("Fullscreen");
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			WindowModeComboBox->SetSelectedOption("Windowed Fullscreen");
			break;
		}
	case EWindowMode::Windowed:
		{
			WindowModeComboBox->SetSelectedOption("Windowed");
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

float UVideoAndSoundSettingsWidget::ChangeValueOnSliderChange(const float SliderValue, UEditableTextBox* TextBoxToChange, const float SnapSize)
{
	const float NewValue = UKismetMathLibrary::GridSnap_Float(SliderValue, SnapSize);
	TextBoxToChange->SetText(FText::AsNumber(NewValue));
	return NewValue;
}

float UVideoAndSoundSettingsWidget::ChangeSliderOnValueChange(const FText& TextValue, USlider* SliderToChange, const float SnapSize)
{
	const float NewValue = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*TextValue.ToString()), SnapSize);
	SliderToChange->SetValue(NewValue);
	return NewValue;
}
