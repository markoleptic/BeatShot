// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.


#include "OverlayWidgets/AudioSelectWidget.h"
#include "AudioAnalyzerManager.h"
#include "WidgetComponents/TooltipImage.h"
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
	BackButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::FadeOut);
	StartButton->SetIsEnabled(false);
	LoadFileButton->SetIsEnabled(false);
	StartButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnStartButtonClicked);
	AudioFromFileButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnAudioFromFileButtonClicked);
	LoadFileButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnLoadFileButtonClicked);
	CaptureAudioButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnCaptureAudioButtonClicked);
	SongTitleText->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSongTitleValueCommitted);
	Seconds->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSecondsValueCommitted);
	Minutes->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnMinutesValueCommitted);
	InAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnInAudioDeviceSelectionChanged);
	OutAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnOutAudioDeviceSelectionChanged);
	SongTitleComboBox->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnSongTitleSelectionChanged);
	PlaybackAudioCheckbox->OnCheckStateChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnPlaybackAudioCheckStateChanged);

	AddToTooltipData(PlaybackAudioQMark, FText::FromStringTable("/Game/StringTables/ST_Tooltips.ST_Tooltips", "PlaybackAudio"));

	UAudioAnalyzerManager* Manager = NewObject<UAudioAnalyzerManager>(this);
	TArray<FString> OutAudioDeviceList;
	TArray<FString> InAudioDeviceList;
	Manager->GetOutputAudioDevices(OutAudioDeviceList);
	Manager->GetInputAudioDevices(InAudioDeviceList);
	for (const FString& AudioDevice : OutAudioDeviceList)
	{
		OutAudioDevices->AddOption(AudioDevice);
	}
	for (const FString& AudioDevice : InAudioDeviceList)
	{
		InAudioDevices->AddOption(AudioDevice);
	}
	PopulateSongOptionComboBox();
	AudioDeviceBox->SetVisibility(ESlateVisibility::Collapsed);
	SongTitleLengthBox->SetVisibility(ESlateVisibility::Collapsed);
	OnSecondsValueCommitted(FText::AsNumber(0), ETextCommit::Type::Default);
	OnMinutesValueCommitted(FText::AsNumber(0), ETextCommit::Type::Default);
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

void UAudioSelectWidget::OnAudioFromFileButtonClicked()
{
	AudioSelectStruct.AudioFormat = EAudioFormat::File;
	StartButton->SetIsEnabled(false);
	LoadFileButton->SetIsEnabled(true);
	InAudioDevices->ClearSelection();
	OutAudioDevices->ClearSelection();
	AudioFromFileButton->SetBackgroundColor(BeatShotBlue);
	CaptureAudioButton->SetBackgroundColor(White);
	AudioDeviceBox->SetVisibility(ESlateVisibility::Collapsed);
	SongTitleLengthBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UAudioSelectWidget::OnCaptureAudioButtonClicked()
{
	AudioSelectStruct.AudioFormat = EAudioFormat::Capture;
	StartButton->SetIsEnabled(false);
	LoadFileButton->SetIsEnabled(false);
	InAudioDevices->ClearSelection();
	OutAudioDevices->ClearSelection();
	AudioFromFileButton->SetBackgroundColor(White);
	CaptureAudioButton->SetBackgroundColor(BeatShotBlue);
	AudioDeviceBox->SetVisibility(ESlateVisibility::Visible);
	SongTitleLengthBox->SetVisibility(ESlateVisibility::Collapsed);

	const FAASettingsStruct PlayerSettings = LoadAASettings();
	InAudioDevices->SetSelectedOption(PlayerSettings.LastSelectedInputAudioDevice);
	OutAudioDevices->SetSelectedOption(PlayerSettings.LastSelectedOutputAudioDevice);
}

void UAudioSelectWidget::OnStartButtonClicked()
{
	FAASettingsStruct PlayerSettings = LoadAASettings();
	if (!InAudioDevices->GetSelectedOption().IsEmpty())
	{
		PlayerSettings.LastSelectedInputAudioDevice = InAudioDevices->GetSelectedOption();
	}
	if (!OutAudioDevices->GetSelectedOption().IsEmpty())
	{
		PlayerSettings.LastSelectedOutputAudioDevice = OutAudioDevices->GetSelectedOption();
	}
	SaveAASettings(PlayerSettings);
	if (!OnStartButtonClickedDelegate.ExecuteIfBound(AudioSelectStruct))
	{
		UE_LOG(LogTemp, Display, TEXT("OnStartButtonClickedDelegate not bound."));
	}
}

void UAudioSelectWidget::OnLoadFileButtonClicked()
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
	AudioSelectStruct.SongPath = FileNames[0];

	UAudioAnalyzerManager* Manager = NewObject<UAudioAnalyzerManager>(this);
	if (!Manager->InitPlayerAudio(AudioSelectStruct.SongPath))
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
		if (SongTitleComboBox->FindOptionIndex(Filename) == -1)
		{
			SongTitleComboBox->AddOption(Filename);
		}
		SongTitleComboBox->SetSelectedOption(Filename);
		SongTitleText->SetText(FText::FromString(Filename));
		AudioSelectStruct.SongTitle = Filename;
	}
	else
	{
		if (SongTitleComboBox->FindOptionIndex(Title) == -1)
		{
			SongTitleComboBox->AddOption(Title);
		}
		SongTitleComboBox->SetSelectedOption(Title);
		SongTitleText->SetText(FText::FromString(Title));
		AudioSelectStruct.SongTitle = Title;
	}
	AudioSelectStruct.SongLength = Manager->GetTotalDuration();
	SongTitleLengthBox->SetVisibility(ESlateVisibility::Visible);
	SongTitleBox->SetVisibility(ESlateVisibility::Visible);
	SongLengthBox->SetVisibility(ESlateVisibility::Collapsed);
	StartButton->SetIsEnabled(true);
}

