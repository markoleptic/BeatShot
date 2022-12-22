// Fill out your copyright notice in the Description page of Project Settings.


// ReSharper disable CppMemberFunctionMayBeConst
#include "SettingsMenuWidget.h"
#include "DefaultGameInstance.h"
#include "SlideRightButton.h"
#include "Components/WidgetSwitcher.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/Slider.h"
#include <Blueprint/UserWidget.h>
#include <Kismet/GameplayStatics.h>
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "Sound/SoundMix.h"

void USettingsMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	{
		AA0->SettingType, AA1->SettingType, AA2->SettingType, AA3->SettingType = ESettingType::AntiAliasing;
		AA0->Quality = 0;
		AA1->Quality = 1;
		AA2->Quality = 2;
		AA3->Quality = 3;

		GIQ0->SettingType, GIQ1->SettingType, GIQ2->SettingType, GIQ3->SettingType = ESettingType::GlobalIllumination;
		GIQ0->Quality = 0;
		GIQ1->Quality = 1;
		GIQ2->Quality = 2;
		GIQ3->Quality = 3;

		PP0->SettingType, PP1->SettingType, PP2->SettingType, PP3->SettingType = ESettingType::PostProcessing;
		PP0->Quality = 0;
		PP1->Quality = 1;
		PP2->Quality = 2;
		PP3->Quality = 3;

		RQ0->SettingType, RQ1->SettingType, RQ2->SettingType, RQ3->SettingType = ESettingType::Reflection;
		RQ0->Quality = 0;
		RQ1->Quality = 1;
		RQ2->Quality = 2;
		RQ3->Quality = 3;

		SW0->SettingType, SW1->SettingType, SW2->SettingType, SW3->SettingType = ESettingType::Shadow;
		SW0->Quality = 0;
		SW1->Quality = 1;
		SW2->Quality = 2;
		SW3->Quality = 3;

		SGQ0->SettingType, SGQ1->SettingType, SGQ2->SettingType, SGQ3->SettingType = ESettingType::Shading;
		SGQ0->Quality = 0;
		SGQ1->Quality = 1;
		SGQ2->Quality = 2;
		SGQ3->Quality = 3;

		TQ0->SettingType, TQ1->SettingType, TQ2->SettingType, TQ3->SettingType = ESettingType::Texture;
		TQ0->Quality = 0;
		TQ1->Quality = 1;
		TQ2->Quality = 2;
		TQ3->Quality = 3;

		VD0->SettingType, VD1->SettingType, VD2->SettingType, VD3->SettingType = ESettingType::ViewDistance;
		VD0->Quality = 0;
		VD1->Quality = 1;
		VD2->Quality = 2;
		VD3->Quality = 3;

		VEQ0->SettingType, VEQ1->SettingType, VEQ2->SettingType, VEQ3->SettingType = ESettingType::VisualEffect;
		VEQ0->Quality = 0;
		VEQ1->Quality = 1;
		VEQ2->Quality = 2;
		VEQ3->Quality = 3;
	}
	GlobalSound = GlobalSoundClass->GetDefaultObject<USoundClass>();
	MenuSound = MenuSoundClass->GetDefaultObject<USoundClass>();
	GlobalSoundMix = GlobalSoundMixClass->GetDefaultObject<USoundMix>();

	MenuWidgets.Add(VideoAndSoundSettingsButton, VideoAndSoundSettings);
	MenuWidgets.Add(AASettingsButton, AASettings);
	MenuWidgets.Add(SensitivityButton, Sensitivity);
	MenuWidgets.Add(CrossHairButton, CrossHair);

	VideoAndSoundSettingsButton->Button->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::OnVideoAndSoundSettingsButtonClicked);
	AASettingsButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnAASettingsButtonClicked);
	SensitivityButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnSensitivityButtonClicked);
	CrossHairButton->Button->OnClicked.AddDynamic(this, &USettingsMenuWidget::OnCrossHairButtonClicked);

	NewSensitivityValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnNewSensitivityValue);
	NewSensitivityCsgoValue->OnTextCommitted.AddDynamic(this, &USettingsMenuWidget::OnNewSensitivityCsgoValue);
	SensSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnSensitivitySliderChanged);

	SaveVideoAndSoundSettingsButton->OnClicked.AddDynamic(
		this, &USettingsMenuWidget::SaveVideoAndSoundSettingsButtonClicked);

	LoadPlayerSettings();
	PopulatePlayerSettings();
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

