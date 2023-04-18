// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenu_Video.h"

#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SubMenuWidgets/VideoAndSoundSettingsWidget.h"
#include "WidgetComponents/BSHorizontalBox.h"
#include "WidgetComponents/VideoSettingButton.h"

ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSSupport, UDLSSSupport::Supported, UDLSSSupport::NotSupportedIncompatibleAPICaptureToolActive);
ENUM_RANGE_BY_FIRST_AND_LAST(UDLSSMode, UDLSSMode::Off, UDLSSMode::UltraPerformance);
ENUM_RANGE_BY_FIRST_AND_LAST(UNISMode, UNISMode::Off, UNISMode::Custom);

void USettingsMenu_Video::NativeConstruct()
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
		AAQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		AAQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		AAQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		AAQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
	

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
		GIQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		GIQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		GIQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		GIQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		PPQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		PPQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		PPQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		PPQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		RQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		RQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		RQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		RQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		SWQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SWQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SWQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SWQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		SGQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SGQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SGQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		SGQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		TQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		TQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		TQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		TQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		VDQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VDQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VDQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VDQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);

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
		VEQ0->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VEQ1->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VEQ2->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
		VEQ3->OnButtonClicked.AddDynamic(this, &USettingsMenu_Video::OnVideoQualityButtonClicked);
	}
	
	ComboBox_WindowMode->OnSelectionChanged.AddDynamic(this, &USettingsMenu_Video::OnWindowModeSelectionChanged);
	ComboBox_Resolution->OnSelectionChanged.AddDynamic(this, &USettingsMenu_Video::OnResolutionSelectionChanged);
	Value_FrameLimitMenu->OnTextCommitted.AddDynamic(this, &USettingsMenu_Video::OnFrameLimitMenuValueChanged);
	Value_FrameLimitGame->OnTextCommitted.AddDynamic(this, &USettingsMenu_Video::OnFrameLimitGameValueChanged);
	CheckBox_VSyncEnabled->OnCheckStateChanged.AddDynamic(this, &USettingsMenu_Video::OnVSyncEnabledCheckStateChanged);
	CheckBox_FPSCounter->OnCheckStateChanged.AddDynamic(this, &USettingsMenu_Video::OnFPSCounterCheckStateChanged);

	/* Reflex */
	ComboBox_Reflex->OnSelectionChanged.AddDynamic(this, &USettingsMenu_Video::OnReflexSelectionChanged);
	ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Disabled).ToString());
	if (GetReflexAvailable())
	{
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::Enabled).ToString());
		ComboBox_Reflex->AddOption(UEnum::GetDisplayValueAsText(EBudgetReflexMode::EnabledPlusBoost).ToString());
	}
	
	/* DLSS & NIS */
	ComboBox_DLSS->OnSelectionChanged.AddDynamic(this, &USettingsMenu_Video::OnDLSSSelectionChanged);
	ComboBox_NIS->OnSelectionChanged.AddDynamic(this, &USettingsMenu_Video::OnNISSelectionChanged);

	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported)
	{
		for (const UDLSSMode Mode : UDLSSLibrary::GetSupportedDLSSModes())
		{
			ComboBox_DLSS->AddOption(UEnum::GetDisplayValueAsText(Mode).ToString());
		}
		BSBox_NIS->SetVisibility(ESlateVisibility::Collapsed);
		MainContainer->UpdateBrushColors();
	}
	else
	{
		BSBox_DLSS->SetVisibility(ESlateVisibility::Collapsed);
		MainContainer->UpdateBrushColors();
		
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
			MainContainer->UpdateBrushColors();
		}
	}
}

void USettingsMenu_Video::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void USettingsMenu_Video::InitializeVideoSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
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

