// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameModeButton.h"
#include "TooltipImage.h"
#include "TooltipWidget.h"
#include "ConstrainedSlider.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UHorizontalBox;
class UPopupMessageWidget;
class USlideRightButton;
class UVerticalBox;
class UDefaultGameInstance;
class UButton;
class UBorder;
class UEditableTextBox;
class UWidgetSwitcher;
class UComboBoxString;
class USlider;
class UCheckBox;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateBeatGridConstraints, float, value);

UCLASS()
class BEATSHOT_API UGameModesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

#pragma region Navigation

	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	/** Switch between DefaultGameModes and CustomGameModes */
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

private:
	/** Function to Play the Slide Animation for Navigation Buttons */
	UFUNCTION()
	void SlideButtons(const USlideRightButton* ActiveButton);

	UFUNCTION()
	void OnDefaultGameModesButtonClicked() { SlideButtons(DefaultGameModesButton); }

	UFUNCTION()
	void OnCustomGameModesButtonClicked() { SlideButtons(CustomGameModesButton); }

#pragma endregion

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
	void OnDefaultGameModeButtonClicked(const UGameModeButton* GameModeButton);

	/** Changes the background colors for the Game Mode of the ClickedButton */
	void SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const;

	/** Changes the SpreadType of SelectedGameMode to Dynamic */
	UFUNCTION()
	void OnDynamicSpreadButtonClicked();

	/** Changes the SpreadType of SelectedGameMode to Narrow */
	UFUNCTION()
	void OnNarrowSpreadButtonClicked();

	/** Changes the SpreadType of SelectedGameMode to Wide */
	UFUNCTION()
	void OnWideSpreadButtonClicked();

	/** Switches to CustomGameModes, making sure to Populate the CustomGameModeOptions with what's selected in DefaultGameModes */
	UFUNCTION()
	void OnCustomizeFromStandardButtonClicked();

	ESpreadType DefaultGameModeSpreadType;
	EGameModeActorName DefaultGameModeActorName;
	EGameModeDifficulty DefaultGameModeDifficulty;

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
	UComboBoxString* PlayerDelayComboBox;
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
	UHorizontalBox* MinTargetDistanceBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Spread")
	UHorizontalBox* SpreadTypeBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Sizing")
	UCheckBox* DynamicTargetScaleCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Sizing")
	UConstrainedSlider* TargetScaleConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UBorder* BeatGridSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* BeatGridHorizontalSpacingSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	USlider* BeatGridVerticalSpacingSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* BeatGridHorizontalSpacingValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UEditableTextBox* BeatGridVerticalSpacingValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UComboBoxString* NumBeatGridTargetsComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* ConstantBeatGridSpacingCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid")
	UCheckBox* RandomizeNextBeatGridTargetCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UBorder* BeatTrackSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UConstrainedSlider* TargetSpeedConstrained;

