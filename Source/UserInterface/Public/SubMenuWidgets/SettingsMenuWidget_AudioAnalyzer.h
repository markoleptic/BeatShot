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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UVerticalBox* BandChannelBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UVerticalBox* BandThresholdBounds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	TSubclassOf<UBandChannelWidget> BandChannelWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	TSubclassOf<UBandThresholdWidget> BandThresholdWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	UBandChannelWidget* BandChannelWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
	UBandThresholdWidget* BandThresholdWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UComboBoxString* NumBandChannels;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* TimeWindowSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* TimeWindowValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* ResetButton_AASettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* SaveButton_AASettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* SaveAndRestartButton;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;

	UPROPERTY()
	FPlayerSettings_AudioAnalyzer AASettings;
	UPROPERTY()
	FPlayerSettings_AudioAnalyzer NewAASettings;

	UFUNCTION()
	void OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue);
	UFUNCTION()
	void OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue);
	UFUNCTION()
	void OnNumBandChannelsSelectionChanged(FString NewNum, ESelectInfo::Type SelectType);

	/** Update values in Settings Menu to match AASettings */
	UFUNCTION()
	void PopulateAASettings();

	/** Save AASettings to Save slot */
	UFUNCTION()
	void SaveAASettingsToSlot();

	/** Reset AASettings to default value and repopulate in Settings Menu. Doesn't automatically save */
	UFUNCTION()
	void ResetAASettings();

	/** Sorts NewAASettings and checks for overlapping Band Channels. Calls ShowBandLimitErrorMessage() if it finds
	 *  an overlap */
	UFUNCTION()
	void SortAndCheckOverlap();

	/** Adds a popup message to the viewport displaying the error */
	void ShowBandLimitErrorMessage();

	/** Hides the error message */
	UFUNCTION()
	void HideSongPathErrorMessage();

	const float Hundredths = 0.01f;
};