FPlayerSettings_VideoAndSound USettingsMenu_Video::GetVideoSettings() const
{
	FPlayerSettings_VideoAndSound ReturnSettings;
	ReturnSettings.DLSSMode = GetDLSSMode();
	ReturnSettings.NISMode = GetNISMode();
	ReturnSettings.ReflexMode = GetReflexMode();
	ReturnSettings.bShowFPSCounter = CheckBox_FPSCounter->IsChecked();
	ReturnSettings.FrameRateLimitGame = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*Value_FrameLimitGame->GetText().ToString()), 1);
	ReturnSettings.FrameRateLimitMenu = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*Value_FrameLimitMenu->GetText().ToString()), 1);
	return ReturnSettings;
}

void USettingsMenu_Video::OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton)
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

void USettingsMenu_Video::SetVideoSettingButtonBackgroundColor(UVideoSettingButton* ClickedButton)
{
	ClickedButton->Button->SetBackgroundColor(Constants::BeatShotBlue);
	const int32 ClickedButtonQuality = ClickedButton->Quality;
	UVideoSettingButton* Head = ClickedButton->Next;

	while (Head->Quality != ClickedButtonQuality)
	{
		Head->Button->SetBackgroundColor(FLinearColor::White);
		Head = Head->Next;
	}
}

UVideoSettingButton* USettingsMenu_Video::FindVideoSettingButtonFromQuality(const int32 Quality, const ESettingType& SettingType) const
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

void USettingsMenu_Video::OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
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
	OnWindowOrResolutionChanged.Broadcast();
}

void USettingsMenu_Video::OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
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
		OnWindowOrResolutionChanged.Broadcast();
	}
}

void USettingsMenu_Video::OnFrameLimitMenuValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(FrameLimit);
}

void USettingsMenu_Video::OnFrameLimitGameValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
}

void USettingsMenu_Video::OnVSyncEnabledCheckStateChanged(const bool bIsChecked)
{
	UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(bIsChecked);
}

void USettingsMenu_Video::OnFPSCounterCheckStateChanged(const bool bIsChecked)
{
}

void USettingsMenu_Video::OnReflexSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	for (const EBudgetReflexMode Mode : TEnumRange<EBudgetReflexMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			SetReflexMode(Mode);
		}
	}
}

void USettingsMenu_Video::OnDLSSSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (UDLSSLibrary::QueryDLSSSupport() != UDLSSSupport::Supported)
	{
		return;
	}
	for (const UDLSSMode Mode : TEnumRange<UDLSSMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
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

void USettingsMenu_Video::OnNISSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (UDLSSLibrary::QueryDLSSSupport() == UDLSSSupport::Supported && !UNISLibrary::IsNISSupported())
	{
		return;
	}
	for (const UNISMode Mode : TEnumRange<UNISMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			UNISLibrary::SetNISMode(Mode);
		}
	}
}

void USettingsMenu_Video::PopulateResolutionComboBox()
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

void USettingsMenu_Video::OnConfirmVideoSettingsButtonClicked()
{
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->ConfirmVideoMode();
	LastConfirmedResolution = Settings->GetScreenResolution();
	PopulateResolutionComboBox();
}

void USettingsMenu_Video::OnCancelVideoSettingsButtonClicked()
{
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
}

UDLSSMode USettingsMenu_Video::GetDLSSMode() const
{
	if (UDLSSLibrary::QueryDLSSSupport() != UDLSSSupport::Supported)
	{
		return UDLSSMode::Off;
	}
	const FString SelectedOption = ComboBox_DLSS->GetSelectedOption();
	for (const UDLSSMode Mode : TEnumRange<UDLSSMode>())
	{
		if (SelectedOption.Equals(UEnum::GetDisplayValueAsText(Mode).ToString()))
		{
			if (Mode == UDLSSMode::Auto)
			{
				return UDLSSLibrary::GetDefaultDLSSMode();
			}
			return Mode;
		}
	}
	return UDLSSMode::Off;
}

UNISMode USettingsMenu_Video::GetNISMode() const
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

EBudgetReflexMode USettingsMenu_Video::GetReflexMode() const
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
