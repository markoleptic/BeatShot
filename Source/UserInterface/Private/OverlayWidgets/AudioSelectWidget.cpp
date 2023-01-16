// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidgets/AudioSelectWidget.h"
#include "AudioAnalyzerManager.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameUserSettings.h"
#include "OverlayWidgets/PopupMessageWidget.h"

void UAudioSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	NumberFormattingOptions.MinimumIntegralDigits = 2;
	NumberFormattingOptions.MaximumIntegralDigits = 2;
	BackButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::FadeOut);
	StartButton->SetIsEnabled(false);
	StartButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnStartButtonClicked);
	AudioFromFileButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnAudioFromFileButtonClicked);
	StreamAudioButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnStreamAudioButtonClicked);
	SongTitleText->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSongTitleValueCommitted);
	Seconds->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSecondsValueCommitted);
	Minutes->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnMinutesValueCommitted);
	InAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnInAudioDeviceSelectionChanged);
	OutAudioDevices->OnSelectionChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnOutAudioDeviceSelectionChanged);
	PlaybackAudioCheckbox->OnCheckStateChanged.AddUniqueDynamic(this, &UAudioSelectWidget::OnPlaybackAudioCheckStateChanged);

	Tooltip = CreateWidget<UTooltipWidget>(this, TooltipWidgetClass);

	PlaybackAudioQMark->TooltipText = FText::FromStringTable(
		"/Game/StringTables/ST_GameModesWidget.ST_GameModesWidget", "PlaybackAudio");
	PlaybackAudioQMark->OnTooltipImageHovered.AddDynamic(this, &UAudioSelectWidget::OnTooltipImageHovered);
	
	UAudioAnalyzerManager* Manager = NewObject<UAudioAnalyzerManager>(this);
	TArray<FString> OutAudioDeviceList;
	TArray<FString> InAudioDeviceList;
	Manager->GetOutputAudioDevices(OutAudioDeviceList);
	Manager->GetInputAudioDevices(InAudioDeviceList);

	for (FString AudioDevice : OutAudioDeviceList)
	{
		OutAudioDevices->AddOption(AudioDevice);
	}

	for (FString AudioDevice : InAudioDeviceList)
	{
		InAudioDevices->AddOption(AudioDevice);
	}

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
	bShowOpenFileDialog = true;
	AudioFromFileButton->SetBackgroundColor(BeatShotBlue);
	StreamAudioButton->SetBackgroundColor(White);
	StartButton->SetIsEnabled(true);
	AudioDeviceBox->SetVisibility(ESlateVisibility::Collapsed);
	SongTitleLengthBox->SetVisibility(ESlateVisibility::Collapsed);
	InAudioDevices->ClearSelection();
	OutAudioDevices->ClearSelection();
}

void UAudioSelectWidget::OnStreamAudioButtonClicked()
{
	bShowOpenFileDialog = false;
	AudioFromFileButton->SetBackgroundColor(White);
	StreamAudioButton->SetBackgroundColor(BeatShotBlue);
	AudioDeviceBox->SetVisibility(ESlateVisibility::Visible);
}

void UAudioSelectWidget::OnStartButtonClicked()
{
	if (bShowOpenFileDialog)
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
		if (FileNames.Num() == 0 || FileNames[0].IsEmpty())
		{
			ShowSongPathErrorMessage();
			return;
		}
		AudioSelectStruct.SongPath = FileNames[0];
	}
	if(!OnStartButtonClickedDelegate.ExecuteIfBound(AudioSelectStruct))
	{
		UE_LOG(LogTemp, Display, TEXT("OnStartButtonClickedDelegate not bound."));
	}
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

void UAudioSelectWidget::OnInAudioDeviceSelectionChanged(const FString SelectedInAudioDevice,
	const ESelectInfo::Type SelectionType)
{
	AudioSelectStruct.InAudioDevice = SelectedInAudioDevice;
	if (OutAudioDevices->GetSelectedIndex() != -1 && InAudioDevices->GetSelectedIndex() != -1)
	{
		SongTitleLengthBox->SetVisibility(ESlateVisibility::Visible);
		StartButton->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnOutAudioDeviceSelectionChanged(const FString SelectedOutAudioDevice,
	const ESelectInfo::Type SelectionType)
{
	AudioSelectStruct.OutAudioDevice = SelectedOutAudioDevice;
	if (OutAudioDevices->GetSelectedIndex() != -1 && InAudioDevices->GetSelectedIndex() != -1)
	{
		SongTitleLengthBox->SetVisibility(ESlateVisibility::Visible);
		StartButton->SetIsEnabled(true);
	}
}

void UAudioSelectWidget::OnPlaybackAudioCheckStateChanged(const bool bIsChecked)
{
	AudioSelectStruct.bPlaybackAudio = bIsChecked;
}

void UAudioSelectWidget::OpenSongFileDialog_Implementation(TArray<FString>& OutFileNames)
{
	/** Cheap fix to make sure open file dialog is always on top of the game */
	if (UGameUserSettings* GameUserSettings = UGameUserSettings::GetGameUserSettings(); GameUserSettings->
		GetFullscreenMode() == EWindowMode::Fullscreen)
	{
		bWasInFullScreenMode = true;
		GameUserSettings->SetFullscreenMode(EWindowMode::WindowedFullscreen);
		GameUserSettings->ApplySettings(false);
	}
}

void UAudioSelectWidget::OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage, const FText& TooltipTextToShow)
{
	Tooltip->TooltipDescriptor->SetText(TooltipTextToShow);
	HoveredTooltipImage->SetToolTip(Tooltip);
}

void UAudioSelectWidget::ShowSongPathErrorMessage()
{
	PopupMessageWidget = CreateWidget<UPopupMessageWidget>(GetWorld(), PopupMessageClass);
	PopupMessageWidget->InitPopup("Error",
								  "There was a problem loading the song. Make sure the song is in mp3 or ogg format. If this problem persists, please contact support.",
								  "Okay");
	PopupMessageWidget->Button1->OnClicked.AddDynamic(this, &UAudioSelectWidget::HideSongPathErrorMessage);
	PopupMessageWidget->AddToViewport();
	PopupMessageWidget->FadeIn();
}

void UAudioSelectWidget::HideSongPathErrorMessage()
{
	PopupMessageWidget->RemoveFromViewport();
	FadeOut();
}