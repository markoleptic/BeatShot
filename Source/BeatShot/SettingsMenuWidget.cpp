// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SettingsMenuWidget.h"
#include "DefaultGameInstance.h"
#include "SlideRightButton.h"
#include "TimerManager.h"
#include "PopupMessageWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>

#include "AASettings.h"
#include "DefaultPlayerController.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"

void USettingsMenuWidget::NativeConstruct()
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
		AAQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		AAQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		AAQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		AAQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		GIQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		GIQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		GIQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		GIQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		PPQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		PPQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		PPQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		PPQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		RQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		RQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		RQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		RQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		SWQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SWQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SWQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SWQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		SGQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SGQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SGQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		SGQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		TQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		TQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		TQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		TQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		VDQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VDQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VDQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VDQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);

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
		VEQ0->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VEQ1->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VEQ2->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
		VEQ3->OnButtonClicked.AddDynamic(this, &USettingsMenuWidget::OnVideoQualityButtonClicked);
	}

	MenuWidgets.Add(VideoAndSoundSettingsButton, VideoAndSoundSettings);
	MenuWidgets.Add(AASettingsButton, AASettings);
	MenuWidgets.Add(SensitivityButton, Sensitivity);
	MenuWidgets.Add(CrossHairButton, CrossHair);

	VideoAndSoundSettingsButton->Button->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnVideoAndSoundSettingsButtonClicked);
	AASettingsButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnAASettingsButtonClicked);
	SensitivityButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSensitivityButtonClicked);
	CrossHairButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnCrossHairButtonClicked);

	GlobalSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnGlobalSoundSliderChanged);
	MenuSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnMenuSoundSliderChanged);
	MusicSoundSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnMusicSoundSliderChanged);
	GlobalSoundValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnGlobalSoundValueChanged);
	MenuSoundValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnMenuSoundValueChanged);
	MusicSoundValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnMusicSoundValueChanged);

	WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnWindowModeSelectionChanged);
	ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnResolutionSelectionChanged);

	FrameLimitMenuValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnFrameLimitMenuValueChanged);
	FrameLimitGameValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnFrameLimitGameValueChanged);

	VSyncEnabledCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnVSyncEnabledCheckStateChanged);

	FPSCounterCheckBox->OnCheckStateChanged.AddDynamic(this, &USettingsMenuWidget::OnFPSCounterCheckStateChanged);

	CombatTextFrequency->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnCombatTextFrequencyValueChanged);
	ShowStreakCombatTextCheckBox->OnCheckStateChanged.AddDynamic(
		this, &USettingsMenuWidget::OnShowCombatTextCheckStateChanged);

	NewSensitivityValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnNewSensitivityValue);
	NewSensitivityCsgoValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnNewSensitivityCsgoValue);
	SensSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnSensitivitySliderChanged);

	ResetVideoAndSoundButton->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnResetVideoAndSoundButtonClicked);
	SaveVideoAndSoundSettingsButton->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnSaveVideoAndSoundSettingsButtonClicked);
	SaveSensitivityButton->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnSaveSensitivitySettingsButtonClicked);

	LoadPlayerSettings();
	InitializeSettings();

	if (bIsMainMenuChild)
	{
		AASettingsWidget->InitMainMenuChild();
	}
}

