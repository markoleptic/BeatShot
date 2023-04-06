// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "TooltipInterface.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "AudioSelectWidget.generated.h"

/* Information about the transition state of the game */
USTRUCT()
struct FAudioSelectStruct
{
	GENERATED_BODY()

	EAudioFormat AudioFormat;

	float SongLength = 0;

	FString SongPath = "";

	FString SongTitle = "";

	FString InAudioDevice = "";

	FString OutAudioDevice = "";

	bool bPlaybackAudio;
};

DECLARE_DELEGATE_OneParam(FOnStartButtonClicked, const FAudioSelectStruct AudioSelectStruct);

class UComboBoxString;
class UVerticalBox;
class UHorizontalBox;
class UPopupMessageWidget;
class UButton;
class UBorder;
class UTextBlock;
class UTooltipImage;
class UTooltipWidget;
class UCheckBox;
class UEditableTextBox;
class UWidgetAnimation;

UCLASS()
class USERINTERFACE_API UAudioSelectWidget : public UUserWidget, public ISaveLoadInterface, public ITooltipInterface
{
	GENERATED_BODY()

protected:
	virtual UTooltipWidget* ConstructTooltipWidget() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();

	FOnStartButtonClicked OnStartButtonClickedDelegate;

protected:
	FWidgetAnimationDynamicEvent FadeOutDelegate;

	UFUNCTION()
	void OnFadeOutFinish();
	UFUNCTION()
	void OnAudioFromFileButtonClicked();
	UFUNCTION()
	void OnCaptureAudioButtonClicked();
	UFUNCTION()
	void OnStartButtonClicked();
	UFUNCTION()
	void OnLoadFileButtonClicked();
	UFUNCTION()
	void OnSongTitleValueCommitted(const FText& NewSongTitle, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnMinutesValueCommitted(const FText& NewMinutes, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSecondsValueCommitted(const FText& NewSeconds, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnInAudioDeviceSelectionChanged(const FString SelectedInAudioDevice, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnOutAudioDeviceSelectionChanged(const FString SelectedOutAudioDevice, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSongTitleSelectionChanged(const FString SelectedSongTitle, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnPlaybackAudioCheckStateChanged(const bool bIsChecked);

	void PopulateSongOptionComboBox();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* AudioFromFileButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* CaptureAudioButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* BackButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* StartButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* LoadFileButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* AudioDeviceBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* InAudioDevices;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* OutAudioDevices;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SongTitleLengthBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* SongLengthBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SongTitleBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* PlaybackAudioCheckbox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* SongTitleComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* SongTitleText;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Minutes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Seconds;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* PlaybackAudioQMark;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;

	/** Opens file dialog for song selection. The Implementation version only checks the fullscreen mode,
	 *  and changes it to Windowed Fullscreen if necessary */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OpenSongFileDialog(TArray<FString>& OutFileNames);

private:
	/** Displays an error message upon failed AudioAnalyzer initialization */
	UFUNCTION()
	void ShowSongPathErrorMessage();

	/** Hides the error message */
	UFUNCTION()
	void HideSongPathErrorMessage();

	/** Number formatting options for song length text boxes */
	FNumberFormattingOptions NumberFormattingOptions;

	/** The color used to change the GameModeButton color to when selected */
	const FLinearColor BeatShotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);

	/** The color used to change the GameModeButton color to when not selected */
	const FLinearColor White = FLinearColor::White;

	/** Contains information relating to the audio format the user has selected. Passed to GameModesWidget
	 *  using OnStartButtonClickedDelegate */
	FAudioSelectStruct AudioSelectStruct;

	/** Whether or not the user was in fullscreen mode before OpenFileDialog */
	bool bWasInFullScreenMode;
};
