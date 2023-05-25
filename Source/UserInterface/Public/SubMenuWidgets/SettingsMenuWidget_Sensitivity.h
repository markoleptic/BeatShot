// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget_Sensitivity.generated.h"

class UEditableTextBox;
class UTextBlock;
class USlider;
class UBSButton;
class USavedTextWidget;

/** Settings category widget holding Sensitivity settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_Sensitivity : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Returns OnPlayerSettingsChangedDelegate_User, the delegate that is broadcast when this class saves User settings */
	FOnPlayerSettingsChanged_User& GetPublicUserSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_User; }

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UTextBlock* Value_CurrentSensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* Value_NewSensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	UEditableTextBox* Value_NewSensitivityCsgo;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sensitivity")
	USlider* Slider_Sensitivity;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UBSButton* Button_Save;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;

	float Sensitivity;

	UFUNCTION()
	void OnValueChanged_NewSensitivity(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_NewSensitivityCsgo(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_Sensitivity(const float NewValue);
	/** Saves the Sensitivity Settings */
	UFUNCTION()
	void OnButtonClicked_Save();
	UFUNCTION()
	void OnButtonClicked_BSButton(const UBSButton* Button);
};