void USettingsMenuWidget::ResetPlayerSettings()
{
	NewPlayerSettings.ResetStruct();
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

void USettingsMenuWidget::SaveVideoAndSoundSettingsButtonClicked()
{
	const UDefaultGameInstance* GI = Cast<UDefaultGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	FPlayerSettings Settings = GI->LoadPlayerSettings();
	Settings.GlobalVolume = GlobalSoundSlider->GetValue();
	Settings.MenuVolume = MenuSoundSlider->GetValue();
	Settings.MusicVolume = MusicSoundSlider->GetValue();
	GI->SavePlayerSettings(Settings);
}

void USettingsMenuWidget::OnGlobalSoundSliderChanged(const float NewValue)
{
	const float NewVolume = ChangeValueOnSliderChange(NewValue, GlobalSoundInputValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, NewVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	NewPlayerSettings.GlobalVolume = NewVolume;
}

void USettingsMenuWidget::OnMenuSoundSliderChanged(const float NewValue)
{
	const float NewVolume = ChangeValueOnSliderChange(NewValue, MenuSoundInputValue, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, NewVolume, 1, 0.1f);
	UGameplayStatics::PushSoundMixModifier(GetWorld(), GlobalSoundMix);
	NewPlayerSettings.MenuVolume = NewVolume;
}

void USettingsMenuWidget::OnMusicSoundSliderChanged(const float NewValue)
{
	const float NewVolume = ChangeValueOnSliderChange(NewValue, MusicSoundInputValue, 1);
	NewPlayerSettings.MusicVolume = NewVolume;
}

void USettingsMenuWidget::OnGlobalSoundValueChanged(const FText& NewValue)
{
	ChangeSliderOnValueChange(NewValue, GlobalSoundSlider, 1);
}

void USettingsMenuWidget::OnMenuSoundValueChanged(const FText& NewValue)
{
	ChangeSliderOnValueChange(NewValue, MenuSoundSlider, 1);
}

void USettingsMenuWidget::OnMusicSoundValueChanged(const FText& NewValue)
{
	ChangeSliderOnValueChange(NewValue, MusicSoundSlider, 1);
}

void USettingsMenuWidget::OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	if (!SelectionType == ESelectInfo::Type::Direct)
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
	/** TODO: Resolution stuff, populating resolution combo box */
}

void USettingsMenuWidget::OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType)
{
	FString LeftS;
	FString RightS;
	SelectedOption.Split("x", &LeftS, &RightS);
	const FIntPoint NewResolution = FIntPoint(UKismetStringLibrary::Conv_StringToInt(LeftS), UKismetStringLibrary::Conv_StringToInt(RightS));
	UGameUserSettings::GetGameUserSettings()->SetScreenResolution(NewResolution);
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

void USettingsMenuWidget::OnStreakFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType)
{
	NewPlayerSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*NewValue.ToString()), 1);
}

void USettingsMenuWidget::OnShowStreakCombatTextCheckStateChanged(const bool bIsChecked)
{
	StreakFrequency->SetIsReadOnly(bIsChecked);
	NewPlayerSettings.bShowStreakCombatText = bIsChecked;
	if (!bIsChecked)
	{
		StreakFrequency->SetText(FText());
		return;
	}
	NewPlayerSettings.CombatTextFrequency = UKismetMathLibrary::GridSnap_Float(FCString::Atof(*StreakFrequency->GetText().ToString()), 1);
}

void USettingsMenuWidget::OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton)
{
	/** TODO: Change colors of buttons */
	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	switch (ClickedButton->SettingType)
	{
	case ESettingType::AntiAliasing:
		{
			Settings->SetAntiAliasingQuality(ClickedButton->Quality);
		}
	case ESettingType::GlobalIllumination:
		{
			Settings->SetGlobalIlluminationQuality(ClickedButton->Quality);
		}
	case ESettingType::PostProcessing:
		{
			Settings->SetPostProcessingQuality(ClickedButton->Quality);
		}
	case ESettingType::Reflection:
		{
			Settings->SetReflectionQuality(ClickedButton->Quality);
		}
	case ESettingType::Shadow:
		{
			Settings->SetShadowQuality(ClickedButton->Quality);
		}
	case ESettingType::Shading:
		{
			Settings->SetShadingQuality(ClickedButton->Quality);
		}
	case ESettingType::Texture:
		{
			Settings->SetTextureQuality(ClickedButton->Quality);
		}
	case ESettingType::ViewDistance:
		{
			Settings->SetViewDistanceQuality(ClickedButton->Quality);
		}
	case ESettingType::VisualEffect:
		{
			Settings->SetVisualEffectQuality(ClickedButton->Quality);
		}
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
}

void USettingsMenuWidget::OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType)
{
	const float NewCsgoSensValue = FCString::Atof(*NewValue.ToString());
	SensSlider->SetValue(NewCsgoSensValue / CsgoMultiplier);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewCsgoSensValue / CsgoMultiplier)));
}

void USettingsMenuWidget::OnSensitivitySliderChanged(const float NewValue)
{
	const float NewSensValue = UKismetMathLibrary::GridSnap_Float(NewValue, 0.1);
	SensSlider->SetValue(NewSensValue);
	NewSensitivityValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue)));
	NewSensitivityCsgoValue->SetText(FText::FromString(FString::SanitizeFloat(NewSensValue * CsgoMultiplier)));
}
