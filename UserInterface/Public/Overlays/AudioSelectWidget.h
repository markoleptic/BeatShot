// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSPlayerSettingsInterface.h"
#include "Blueprint/UserWidget.h"
#include "BSGameModeConfig/AudioConfig.h"
#include "Delegates/DelegateCombinations.h"
#include "PlayerSettings/PlayerSettings_User.h"
#include "Utilities/BSWidgetInterface.h"
#include "AudioSelectWidget.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogAudioSelect, Log, All);

class USongMetadataWidget;
class USaveGamePlayerScore;
class UAudioAnalyzerManager;
class UComboBoxString;
class UVerticalBox;
class UHorizontalBox;
class UPopupMessageWidget;
class UBSButton;
class UBSComboBoxString;
class UBorder;
class UTextBlock;
class UTooltipIcon;
class UCheckBox;
class UEditableTextBox;
class UWidgetAnimation;

/** Widget that prompts user to choose an audio input type and gather info about the audio configuration to be used for
 *  the game mode. */
UCLASS()
class USERINTERFACE_API UAudioSelectWidget : public UUserWidget,
                                             public IBSPlayerSettingsInterface,
                                             public IBSWidgetInterface
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void FadeIn();
	UFUNCTION()
	void FadeOut();

	TDelegate<void(const FBS_AudioConfig& AudioConfig)> OnStartButtonClickedDelegate;

	void SetSaveGamePlayerScore(USaveGamePlayerScore* InSaveGamePlayerScore);

	/** Broadcasts to parent so it can slide menu button back to starting position. */
	TDelegate<void()> OnExitAudioSelect;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio Select Widget | Classes")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;

	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;

	UPROPERTY(meta = (BindWidget))
	UTooltipIcon* QMark_PlaybackAudio;
	UPROPERTY(meta = (BindWidget))
	UTooltipIcon* QMark_Input;
	UPROPERTY(meta = (BindWidget))
	UTooltipIcon* QMark_AudioDeviceSong;

	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_AudioFromFile;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_CaptureAudio;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Back;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_Start;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_LoadFile;
	UPROPERTY(meta = (BindWidget))
	UBSButton* Button_AddSong;

	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Box_AudioFromFile;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Box_AudioDevice;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* Box_SongMetadata;
	UPROPERTY(meta = (BindWidget))
	USongMetadataWidget* SongMetadataWidget;

	UPROPERTY(meta = (BindWidget))
	UBSComboBoxString* ComboBox_InAudioDevices;
	UPROPERTY(meta = (BindWidget))
	UBSComboBoxString* ComboBox_AudioFromFileSongs;
	UPROPERTY(meta = (BindWidget))
	UBSComboBoxString* ComboBox_AudioDeviceSongs;
	UPROPERTY(meta = (BindWidget))
	UBSComboBoxString* ComboBox_SortingMethod;

	UPROPERTY(meta = (BindWidget))
	UCheckBox* Checkbox_PlaybackAudio;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;

private:
	UFUNCTION()
	void OnFadeOutFinish();

	void HandleLoadFileButtonClicked(const UBSButton*);

	void HandleAudioFromFileButtonClicked(const UBSButton*);

	void HandleCaptureAudioButtonClicked(const UBSButton*);

	void HandleAddSongButtonClicked(const UBSButton*);

	void HandleBackButtonClicked(const UBSButton*);

	void HandleStartButtonClicked(const UBSButton*);

	void HandleCancelButtonClicked();

	void HandleEditButtonClicked();

	void HandleRemoveButtonClicked();

	void HandleAddButtonClicked();

	void HandleCommitButtonClicked();

	UFUNCTION()
	void OnSelectionChanged_InAudioDevice(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_AudioFromFileSongs(const TArray<FString>& ActiveSelections,
	                                           ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_AudioDeviceSongs(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_SortingMethod(const TArray<FString>& ActiveSelections, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnCheckStateChanged_PlaybackAudio(bool bIsChecked);

	void UpdateSongInfoWidgets(EAudioFormat AudioFormat);

	/** Rebuilds the capture song options from the saved list, then selects the given entry. */
	void RebuildDeviceSongOptions(const FSongMetadata& SelectedSongMetadata);

	/** Rebuilds the file song options from the saved list, then selects the given path. */
	void RebuildFileSongOptions(const FString& SelectedSongFilePath);

	/** Displays an error message upon failed AudioAnalyzer initialization. */
	UFUNCTION()
	void ShowSongPathErrorMessage();

	/** Opens file dialog for song selection. */
	bool OpenFileDialog(TArray<FString>& OutFileNames);

	bool FileDialogShared(bool bSave,
	                      const void* ParentWindowHandle,
	                      const FString& DialogTitle,
	                      const FString& DefaultPath,
	                      const FString& DefaultFile,
	                      const FString& FileTypes,
	                      uint32 Flags,
	                      TArray<FString>& OutFilenames,
	                      int32& OutFilterIndex);

	FWidgetAnimationDynamicEvent FadeOutDelegate;

	/** Number formatting options for song length text boxes. */
	FNumberFormattingOptions NumberFormattingOptions;

	/** Contains information relating to the audio format the user has selected. Passed to GameModesWidget
	 *  using OnStartButtonClickedDelegate. */
	FBS_AudioConfig AudioFromFileAudioConfig;
	FBS_AudioConfig AudioDeviceAudioConfig;

	UPROPERTY()
	UTooltipWidget* ActiveTooltipWidget;

	UPROPERTY()
	TObjectPtr<UAudioAnalyzerManager> Manager;

	UPROPERTY()
	TObjectPtr<USaveGamePlayerScore> SaveGamePlayerScore;
};
