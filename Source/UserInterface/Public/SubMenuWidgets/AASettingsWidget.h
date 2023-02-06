// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/BandChannelWidget.h"
#include "WidgetComponents/BandThresholdWidget.h"
#include "AASettingsWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnRestartButtonClicked);

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

/**
 * 
 */

UCLASS()
class USERINTERFACE_API UAASettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;
	
	UPROPERTY()
	FOnRestartButtonClicked OnRestartButtonClicked;

	UPROPERTY()
	FOnAASettingsChange OnAASettingsChange;
	
	/** Do specific things if this instance of AASettings belongs to MainMenuWidget */
	void InitMainMenuChild();

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
	
	UPROPERTY()
	FAASettingsStruct AASettings;
	UPROPERTY()
	FAASettingsStruct NewAASettings;

	UFUNCTION()
	void OnChannelValueCommitted(const UBandChannelWidget* BandChannel, const int32 Index, const float NewValue, const bool bIsMinValue);
	UFUNCTION()
	void OnBandThresholdChanged(const UBandThresholdWidget* BandThreshold, const int32 Index, const float NewValue);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UComboBoxString* NumBandChannels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* TimeWindowSlider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* TimeWindowValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* ResetAASettingsButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* SaveAASettingsButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* SaveAndRestartButton;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	
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


