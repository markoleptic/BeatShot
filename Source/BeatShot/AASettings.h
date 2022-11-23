// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGameAASettings.h"
#include "Blueprint/UserWidget.h"
#include "AASettings.generated.h"

class USlider;
class UButton;
class UEditableTextBox;
class UHorizontalBox;
class UComboBoxString;
class UComboBox;
class UTextBlock;
class UProgressBar;
class GameModeActorBase;

/**
 * 
 */

UCLASS()
class BEATSHOT_API UAASettings : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeConstruct() override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AA Settings")
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
	UButton* SaveButtonOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UButton* SaveAndRestartButtonOne;

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
	UHorizontalBox* ThresholdBoxOne;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxTwo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxThree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UHorizontalBox* ThresholdBoxFour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	USlider* TimeWindowSlider;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "AA Settings")
	UEditableTextBox* TimeWindowValue;

	// Called anytime there is a change to the number of band channels selected
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void ShowBandChannelsAndThresholds();

	// Load AASettings from Save slot
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void LoadAASettings();

	// Update values in Settings Menu to match AASettings
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void PopulateAASettings();

	// Save AASettings to Save slot
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void SaveAASettings();

	// Reset AASettings to default value and repopulate in Settings Menu. Doesn't automatically save
	UFUNCTION(BlueprintCallable, Category = "AA Settings")
	void ResetAASettings();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	class UDefaultGameInstance* GI;
};
