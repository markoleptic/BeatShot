// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"
#include "WidgetComponents/ConstrainedSlider.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "WidgetComponents/ConstrainedSlider_BeatGrid.h"
#include "GameModesWidget.generated.h"

class UAudioSelectWidget;
class UHorizontalBox;
class USavedTextWidget;
class UPopupMessageWidget;
class USlideRightButton;
class UVerticalBox;
class UButton;
class UBorder;
class UEditableTextBox;
class UWidgetSwitcher;
class UComboBoxString;
class USlider;
class UCheckBox;

UCLASS()
class USERINTERFACE_API UGameModesWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Whether or not this widget is MainMenu child or a PostGameMenu child */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	/** Executes when the user is exiting the GameModesWidget, broadcast to GameInstance to handle transition */
	FOnGameModeStateChanged OnGameModeStateChanged;

protected:
	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	USlideRightButton* DefaultGameModesButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	USlideRightButton* CustomGameModesButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UVerticalBox* DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UVerticalBox* CustomGameModes;
	
	UPROPERTY(EditDefaultsOnly, Category = "AudioSelect")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, Category = "AudioSelect")
	TSubclassOf<UAudioSelectWidget> AudioSelectClass;
	UPROPERTY(EditDefaultsOnly, Category = "AudioSelect")
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(EditDefaultsOnly, Category = "AudioSelect")
	UAudioSelectWidget* AudioSelectWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* SaveCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* StartWithoutSavingButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* SaveCustomAndStartButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* StartCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* RemoveSelectedCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UButton* RemoveAllCustomButton;

private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();
	
	void SetHiddenConfigParameters(FBSConfig& Config);

	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);
	
	UFUNCTION()
	void OnButtonClicked_DefaultGameModes() { SlideButtons(DefaultGameModesButton); }

	UFUNCTION()
	void OnButtonClicked_CustomGameModes() { SlideButtons(CustomGameModesButton); }

	/** Saves the custom game mode to slot, repopulates GameModeNameComboBox, and selects the new custom game mode */
	UFUNCTION()
	void OnButtonClicked_SaveCustom();

	/** Passes SelectedGameMode to GameInstance without saving, fades to black, and opens the Range level */
	UFUNCTION()
	void OnButtonClicked_StartWithoutSaving();

	/** Saves the custom game mode and passes it to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnButtonClicked_SaveCustomAndStart();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnButtonClicked_PlayFromStandard();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnButtonClicked_StartCustom();

	/** Calls RemoveCustomGameMode */
	UFUNCTION()
	void OnButtonClicked_RemoveSelectedCustom();
	
	UFUNCTION()
	void OnButtonClicked_RemoveAllCustom();

	/** Overwrites a custom game mode by invoking SaveCustomGameModeToSlot and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnButtonClicked_ConfirmOverwrite();

	/** Overwrites a custom game mode by invoking SaveCustomGameModeToSlot and calls DefaultPlayerController to hide the message. Calls ShowAudioFormatSelect after */
	UFUNCTION()
	void OnButtonClicked_ConfirmOverwriteAndStartGame();

	/** Does not overwrite a custom game mode, and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnButtonClicked_CancelOverwrite();

	/** Initializes all Custom game mode options based on the BSConfig */
	void PopulateGameModeOptions(const FBSConfig& InBSConfig);

	/** Updates GameModeNameComboBox with CustomGameModes */
	void PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect);

	/** Retrieves all Custom game mode options and returns a BSConfig with those options */
	FBSConfig GetCustomGameModeOptions() const;
	
	/** Saves a CustomGameMode to save slot */
	void SaveCustomGameModeToSlot(FBSConfig& GameModeToSave);

	/** Changes the Save and Start Button states depending on what is selected in GameModeNameComboBox and CustomGameModeETB */
	void UpdateSaveStartButtonStates();

	/** Initializes a PopupMessage using DefaultPlayerController, and binds to the buttons. Optionally starts the game afterwards */
	void ShowConfirmOverwriteMessage(const bool bStartGameAfter);

	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void ShowAudioFormatSelect(const bool bStartFromDefaultGameMode);

	/** Returns the ESpreadType corresponding to the SpreadType string */
	ESpreadType GetSpreadType() const;

	/** Returns the FBSConfig corresponding to the input DefaultMode string */
	FBSConfig FindDefaultGameMode(const FString& GameModeName) const;

	/** Returns the FBSConfig corresponding to the input CustomGameModeName string */
	FBSConfig FindCustomGameMode(const FString& CustomGameModeName) const;

	/** Returns whether or not the DefaultMode is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName) const;

	/** Returns whether or not the DefaultMode is already a custom game mode name */
	bool IsCustomGameMode(const FString& GameModeName) const;

	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange, const float GridSnapSize, const float Min, const float Max) const;

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize) const;
	
	UTooltipImage* ConstructBeatGridWarningEMarkWidget(UHorizontalBox& BoxToPlaceIn);

	/** The array of default Game Modes */
	TArray<FBSConfig> DefaultModes;
	
	/** The color used to change the GameModeButton color to when selected */
	const FLinearColor BeatShotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	
	/** The color used to change the GameModeButton color to when not selected */
	const FLinearColor White = FLinearColor::White;
	
	/** The diameter of a target */
	const float SphereDiameter = 100.f;

	const FText BeatGridConstrainedText_Start = FText::FromString("There isn't enough space for BeatGrid options provided. Change ");
	const FText BeatGridConstrainedText_HorizontalSuggest = FText::FromString("number of Horizontal Targets to less than");
	const FText BeatGridConstrainedText_VerticalSuggest = FText::FromString("number of Vertical Targets to less than");
	const FText BeatGridConstrainedText_ScaleSuggest = FText::FromString("maximum Target Scale to less than");
	const FText BeatGridConstrainedText_HorizontalSpacingSuggest = FText::FromString("Horizontal Spacing to less than");
	const FText BeatGridConstrainedText_VerticalSpacingSuggest = FText::FromString("Vertical Spacing to less than");

