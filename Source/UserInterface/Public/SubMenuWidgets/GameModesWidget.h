// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalDelegates.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UTooltipImage;
class UGameModesWidget_DefiningConfig;
class UGameModesWidget_TargetConfig;
class UGameModesWidget_AIConfig;
class UGameModesWidget_GridConfig;
class UGameModesWidget_SpatialConfig;
class UAudioSelectWidget;
class UHorizontalBox;
class USavedTextWidget;
class UPopupMessageWidget;
class UVerticalBox;
class UBorder;
class UEditableTextBox;
class UWidgetSwitcher;
class UComboBoxString;
class USlider;
class UCheckBox;
class UMenuButton;
class UBSButton;

/** The base widget for selecting or customizing a game mode. The custom portion is split into multiple SettingsCategoryWidgets. Includes a default game modes section */
UCLASS()
class USERINTERFACE_API UGameModesWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override { return GameModeDataAsset.Get(); }

public:
	/** Whether or not this widget is MainMenu child or a PostGameMenu child */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	/** Executes when the user is exiting the GameModesWidget, broadcast to GameInstance to handle transition */
	FOnGameModeStateChanged OnGameModeStateChanged;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Game Modes")
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;
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
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AI Config")
	TSubclassOf<UGameModesWidget_AIConfig> AIConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Target Config")
	TSubclassOf<UGameModesWidget_TargetConfig> TargetConfigClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipImage> WarningEMarkClass;

	TObjectPtr<UTooltipImage> TooltipWarningImage_EnableAI;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	USavedTextWidget* SavedTextWidget;
	
	TObjectPtr<UGameModesWidget_DefiningConfig> DefiningConfig;
	TObjectPtr<UGameModesWidget_TargetConfig> TargetConfig;
	TObjectPtr<UGameModesWidget_AIConfig> AIConfig;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Defining Config")
	UVerticalBox* Box_DefiningConfig;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | Target Config")
	UVerticalBox* Box_TargetConfig;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | AI Config")
	UVerticalBox* Box_AIConfig;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UVerticalBox* Box_DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UVerticalBox* Box_CustomGameModes;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UMenuButton* MenuButton_DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UMenuButton* MenuButton_CustomGameModes;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_SaveCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_StartWithoutSaving;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_SaveCustomAndStart;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_StartCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_RemoveSelectedCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | SaveStart")
	UBSButton* Button_RemoveAllCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Navigation")
	UBSButton* Button_CustomizeFromStandard;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Navigation")
	UBSButton* Button_PlayFromStandard;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | Spread")
	UBorder* Border_DifficultySelect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_NormalDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_HardDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_DeathDifficulty;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_BeatGrid;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_BeatTrack;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_MultiBeat;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Default Game Modes | ModeButtons")
	UBSButton* Button_SingleBeat;
	
	private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();
	
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

	UFUNCTION()
	void OnButtonClicked_CustomGameModeButton(const UBSButton* Button);

	UFUNCTION()
	void OnButtonClicked_MenuButton(const UBSButton* Button);

	/** Saves the custom game mode to slot, repopulates ComboBox_GameModeName, and selects the new custom game mode */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustom();

	/** Calls CheckForExistingAndDisplayOverwriteMessage, and calls ShowAudioFormatSelect if no existing found */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustomAndStart();

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
	void OnButtonClicked_DefaultGameMode(const UBSButton* GameModeButton);

	/** Changes the SelectedGameMode depending on input button */
	UFUNCTION(Category = "Default Game Modes | ModeButtons")
	void OnButtonClicked_Difficulty(const UBSButton* GameModeButton);

	UFUNCTION()
	void OnTargetDamageTypeChanged();

	/** Returns false only if the TargetDistributionPolicy is set to grid, AI is enabled, and the grid isn't in increments of 5 */
	bool IsAIValid();

	/** Returns a TooltipImage widget created and placed inside the BoxToPlaceIn */
	UTooltipImage* ConstructWarningEMarkWidget(UHorizontalBox* BoxToPlaceIn);
	
	/** The BoundsScalingPolicy for a selected Preset Game Mode */
	EBoundsScalingPolicy PresetSelection_BoundsScalingMethod;
	
	/** The BaseGameMode for a selected Preset Game Mode */
	EBaseGameMode PresetSelection_PresetGameMode;
	
	/** The difficulty for a selected Preset Game Mode */
	EGameModeDifficulty PresetSelection_Difficulty;
};
