// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/AudioSelectWidget.h"
#include "AudioAnalyzerManager.h"
#include "WidgetComponents/TooltipWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "GameFramework/GameUserSettings.h"
#include "OverlayWidgets/PopupMessageWidget.h"

UTooltipWidget* UAudioSelectWidget::ConstructTooltipWidget()
{
	return CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);
}

void UAudioSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetTooltipWidget(ConstructTooltipWidget());
	NumberFormattingOptions.MinimumIntegralDigits = 2;
	NumberFormattingOptions.MaximumIntegralDigits = 2;
	Button_Back->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::FadeOut);
	Button_Start->SetIsEnabled(false);
	Button_LoadFile->SetIsEnabled(false);
	Button_Start->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnButtonClicked_Start);
	Button_AudioFromFile->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnButtonClicked_AudioFromFile);
	Button_LoadFile->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnButtonClicked_LoadFile);
	Button_CaptureAudio->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnButtonClicked_CaptureAudio);
	Value_SongTitle->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnValueChanged_SongTitle);
	Value_Seconds->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnValueChanged_Seconds);
	Value_Minutes->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnValueChanged_Minutes);
	ComboBox_InAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnSelectionChanged_InAudioDevice);
	ComboBox_OutAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnSelectionChanged_OutAudioDevice);
	ComboBox_SongTitle->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnSelectionChanged_SongTitle);
	Checkbox_PlaybackAudio->OnCheckStateChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnCheckStateChanged_PlaybackAudio);

	AddToTooltipData(QMark_PlaybackAudio, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "PlaybackAudio"));

	UAudioAnalyzerManager* Manager = NewObject<UAudioAnalyzerManager>(this);
	TArray<FString> OutAudioDeviceList;
	TArray<FString> InAudioDeviceList;
	Manager->GetOutputAudioDevices(OutAudioDeviceList);
	Manager->GetInputAudioDevices(InAudioDeviceList);
	for (const FString& AudioDevice : OutAudioDeviceList)
	{
		ComboBox_OutAudioDevices->AddOption(AudioDevice);
	}
	for (const FString& AudioDevice : InAudioDeviceList)
	{
		ComboBox_InAudioDevices->AddOption(AudioDevice);
	}
	PopulateSongOptionComboBox();
	Box_AudioDevice->SetVisibility(ESlateVisibility::Collapsed);
	Box_SongTitleLength->SetVisibility(ESlateVisibility::Collapsed);
	OnValueChanged_Seconds(FText::AsNumber(0), ETextCommit::Type::Default);
	OnValueChanged_Minutes(FText::AsNumber(0), ETextCommit::Type::Default);
}

void UAudioSelectWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UAudioSelectWidget::FadeIn()
{
	PlayAnimationForward(FadeInAnim);
}

void UAudioSelectWidget::FadeOut()
{
	FadeOutDelegate.BindDynamic(this, &UAudioSelectWidget::OnFadeOutFinish);
	PlayAnimationForward(FadeOutAnim);
}

void UAudioSelectWidget::OnFadeOutFinish()
{
	RemoveFromParent();
}

void UAudioSelectWidget::OnButtonClicked_AudioFromFile()
{
	AudioConfig.AudioFormat = EAudioFormat::File;
	Button_Start->SetIsEnabled(false);
	Button_LoadFile->SetIsEnabled(true);
	ComboBox_InAudioDevices->ClearSelection();
	ComboBox_OutAudioDevices->ClearSelection();
	Button_AudioFromFile->SetBackgroundColor(BeatShotBlue);
	Button_CaptureAudio->SetBackgroundColor(FLinearColor::White);
	Box_AudioDevice->SetVisibility(ESlateVisibility::Collapsed);
	Box_SongTitleLength->SetVisibility(ESlateVisibility::Collapsed);
}