void USettingsMenuWidget::InitializeSettings()
{
	SensSlider->SetValue(InitialPlayerSettings.Sensitivity);
	CurrentSensitivityValue->SetText(FText::AsNumber(InitialPlayerSettings.Sensitivity));

	GlobalSoundSlider->SetValue(InitialPlayerSettings.GlobalVolume);
	MenuSoundSlider->SetValue(InitialPlayerSettings.MenuVolume);
	MusicSoundSlider->SetValue(InitialPlayerSettings.MusicVolume);

	GlobalSoundValue->SetText(FText::AsNumber(InitialPlayerSettings.GlobalVolume));
	MenuSoundValue->SetText(FText::AsNumber(InitialPlayerSettings.MenuVolume));
	MusicSoundValue->SetText(FText::AsNumber(InitialPlayerSettings.MusicVolume));

	FPSCounterCheckBox->SetIsChecked(InitialPlayerSettings.bShowFPSCounter);

	FrameLimitGameValue->SetText(FText::AsNumber(InitialPlayerSettings.FrameRateLimitGame));
	FrameLimitMenuValue->SetText(FText::AsNumber(InitialPlayerSettings.FrameRateLimitMenu));

	ShowStreakCombatTextCheckBox->SetIsChecked(InitialPlayerSettings.bShowStreakCombatText);
	CombatTextFrequency->SetText(FText::AsNumber(InitialPlayerSettings.CombatTextFrequency));


	PopulateResolutionComboBox();

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

	const UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();

	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetAntiAliasingQuality(), ESettingType::AntiAliasing));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetGlobalIlluminationQuality(),
		                                  ESettingType::GlobalIllumination));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetPostProcessingQuality(), ESettingType::PostProcessing));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetReflectionQuality(), ESettingType::Reflection));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetShadowQuality(), ESettingType::Shadow));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetShadingQuality(), ESettingType::Shading));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetTextureQuality(), ESettingType::Texture));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetViewDistanceQuality(), ESettingType::ViewDistance));
	OnVideoQualityButtonClicked(
		FindVideoSettingButtonFromQuality(GameUserSettings->GetVisualEffectQuality(), ESettingType::VisualEffect));

	OnVideoAndSoundSettingsButtonClicked();
}

void USettingsMenuWidget::LoadPlayerSettings()
{
	InitialPlayerSettings = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->
		LoadPlayerSettings();
	NewPlayerSettings = InitialPlayerSettings;
}

void USettingsMenuWidget::SavePlayerSettings() const
{
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->SavePlayerSettings(NewPlayerSettings);
}

void USettingsMenuWidget::OnSaveVideoAndSoundSettingsButtonClicked()
{
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	SavePlayerSettings();
}

void USettingsMenuWidget::OnResetVideoAndSoundButtonClicked()
{
	UGameUserSettings::GetGameUserSettings()->SetToDefaults();
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	NewPlayerSettings.ResetVideoAndSoundSettings();
	Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()))->SavePlayerSettings(NewPlayerSettings);
	InitialPlayerSettings = NewPlayerSettings;
	InitializeSettings();
}

void USettingsMenuWidget::ShowConfirmVideoSettingsMessage()
{
	ADefaultPlayerController* PlayerController = Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0));
	UPopupMessageWidget* PopupMessageWidget = PlayerController->CreatePopupMessageWidget(true, 1);
	PopupMessageWidget->InitPopup("Confirm Video Settings?",
	                              "Settings will be reverted if not confirmed in 10 seconds",
	                              "Confirm", "Revert");
	PopupMessageWidget->Button1->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnConfirmVideoSettingsButtonClicked);
	PopupMessageWidget->Button2->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnCancelVideoSettingsButtonClicked);
	GetWorld()->GetTimerManager().SetTimer(RevertVideoSettingsTimer, this,
	                                       &USettingsMenuWidget::OnCancelVideoSettingsButtonClicked, 10.f, false);
}

void USettingsMenuWidget::OnConfirmVideoSettingsButtonClicked()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->ConfirmVideoMode();
	Settings->ApplyResolutionSettings(false);
	PopulateResolutionComboBox();
	
	Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0))->HidePopupMessage();
}

void USettingsMenuWidget::OnCancelVideoSettingsButtonClicked()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	Settings->RevertVideoMode();
	Settings->SetScreenResolution(Settings->GetLastConfirmedScreenResolution());
	PopulateResolutionComboBox();

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
	
	Cast<ADefaultPlayerController>(
		UGameplayStatics::GetPlayerController(
			GetWorld(), 0))->HidePopupMessage();
}

