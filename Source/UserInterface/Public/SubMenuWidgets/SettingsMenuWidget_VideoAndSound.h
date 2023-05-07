// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "WidgetComponents/VideoSettingButton.h"
#include "SettingsMenuWidget_VideoAndSound.generated.h"

class UCheckBox;
class UComboBoxString;
class USlider;
class UEditableTextBox;
class UBSHorizontalBox;
class UBSVerticalBox;
class UPopupMessageWidget;
class USavedTextWidget;
class UVerticalBox;
class UButton;


/** Settings category widget holding Video and Sound settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_VideoAndSound : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class USettingsMenuWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;
	
	void InitializeVideoAndSoundSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);
	FPlayerSettings_VideoAndSound GetVideoAndSoundSettings() const;
	
	/** Returns OnPlayerSettingsChangedDelegate_VideoAndSound, the delegate that is broadcast when this class saves Video and Sound settings */
	FOnPlayerSettingsChanged_VideoAndSound& GetPublicVideoAndSoundSettingsChangedDelegate() {return OnPlayerSettingsChangedDelegate_VideoAndSound;}

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | PopUp")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;

	UPROPERTY()
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* Button_Save;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* Button_Reset;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Video;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* BSBox_Sound;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UBSHorizontalBox* BSBox_MusicSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_AntiAliasing;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_GlobalIllumination;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_PostProcessing;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Reflection;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Shadow;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Shading;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Texture;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_ViewDistance;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_VisualEffect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_FrameLimitMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_FrameLimitGame;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Resolution;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_WindowMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_VSync;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_FPSCounter;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_Reflex;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_DLSS;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UBSHorizontalBox* BSBox_NIS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* GlobalSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundClass* MenuSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Classes | Sound")
	USoundMix* GlobalSoundMix;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* Value_MusicSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound ")
	USlider* Slider_GlobalSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* Slider_MenuSound;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* Slider_MusicSound;

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

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UEditableTextBox* Value_FrameLimitMenu;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UEditableTextBox* Value_FrameLimitGame;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UComboBoxString* ComboBox_Resolution;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UComboBoxString* ComboBox_WindowMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UCheckBox* CheckBox_VSyncEnabled;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video")
	UCheckBox* CheckBox_FPSCounter;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "Video | NVIDIA")
	UComboBoxString* ComboBox_Reflex;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | NVIDIA")
	UComboBoxString* ComboBox_DLSS;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Video | NVIDIA")
	UComboBoxString* ComboBox_NIS;

private:
	
	/** Adds the ConfirmVideoSettingsMessage to viewport, and starts the RevertVideoSettingsTimer */
	UFUNCTION()
	void ShowConfirmVideoSettingsMessage();

	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnButtonClicked_ConfirmVideoSettings();

	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	UFUNCTION()
	void OnButtonClicked_CancelVideoSettings();

	/** Saves the Video and Sound Settings */
	UFUNCTION()
	void OnButtonClicked_Save();

	/** Reset Video and Sound Settings to defaults and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnButtonClicked_Reset();

	UFUNCTION()
	void OnSliderChanged_GlobalSound(const float NewGlobalSound);
	UFUNCTION()
	void OnSliderChanged_MenuSound(const float NewMenuSound);
	UFUNCTION()
	void OnSliderChanged_MusicSound(const float NewMusicSound);
	UFUNCTION()
	void OnValueChanged_GlobalSound(const FText& NewGlobalSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MenuSound(const FText& NewMenuSound, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_MusicSound(const FText& NewMusicSound, ETextCommit::Type CommitType);

	/** Changes video settings quality depending on input button */
	UFUNCTION()
	void OnButtonClicked_VideoQuality(const UVideoSettingButton* ClickedButton);
	
	/** Returns the associated button given the quality and SettingType */
	UFUNCTION()
	UVideoSettingButton* FindVideoSettingButton(const int32 Quality, const EVideoSettingType& SettingType) const;
	UFUNCTION()
	void OnSelectionChanged_WindowMode(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_Resolution(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnValueChanged_FrameLimitMenu(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnValueChanged_FrameLimitGame(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_VSyncEnabled(const bool bIsChecked);
	UFUNCTION()
	void OnCheckStateChanged_FPSCounter(const bool bIsChecked);
	UFUNCTION()
	void OnSelectionChanged_Reflex(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_DLSS(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_NIS(const FString SelectedOption, ESelectInfo::Type SelectionType);

	/** Function bound to RevertVideoSettingsTimer_UpdateSecond */
	UFUNCTION()
	void RevertVideoSettingsTimerCallback();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	/** Hopefully a temporary solution. Calls the GetReflexAvailable function from ReflexBlueprintLibrary */
	bool GetReflexAvailable();
	
	/** Hopefully a temporary solution. Calls the SetReflexMode function from ReflexBlueprintLibrary */
	UFUNCTION(BlueprintImplementableEvent)
	void SetReflexMode(const EBudgetReflexMode Mode);

private:
	/** Clears and repopulates the ComboBox_Resolution based on the resolutions from GetSupportedFullscreenResolutions or GetConvenientWindowedResolutions */
	void PopulateResolutionComboBox();
	
	UDLSSMode GetDLSSMode() const;
	UNISMode GetNISMode() const;
	EBudgetReflexMode GetReflexMode() const;

	/** Holds the last confirmed resolution, since RevertVideoMode does not actually revert the resolution */
	FIntPoint LastConfirmedResolution;

	/** Timer that starts when window mode or resolution is changed. If it expires, it reverts those changes. Not bound to any function, but checked every second in RevertVideoSettingsTimerCallback */
	FTimerHandle RevertVideoSettingsTimer;
	
	/** Timer that starts when window mode or resolution is changed, and calls RevertVideoSettingsTimerCallback every second */
	FTimerHandle RevertVideoSettingsTimer_UpdateSecond;
};