#pragma region DefaultGameModes

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatGridDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* BeatTrackDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* MultiBeatDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UGameModeButton* SingleBeatDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UBorder* SpreadSelect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UButton* DynamicSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UButton* NarrowSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UButton* WideSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UButton* CustomizeFromStandardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UButton* PlayFromStandardButton;

private:
	/** Changes the SelectedGameMode depending on input button */
	UFUNCTION()
	void OnButtonClicked_DefaultGameMode(const UGameModeButton* GameModeButton);

	/** Changes the background colors for the Game Mode of the ClickedButton */
	void SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const;
	
	UFUNCTION()
	void OnButtonClicked_DynamicSpread();
	UFUNCTION()
	void OnButtonClicked_NarrowSpread();
	UFUNCTION()
	void OnButtonClicked_WideSpread();

	/** Switches to CustomGameModes and populates the CustomGameModeOptions with what's selected in DefaultGameModes */
	UFUNCTION()
	void OnButtonClicked_CustomizeFromStandard();

	ESpreadType DefaultSpreadType;
	EDefaultMode DefaultMode;
	EGameModeDifficulty DefaultDifficulty;

#pragma endregion

#pragma region CustomGameModes

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* GameModeNameComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UEditableTextBox* CustomGameModeETB;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* BaseGameModeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* GameModeDifficultyComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UHorizontalBox* BaseGameModeBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	USlider* PlayerDelaySlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	UEditableTextBox* PlayerDelayValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	USlider* LifespanSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	UEditableTextBox* LifespanValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	USlider* TargetSpawnCDSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Time Related")
	UEditableTextBox* TargetSpawnCDValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* HeadShotOnlyCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* WallCenteredCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UHorizontalBox* MinTargetDistanceBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* MinTargetDistanceSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* MinTargetDistanceValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UComboBoxString* SpreadTypeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* HorizontalSpreadSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* HorizontalSpreadValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* VerticalSpreadSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* VerticalSpreadValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UHorizontalBox* ForwardSpreadBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UCheckBox* ForwardSpreadCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	USlider* ForwardSpreadSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UEditableTextBox* ForwardSpreadValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UHorizontalBox* SpreadTypeBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Sizing")
	UCheckBox* DynamicTargetScaleCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Sizing")
	UConstrainedSlider* TargetScaleConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UBorder* AISpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UHorizontalBox* EnableAIBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UCheckBox* EnableAICheckBox;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UHorizontalBox* AIAlphaBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* AIAlphaSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* AIAlphaValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UHorizontalBox* AIEpsilonBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* AIEpsilonSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* AIEpsilonValue;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UHorizontalBox* AIGammaBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	USlider* AIGammaSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI")
	UEditableTextBox* AIGammaValue;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UBorder* BeatGridSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* RandomizeNextBeatGridTargetCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UConstrainedSlider_BeatGrid* BeatGridSpacingConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* BeatGridNumHorizontalTooltipTextBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UHorizontalBox* BeatGridNumVerticalTooltipTextBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* BeatGridNumHorizontalTargetsSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* BeatGridNumHorizontalTargetsValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* BeatGridNumVerticalTargetsSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* BeatGridNumVerticalTargetsValue;
	
	FBeatGridUpdate_NumTargets BeatGridUpdate_NumVerticalTargets;
	FBeatGridUpdate_NumTargets BeatGridUpdate_NumHorizontalTargets;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UBorder* BeatTrackSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UConstrainedSlider* TargetSpeedConstrained;

	bool bHorizontalTargetConstraint = false;
	bool bVerticalTargetConstraint = false;
	bool bTargetScaleConstraint = false;
	bool bHorizontalSpacingConstraint = false;
	bool bVerticalSpacingConstraint = false;
	
	const float MinPlayerDelayValue = 0;
	const float MaxPlayerDelayValue = 0.5;
	const float PlayerDelayGridSnapSize = 0.01;

	const float MinLifespanValue = 0.25;
	const float MaxLifespanValue = 2;
	const float LifespanGridSnapSize = 0.01;

	const float MinTargetSpawnCDValue = 0.05;
	const float MaxTargetSpawnCDValue = 2;
	const float TargetSpawnCDGridSnapSize = 0.01;

	const float MinMinTargetDistanceValue = 0;
	const float MaxMinTargetDistanceValue = 600;
	const float MinTargetDistanceGridSnapSize = 5;

	const float MinHorizontalSpreadValue = 200;
	const float MaxHorizontalSpreadValue = 3200;
	const float MinVerticalSpreadValue = 200;
	const float MaxVerticalSpreadValue = 1000;
	const float MinForwardSpreadValue = 100;
	const float MaxForwardSpreadValue = 5000;
	const float HorizontalSpreadGridSnapSize = 100;
	const float VerticalSpreadGridSnapSize = 100;
	
	const float MinTargetScaleValue = 0.1;
	const float MaxTargetScaleValue = 2;
	const float TargetScaleSnapSize = 0.01;

	const float MinTargetSpeedValue = 300;
	const float MaxTargetSpeedValue = 1000;
	const float TargetSpeedSnapSize = 10;

	const float MinAlphaValue = 0.1;
	const float MaxAlphaValue = 1;
	const float AlphaSnapSize = 0.01;
	
	const float MinEpsilonValue = 0;
	const float MaxEpsilonValue = 1;
	const float EpsilonSnapSize = 0.01;
	
	const float MinGammaValue = 0.1;
	const float MaxGammaValue = 1;
	const float GammaSnapSize = 0.01;

	float MinBeatGridHorizontalSpacingValue = 10;
	float MaxBeatGridHorizontalSpacingValue = 3200;
	const float BeatGridHorizontalSpacingSnapSize = 10;

	float MinBeatGridVerticalSpacingValue = 10;
	float MaxBeatGridVerticalSpacingValue = 1000;
	const float BeatGridVerticalSpacingSnapSize = 10;

	
	const int32 MinNumBeatGridHorizontalTargetsValue = 4;
	const int32 MaxNumBeatGridHorizontalTargetsValue = 100;
	const int32 NumBeatGridHorizontalTargetsSnapSize = 1;

	const int32 MinNumBeatGridVerticalTargetsValue = 4;
	const int32 MaxNumBeatGridVerticalTargetsValue = 50;
	const int32 NumBeatGridVerticalTargetsSnapSize = 1;