void USettingsMenuWidget::PopulateResolutionComboBox()
{
	TArray<FIntPoint> Resolutions;
	switch (UGameUserSettings::GetGameUserSettings()->GetFullscreenMode())
	{
	case EWindowMode::Fullscreen:
		{
			UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
			break;
		}
	case EWindowMode::WindowedFullscreen:
		{
			UKismetSystemLibrary::GetConvenientWindowedResolutions(Resolutions);
			break;
		}
	case EWindowMode::Windowed:
		{
			UKismetSystemLibrary::GetConvenientWindowedResolutions(Resolutions);
			break;
		}
	case EWindowMode::NumWindowModes:
		{
			break;
		}
	}
	ResolutionComboBox->ClearOptions();
	const FIntPoint CurrentRes = UGameUserSettings::GetGameUserSettings()->GetLastConfirmedScreenResolution();
	for (const FIntPoint Resolution : Resolutions)
	{
		ResolutionComboBox->AddOption(
			FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
		if (Resolution == CurrentRes)
		{
			ResolutionComboBox->SetSelectedOption(
				FString::FormatAsNumber(Resolution.X) + "x" + FString::FormatAsNumber(Resolution.Y));
		}
	}
}

void USettingsMenuWidget::OnGlobalSoundSliderChanged(const float NewValue)
{
	const float DecimalVolume = roundf(NewValue * 100) / 10000;
	ChangeValueOnSliderChange(NewValue, GlobalSoundValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, DecimalVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	NewPlayerSettings.GlobalVolume = roundf(NewValue);
}

void USettingsMenuWidget::OnMenuSoundSliderChanged(const float NewValue)
{
	const float DecimalVolume = roundf(NewValue * 100) / 10000;
	ChangeValueOnSliderChange(NewValue, MenuSoundValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, DecimalVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	NewPlayerSettings.MenuVolume = roundf(NewValue);
}

void USettingsMenuWidget::OnMusicSoundSliderChanged(const float NewValue)
{
	ChangeValueOnSliderChange(NewValue, MusicSoundValue, 1);
	NewPlayerSettings.MusicVolume = roundf(NewValue);
}

void USettingsMenuWidget::OnGlobalSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float DecimalVolume = roundf(FCString::Atof(*NewValue.ToString()) * 100) / 10000;
	ChangeSliderOnValueChange(NewValue, GlobalSoundSlider, 1);
	NewPlayerSettings.GlobalVolume = FCString::Atof(*NewValue.ToString());
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, DecimalVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
}

void USettingsMenuWidget::OnMenuSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float DecimalVolume = roundf(FCString::Atof(*NewValue.ToString()) * 100) / 10000;
	ChangeSliderOnValueChange(NewValue, MenuSoundSlider, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, DecimalVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	NewPlayerSettings.MenuVolume = roundf(FCString::Atof(*NewValue.ToString()));
}

void USettingsMenuWidget::OnMusicSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	ChangeSliderOnValueChange(NewValue, MusicSoundSlider, 1);
	NewPlayerSettings.MusicVolume = roundf(FCString::Atof(*NewValue.ToString()));
}

void USettingsMenuWidget::OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (SelectionType == ESelectInfo::Type::Direct)
	{
		return;
	}
	if (SelectedOption.Equals("Fullscreen"))
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Fullscreen);
	}
	else if (SelectedOption.Equals("Windowed Fullscreen"))
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::WindowedFullscreen);
	}
	else if (SelectedOption.Equals("Windowed"))
	{
		UGameUserSettings::GetGameUserSettings()->SetFullscreenMode(EWindowMode::Windowed);
	}
	ShowConfirmVideoSettingsMessage();
}

void USettingsMenuWidget::OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	FString LeftS;
	FString RightS;
	SelectedOption.Split("x", &LeftS, &RightS);
	LeftS = UKismetStringLibrary::Replace(LeftS, ",", "");
	RightS = UKismetStringLibrary::Replace(RightS, ",", "");
	const FIntPoint NewResolution = FIntPoint(FCString::Atoi(*LeftS),
	                                          FCString::Atoi(*RightS));
	UGameUserSettings::GetGameUserSettings()->SetScreenResolution(NewResolution);
	if (SelectionType != ESelectInfo::Direct)
	{
		ShowConfirmVideoSettingsMessage();
	}
}

