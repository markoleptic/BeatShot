// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalStructs.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "SettingsMenu_Video.generated.h"

class UBSHorizontalBox;
class UCheckBox;
class UComboBoxString;
class UEditableTextBox;
class USlider;
class UVideoSettingButton;

DECLARE_MULTICAST_DELEGATE(FOnWindowOrResolutionChanged);

UCLASS()
class USERINTERFACE_API USettingsMenu_Video : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class USettingsMenuWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;

public:
	void InitializeVideoSettings(const FPlayerSettings_VideoAndSound& InVideoAndSoundSettings);
	FPlayerSettings_VideoAndSound GetVideoSettings() const;
	FOnWindowOrResolutionChanged OnWindowOrResolutionChanged;
	
	/** Stops the RevertVideoSettingsTimer and applies the video settings and closes the ConfirmVideoSettingsMessage */
	void OnConfirmVideoSettingsButtonClicked();
	
	/** Reverts the video settings and closes the ConfirmVideoSettingsMessage */
	void OnCancelVideoSettingsButtonClicked();

protected:
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

	/** Changes video settings quality depending on input button */
	UFUNCTION()
	void OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton);
	/** Changes video settings background color */
	UFUNCTION()
	void SetVideoSettingButtonBackgroundColor(UVideoSettingButton* ClickedButton);
	/** Returns the associated button given the quality and SettingType */
	UFUNCTION()
	UVideoSettingButton* FindVideoSettingButtonFromQuality(const int32 Quality, const ESettingType& SettingType) const;

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
	void OnReflexSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnDLSSSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnNISSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);

	UFUNCTION(BlueprintImplementableEvent)
	/** Hopefully a temporary solution. Calls the GetReflexAvailable function from ReflexBlueprintLibrary */
	bool GetReflexAvailable();
	
	/** Hopefully a temporary solution. Calls the SetReflexMode function from ReflexBlueprintLibrary */
	UFUNCTION(BlueprintImplementableEvent)
	void SetReflexMode(const EBudgetReflexMode Mode);
	
	/** Clears and repopulates the ComboBox_Resolution based on the resolutions from GetSupportedFullscreenResolutions or GetConvenientWindowedResolutions */
	void PopulateResolutionComboBox();
	
	UDLSSMode GetDLSSMode() const;
	UNISMode GetNISMode() const;
	EBudgetReflexMode GetReflexMode() const;

	/** Holds the last confirmed resolution, since RevertVideoMode does not actually revert the resolution */
	FIntPoint LastConfirmedResolution;
};
