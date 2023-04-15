// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalDelegates.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/DoubleSyncedSliderAndTextBox.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UGameModesWidget_DefiningConfig;
class UGameModesWidget_TargetConfig;
class UGameModesWidget_BeatTrackConfig;
class UGameModesWidget_AIConfig;
class UGameModesWidget_BeatGridConfig;
class UGameModesWidget_SpatialConfig;
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
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	TSubclassOf<UAudioSelectWidget> AudioSelectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	UPopupMessageWidget* PopupMessageWidget;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	UAudioSelectWidget* AudioSelectWidget;
	
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Defining Config")
	TSubclassOf<UGameModesWidget_DefiningConfig> DefiningConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Spatial Config")
	TSubclassOf<UGameModesWidget_SpatialConfig> SpatialConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | BeatGrid Config")
	TSubclassOf<UGameModesWidget_BeatGridConfig> BeatGridConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AI Config")
	TSubclassOf<UGameModesWidget_AIConfig> AIConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | BeatTrack Config")
	TSubclassOf<UGameModesWidget_BeatTrackConfig> BeatTrackConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Target Config")
	TSubclassOf<UGameModesWidget_TargetConfig> TargetConfigClass;
	
	TObjectPtr<UGameModesWidget_DefiningConfig> DefiningConfig;
	TObjectPtr<UGameModesWidget_SpatialConfig> SpatialConfig;
	TObjectPtr<UGameModesWidget_TargetConfig> TargetConfig;
	TObjectPtr<UGameModesWidget_BeatGridConfig> BeatGridConfig;
	TObjectPtr<UGameModesWidget_AIConfig> AIConfig;
	TObjectPtr<UGameModesWidget_BeatTrackConfig> BeatTrackConfig;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Defining Config")
	UVerticalBox* DefiningConfigBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Spatial Config")
	UVerticalBox* SpatialConfigBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Config")
	UVerticalBox* TargetConfigBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI Config")
	UVerticalBox* AIConfigBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatGrid Config")
	UVerticalBox* BeatGridBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack Config")
	UVerticalBox* BeatTrackConfigBox;
	
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
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatGridNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatGridHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatGridDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatTrackNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatTrackHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* BeatTrackDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* MultiBeatNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* MultiBeatHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* MultiBeatDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* SingleBeatNormalButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* SingleBeatHardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UGameModeButton* SingleBeatDeathButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Spread")
	UBorder* SpreadSelect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Spread")
	UButton* DynamicSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Spread")
	UButton* NarrowSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Spread")
	UButton* WideSpreadButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Navigation")
	UButton* CustomizeFromStandardButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Navigation")
	UButton* PlayFromStandardButton;

	private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();

	/** Changes the background colors for the Game Mode of the ClickedButton */
	void SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const;
	
	/** Initializes all Custom game mode options based on the BSConfig */
	void PopulateGameModeOptions(const FBSConfig& InBSConfig);

	/** Retrieves all Custom game mode options and returns a BSConfig with those options */
	FBSConfig GetCustomGameModeOptions() const;
	
	/** Saves a CustomGameMode to save slot and displays the saved text */
	void SaveCustomGameModeAndShowSavedText(const FBSConfig& GameModeToSave);

	/** Changes the Save and Start Button states depending on what is selected in ComboBox_GameModeName and TextBox_CustomGameModeName */
	UFUNCTION()
	void UpdateSaveStartButtonStates();

	/** Initializes a PopupMessage using DefaultPlayerController, and binds to the buttons. Optionally starts the game afterwards */
	void ShowConfirmOverwriteMessage(const bool bStartGameAfter);

	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void ShowAudioFormatSelect(const bool bStartFromDefaultGameMode);

	/** Checks to see if the GameModeName ComboBox or the CustomGameModeName text box has a matching custom game mode that is already saved,
	 *  and calls ShowConfirmOverwriteMessage and returning true if so */
	bool CheckForExistingAndDisplayOverwriteMessage(const bool bStartGameAfter);
	
	UFUNCTION(Category = "Navigation")
	void SlideButtons(const USlideRightButton* ActiveButton);
	
	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_DefaultGameModes() { SlideButtons(DefaultGameModesButton); }

	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_CustomGameModes() { SlideButtons(CustomGameModesButton); }

	/** Switches to CustomGameModes and populates the CustomGameModeOptions with what's selected in DefaultGameModes */
	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_CustomizeFromStandard();

	/** Saves the custom game mode to slot, repopulates ComboBox_GameModeName, and selects the new custom game mode */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustom();

	/** Calls ShowAudioFormatSelect */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_StartWithoutSaving();

	/** Calls CheckForExistingAndDisplayOverwriteMessage, and calls ShowAudioFormatSelect if no existing found */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustomAndStart();

	/** Calls ShowAudioFormatSelect */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_PlayFromStandard();

	/** Calls ShowAudioFormatSelect */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_StartCustom();

	/** Calls RemoveCustomGameMode */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_RemoveSelectedCustom();
	
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_RemoveAllCustom();

	/** Overwrites a custom game mode and hides the message */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_ConfirmOverwrite();

	/** Overwrites a custom game mode and hides the message. Calls ShowAudioFormatSelect after */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_ConfirmOverwriteAndStartGame();

	/** Does not overwrite a custom game mode, and hides the message */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_CancelOverwrite();

	/** Changes the SelectedGameMode depending on input button */
	UFUNCTION(Category = "Default Game Modes | ModeButtons")
	void OnButtonClicked_DefaultGameMode(const UGameModeButton* GameModeButton);

	UFUNCTION(Category = "Default Game Modes | Spread")
	void OnButtonClicked_DynamicSpread();
	
	UFUNCTION(Category = "Default Game Modes | Spread")
	void OnButtonClicked_NarrowSpread();
	
	UFUNCTION(Category = "Default Game Modes | Spread")
	void OnButtonClicked_WideSpread();
	
	/** The Spread type for a selected Default Game Mode */
	ESpreadType DefaultSpreadType;
	
	/** The Default Mode type for a selected Default Game Mode */
	EDefaultMode DefaultMode;
	
	/** The difficulty for a selected Default Game Mode */
	EGameModeDifficulty DefaultDifficulty;
};
