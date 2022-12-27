// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModeActorBase.h"
#include "GameModeButton.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

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

/**
 *
 */
UCLASS()
class BEATSHOT_API UGameModesWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	virtual void NativeConstruct() override;

#pragma region NavigationWidgets
	
	/** A map to store buttons and the widgets they associate with */
	UPROPERTY()
	TMap<USlideRightButton*, UVerticalBox*> MenuWidgets;
	/** Switch between DefaultGameModes and CustomGameModes */
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* DefaultGameModesButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlideRightButton* CustomGameModesButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
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
	void OnDefaultGameModeButtonClicked(UGameModeButton* GameModeButton);
	/** Changes the background colors for the Game Mode of the ClickedButton */
	void SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton);
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
	/** TODO: Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION()
	void OnPlayFromStandardButtonClicked();

#pragma endregion

#pragma region CustomGeneral

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* GameModeNameComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes")
	UEditableTextBox* CustomGameModeETB;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* BaseGameModeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* GameModeDifficultyComboBox;

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

#pragma endregion

#pragma region CustomSaveStart

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* SaveCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* StartWithoutSavingButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* SaveCustomAndStartButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* StartCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* RemoveSelectedCustomButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UButton* RemoveAllCustomButton;

private:
	
	/** saves CustomGameModesArray to CustomGameModes save slot */
	void SaveCustomGameModeArrayToSlot() const;
	/** saves CustomGameModesArray to local CustomGameModesArray, then saves to slot */
	void SaveCustomGameMode(const FGameModeActorStruct GameModeToSave);
	/** Removes a Custom Game Mode from CustomGameModesArray, then saves to slot */
	bool RemoveCustomGameMode(const FString& CustomGameModeName);
	/** Removes all Custom Game Mode from CustomGameModesArray, then saves to slot */
	void RemoveAllCustomGameModes();
	/** loads and returns CustomGameModesMap from CustomGameModes save slot */
	TArray<FGameModeActorStruct> LoadCustomGameModes();
	/** Changes the Save and Start Button states depending on what is selected in GameModeNameComboBox and CustomGameModeETB */
	void UpdateSaveStartButtonStates(const FString& CustomGameModeName);
	/** Saves the custom game mode to local array and slot, repopulates GameModeNameComboBox, and selects the new custom game mode */
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
	void OnStartCustomButtonClicked();
	/** Calls RemoveCustomGameMode */
	UFUNCTION()
	void OnRemoveSelectedCustomButtonClicked();
	/** Calls RemoveAllCustomGameModes */
	UFUNCTION()
	void OnRemoveAllCustomButtonClicked();
	/** Initializes a PopupMessage using DefaultPlayerController, and binds to the buttons */
	void ShowConfirmOverwriteMessage();
	/** Overwrites a custom game mode by invoking SaveCustomGameMode and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnConfirmOverwriteButtonClicked();
	/** Does not overwrite a custom game mode, and calls DefaultPlayerController to hide the message */
	UFUNCTION()
	void OnCancelOverwriteButtonClicked();

#pragma endregion

#pragma region TimeRelated

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UComboBoxString* PlayerDelayComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* LifespanSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UEditableTextBox* LifespanValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	USlider* TargetSpawnCDSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes")
	UEditableTextBox* TargetSpawnCDValue;

private:
	
	UFUNCTION()
	void OnPlayerDelaySelectionChanged(const FString SelectedPlayerDelay, const ESelectInfo::Type SelectionType) { SelectedGameMode.PlayerDelay = FCString::Atof(*SelectedPlayerDelay); }
	UFUNCTION()
	void OnLifespanSliderChanged(const float NewLifespan) { SelectedGameMode.TargetMaxLifeSpan = NewLifespan; }
	UFUNCTION()
	void OnLifespanValueCommitted(const FText& NewLifespan, ETextCommit::Type CommitType) { SelectedGameMode.TargetMaxLifeSpan = FCString::Atof(*NewLifespan.ToString()); }
	UFUNCTION()
	void OnTargetSpawnCDSliderChanged(const float NewTargetSpawnCD) { SelectedGameMode.TargetSpawnCD = NewTargetSpawnCD; }
	UFUNCTION()
	void OnTargetSpawnCDValueCommitted(const FText& NewTargetSpawnCD, ETextCommit::Type CommitType) { SelectedGameMode.TargetSpawnCD = FCString::Atof(*NewTargetSpawnCD.ToString()); }

#pragma endregion

