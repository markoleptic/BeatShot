// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SensitivitySettingsWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSettingsSaved_Sensitivity);

class UEditableTextBox;
class UTextBlock;
class USlider;
class UButton;
class USavedTextWidget;

UCLASS()
class USERINTERFACE_API USensitivitySettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	FOnSettingsSaved_Sensitivity OnSettingsSaved_Sensitivity;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UTextBlock* CurrentSensitivityValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* NewSensitivityValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* NewSensitivityCsgoValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	USlider* SensSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveButton_Sensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;

	float Sensitivity;

	UFUNCTION()
	void OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSensitivitySliderChanged(const float NewValue);
	/** Saves the Sensitivity Settings */
	UFUNCTION()
	void OnSaveButtonClicked_Sensitivity();

	/** The value to divide the game sensitivity by to convert to Csgo sensitivity */
	const float CsgoMultiplier = 3.18;
};
