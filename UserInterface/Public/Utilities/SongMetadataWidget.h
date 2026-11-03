// Copyright 2022-2026 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Utilities/SongMetadata.h"
#include "SongMetadataWidget.generated.h"

class UTooltipIcon;
class UHorizontalBox;
class UTextBlock;
enum class EAudioFormat : uint8;
class UBSButton;
class UEditableTextBox;


UCLASS()
class USERINTERFACE_API USongMetadataWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	enum class EMode
	{
		Add,
		Edit
	};

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Title;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Minutes;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Seconds;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Artist;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Album;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Year;
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* Value_Genre;

	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Cancel;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Remove;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Edit;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Add;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Commit;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ErrorText;

	UPROPERTY(meta = (BindWidget))
	UTooltipIcon* QMark_SongLength;

	/** Shows the full file path for file-mode selections; hidden otherwise. */
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* Box_SongFilePath;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_SongFilePath;

	void Populate(const FSongMetadata& InSongMetadata,
	              EAudioFormat AudioFormat,
	              EMode InMode,
	              const FString& SongFilePath = FString());

	void SetErrorMessage(const FText& ErrorMessage);

	FSongMetadata GetSongMetadata() const;

	TDelegate<void()> OnCancelButtonClicked;
	TDelegate<void()> OnRemoveButtonClicked;
	TDelegate<void()> OnEditButtonClicked;
	TDelegate<void()> OnAddButtonClicked;
	TDelegate<void()> OnCommitButtonClicked;

protected:
	UFUNCTION()
	void OnTextChanged_Minutes(const FText& NewMinutes);
	UFUNCTION()
	void OnTextCommitted_Minutes(const FText& NewMinutes, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextChanged_Seconds(const FText& NewSeconds);
	UFUNCTION()
	void OnTextCommitted_Seconds(const FText& NewSeconds, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextChanged_Title(const FText& NewTitle);

	void HandleCancelButtonClicked(const UBSButton*);

	void HandleRemoveButtonClicked(const UBSButton*);

	void HandleEditButtonClicked(const UBSButton*);

	void HandleAddButtonClicked(const UBSButton*);

	void HandleCommitButtonClicked(const UBSButton*);

	bool AllowMinutesSecondsToBeChangedOnEdit = false;

	bool IsPopulating = false;

	FNumberFormattingOptions FormattingOptions;
};
