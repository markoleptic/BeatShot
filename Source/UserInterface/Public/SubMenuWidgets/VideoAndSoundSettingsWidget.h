// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/VideoSettingButton.h"
#include "VideoAndSoundSettingsWidget.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnSettingsSaved_VideoAndSound);

class UPopupMessageWidget;
class USavedTextWidget;
class UCheckBox;
class USlider;
class UEditableTextBox;
class UComboBoxString;
class UVerticalBox;
class UTextBlock;
class UButton;

UCLASS()
class USERINTERFACE_API UVideoAndSoundSettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	FOnSettingsSaved_VideoAndSound OnSettingsSaved_VideoAndSound;

#pragma region Video

#pragma region Quality

protected:
	/* Anti-Aliasing */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Anti-Aliasing Quality")
	UVideoSettingButton* AAQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Anti-Aliasing Quality")
	UVideoSettingButton* AAQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Anti-Aliasing Quality")
	UVideoSettingButton* AAQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Anti-Aliasing Quality")
	UVideoSettingButton* AAQ3;

	/* Global Illumination */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Global Illumination Quality")
	UVideoSettingButton* GIQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Global Illumination Quality")
	UVideoSettingButton* GIQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Global Illumination Quality")
	UVideoSettingButton* GIQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Global Illumination Quality")
	UVideoSettingButton* GIQ3;

	/* Post Processing */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Post Processing Quality")
	UVideoSettingButton* PPQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Post Processing Quality")
	UVideoSettingButton* PPQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Post Processing Quality")
	UVideoSettingButton* PPQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Post Processing Quality")
	UVideoSettingButton* PPQ3;

	/* Reflection */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Reflection Quality")
	UVideoSettingButton* RQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Reflection Quality")
	UVideoSettingButton* RQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Reflection Quality")
	UVideoSettingButton* RQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Reflection Quality")
	UVideoSettingButton* RQ3;

	/* Shadow */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shadow Quality")
	UVideoSettingButton* SWQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shadow Quality")
	UVideoSettingButton* SWQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shadow Quality")
	UVideoSettingButton* SWQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shadow Quality")
	UVideoSettingButton* SWQ3;

	/* Shading */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shading Quality")
	UVideoSettingButton* SGQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shading Quality")
	UVideoSettingButton* SGQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shading Quality")
	UVideoSettingButton* SGQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Shading Quality")
	UVideoSettingButton* SGQ3;

	/* Texture */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Texture Quality")
	UVideoSettingButton* TQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Texture Quality")
	UVideoSettingButton* TQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Texture Quality")
	UVideoSettingButton* TQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Texture Quality")
	UVideoSettingButton* TQ3;

	/* View Distance */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | View Distance Quality")
	UVideoSettingButton* VDQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | View Distance Quality")
	UVideoSettingButton* VDQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | View Distance Quality")
	UVideoSettingButton* VDQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | View Distance Quality")
	UVideoSettingButton* VDQ3;

	/* Visual Effect */

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Visual Effect Quality")
	UVideoSettingButton* VEQ0;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Visual Effect Quality")
	UVideoSettingButton* VEQ1;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Visual Effect Quality")
	UVideoSettingButton* VEQ2;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | Visual Effect Quality")
	UVideoSettingButton* VEQ3;

	/** Changes video settings quality depending on input button */
	UFUNCTION()
	void OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton);
	/** Changes video settings background color */
	UFUNCTION()
	void SetVideoSettingButtonBackgroundColor(UVideoSettingButton* ClickedButton);
	/** Returns the associated button given the quality and SettingType */
	UFUNCTION()
	UVideoSettingButton* FindVideoSettingButtonFromQuality(const int32 Quality, ESettingType SettingType);

#pragma endregion

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UEditableTextBox* FrameLimitMenuValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UEditableTextBox* FrameLimitGameValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UComboBoxString* ResolutionComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UComboBoxString* WindowModeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UCheckBox* VSyncEnabledCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UCheckBox* FPSCounterCheckBox;

	/** Holds the last confirmed resolution, since RevertVideoMode does not actually revert the resolution */
	FIntPoint LastConfirmedResolution;

	UFUNCTION()
	void OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnFrameLimitMenuValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnFrameLimitGameValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnVSyncEnabledCheckStateChanged(const bool bIsChecked);
	UFUNCTION()
	void OnFPSCounterCheckStateChanged(const bool bIsChecked);

	/** Clears and repopulates the ResolutionComboBox based on the resolutions from
	 *  GetSupportedFullscreenResolutions or GetConvenientWindowedResolutions */
	UFUNCTION()
	void PopulateResolutionComboBox();

#pragma	endregion

#pragma region Sound

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* GlobalSoundValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MenuSoundValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MusicSoundValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound ")
	USlider* GlobalSoundSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* MenuSoundSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* MusicSoundSlider;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sound")
	USoundMix* GlobalSoundMix;

	UFUNCTION()
	void OnGlobalSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnMenuSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnMusicSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnGlobalSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnMenuSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnMusicSoundValueChanged(const FText& NewValue, ETextCommit::Type CommitType);

#pragma endregion

#pragma region LoadingAndSaving

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveButton_VideoAndSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* ResetButton_VideoAndSound;

	/** The Player Settings that are changed during setting menu interaction */
	UPROPERTY()
	FPlayerSettings_VideoAndSound NewSettings;

	/** Fills all widgets with values from NewSettings */
	void PopulateSettings();

	/** Saves the Video and Sound Settings */
	UFUNCTION()
	void OnSaveButtonClicked_VideoAndSound();

	/** Reset Video and Sound Settings to defaults and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnResetButtonClicked_VideoAndSound();

#pragma endregion

	/** Timer that starts when window mode or resolution is changed. If it expires, it reverts those changes */
	UPROPERTY()
	FTimerHandle RevertVideoSettingsTimer;

	/** The color used to change the VideoSettingButton color to when selected */
	const FLinearColor BeatShotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);

	/** The color used to change the VideoSettingButton color to when not selected */
	const FLinearColor White = FLinearColor::White;

	/** Rounds the slider value to the snap size and sets the corresponding text box text to the rounded value */
	UFUNCTION()
	float ChangeValueOnSliderChange(const float SliderValue, UEditableTextBox* TextBoxToChange, const float SnapSize);

	/** Rounds the text value to the snap size and sets the corresponding slider value to the rounded value */
	UFUNCTION()
	float ChangeSliderOnValueChange(const FText& TextValue, USlider* SliderToChange, const float SnapSize);

	/** Adds the ConfirmVideoSettingsMessage to viewport, and starts the RevertVideoSettingsTimer */
	UFUNCTION()
	void ShowConfirmVideoSettingsMessage();

	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnConfirmVideoSettingsButtonClicked();

	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnCancelVideoSettingsButtonClicked();
};