void UAudioSelectWidget::OnSongTitleValueCommitted(const FText& NewSongTitle, ETextCommit::Type CommitType)
{
	if (NewSongTitle.IsEmptyOrWhitespace())
	{
		AudioSelectStruct.SongTitle = "";
	}
	else
	{
		AudioSelectStruct.SongTitle = NewSongTitle.ToString();
	}
}

void UAudioSelectWidget::OnMinutesValueCommitted(const FText& NewMinutes, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewMinutes.ToString()), 0, 99);
	Minutes->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	AudioSelectStruct.SongLength = ClampedValue * 60 + FMath::Clamp(FCString::Atoi(*Seconds->GetText().ToString()), 0, 60);
}

void UAudioSelectWidget::OnSecondsValueCommitted(const FText& NewSeconds, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewSeconds.ToString()), 0, 60);
	Seconds->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	AudioSelectStruct.SongLength = FMath::Clamp(FCString::Atoi(*Minutes->GetText().ToString()), 0, 99) * 60 + ClampedValue;
}

void UAudioSelectWidget::OnInAudioDeviceSelectionChanged(const FString SelectedInAudioDevice, const ESelectInfo::Type SelectionType)
{
	UE_LOG(LogTemp, Display, TEXT("Selection changed"));
	AudioSelectStruct.InAudioDevice = SelectedInAudioDevice;
	if (OutAudioDevices->GetSelectedIndex() != -1 && InAudioDevices->GetSelectedIndex() != -1)
	{
		SongTitleLengthBox->SetVisibility(ESlateVisibility::Visible);
		SongTitleBox->SetVisibility(ESlateVisibility::Visible);
		SongLengthBox->SetVisibility(ESlateVisibility::Visible);
		StartButton->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnOutAudioDeviceSelectionChanged(const FString SelectedOutAudioDevice, const ESelectInfo::Type SelectionType)
{
	AudioSelectStruct.OutAudioDevice = SelectedOutAudioDevice;
	if (OutAudioDevices->GetSelectedIndex() != -1 && InAudioDevices->GetSelectedIndex() != -1)
	{
		SongTitleLengthBox->SetVisibility(ESlateVisibility::Visible);
		SongTitleBox->SetVisibility(ESlateVisibility::Visible);
		SongLengthBox->SetVisibility(ESlateVisibility::Visible);
		StartButton->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnSongTitleSelectionChanged(const FString SelectedSongTitle, const ESelectInfo::Type SelectionType)
{
	AudioSelectStruct.SongTitle = SelectedSongTitle;
}

void UAudioSelectWidget::OnPlaybackAudioCheckStateChanged(const bool bIsChecked)
{
	AudioSelectStruct.bPlaybackAudio = bIsChecked;
}

void UAudioSelectWidget::PopulateSongOptionComboBox()
{
	for (FPlayerScore SavedScoreObj : LoadPlayerScores())
	{
		if (SongTitleComboBox->FindOptionIndex(SavedScoreObj.SongTitle) == -1)
		{
			SongTitleComboBox->AddOption(SavedScoreObj.SongTitle);
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
	PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UAudioSelectWidget::HideSongPathErrorMessage);
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UAudioSelectWidget::HideSongPathErrorMessage()
{
	PopupMessageWidget->FadeOut();
	FadeOut();
}
