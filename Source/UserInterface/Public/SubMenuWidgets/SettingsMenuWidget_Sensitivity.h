// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget_Sensitivity.generated.h"

class UEditableTextBox;
class UTextBlock;
class USlider;
class UButton;
class USavedTextWidget;

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
};
