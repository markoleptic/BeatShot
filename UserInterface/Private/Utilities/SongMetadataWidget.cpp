// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.


#include "Utilities/SongMetadataWidget.h"
#include "BSGameModeConfig/AudioConfig.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Utilities/Buttons/BSButton.h"

namespace
{
FString StripNonNumeric(const FString& InText)
{
	FString Digits;
	for (const auto& Char : InText)
	{
		if (FChar::IsDigit(Char))
		{
			Digits.AppendChar(Char);
		}
	}
	return Digits;
}
}

void USongMetadataWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Value_Minutes->OnTextChanged.AddUniqueDynamic(this, &USongMetadataWidget::OnTextChanged_Minutes);
	Value_Seconds->OnTextChanged.AddUniqueDynamic(this, &USongMetadataWidget::OnTextChanged_Seconds);
	Value_Minutes->OnTextCommitted.AddUniqueDynamic(this, &USongMetadataWidget::OnTextCommitted_Minutes);
	Value_Seconds->OnTextCommitted.AddUniqueDynamic(this, &USongMetadataWidget::OnTextCommitted_Seconds);
	Value_Title->OnTextChanged.AddUniqueDynamic(this, &USongMetadataWidget::OnTextChanged_Title);
	FormattingOptions.SetMinimumIntegralDigits(2).SetMaximumIntegralDigits(2);

	ErrorText->SetVisibility(ESlateVisibility::Hidden);

	Button_Cancel->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleCancelButtonClicked);
	Button_Remove->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleRemoveButtonClicked);
	Button_Edit->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleEditButtonClicked);
	Button_Add->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleAddButtonClicked);
	Button_Commit->OnBSButtonPressed.AddUObject(this, &ThisClass::HandleCommitButtonClicked);
}

void USongMetadataWidget::Populate(const FSongMetadata& InSongMetadata,
                                   const EAudioFormat AudioFormat,
                                   const EMode InMode)
{
	IsPopulating = true;
	ON_SCOPE_EXIT { IsPopulating = false; };

	Value_Title->SetText(FText::FromString(InSongMetadata.Title));
	Value_Artist->SetText(FText::FromString(InSongMetadata.Artist));
	Value_Album->SetText(FText::FromString(InSongMetadata.Album));
	Value_Year->SetText(FText::FromString(InSongMetadata.Year));
	Value_Genre->SetText(FText::FromString(InSongMetadata.Genre));

	const int32 TotalSeconds = FMath::FloorToInt(InSongMetadata.Length);
	Value_Minutes->SetText(FText::AsNumber(TotalSeconds / 60, &FormattingOptions));
	Value_Seconds->SetText(FText::AsNumber(TotalSeconds % 60, &FormattingOptions));

	AllowMinutesSecondsToBeChangedOnEdit = AudioFormat == EAudioFormat::Capture;
	SetErrorMessage({});

	switch (InMode)
	{
	case EMode::Add:
		Button_Cancel->SetVisibility(ESlateVisibility::Visible);
		Button_Remove->SetVisibility(ESlateVisibility::Collapsed);
		Button_Edit->SetVisibility(ESlateVisibility::Collapsed);
		Button_Add->SetVisibility(ESlateVisibility::Visible);
		Button_Commit->SetVisibility(ESlateVisibility::Collapsed);

		Button_Add->SetIsEnabled(!Value_Title->GetText().IsEmptyOrWhitespace());

		Value_Title->SetIsReadOnly(false);
		Value_Artist->SetIsReadOnly(false);
		Value_Album->SetIsReadOnly(false);
		Value_Year->SetIsReadOnly(false);
		Value_Genre->SetIsReadOnly(false);

		Value_Minutes->SetIsReadOnly(false);
		Value_Seconds->SetIsReadOnly(false);
		break;
	case EMode::Edit:
		Button_Cancel->SetVisibility(ESlateVisibility::Collapsed);
		Button_Remove->SetVisibility(ESlateVisibility::Visible);
		Button_Edit->SetVisibility(ESlateVisibility::Visible);
		Button_Add->SetVisibility(ESlateVisibility::Collapsed);
		Button_Commit->SetVisibility(ESlateVisibility::Collapsed);

		Value_Title->SetIsReadOnly(true);
		Value_Artist->SetIsReadOnly(true);
		Value_Album->SetIsReadOnly(true);
		Value_Year->SetIsReadOnly(true);
		Value_Genre->SetIsReadOnly(true);

		Value_Minutes->SetIsReadOnly(true);
		Value_Seconds->SetIsReadOnly(true);
		break;
	}
}

