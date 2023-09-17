// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/Buttons/VideoSettingButton.h"
#include "SettingsMenuWidget_VideoAndSound.generated.h"

class USliderTextBoxWidget;
class UCheckBoxOptionWidget;
class UCheckBox;
class UComboBoxString;
class USlider;
class UEditableTextBox;
class UBSHorizontalBox;
class UBSVerticalBox;
class UPopupMessageWidget;
class USavedTextWidget;
class UVerticalBox;
class UBSButton;

/** Settings category widget holding Video and Sound settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_VideoAndSound : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class USettingsMenuWidget;
	
	virtual void NativeConstruct() override;
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void InitSettingCategoryWidget() override;

	/** Populates the settings menu with InVideoAndSoundSettings */
	void InitializeVideoAndSoundSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);

	/** Returns the video and sound settings that are currently populated in the menu */
	FPlayerSettings_VideoAndSound GetVideoAndSoundSettings() const;
	
	/** Returns OnPlayerSettingsChangedDelegate_VideoAndSound, the delegate that is broadcast when this class saves Video and Sound settings */
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_VideoAndSound;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | PopUp")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundMix* GlobalSoundMix;
	
	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Save;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_Reset;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Video;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Sound;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_HDREnabled;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_HDRNits;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USliderTextBoxWidget* SliderTextBoxOption_Brightness;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_MusicSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_AntiAliasing;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_GlobalIllumination;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_PostProcessing;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Reflection;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Shadow;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Shading;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Texture;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ViewDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_VisualEffect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_FrameLimitMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_FrameLimitGame;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Resolution;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_WindowMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_FPSCounter;
	

	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_MusicSound;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_MusicSound;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_FrameLimitMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* Value_FrameLimitGame;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* ComboBox_Resolution;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxString* ComboBox_WindowMode;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox_FPSCounter;

	// NVIDIA
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_DLSS;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSComboBoxString* ComboBox_DLSS;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSHorizontalBox* BSBox_FrameGeneration;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			UBSComboBoxString* ComboBox_FrameGeneration;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSHorizontalBox* BSBox_SuperResolution;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			UBSComboBoxString* ComboBox_SuperResolution;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSHorizontalBox* BSBox_DLSS_Sharpness;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			USlider* Slider_DLSS_Sharpness;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			UEditableTextBox* Value_DLSS_Sharpness;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_NIS;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSComboBoxString* ComboBox_NIS;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSHorizontalBox* BSBox_NIS_Mode;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			UBSComboBoxString* ComboBox_NIS_Mode;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UBSHorizontalBox* BSBox_NIS_Sharpness;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			USlider* Slider_NIS_Sharpness;
			UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
			UEditableTextBox* Value_NIS_Sharpness;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_ResolutionScale;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		USlider* Slider_ResolutionScale;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* Value_ResolutionScale;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_VSync;
		UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UCheckBox* CheckBox_VSyncEnabled;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Reflex;
		UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UBSComboBoxString* ComboBox_Reflex;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_DLSS;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_FrameGeneration;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_SuperResolution;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_NIS;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTooltipImage* QMark_Reflex;

	#pragma region Quality
	
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

#pragma endregion