private:
	/** Populate Game Mode Options and changes the Custom Save Button states */
	UFUNCTION()
	void OnSelectionChanged_GameModeName(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType);

	/** Changes the CustomGameModeName in SelectedGameMode, and changes the Custom Save Button states */
	UFUNCTION()
	void OnTextChanged_CustomGameMode(const FText& NewCustomGameModeText);
	
	UFUNCTION()
	void OnSelectionChanged_BaseGameMode(const FString SelectedBaseGameMode, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_GameModeDifficulty(const FString SelectedDifficulty, const ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnSliderChanged_PlayerDelay(const float NewPlayerDelay);
	UFUNCTION()
	void OnTextCommitted_PlayerDelay(const FText& NewPlayerDelay, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_Lifespan(const float NewLifespan);
	UFUNCTION()
	void OnTextCommitted_Lifespan(const FText& NewLifespan, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_TargetSpawnCD(const float NewTargetSpawnCD);
	UFUNCTION()
	void OnTextCommitted_TargetSpawnCD(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_HeadShotOnly(const bool bHeadshotOnly);
	UFUNCTION()
	void OnSliderChanged_MinTargetDistance(const float NewMinTargetDistance);
	UFUNCTION()
	void OnTextCommitted_MinTargetDistance(const FText& NewMinTargetDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_HorizontalSpread(const float NewHorizontalSpread);
	UFUNCTION()
	void OnTextCommitted_HorizontalSpread(const FText& NewHorizontalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_VerticalSpread(const float NewVerticalSpread);
	UFUNCTION()
	void OnTextCommitted_VerticalSpread(const FText& NewVerticalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_ForwardSpread(const bool bUseForwardSpread);
	UFUNCTION()
	void OnSliderChanged_ForwardSpread(const float NewForwardSpread);
	UFUNCTION()
	void OnTextCommitted_ForwardSpread(const FText& NewForwardSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_BeatGridNumHorizontalTargets(const float NewNumHorizontalTargets);
	UFUNCTION()
	void OnTextCommitted_BeatGridNumHorizontalTargets(const FText& NewNumHorizontalTargets, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_BeatGridNumVerticalTargets(const float NewNumVerticalTargets);
	UFUNCTION()
	void OnTextCommitted_BeatGridNumVerticalTargets(const FText& NewNumVerticalTargets, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnCheckStateChanged_EnableAI(const bool bEnableAI);
	UFUNCTION()
	void OnSliderChanged_AIAlpha(const float NewAlpha);
	UFUNCTION()
	void OnTextCommitted_AIAlpha(const FText& NewAlpha, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_AIEpsilon(const float NewEpsilon);
	UFUNCTION()
	void OnTextCommitted_AIEpsilon(const FText& NewEpsilon, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnSliderChanged_AIGamma(const float NewGamma);
	UFUNCTION()
	void OnTextCommitted_AIGamma(const FText& NewGamma, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnBeatGridSpacingConstrained(const FBeatGridConstraints& BeatGridConstraints);

#pragma endregion

#pragma region Tooltips

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* GameModeTemplateQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* CustomGameModeNameQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BaseGameModeQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* GameModeDifficultyQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* SpawnBeatDelayQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* LifespanQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* TargetSpawnCDQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* HeadshotHeightQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* CenterTargetsQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* ForwardSpreadQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* MinDistanceQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* SpreadTypeQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* DynamicTargetScaleQMark;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* EnableAIQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* AlphaQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* EpsilonQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* GammaQMark;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BeatGridAdjacentOnlyQMark;
	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<UTooltipImage> BeatGridWarningEMarkClass;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UHorizontalBox* NumHorizontalTargetsTextTooltipBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UHorizontalBox* NumVerticalTargetsTextTooltipBox;
	
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* BeatGridWarningEMark_MaxTargetScale;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* BeatGridWarningEMark_BeatGridNumHorizontalTargets;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* BeatGridWarningEMark_BeatGridNumVerticalTargets;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* BeatGridWarningEMark_BeatGridHorizontalSpacing;
	UPROPERTY(BlueprintReadOnly, Category = "Tooltip")
	UTooltipImage* BeatGridWarningEMark_BeatGridVerticalSpacing;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	UPROPERTY()
	UTooltipWidget* Tooltip;

private:
	/** Updates the tooltip text and shows the tooltip at the location of the Button (which is just the question mark image) */
	UFUNCTION()
	void OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage, const FText& TooltipTextToShow);

#pragma endregion;
};