void USongMetadataWidget::SetErrorMessage(const FText& ErrorMessage)
{
	ErrorText->SetVisibility(ErrorMessage.IsEmpty() ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	ErrorText->SetText(ErrorMessage);
}

FSongMetadata USongMetadataWidget::GetSongMetadata() const
{
	return FSongMetadata{
		.Title = Value_Title->GetText().ToString().TrimStartAndEnd(),
		.Artist = Value_Artist->GetText().ToString().TrimStartAndEnd(),
		.Album = Value_Album->GetText().ToString().TrimStartAndEnd(),
		.Year = Value_Year->GetText().ToString().TrimStartAndEnd(),
		.Genre = Value_Genre->GetText().ToString().TrimStartAndEnd(),
		.Length = static_cast<float>(FCString::Atoi(*Value_Minutes->GetText().ToString()) * 60 + FCString::Atoi(
			                             *Value_Seconds->GetText().ToString()))
	};
}

void USongMetadataWidget::OnTextChanged_Minutes(const FText& NewMinutes)
{
	if (!IsPopulating)
	{
		const FString DigitsOnly = StripNonNumeric(NewMinutes.ToString());
		if (DigitsOnly != NewMinutes.ToString())
		{
			Value_Minutes->SetText(FText::FromString(DigitsOnly));
		}
	}
}

void USongMetadataWidget::OnTextCommitted_Minutes(const FText& NewMinutes, ETextCommit::Type /*CommitType*/)
{
	const int32 ClampedMinutes = FMath::Clamp(FCString::Atoi(*NewMinutes.ToString()), 0, 99);
	Value_Minutes->SetText(FText::AsNumber(ClampedMinutes, &FormattingOptions));
}

void USongMetadataWidget::OnTextChanged_Seconds(const FText& NewSeconds)
{
	if (!IsPopulating)
	{
		const FString DigitsOnly = StripNonNumeric(NewSeconds.ToString());
		if (DigitsOnly != NewSeconds.ToString())
		{
			Value_Seconds->SetText(FText::FromString(DigitsOnly));
		}
	}
}

void USongMetadataWidget::OnTextCommitted_Seconds(const FText& NewSeconds, ETextCommit::Type /*CommitType*/)
{
	const int32 ClampedSeconds = FMath::Clamp(FCString::Atoi(*NewSeconds.ToString()), 0, 60);
	Value_Seconds->SetText(FText::AsNumber(ClampedSeconds, &FormattingOptions));
}

void USongMetadataWidget::OnTextChanged_Title(const FText& NewTitle)
{
	if (!IsPopulating)
	{
		if (Button_Add->IsVisible())
		{
			Button_Add->SetIsEnabled(!NewTitle.IsEmptyOrWhitespace());
		}
		else if (Button_Commit->IsVisible())
		{
			Button_Commit->SetIsEnabled(!NewTitle.IsEmptyOrWhitespace());
		}
	}
}

void USongMetadataWidget::HandleCancelButtonClicked(const UBSButton*)
{
	OnCancelButtonClicked.Execute();
}

void USongMetadataWidget::HandleRemoveButtonClicked(const UBSButton*)
{
	OnRemoveButtonClicked.Execute();
}

void USongMetadataWidget::HandleEditButtonClicked(const UBSButton*)
{
	Value_Title->SetIsReadOnly(false);
	Value_Artist->SetIsReadOnly(false);
	Value_Album->SetIsReadOnly(false);
	Value_Year->SetIsReadOnly(false);
	Value_Genre->SetIsReadOnly(false);

	if (AllowMinutesSecondsToBeChangedOnEdit)
	{
		Value_Minutes->SetIsReadOnly(false);
		Value_Seconds->SetIsReadOnly(false);
	}

	Button_Remove->SetVisibility(ESlateVisibility::Collapsed);
	Button_Cancel->SetVisibility(ESlateVisibility::Visible);
	Button_Edit->SetVisibility(ESlateVisibility::Collapsed);
	Button_Commit->SetVisibility(ESlateVisibility::Visible);
	Button_Commit->SetIsEnabled(!Value_Title->GetText().IsEmptyOrWhitespace());

	OnEditButtonClicked.Execute();
}

void USongMetadataWidget::HandleAddButtonClicked(const UBSButton*)
{
	OnAddButtonClicked.Execute();
}

void USongMetadataWidget::HandleCommitButtonClicked(const UBSButton*)
{
	OnCommitButtonClicked.Execute();
}
