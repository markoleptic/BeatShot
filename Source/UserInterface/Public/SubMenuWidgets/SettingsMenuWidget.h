// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "WidgetComponents/VideoSettingButton.h"
#include "SettingsMenuWidget.generated.h"

class UPopupMessageWidget;
class USavedTextWidget;
class UCheckBox;
class UAASettingsWidget;
class USlider;
class UEditableTextBox;
class UComboBoxString;
class UVerticalBox;
class UTextBlock;
class UButton;
class UWidgetSwitcher;
class UCrossHairSettingsWidget;

UCLASS()
class USERINTERFACE_API USettingsMenuWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

public:
	/** Whether or not this instance of SettingsMenuWidget belongs to MainMenuWidget or not */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	UPROPERTY()
	FOnPlayerSettingsChange OnPlayerSettingsChange;

protected:
	virtual void NativeConstruct() override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;

	/** Containers for each widget in SettingsMenu */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoAndSoundSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* AASettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Sensitivity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* CrossHair;

	/** SubMenu Widgets */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoAndSoundSettingsWidget;

public:
	
	/** So that parent widgets can bind to OnRestartButtonClicked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UAASettingsWidget* AASettingsWidget;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SensitivityWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UCrossHairSettingsWidget* CrossHairWidget;

	/** Settings Navigation Buttons */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* VideoAndSoundSettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* AASettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SensitivityButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* CrossHairButton;

	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnVideoAndSoundSettingsButtonClicked() { SlideButtons(VideoAndSoundSettingsButton); }
	UFUNCTION()
	void OnAASettingsButtonClicked() { SlideButtons(AASettingsButton); }
	UFUNCTION()
	void OnSensitivityButtonClicked() { SlideButtons(SensitivityButton); }
	UFUNCTION()
	void OnCrossHairButtonClicked() { SlideButtons(CrossHairButton); }

#pragma	endregion

#pragma region Video

#pragma region Quality
	/** Anti-Aliasing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AAQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AAQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AAQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AAQ3;
	/** Global Illumination */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Global Illumination Quality")
	UVideoSettingButton* GIQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Global Illumination Quality")
	UVideoSettingButton* GIQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Global Illumination Quality")
	UVideoSettingButton* GIQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Global Illumination Quality")
	UVideoSettingButton* GIQ3;
	/** Post Processing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PPQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PPQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PPQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PPQ3;
	/** Reflection */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Reflection Quality")
	UVideoSettingButton* RQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Reflection Quality")
	UVideoSettingButton* RQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Reflection Quality")
	UVideoSettingButton* RQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Reflection Quality")
	UVideoSettingButton* RQ3;
	/** Shadow */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SWQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SWQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SWQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SWQ3;
	/** Shading */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shading Quality")
	UVideoSettingButton* SGQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shading Quality")
	UVideoSettingButton* SGQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shading Quality")
	UVideoSettingButton* SGQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shading Quality")
	UVideoSettingButton* SGQ3;
	/** Texture */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Texture Quality")
	UVideoSettingButton* TQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Texture Quality")
	UVideoSettingButton* TQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Texture Quality")
	UVideoSettingButton* TQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Texture Quality")
	UVideoSettingButton* TQ3;
	/** View Distance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VDQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VDQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VDQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VDQ3;
	/** Visual Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
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

	/** FRAME LIMIT */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Frame Limit")
	UEditableTextBox* FrameLimitMenuValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Frame Limit")
	UEditableTextBox* FrameLimitGameValue;
	/** COMBAT TEXT */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Streak")
	UEditableTextBox* CombatTextFrequency;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Streak")
	UCheckBox* ShowStreakCombatTextCheckBox;
	/** RESOLUTION */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Resolution")
	UComboBoxString* ResolutionComboBox;
	/** WINDOW MODE */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Window Mode")
	UComboBoxString* WindowModeComboBox;
	/** VSYNC */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "VSync")
	UCheckBox* VSyncEnabledCheckBox;
	/** FPSCOUNTER */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "FPS Counter")
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

	UFUNCTION()
	void OnCombatTextFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnShowCombatTextCheckStateChanged(const bool bIsChecked);

	/** Clears and repopulates the ResolutionComboBox based on the resolutions from
	 *  GetSupportedFullscreenResolutions or GetConvenientWindowedResolutions */
	UFUNCTION()
	void PopulateResolutionComboBox();

#pragma	endregion

#pragma region Sound

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* GlobalSoundValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MenuSoundValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MusicSoundValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound ")
	USlider* GlobalSoundSlider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* MenuSoundSlider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
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

#pragma region Sensitivity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UTextBlock* CurrentSensitivityValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UEditableTextBox* NewSensitivityValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UEditableTextBox* NewSensitivityCsgoValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	USlider* SensSlider;

	UFUNCTION()
	void OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSensitivitySliderChanged(const float NewValue);

#pragma	endregion

#pragma region LoadingAndSaving
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidgetSens;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveVideoAndSoundSettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveSensitivityButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* ResetVideoAndSoundButton;
	/** The Player Settings loaded upon initialization */
	FPlayerSettings InitialPlayerSettings;
	/** The Player Settings that are changed during setting menu interaction */
	FPlayerSettings NewPlayerSettings;

	/** Saves the Video and Sound Settings using Game Instance */
	UFUNCTION()
	void OnSaveVideoAndSoundSettingsButtonClicked();
	/** Saves the Sensitivity Settings using Game Instance */
	UFUNCTION()
	void OnSaveSensitivitySettingsButtonClicked();
	/** Reset AASettings to default value and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnResetVideoAndSoundButtonClicked();

#pragma endregion

#pragma region Utility
	
	/** Timer that starts when window mode or resolution is changed. If it expires, it reverts those changes */
	UPROPERTY()
	FTimerHandle RevertVideoSettingsTimer;
	/** Adds the ConfirmVideoSettingsMessage to viewport, and starts the RevertVideoSettingsTimer */
	UFUNCTION()
	void ShowConfirmVideoSettingsMessage();
	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnConfirmVideoSettingsButtonClicked();
	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnCancelVideoSettingsButtonClicked();
	/** Rounds the slider value to the snap size and sets the corresponding text box text to the rounded value */
	UFUNCTION()
	float ChangeValueOnSliderChange(const float SliderValue, UEditableTextBox* TextBoxToChange, const float SnapSize);
	/** Rounds the text value to the snap size and sets the corresponding slider value to the rounded value */
	UFUNCTION()
	float ChangeSliderOnValueChange(const FText& TextValue, USlider* SliderToChange, const float SnapSize);
	/** Loads the save file containing the player settings, populates all settings */
	UFUNCTION()
	void InitializeSettings();
	/** The value to divide the game sensitivity by to convert to Csgo sensitivity */
	const float CsgoMultiplier = 3.18;
	/** The color used to change the VideoSettingButton color to when selected */
	const FLinearColor BeatshotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	/** The color used to change the VideoSettingButton color to when not selected */
	const FLinearColor White = FLinearColor::White;

#pragma endregion
};
