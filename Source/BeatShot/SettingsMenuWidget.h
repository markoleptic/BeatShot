// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "Components/Button.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class UCheckBox;
class UCrossHairSettingsWidget;
class UAASettings;
class USlider;
class UEditableTextBox;
class UComboBoxString;
class UVerticalBox;
class UTextBlock;
class UButton;
class UWidgetSwitcher;
class SettingsMenuWidget;
class UCrossHairSettingsWidget;

UENUM()
enum class ESettingType : uint8
{
	AntiAliasing UMETA(DisplayName, "AntiAliasing"),
	GlobalIllumination UMETA(DisplayName, "GlobalIllumination"),
	PostProcessing  UMETA(DisplayName, "PostProcessing"),
	Reflection UMETA(DisplayName, "Reflection"),
	Shadow UMETA(DisplayName, "Shadow"),
	Shading UMETA(DisplayName, "Shading"),
	Texture UMETA(DisplayName, "Texture"),
	ViewDistance UMETA(DisplayName, "ViewDistance"),
	VisualEffect UMETA(DisplayName, "VisualEffect")
};

ENUM_RANGE_BY_FIRST_AND_LAST(ESettingType, ESettingType::AntiAliasing, ESettingType::VisualEffect);

UCLASS()
class BEATSHOT_API UVideoSettingButton : public UButton
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly)
	int32 Quality;
	UPROPERTY(EditDefaultsOnly)
	ESettingType SettingType;
};

UCLASS()
class BEATSHOT_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

#pragma region MenuWidgets

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoAndSoundSettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* AASettings;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Sensitivity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* CrossHair;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* VideoAndSoundSettingsWidget;
public:
	/** So that parent widgets can bind to OnRestartButtonClicked */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UAASettings* AASettingsWidget;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* SensitivityWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UCrossHairSettingsWidget* CrossHairWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* VideoAndSoundSettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* AASettingsButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* SensitivityButton;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* CrossHairButton;

#pragma	endregion

#pragma region VideoAndSound

	/** SOUND */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* GlobalSoundInputValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MenuSoundInputValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	UEditableTextBox* MusicSoundInputValue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound ")
	USlider* GlobalSoundSlider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* MenuSoundSlider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Sound")
	USlider* MusicSoundSlider;
	/** Anti-Aliasing */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AA0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AA1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AA2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Anti-Aliasing Quality")
	UVideoSettingButton* AA3;
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
	UVideoSettingButton* PP0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PP1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PP2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Post Processing Quality")
	UVideoSettingButton* PP3;
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
	UVideoSettingButton* SW0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SW1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SW2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Shadow Quality")
	UVideoSettingButton* SW3;
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
	UVideoSettingButton* VD0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VD1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VD2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "View Distance Quality")
	UVideoSettingButton* VD3;
	/** Visual Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Visual Effect Quality")
	UVideoSettingButton* VEQ3;
	/** FRAME LIMIT */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Frame Limit")
	UEditableTextBox* FrameLimitMenuValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Frame Limit")
	UEditableTextBox* FrameLimitGameValue;
	/** STREAK */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Streak")
	UEditableTextBox* StreakFrequency;
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveVideoAndSoundSettingsButton;

	/** Saves the Video and Sound Settings using Game Instance */
	UFUNCTION()
	void SaveVideoAndSoundSettingsButtonClicked();
	UFUNCTION()
	void OnVideoAndSoundSettingsButtonClicked() {SlideButtons(VideoAndSoundSettingsButton); }

	UFUNCTION()
	void OnGlobalSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnMenuSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnMusicSoundSliderChanged(const float NewValue);
	UFUNCTION()
	void OnGlobalSoundValueChanged(const FText& NewValue);
	UFUNCTION()
	void OnMenuSoundValueChanged(const FText& NewValue);
	UFUNCTION()
	void OnMusicSoundValueChanged(const FText& NewValue);
	
	UFUNCTION()
	void OnWindowModeSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnResolutionSelectionChanged(const FString SelectedOption, ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnFrameLimitMenuValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnFrameLimitGameValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnStreakFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnVSyncEnabledCheckStateChanged(const bool bIsChecked);
	UFUNCTION()
	void OnFPSCounterCheckStateChanged(const bool bIsChecked);
	UFUNCTION()
	void OnShowStreakCombatTextCheckStateChanged(const bool bIsChecked);

	/** Changes video settings quality depending on input button */
	UFUNCTION()
	void OnVideoQualityButtonClicked(UVideoSettingButton* ClickedButton);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	TSubclassOf<USoundClass> GlobalSoundClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	TSubclassOf<USoundClass> MenuSoundClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	TSubclassOf<USoundMix> GlobalSoundMixClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	USoundClass* GlobalSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	USoundClass* MenuSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Settings")
	USoundMix* GlobalSoundMix;

#pragma	endregion

#pragma region Sensitivity

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UTextBlock* CurrentSensitivityValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UEditableTextBox* NewSensitivityValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	UEditableTextBox* NewSensitivityCsgoValue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "Player Settings")
	USlider* SensSlider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	UButton* SaveSensitivityButton;
	
	UFUNCTION()
	void OnSaveSensitivitySettingsButtonClicked();
	UFUNCTION()
	void OnSensitivityButtonClicked() { SlideButtons(SensitivityButton); }
	UFUNCTION()
	void OnNewSensitivityValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnNewSensitivityCsgoValue(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSensitivitySliderChanged(const float NewValue);

#pragma	endregion
	
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void LoadPlayerSettings();
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void SavePlayerSettings() const;
	/** Reset AASettings to default value and repopulate in Settings Menu. Does not automatically save */
	UFUNCTION(BlueprintCallable, Category = "Player Settings")
	void ResetPlayerSettings();
	/** TODO: Implement this in c++ */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Player Settings")
	void PopulatePlayerSettings();
	
	FPlayerSettings InitialPlayerSettings;
	FPlayerSettings NewPlayerSettings;
	
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	UFUNCTION()
	void OnAASettingsButtonClicked() {SlideButtons(AASettingsButton); }
	UFUNCTION()
	void OnCrossHairButtonClicked() {SlideButtons(CrossHairButton); }
	UFUNCTION()
	float ChangeValueOnSliderChange(const float SliderValue, UEditableTextBox* TextBoxToChange, const float SnapSize);
	UFUNCTION()
	float ChangeSliderOnValueChange(const FText& TextValue, USlider* SliderToChange, const float SnapSize);
	
	const float CsgoMultiplier = 3.18;
	const FLinearColor BeatshotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	const FLinearColor White = FLinearColor::White;
};



