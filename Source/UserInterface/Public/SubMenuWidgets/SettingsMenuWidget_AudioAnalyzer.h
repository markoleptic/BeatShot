// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/BandChannelWidget.h"
#include "WidgetComponents/BandThresholdWidget.h"
#include "SettingsMenuWidget_AudioAnalyzer.generated.h"

DECLARE_DELEGATE(FOnRestartButtonClicked);

class USlider;
class UButton;
class UEditableTextBox;
class UVerticalBox;
class UHorizontalBox;
class UComboBoxString;
class UComboBox;
class UTextBlock;
class USavedTextWidget;
class UProgressBar;
class UPopupMessageWidget;

/** Settings category widget holding AudioAnalyzer settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_AudioAnalyzer : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	/** Broadcast when the restart and save button is clicked to apply AudioAnalyzer settings that require a restart */
	FOnRestartButtonClicked OnRestartButtonClicked;

	/** Do specific things if this instance of AASettings belongs to MainMenuWidget */
	void InitMainMenuChild();

	/** Returns OnPlayerSettingsChangedDelegate_AudioAnalyzer, the delegate that is broadcast when this class saves Audio Analyzer settings */
	FOnPlayerSettingsChanged_AudioAnalyzer& GetPublicAudioAnalyzerSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_AudioAnalyzer; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AudioAnalyzer | Classes")
	TSubclassOf<UBandChannelWidget> BandChannelWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "AudioAnalyzer | Classes")
	TSubclassOf<UBandThresholdWidget> BandThresholdWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "AudioAnalyzer | Classes")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;

	UPROPERTY()
	UBandChannelWidget* BandChannelWidget;
	UPROPERTY()
	UBandThresholdWidget* BandThresholdWidget;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UVerticalBox* Box_BandChannelBounds;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UVerticalBox* Box_BandThresholdBounds;
	
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UComboBoxString* ComboBox_NumBandChannels;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	USlider* Slider_TimeWindow;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UEditableTextBox* Value_TimeWindow;

	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button_Reset;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button_Save;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UButton* Button_SaveAndRestart;
	
	UPROPERTY()
	FPlayerSettings_AudioAnalyzer AASettings;
	UPROPERTY()
	FPlayerSettings_AudioAnalyzer NewAASettings;

	UFUNCTION()
	void OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue);
	UFUNCTION()
	void OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue);
	UFUNCTION()
	void OnSelectionChanged_NumBandChannels(FString NewNum, ESelectInfo::Type SelectType);
	UFUNCTION()
	void OnButtonClicked_Reset();
	UFUNCTION()
	void OnButtonClicked_Save();
	UFUNCTION()
	void OnButtonClicked_SaveAndRestart();
	
	/** Update values in Settings Menu to match AASettings */
	UFUNCTION()
	void PopulateAASettings();

	/** Reset AASettings to default value and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void ResetAASettings();

	/** Sorts NewAASettings and checks for overlapping Band Channels. Calls ShowBandLimitErrorMessage() if it finds an overlap, otherwise saves */
	void SortAndCheckOverlap();

	/** Adds a popup message to the viewport displaying the error */
	void ShowBandLimitErrorMessage();

	/** Hides the error message */
	UFUNCTION()
	void HideSongPathErrorMessage();
};
