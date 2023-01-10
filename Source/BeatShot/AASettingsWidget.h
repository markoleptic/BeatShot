// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameAASettings.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "AASettingsWidget.generated.h"

DECLARE_DYNAMIC_DELEGATE(FOnRestartButtonClicked);

class USlider;
class UButton;
class UEditableTextBox;
class UHorizontalBox;
class UComboBoxString;
class UComboBox;
class UTextBlock;
class USavedTextWidget;
class UProgressBar;

/**
 * 
 */

UCLASS()
class BEATSHOT_API UAASettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	FOnRestartButtonClicked OnRestartButtonClicked;
	
	/** Do specific things if this instance of AASettings belongs to MainMenuWidget */
	void InitMainMenuChild();

protected:

	virtual void NativeConstruct() override;
	
	UPROPERTY()
	FAASettingsStruct AASettings;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UComboBoxString* NumBandChannels;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* BandChannelOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* BandChannelTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* BandChannelThree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* BandChannelFour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxThree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxFour;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelOneMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelOneMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelTwoMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelTwoMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelThreeMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelThreeMax;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelFourMin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* BandChannelFourMax;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* ThresholdSliderOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* ThresholdSliderTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* ThresholdSliderThree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* ThresholdSliderFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* ThresholdValueOne;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* ThresholdValueTwo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* ThresholdValueThree;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* ThresholdValueFour;

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
	
	UFUNCTION()
	void OnSaveAndRestartButtonClicked();

	UFUNCTION()
	void OnThresholdSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float SnapSize);
	UFUNCTION()
	void OnThresholdValueChanged(const FText& NewText, USlider* SliderToChange, const float SnapSize);
	
	UFUNCTION()
	void OnThresholdSliderOneChanged(const float NewValue) { OnThresholdSliderChanged(NewValue, ThresholdValueOne, Hundredths); }
	UFUNCTION()
	void OnThresholdSliderTwoChanged(const float NewValue) { OnThresholdSliderChanged(NewValue, ThresholdValueTwo, Hundredths); }
	UFUNCTION()
	void OnThresholdSliderThreeChanged(const float NewValue) { OnThresholdSliderChanged(NewValue, ThresholdValueThree, Hundredths); }
	UFUNCTION()
	void OnThresholdSliderFourChanged(const float NewValue) { OnThresholdSliderChanged(NewValue, ThresholdValueFour, Hundredths); }
	UFUNCTION()
	void OnThresholdValueOneChanged(const FText& NewText) { OnThresholdValueChanged(NewText, ThresholdSliderOne, Hundredths); }
	UFUNCTION()
	void OnThresholdValueTwoChanged(const FText& NewText) { OnThresholdValueChanged(NewText, ThresholdSliderTwo, Hundredths); }
	UFUNCTION()
	void OnThresholdValueThreeChanged(const FText& NewText) { OnThresholdValueChanged(NewText, ThresholdSliderThree, Hundredths); }
	UFUNCTION()
	void OnThresholdValueFourChanged(const FText& NewText) { OnThresholdValueChanged(NewText, ThresholdSliderFour, Hundredths); }
	
	/* Called anytime there is a change to the number of band channels selected */
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void ShowBandChannelsAndThresholds(const FString SelectedOption, ESelectInfo::Type SelectionType);
	/* Load AASettings from Save slot */
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void LoadAASettings();
	/* Update values in Settings Menu to match AASettings */
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void PopulateAASettings();
	/* Save AASettings to Save slot */
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void SaveAASettings();
	/* Reset AASettings to default value and repopulate in Settings Menu. Doesn't automatically save */
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void ResetAASettings();

	const float Hundredths = 0.01f;
};