#pragma region TargetSpread

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UCheckBox* HeadShotOnlyCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UCheckBox* WallCenteredCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	USlider* MinTargetDistanceSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UEditableTextBox* MinTargetDistanceValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UComboBoxString* SpreadTypeComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	USlider* HorizontalSpreadSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UEditableTextBox* HorizontalSpreadValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	USlider* VerticalSpreadSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Spread")
	UEditableTextBox* VerticalSpreadValue;

private:
	
	UFUNCTION()
	void OnHeadShotOnlyCheckStateChanged(const bool bHeadshotOnly) {SelectedGameMode.HeadshotHeight = bHeadshotOnly; }
	UFUNCTION()
	void OnWallCenteredCheckStateChanged(const bool bWallCentered) {SelectedGameMode.WallCentered = bWallCentered; }
	UFUNCTION()
	void OnMinTargetDistanceSliderChanged(const float NewMinTargetDistance) {SelectedGameMode.MinDistanceBetweenTargets = NewMinTargetDistance; }
	UFUNCTION()
	void OnMinTargetDistanceValueCommitted(const FText& NewMinTargetDistance, ETextCommit::Type CommitType) {SelectedGameMode.MinDistanceBetweenTargets = FCString::Atof(*NewMinTargetDistance.ToString()); }
	UFUNCTION()
	void OnSpreadTypeSelectionChanged(const FString SelectedSpreadType, const ESelectInfo::Type SelectionType) {SelectedGameMode.SpreadType = GetSpreadType(SelectedSpreadType); }
	UFUNCTION()
	void OnHorizontalSpreadSliderChanged(const float NewHorizontalSpread) {SelectedGameMode.BoxBounds.Y = NewHorizontalSpread; }
	UFUNCTION()
	void OnHorizontalSpreadValueCommitted(const FText& NewHorizontalSpread, ETextCommit::Type CommitType) {SelectedGameMode.BoxBounds.Y = FCString::Atof(*NewHorizontalSpread.ToString()); }
	UFUNCTION()
	void OnVerticalSpreadSliderChanged(const float NewVerticalSpread) {SelectedGameMode.BoxBounds.Z = NewVerticalSpread; }
	UFUNCTION()
	void OnVerticalSpreadValueCommitted(const FText& NewVerticalSpread, ETextCommit::Type CommitType) {SelectedGameMode.BoxBounds.Z = FCString::Atof(*NewVerticalSpread.ToString()); }

#pragma endregion

#pragma region TargetSizing

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	UCheckBox* ConstantTargetScaleCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	UCheckBox* DynamicTargetScaleCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	USlider* MinTargetScaleSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	UEditableTextBox* MinTargetScaleValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	USlider* MaxTargetScaleSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Sizing")
	UEditableTextBox* MaxTargetScaleValue;

private:
	
	UFUNCTION()
	void OnConstantTargetScaleCheckStateChanged(const bool bConstantTargetScale);
	UFUNCTION()
	void OnDynamicTargetScaleCheckStateChanged(const bool bDynamicTargetScale) { SelectedGameMode.UseDynamicSizing = bDynamicTargetScale; }
	UFUNCTION()
	void OnMinTargetScaleSliderChanged(const float NewMinTargetScale) { SelectedGameMode.MinTargetScale = NewMinTargetScale; }
	UFUNCTION()
	void OnMinTargetScaleValueCommitted(const FText& NewMinTargetScale, ETextCommit::Type CommitType) { SelectedGameMode.MinTargetScale = FCString::Atof(*NewMinTargetScale.ToString()); }
	UFUNCTION()
	void OnMaxTargetScaleSliderChanged(const float NewMaxTargetScale) { SelectedGameMode.MaxTargetScale = NewMaxTargetScale; }
	UFUNCTION()
	void OnMaxTargetScaleValueCommitted(const FText& NewMaxTargetScale, ETextCommit::Type CommitType) { SelectedGameMode.MaxTargetScale = FCString::Atof(*NewMaxTargetScale.ToString()); }

#pragma endregion

#pragma region BeatGrid

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	UBorder* BeatGridSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	USlider* BeatGridHorizontalSpacingSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	USlider* BeatGridVerticalSpacingSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	UComboBoxString* NumBeatGridTargetsComboBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	UCheckBox* ConstantBeatGridSpacingCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatGrid")
	UCheckBox* RandomizeNextBeatGridTargetCheckBox;
	
