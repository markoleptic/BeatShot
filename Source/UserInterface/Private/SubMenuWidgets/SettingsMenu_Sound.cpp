// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/SettingsMenu_Sound.h"
#include "Components/Slider.h"
#include "Components/EditableTextBox.h"
#include "UserInterface.h"
#include "Kismet/GameplayStatics.h"

void USettingsMenu_Sound::NativeConstruct()
{
	Super::NativeConstruct();

	Slider_GlobalSound->OnValueChanged.AddDynamic(this, &USettingsMenu_Sound::OnSliderChanged_GlobalSound);
	Slider_MenuSound->OnValueChanged.AddDynamic(this, &USettingsMenu_Sound::OnSliderChanged_MenuSound);
	Slider_MusicSound->OnValueChanged.AddDynamic(this, &USettingsMenu_Sound::OnSliderChanged_MusicSound);
	Value_GlobalSound->OnTextCommitted.AddDynamic(this, &USettingsMenu_Sound::OnValueChanged_GlobalSound);
	Value_MenuSound->OnTextCommitted.AddDynamic(this, &USettingsMenu_Sound::OnValueChanged_MenuSound);
	Value_MusicSound->OnTextCommitted.AddDynamic(this, &USettingsMenu_Sound::OnValueChanged_MusicSound);
}

void USettingsMenu_Sound::InitSettingCategoryWidget()
{
	Super::InitSettingCategoryWidget();
}

void USettingsMenu_Sound::InitializeAudioSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings)
{
	Slider_GlobalSound->SetValue(InVideoAndSoundSettings.GlobalVolume);
	Slider_MenuSound->SetValue(InVideoAndSoundSettings.MenuVolume);
	Slider_MusicSound->SetValue(InVideoAndSoundSettings.MusicVolume);
	Value_GlobalSound->SetText(FText::AsNumber(InVideoAndSoundSettings.GlobalVolume));
	Value_MenuSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MenuVolume));
	Value_MusicSound->SetText(FText::AsNumber(InVideoAndSoundSettings.MusicVolume));
}

FPlayerSettings_VideoAndSound USettingsMenu_Sound::GetAudioSettings() const
{
	FPlayerSettings_VideoAndSound ReturnSettings;
	ReturnSettings.GlobalVolume = roundf(Slider_GlobalSound->GetValue());
	ReturnSettings.MenuVolume = roundf(Slider_MenuSound->GetValue());
	ReturnSettings.MusicVolume = roundf(Slider_MusicSound->GetValue());
	return ReturnSettings;
}

void USettingsMenu_Sound::OnSliderChanged_GlobalSound(const float NewGlobalSound)
{
	const float Value = UUserInterface::OnSliderChanged(NewGlobalSound, Value_GlobalSound, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenu_Sound::OnSliderChanged_MenuSound(const float NewMenuSound)
{
	const float Value = UUserInterface::OnSliderChanged(NewMenuSound, Value_MenuSound, 1);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenu_Sound::OnSliderChanged_MusicSound(const float NewMusicSound)
{
	UUserInterface::OnSliderChanged(NewMusicSound, Value_MusicSound, 1);
}

void USettingsMenu_Sound::OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType)
{
	const float Value = UUserInterface::OnEditableTextBoxChanged(NewGlobalSound, Value_GlobalSound, Slider_GlobalSound, 1, 0, 100);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, GlobalSound, Value / 100.f, 1, 0.1f);
}

void USettingsMenu_Sound::OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType)
{
	const float Value = UUserInterface::OnEditableTextBoxChanged(NewMenuSound, Value_MenuSound, Slider_MenuSound, 1, 0, 100);
	UGameplayStatics::SetSoundMixClassOverride(GetWorld(), GlobalSoundMix, MenuSound, Value / 100.f, 1, 0.0f);
}

void USettingsMenu_Sound::OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType)
{
	UUserInterface::OnEditableTextBoxChanged(NewMusicSound, Value_MusicSound, Slider_MusicSound, 1, 0, 100);
}