private:
	/** Populate Game Mode Options and changes the Custom Save Button states */
	UFUNCTION()
	void OnGameModeNameSelectionChange(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType);

	/** Changes the CustomGameModeName in SelectedGameMode, and changes the Custom Save Button states */
	UFUNCTION()
	void OnCustomGameModeETBChange(const FText& NewCustomGameModeText);

	/** Changes the base game mode in SelectedGameMode */
	UFUNCTION()
	void OnBaseGameModeSelectionChange(const FString SelectedBaseGameMode, const ESelectInfo::Type SelectionType);

	/** Changes the difficulty in SelectedGameMode */
	UFUNCTION()
	void OnGameModeDifficultySelectionChange(const FString SelectedDifficulty, const ESelectInfo::Type SelectionType);

	UFUNCTION()
	void OnLifespanSliderChanged(const float NewLifespan);
	UFUNCTION()
	void OnLifespanValueCommitted(const FText& NewLifespan, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTargetSpawnCDSliderChanged(const float NewTargetSpawnCD);
	UFUNCTION()
	void OnTargetSpawnCDValueCommitted(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnHeadShotOnlyCheckStateChanged(const bool bHeadshotOnly);
	UFUNCTION()
	void OnMinTargetDistanceSliderChanged(const float NewMinTargetDistance);
	UFUNCTION()
	void OnMinTargetDistanceValueCommitted(const FText& NewMinTargetDistance, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnHorizontalSpreadSliderChanged(const float NewHorizontalSpread);
	UFUNCTION()
	void OnHorizontalSpreadValueCommitted(const FText& NewHorizontalSpread, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnVerticalSpreadSliderChanged(const float NewVerticalSpread);
	UFUNCTION()
	void OnVerticalSpreadValueCommitted(const FText& NewVerticalSpread, ETextCommit::Type CommitType);

	UFUNCTION()
	void OnConstantBeatGridSpacingCheckStateChanged(const bool bConstantBeatGridSpacing);
	UFUNCTION()
	bool CheckAllBeatGridConstraints();
	UFUNCTION()
	void BeatGridTargetSizeConstrained(float Value);
	UFUNCTION()
	void BeatGridSpawnAreaConstrained(float Value);
	UFUNCTION()
	void BeatGridNumberOfTargetsConstrained(FString SelectedSong, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void BeatGridSpacingConstrained(float Value);

	FUpdateBeatGridConstraints BeatGridConstraintsDelegate;

#pragma endregion

#pragma region Update

	/** Initializes all Custom game mode options based on the GameModeActorStruct */
	void PopulateGameModeOptions(const FGameModeActorStruct& InputGameModeActorStruct);

	/** Updates GameModeNameComboBox with CustomGameModes */
	void PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect);

	/** Retrieves all Custom game mode options and returns a GameModeActorStruct with those options */
	FGameModeActorStruct GetCustomGameModeOptions();

#pragma endregion;

#pragma region SaveStart

protected:
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
	/** saves a CustomGameMode to save slot  */
	void SaveCustomGameMode(const FGameModeActorStruct GameModeToSave) const;

	/** Loads CustomGameModes, removes the CustomGameMode corresponding to CustomGameModeName, then saves to slot */
	void RemoveCustomGameMode(const FString& CustomGameModeName) const;

	/** Loads CustomGameModes, removes all, then saves to slot */
	void RemoveAllCustomGameModes() const;

	/** loads and returns CustomGameModes array from CustomGameModes save slot */
	TArray<FGameModeActorStruct> LoadCustomGameModes() const;

	/** Changes the Save and Start Button states depending on what is selected in GameModeNameComboBox and CustomGameModeETB */
	void UpdateSaveStartButtonStates();

	/** Saves the custom game mode to slot, repopulates GameModeNameComboBox, and selects the new custom game mode */
	UFUNCTION()
	void OnSaveCustomButtonClicked();

	/** Passes SelectedGameMode to GameInstance without saving, fades to black, and opens the Range level */
	UFUNCTION()
	void OnStartWithoutSavingButtonClicked();

	/** Saves the custom game mode and passes it to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnSaveCustomAndStartButtonClicked();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnPlayFromStandardButtonClicked();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnStartCustomButtonClicked();

	/** Calls RemoveCustomGameMode */
	UFUNCTION()
	void OnRemoveSelectedCustomButtonClicked();

	/** Calls RemoveAllCustomGameModes */
	UFUNCTION()
	void OnRemoveAllCustomButtonClicked();

	/** Initializes a PopupMessage using DefaultPlayerController, and binds to the buttons. Optionally starts the game afterwards */
	void ShowConfirmOverwriteMessage(const bool bStartGameAfter);

	/** Overwrites a custom game mode by invoking SaveCustomGameMode and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnConfirmOverwriteButtonClicked();

	/** Overwrites a custom game mode by invoking SaveCustomGameMode and calls DefaultPlayerController to hide the message. Calls InitializeExit after */
	UFUNCTION()
	void OnConfirmOverwriteButtonClickedAndStartGame();

	/** Does not overwrite a custom game mode, and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnCancelOverwriteButtonClicked();

	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void InitializeExit(const bool bStartFromDefaultGameMode);

	/** Cleans up widgets, fades screen to black, and initializes the CustomGameMode with Game Instance.
	 *  If already in Range level, calls InitializeGameMode, otherwise opens the Range level */
	UFUNCTION()
	void StartGameFromCustomGameModes();

	/** Cleans up widgets, fades screen to black, and initializes the DefaultGameMode with Game Instance.
	 *  If already in Range level, calls InitializeGameMode, otherwise opens the Range level */
	UFUNCTION()
	void StartGameFromDefaultGameModes();

#pragma endregion

#pragma region Utility

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
	UTooltipImage* MinDistanceQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* SpreadTypeQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* DynamicTargetScaleQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BeatGridEvenSpacingQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BeatGridAdjacentOnlyQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* BeatGridNumTargetsQMark;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	UPROPERTY()
	UTooltipWidget* Tooltip;

private:
	/** Updates the tooltip text and shows the tooltip at the location of the Button (which is just the question mark image) */
	UFUNCTION()
	void OnTooltipImageHovered(UTooltipImage* HoveredTooltipImage, const FText& TooltipTextToShow);

	/** Returns the ESpreadType corresponding to the SpreadType string */
	ESpreadType GetSpreadType() const;

	/** Returns the FGameModeActorStruct corresponding to the input GameModeName string */
	FGameModeActorStruct GetDefaultGameMode(const FString& GameModeName) const;

	/** Returns the FGameModeActorStruct corresponding to the input CustomGameModeName string */
	FGameModeActorStruct GetCustomGameMode(const FString& CustomGameModeName) const;

	/** Returns whether or not the GameModeName is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName) const;

	/** Returns whether or not the GameModeName is already a custom game mode name */
	bool IsCustomGameMode(const FString& GameModeName) const;

	/** Clamps NewTextValue, updates associated Slider value while rounding to the GridSnapSize */
	void OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange, USlider* SliderToChange,
	                              const float GridSnapSize, const float Min, const float Max);

	/** Updates associated TextBoxToChange with result of rounding to the GridSnapSize */
	void OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange, const float GridSnapSize);

	/** The array of default Game Modes */
	TArray<FGameModeActorStruct> GameModeActorDefaults;
	/** The color used to change the GameModeButton color to when selected */
	const FLinearColor BeatshotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	/** The color used to change the GameModeButton color to when not selected */
	const FLinearColor White = FLinearColor::White;
	/** The diameter of a target */
	const float SphereDiameter = 100.f;

#pragma endregion
};
