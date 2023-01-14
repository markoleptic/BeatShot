// Fill out your copyright notice in the Description page of Project Settings.


#include "OverlayWidgets/AudioSelectWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/VerticalBox.h"

void UAudioSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();
	NumberFormattingOptions.MinimumIntegralDigits = 2;
	NumberFormattingOptions.MaximumIntegralDigits = 2;
	StartButton->SetIsEnabled(false);
	AudioFromFileButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnAudioFromFileButtonClicked);
	LoopbackAudioButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnLoopbackAudioButtonClicked);
	StartButton->OnClicked.AddUniqueDynamic(this, &UAudioSelectWidget::OnStartButtonClicked);
	SongTitleText->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSongTitleValueCommitted);
	Seconds->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnSecondsValueCommitted);
	Minutes->OnTextCommitted.AddUniqueDynamic(this, &UAudioSelectWidget::OnMinutesValueCommitted);
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
	LoopbackAudioButton->SetBackgroundColor(White);
	StartButton->SetIsEnabled(true);
	LoopbackAudioBox->SetVisibility(ESlateVisibility::Collapsed);
}

void UAudioSelectWidget::OnLoopbackAudioButtonClicked()
{
	bShowOpenFileDialog = false;
	AudioFromFileButton->SetBackgroundColor(White);
	LoopbackAudioButton->SetBackgroundColor(BeatShotBlue);
	StartButton->SetIsEnabled(true);
	LoopbackAudioBox->SetVisibility(ESlateVisibility::Visible);
}

void UAudioSelectWidget::OnStartButtonClicked()
{
	if(!OnStartButtonClickedDelegate.ExecuteIfBound(bShowOpenFileDialog, SongTitle, SongLength))
	{
		UE_LOG(LogTemp, Display, TEXT("OnStartButtonClickedDelegate not bound."));
	}
}

void UAudioSelectWidget::OnSongTitleValueCommitted(const FText& NewSongTitle, ETextCommit::Type CommitType)
{
	if (NewSongTitle.IsEmptyOrWhitespace())
	{
		SongTitle = "";
	}
	else
	{
		SongTitle = NewSongTitle.ToString();
	}
}

void UAudioSelectWidget::OnMinutesValueCommitted(const FText& NewMinutes, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewMinutes.ToString()), 0, 99);
	Minutes->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	SongLength = ClampedValue * 60 + FMath::Clamp(FCString::Atoi(*Seconds->GetText().ToString()), 0, 60);
}

void UAudioSelectWidget::OnSecondsValueCommitted(const FText& NewSeconds, ETextCommit::Type CommitType)
{
	const int32 ClampedValue = FMath::Clamp(FCString::Atoi(*NewSeconds.ToString()), 0, 60);
	Seconds->SetText(FText::AsNumber(ClampedValue, &NumberFormattingOptions));
	SongLength = FMath::Clamp(FCString::Atoi(*Minutes->GetText().ToString()), 0, 99) * 60 + ClampedValue;
}