private:
	/** Adds the ConfirmVideoSettingsMessage to viewport, and starts the RevertVideoSettingsTimer */
	UFUNCTION()
	void ShowConfirmVideoSettingsMessage();

	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnButtonPressed_ConfirmVideoSettings();

	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnButtonPressed_CancelVideoSettings();

	/** Saves the Video and Sound Settings */
	UFUNCTION()
	void OnButtonPressed_Save();

	/** Reset Video and Sound Settings to defaults and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnButtonPressed_Reset();

	UFUNCTION()
	void OnBSButtonPressed_SaveReset(const UBSButton* Button);
	
	/** Changes video settings quality depending on input button */
	UFUNCTION()
	void OnBSButtonPressed_VideoQuality(const UBSButton* Button);

	UFUNCTION()
	void OnSliderChanged_GlobalSound(const float NewGlobalSound);
	UFUNCTION()
	void OnSliderChanged_MenuSound(const float NewMenuSound);
	UFUNCTION()
	void OnSliderChanged_MusicSound(const float NewMusicSound);
	UFUNCTION()
	void OnSliderChanged_DLSS_Sharpness(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_NIS_Sharpness(const float NewValue);
	UFUNCTION()
	void OnSliderChanged_ResolutionScale(const float NewValue);
	
	UFUNCTION()
	void OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_FrameLimitMenu(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_DLSS_Sharpness(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_NIS_Sharpness(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_ResolutionScale(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnSelectionChanged_WindowMode(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_Resolution(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_DLSS_EnabledMode(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_FrameGeneration(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_SuperResolution(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_NIS_EnabledMode(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_NIS_Mode(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_Reflex(const TArray<FString>& SelectedOptions, ESelectInfo::Type SelectionType);


	void OnSliderTextBoxValueChanged(USliderTextBoxWidget* Widget, const float Value);
	
	UFUNCTION()
	void OnCheckStateChanged_VSyncEnabled(const bool bIsChecked);
	UFUNCTION()
	void OnCheckStateChanged_HDREnabled(const bool bIsChecked);
	
	/** Function bound to RevertVideoSettingsTimer_UpdateSecond */
	UFUNCTION()
	void RevertVideoSettingsTimerCallback();
	
	/** Clears and repopulates the ComboBox_Resolution based on the resolutions from GetSupportedFullscreenResolutions or GetConvenientWindowedResolutions */
	void PopulateResolutionComboBox();
	
	/** Sets enabled/disabled states for any NVIDIA DLSS related settings */
	void HandleDLSSEnabledChanged(const EDLSSEnabledMode DLSSEnabledMode);

	/** Forces certain DLSS/NIS settings depending on if DLSS is enabled, and changes those selected options. Only called when a user changes the DLSS Enabled Mode selection */
	void HandleDLSSDependencies(const EDLSSEnabledMode DLSSEnabledMode);
	
	/** Returns the selected DLSS enabled mode */
	EDLSSEnabledMode GetSelectedDLSSEnabledMode() const;
	
	/** Returns the selected Frame Generation mode */
	UStreamlineDLSSGMode GetSelectedFrameGenerationMode() const;

	/** Returns the selected DLSS mode (Super Resolution) */
	UDLSSMode GetSelectedDLSSMode() const;

	/** Returns the selected NIS enabled mode */
	ENISEnabledMode GetSelectedNISEnabledMode() const;

	/** Returns the selected NIS mode */
	UNISMode GetSelectedNISMode() const;

	/** Returns the selected reflex mode */
	UStreamlineReflexMode GetSelectedReflexMode() const;
	
	/** Returns the associated button given the quality and SettingType */
	UVideoSettingButton* FindVideoSettingButton(const int32 Quality, const EVideoSettingType& SettingType) const;

	virtual UBSComboBoxEntry* ConstructComboBoxEntryWidget() override;
	
	/** Returns the String Table key for a specific ComboBox, not the cleanest code but it works */
	virtual FString GetStringTableKeyFromComboBox(const UBSComboBoxString* ComboBoxString, const FString& EnumString) override;
	
	/** Holds the last confirmed resolution */
	FIntPoint LastConfirmedResolution;

	/** Holds the last confirmed WindowMode */
	EWindowMode::Type LastConfirmedWindowMode;

	/** Timer that starts when window mode or resolution is changed. If it expires, it reverts those changes. Not bound to any function, but checked every second in RevertVideoSettingsTimerCallback */
	FTimerHandle RevertVideoSettingsTimer;
	
	/** Timer that starts when window mode or resolution is changed, and calls RevertVideoSettingsTimerCallback every second */
	FTimerHandle RevertVideoSettingsTimer_UpdateSecond;
};