void UAudioSelectWidget::OnButtonClicked_CaptureAudio()
{
	AudioConfig.AudioFormat = EAudioFormat::Capture;
	Button_Start->SetIsEnabled(false);
	Button_LoadFile->SetIsEnabled(false);
	ComboBox_InAudioDevices->ClearSelection();
	ComboBox_OutAudioDevices->ClearSelection();
	Button_AudioFromFile->SetBackgroundColor(FLinearColor::White);
	Button_CaptureAudio->SetBackgroundColor(Constants::BeatShotBlue);
	Box_AudioDevice->SetVisibility(ESlateVisibility::Visible);
	Box_SongTitleLength->SetVisibility(ESlateVisibility::Collapsed);

	const FPlayerSettings_AudioAnalyzer PlayerSettings = LoadPlayerSettings().AudioAnalyzer;
	ComboBox_InAudioDevices->SetSelectedOption(PlayerSettings.LastSelectedInputAudioDevice);
	ComboBox_OutAudioDevices->SetSelectedOption(PlayerSettings.LastSelectedOutputAudioDevice);
}

void UAudioSelectWidget::OnButtonClicked_Start()
{
	FPlayerSettings_AudioAnalyzer PlayerSettings = LoadPlayerSettings().AudioAnalyzer;
	if (!ComboBox_InAudioDevices->GetSelectedOption().IsEmpty())
	{
		PlayerSettings.LastSelectedInputAudioDevice = ComboBox_InAudioDevices->GetSelectedOption();
	}
	if (!ComboBox_OutAudioDevices->GetSelectedOption().IsEmpty())
	{
		PlayerSettings.LastSelectedOutputAudioDevice = ComboBox_OutAudioDevices->GetSelectedOption();
	}
	SavePlayerSettings(PlayerSettings);
	if (!OnStartButtonClickedDelegate.ExecuteIfBound(AudioConfig))
	{
		UE_LOG(LogTemp, Display, TEXT("OnStartButtonClickedDelegate not bound."));
	}
}

void UAudioSelectWidget::OnButtonClicked_LoadFile()
{
	TArray<FString> FileNames = {""};
	OpenSongFileDialog(FileNames);
	if (bWasInFullScreenMode)
	{
		UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings();
		GameUserSettings->SetFullscreenMode(EWindowMode::Fullscreen);
		GameUserSettings->ApplySettings(false);
		bWasInFullScreenMode = false;
	}
	if (FileNames.IsEmpty() || FileNames[0].IsEmpty())
	{
		ShowSongPathErrorMessage();
		return;
	}
	AudioConfig.SongPath = FileNames[0];

	UAudioAnalyzerManager* Manager = NewObject<UAudioAnalyzerManager>(this);
	if (!Manager->InitPlayerAudio(AudioConfig.SongPath))
	{
		ShowSongPathErrorMessage();
		UE_LOG(LogTemp, Display, TEXT("Init Tracker Error"));
		return;
	}
	/* set Song length and song title in BSConfig if using song file */
	FString Filename, Extension, MetaType, Title, Artist, Album, Year, Genre;
	Manager->GetMetadata(Filename, Extension, MetaType, Title, Artist, Album, Year, Genre);
	if (Title.IsEmpty())
	{
		if (ComboBox_SongTitle->FindOptionIndex(Filename) == -1)
		{
			ComboBox_SongTitle->AddOption(Filename);
		}
		ComboBox_SongTitle->SetSelectedOption(Filename);
		Value_SongTitle->SetText(FText::FromString(Filename));
		AudioConfig.SongTitle = Filename;
	}
	else
	{
		if (ComboBox_SongTitle->FindOptionIndex(Title) == -1)
		{
			ComboBox_SongTitle->AddOption(Title);
		}
		ComboBox_SongTitle->SetSelectedOption(Title);
		Value_SongTitle->SetText(FText::FromString(Title));
		AudioConfig.SongTitle = Title;
	}
	AudioConfig.SongLength = Manager->GetTotalDuration();
	Box_SongTitleLength->SetVisibility(ESlateVisibility::Visible);
	Box_SongTitle->SetVisibility(ESlateVisibility::Visible);
	Box_SongLength->SetVisibility(ESlateVisibility::Collapsed);
	Button_Start->SetIsEnabled(true);
}