private:
	
	UFUNCTION()
	void OnConstantBeatGridSpacingCheckStateChanged(const bool bConstantBeatGridSpacing);
	UFUNCTION()
	void OnRandomizeNextBeatGridTargetCheckStateChanged(const bool bRandomizeNextBeatGridTarget) { SelectedGameMode.RandomizeBeatGrid = bRandomizeNextBeatGridTarget; }
	UFUNCTION()
	void OnNumBeatGridTargetsSelectionChanged(const FString NumBeatGridTargets, const ESelectInfo::Type SelectionType) { SelectedGameMode.NumTargetsAtOnceBeatGrid = FCString::Atoi(*NumBeatGridTargets); }
	
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

#pragma region BeatTrack

protected:
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	UBorder* BeatTrackSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	UCheckBox* ConstantTargetSpeedCheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	USlider* BeatTrackMinTargetSpeedSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	UEditableTextBox*  BeatTrackMinTargetSpeedValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	USlider* BeatTrackMaxTargetSpeedSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "BeatTrack")
	UEditableTextBox* BeatTrackMaxTargetSpeedValue;
	
private:
	
	UFUNCTION()
	void OnConstantTargetSpeedCheckStateChanged(const bool bConstantTargetSpeed);
	UFUNCTION()
	void OnBeatTrackMinTargetSpeedSliderChanged(const float NewBeatTrackMinTargetSpeed) { SelectedGameMode.MinTrackingSpeed = NewBeatTrackMinTargetSpeed; }
	UFUNCTION()
	void OnBeatTrackMinTargetSpeedValueCommitted(const FText& NewBeatTrackMinTargetSpeed, ETextCommit::Type CommitType) { SelectedGameMode.MinTrackingSpeed = FCString::Atof(*NewBeatTrackMinTargetSpeed.ToString()); }
	UFUNCTION()
	void OnBeatTrackMaxTargetSpeedSliderChanged(const float NewBeatTrackMaxTargetSpeed) { SelectedGameMode.MaxTrackingSpeed = NewBeatTrackMaxTargetSpeed; }
	UFUNCTION()
	void OnBeatTrackMaxTargetSpeedValueCommitted(const FText& NewBeatTrackMaxTargetSpeed, ETextCommit::Type CommitType) { SelectedGameMode.MaxTrackingSpeed = FCString::Atof(*NewBeatTrackMaxTargetSpeed.ToString()); }

#pragma endregion

#pragma region Update
	
	/** Initializes all Custom game mode options based on the GameModeActorStruct */
	void PopulateGameModeOptions(const FGameModeActorStruct& InputGameModeActorStruct, bool bSelectGameModeNameComboBox);
	/** Updates GameModeNameComboBox with CustomGameModes */
	void PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect);

#pragma endregion;

#pragma region Utility
	
	/** Returns the ESpreadType corresponding to the SpreadType string */
	ESpreadType GetSpreadType(const FString& SpreadType) const;
	/** Returns the FGameModeActorStruct corresponding to the input GameModeName string */
	FGameModeActorStruct GetDefaultGameMode(const FString& GameModeName);
	/** Returns the FGameModeActorStruct corresponding to the input CustomGameModeName string */
	FGameModeActorStruct GetCustomGameMode(const FString& CustomGameModeName);
	/** Returns whether or not the GameModeName is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName);
	/** Returns whether or not the GameModeName is already a custom game mode name */
	bool IsCustomGameMode(const FString& GameModeName);
	/** Creates default game modes based on Difficulty and Spread Type */
	static FGameModeActorStruct GameModeActorStructConstructor(EGameModeActorName GameModeActor,
	                                                           EGameModeDifficulty NewGameModeDifficulty =
		                                                           EGameModeDifficulty::Normal,
	                                                           ESpreadType NewSpreadType = ESpreadType::StaticNarrow);
	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void InitializeExit();
	/** Cleans up widgets, initializes the game mode if in Range level, or opens the Range level if in MainMenuLevel */
	UFUNCTION()
	void StartGame();

#pragma endregion

	
	/** The object used to save custom game mode properties to */
	FGameModeActorStruct SelectedGameMode;
	/** The map to add custom game modes to */
	TArray<FGameModeActorStruct> CustomGameModesArray;
	/** The array of default Game Modes */
	TArray<FGameModeActorStruct> GameModeActorDefaults;
	/** The array of Game Modes to display */
	TArray<FGameModeActorStruct> GameModesToDisplay;
	/** The color used to change the GameModeButton color to when selected */
	const FLinearColor BeatshotBlue = FLinearColor(0.049707, 0.571125, 0.83077, 1.0);
	/** The color used to change the GameModeButton color to when not selected */
	const FLinearColor White = FLinearColor::White;
	/** The diameter of a target */
	const float SphereDiameter = 100.f;
};



