// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "SubMenuWidgets/VideoAndSoundSettingsWidget.h"
#include "Components/VerticalBox.h"
#include "SubMenuWidgets/SettingsMenu_Video.h"
#include "SubMenuWidgets/SettingsMenu_Sound.h"
#include "GameFramework/GameUserSettings.h"
#include "OverlayWidgets/PopupMessageWidget.h"
#include "WidgetComponents/SavedTextWidget.h"

void UVideoAndSoundSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	ResetButton_VideoAndSound->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnResetButtonClicked_VideoAndSound);
	SaveButton_VideoAndSound->OnClicked.AddDynamic(this, &UVideoAndSoundSettingsWidget::OnSaveButtonClicked_VideoAndSound);

	VideoSettings = CreateWidget<USettingsMenu_Video>(this, VideoSettingsClass);
	VideoSettings->OnWindowOrResolutionChanged.AddUObject(this, &UVideoAndSoundSettingsWidget::ShowConfirmVideoSettingsMessage);
	VideoSettingsBox->AddChildToVerticalBox(VideoSettings.Get());
	SoundSettings = CreateWidget<USettingsMenu_Sound>(this, SoundSettingsClass);
	SoundSettingsBox->AddChildToVerticalBox(SoundSettings.Get());
	const FPlayerSettings_VideoAndSound Settings = LoadPlayerSettings().VideoAndSound;
	VideoSettings->InitializeVideoSettings(Settings);
	SoundSettings->InitializeAudioSettings(Settings);
}

void UVideoAndSoundSettingsWidget::OnSaveButtonClicked_VideoAndSound()
{
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	FPlayerSettings_VideoAndSound Video = VideoSettings->GetVideoSettings();
	const FPlayerSettings_VideoAndSound Sound = SoundSettings->GetAudioSettings();
	Video.GlobalVolume = Sound.GlobalVolume;
	Video.MenuVolume = Sound.MenuVolume;
	Video.MusicVolume = Sound.MusicVolume;
	SavePlayerSettings(Video);
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UVideoAndSoundSettingsWidget::OnResetButtonClicked_VideoAndSound()
{
	UGameUserSettings::GetGameUserSettings()->SetToDefaults();
	UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	VideoSettings->InitializeVideoSettings(FPlayerSettings_VideoAndSound());
	SoundSettings->InitializeAudioSettings(FPlayerSettings_VideoAndSound());
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
	VideoSettings->OnConfirmVideoSettingsButtonClicked();
	PopupMessageWidget->FadeOut();
	SavedTextWidget->SetSavedText(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "SM_Saved_VideoAndSound"));
	SavedTextWidget->PlayFadeInFadeOut();
}

void UVideoAndSoundSettingsWidget::OnCancelVideoSettingsButtonClicked()
{
	GetWorld()->GetTimerManager().ClearTimer(RevertVideoSettingsTimer);
	VideoSettings->OnCancelVideoSettingsButtonClicked();
	PopupMessageWidget->FadeOut();
}