void UAudioSelectWidget::OnValueChanged_SongTitle(const FText& NewSongTitle, ETextCommit::Type CommitType)
{
	if (NewSongTitle.IsEmptyOrWhitespace())
	{
		AudioConfig.SongTitle = "";
	}
	else
	{
		AudioConfig.SongTitle = NewSongTitle.ToString();
	}
}

void UAudioSelectWidget::OnValueChanged_Minutes(const FText& NewMinutes, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewMinutes.ToString()), 0, 99);
	Value_Minutes->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	AudioConfig.SongLength = ClampedValue * 60 + FMath::Clamp(FCString::Atoi(*Value_Seconds->GetText().ToString()), 0, 60);
}

void UAudioSelectWidget::OnValueChanged_Seconds(const FText& NewSeconds, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewSeconds.ToString()), 0, 60);
	Value_Seconds->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	AudioConfig.SongLength = FMath::Clamp(FCString::Atoi(*Value_Minutes->GetText().ToString()), 0, 99) * 60 + ClampedValue;
}

void UAudioSelectWidget::OnSelectionChanged_InAudioDevice(const FString SelectedInAudioDevice, const ESelectInfo::Type SelectionType)
{
	AudioConfig.InAudioDevice = SelectedInAudioDevice;
	if (ComboBox_OutAudioDevices->GetSelectedIndex() != -1 && ComboBox_InAudioDevices->GetSelectedIndex() != -1)
	{
		Box_SongTitleLength->SetVisibility(ESlateVisibility::Visible);
		Box_SongTitle->SetVisibility(ESlateVisibility::Visible);
		Box_SongLength->SetVisibility(ESlateVisibility::Visible);
		Button_Start->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnSelectionChanged_OutAudioDevice(const FString SelectedOutAudioDevice, const ESelectInfo::Type SelectionType)
{
	AudioConfig.OutAudioDevice = SelectedOutAudioDevice;
	if (ComboBox_OutAudioDevices->GetSelectedIndex() != -1 && ComboBox_InAudioDevices->GetSelectedIndex() != -1)
	{
		Box_SongTitleLength->SetVisibility(ESlateVisibility::Visible);
		Box_SongTitle->SetVisibility(ESlateVisibility::Visible);
		Box_SongLength->SetVisibility(ESlateVisibility::Visible);
		Button_Start->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnSelectionChanged_SongTitle(const FString SelectedSongTitle, const ESelectInfo::Type SelectionType)
{
	AudioConfig.SongTitle = SelectedSongTitle;
}

void UAudioSelectWidget::OnCheckStateChanged_PlaybackAudio(const bool bIsChecked)
{
	AudioConfig.bPlaybackAudio = bIsChecked;
}

void UAudioSelectWidget::PopulateSongOptionComboBox()
{
	for (FPlayerScore SavedScoreObj : LoadPlayerScores())
	{
		if (ComboBox_SongTitle->FindOptionIndex(SavedScoreObj.SongTitle) == -1)
		{
			ComboBox_SongTitle->AddOption(SavedScoreObj.SongTitle);
		}
	}
}

void UAudioSelectWidget::OpenSongFileDialog_Implementation(TArray<FString>& OutFileNames)
{
	/** Cheap fix to make sure open file dialog is always on top of the game */
	if (UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings(); GameUserSettings->GetFullscreenMode() == EWindowMode::Fullscreen)
	{
		bWasInFullScreenMode = true;
		GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GameUserSettings->ApplySettings(false);
	}
}

void UAudioSelectWidget::ShowSongPathErrorMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(GetWorld(), PopupMessageClass);
	PopupMessageWidget->InitPopup(FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorTitle"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorMessage"),
	                              FText::FromStringTable("/Game/StringTables/ST_Widgets.ST_Widgets", "ASW_SongPathErrorButton"));
	PopupMessageWidget->Button_1->OnClicked.AddDynamic(this, &UAudioSelectWidget::HideSongPathErrorMessage);
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UAudioSelectWidget::HideSongPathErrorMessage()
{
	PopupMessageWidget->FadeOut();
	FadeOut();
}