void USettingsMenuWidget::OnFrameLimitMenuValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	UGameUserSettings::GetGameUserSettings()->SetFrameRateLimit(FrameLimit);
	NewPlayerSettings.FrameRateLimitMenu = FrameLimit;
}

void USettingsMenuWidget::OnFrameLimitGameValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float FrameLimit = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
	NewPlayerSettings.FrameRateLimitGame = FrameLimit;
}

void USettingsMenuWidget::OnVSyncEnabledCheckStateChanged(const bool bIsChecked)
{
	UGameUserSettings::GetGameUserSettings()->SetVSyncEnabled(bIsChecked);
}

void USettingsMenuWidget::OnFPSCounterCheckStateChanged(const bool bIsChecked)
{
	NewPlayerSettings.bShowFPSCounter = bIsChecked;
}

void USettingsMenuWidget::OnShowCombatTextCheckStateChanged(const bool bIsChecked)
{
	CombatTextFrequency->SetIsReadOnly(!bIsChecked);
	NewPlayerSettings.bShowStreakCombatText = bIsChecked;
	if (!bIsChecked)
	{
		CombatTextFrequency->SetText(FText());
		return;
	}
	NewPlayerSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(
		FCString::Atof(*CombatTextFrequency->GetText().ToString()), 1);
}

void USettingsMenuWidget::OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton)
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

void USettingsMenuWidget::SetVideoSettingButtonBackgroundColor(UVideoSettingButton* ClickedButton)
{
	ClickedButton->Button->SetBackgroundColor(BeatshotBlue);
	const int32 ClickedButtonQuality = ClickedButton->Quality;
	UVideoSettingButton* Head = ClickedButton->Next;

	while (Head->Quality != ClickedButtonQuality)
	{
		Head->Button->SetBackgroundColor(White);
		Head = Head->Next;
	}
}

void USettingsMenuWidget::OnSaveSensitivitySettingsButtonClicked()
{
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FPlayerSettings Settings = GI->LoadPlayerSettings();
	Settings.Sensitivity = SensSlider->GetValue();
	GI->SavePlayerSettings(Settings);
	CurrentSensitivityValue->SetText(FText::AsNumber(SensSlider->GetValue()));
}

void USettingsMenuWidget::OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewSensValue);
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	NewPlayerSettings.Sensitivity = NewSensValue;
}

void USettingsMenuWidget::OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewCsgoSensValue / CsgoMultiplier);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / CsgoMultiplier)));
	NewPlayerSettings.Sensitivity = NewCsgoSensValue / CsgoMultiplier;
}

void USettingsMenuWidget::OnSensitivitySliderChanged(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	SensSlider->SetValue(NewSensValue);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
	NewPlayerSettings.Sensitivity = NewSensValue;
}

void USettingsMenuWidget::OnCombatTextFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewPlayerSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void USettingsMenuWidget::SlideButtons(const USlideRightButton* ActiveButton)
{
	for (TTuple<USlideRightButton*, UVerticalBox*>& Elem : MenuWidgets)
	{
		if (Elem.Key != ActiveButton)
		{
			Elem.Key->SlideButton(false);
			continue;
		}
		Elem.Key->SlideButton(true);
		MenuSwitcher->SetActiveWidget(Elem.Value);
	}
}

float USettingsMenuWidget::ChangeValueOnSliderChange(const float SliderValue, UEditableTextBox* TextBoxToChange,
                                                     const float SnapSize)
{
	const float NewValue = UKismetMathLibrary::GridSnap_Float(SliderValue, SnapSize);
	TextBoxToChange->SetText(FText::AsNumber(NewValue));
	return NewValue;
}

float USettingsMenuWidget::ChangeSliderOnValueChange(const FText& TextValue, USlider* SliderToChange,
                                                     const float SnapSize)
{
	const float NewValue = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*TextValue.ToString()), SnapSize);
	SliderToChange->SetValue(NewValue);
	return NewValue;
}

UVideoSettingButton* USettingsMenuWidget::FindVideoSettingButtonFromQuality(
	const int32 Quality, ESettingType SettingType)
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
