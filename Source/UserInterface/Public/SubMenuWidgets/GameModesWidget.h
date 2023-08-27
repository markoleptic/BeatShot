// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalDelegates.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UCustomGameModesWidgetBase;
class UCustomGameModesWidget_PropertyView;
class UCustomGameModesWidget_CreatorView;
class UGameModeSharingWidget;
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

USTRUCT()
struct FStartWidgetProperties
{
	GENERATED_BODY()

	FBS_DefiningConfig DefiningConfig;
	bool bUseTemplateChecked;
	FString NewCustomGameModeName;

	FStartWidgetProperties()
	{
		DefiningConfig = FBS_DefiningConfig();
		bUseTemplateChecked = false;
		NewCustomGameModeName = FString();
	}

	FStartWidgetProperties(const FBS_DefiningConfig& InDefiningConfig, const bool bInUseTemplate)
	{
		DefiningConfig = InDefiningConfig;
		bUseTemplateChecked = bInUseTemplate;
		NewCustomGameModeName = FString();
	}

	FORCEINLINE bool operator==(const FStartWidgetProperties& Other) const
	{
		if (DefiningConfig == Other.DefiningConfig)
		{
			if (bUseTemplateChecked == Other.bUseTemplateChecked)
			{
				if (NewCustomGameModeName.Equals(NewCustomGameModeName, ESearchCase::CaseSensitive))
				{
					return true;
				}
			}
		}
		return false;
	}
};

DECLARE_MULTICAST_DELEGATE(FRequestSimulateTargetManager)
DECLARE_MULTICAST_DELEGATE(FOnPopulateGameModeOptions)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeBreakingChange, const bool bIsGameModeBreaking);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCreatorViewVisibilityChanged, const bool);

/** The base widget for selecting or customizing a game mode. The custom portion is split into multiple SettingsCategoryWidgets. Includes a default game modes section */
UCLASS()
class USERINTERFACE_API UGameModesWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override { return GameModeDataAsset.Get(); }

public:
	/** Whether or not this widget is MainMenu child or a PostGameMenu child */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	/** Executes when the user is exiting the GameModesWidget, broadcast to GameInstance to handle transition */
	FOnGameModeStateChanged OnGameModeStateChanged;

	/** Returns BSConfig */
	FBSConfig* GetConfigPointer() const { return BSConfig; }

	/** Broadcast when this widget wants to run the custom game mode preview using the TargetManagerPreview (MainMenuGameMode) */
	FRequestSimulateTargetManager RequestSimulateTargetManager;

	/** Broadcast PopulateGameModeOptions is called */
	FOnPopulateGameModeOptions OnPopulateGameModeOptions;

	/** Broadcast when CustomGameModesWidget_CreatorView visibility changes */
	FOnCreatorViewVisibilityChanged OnCreatorViewVisibilityChanged;

	/** Broadcast false when any non-defining config option is false. Broadcasts true only if all are true. Only Broadcasts if different than the previous */
	FOnGameModeBreakingChange OnGameModeBreakingChange;

	/** Returns whether or not CustomGameModesWidget_CreatorView is visible */
	bool GetCreatorViewVisible() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Custom Game Modes")
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | AudioSelect")
	TSubclassOf<UAudioSelectWidget> AudioSelectClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipImage> WarningEMarkClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Custom Game Modes")
	TSubclassOf<UGameModeSharingWidget> GameModeSharingClass;

	UPROPERTY()
	TObjectPtr<UTooltipImage> TooltipWarningImage_EnableAI;
	UPROPERTY()
	TObjectPtr<UPopupMessageWidget> PopupMessageWidget;
	UPROPERTY()
	TObjectPtr<UAudioSelectWidget> AudioSelectWidget;
	UPROPERTY()
	TObjectPtr<UGameModeSharingWidget> GameModeSharingWidget;

public:
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_CreatorView> CustomGameModesWidget_CreatorView;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCustomGameModesWidget_PropertyView> CustomGameModesWidget_PropertyView;
	UPROPERTY()
	TObjectPtr<UCustomGameModesWidgetBase> CustomGameModesWidget_Current;
	
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget_PropertyView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_CustomGameModes;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UVerticalBox* Box_PropertyView;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UVerticalBox* Box_CreatorView;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UWidgetSwitcher* MenuSwitcher;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_CustomGameModes;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_CreatorView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UMenuButton* MenuButton_PropertyView;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_SaveCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_StartWithoutSaving;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_SaveCustomAndStart;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RemoveSelectedCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RemoveAllCustom;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_CustomizeFromStandard;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_PlayFromStandard;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ImportCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ExportCustom;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_DifficultySelect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_NormalDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_HardDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_DeathDifficulty;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_BeatGrid;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_BeatTrack;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_MultiBeat;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_SingleBeat;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ClusterBeat;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ChargedBeatTrack;

	UPROPERTY(BlueprintReadOnly,Transient , meta = (BindWidgetAnim))
	UWidgetAnimation* TransitionCustomGameModeView;

	FWidgetAnimationDynamicEvent OnTransitionComplete_ToPropertyView;
	FWidgetAnimationDynamicEvent OnTransitionComplete_ToCreatorView;
	
private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();

	/** Sets button defaults and default enabled states */
	void SetupButtons();

	/** Finds the associated default game mode, sets the StartWidgetProperties for both CustomGameModesWidgets, and calls PopulateGameModeOptions */
	void InitCustomGameModesWidgetOptions(const EBaseGameMode& BaseGameMode, const EGameModeDifficulty& Difficulty);
	
	/** Initializes all Custom game mode options based on the BSConfig */
	void PopulateGameModeOptions(const FBSConfig& InBSConfig);

	/** Retrieves all Custom game mode options and returns a BSConfig with those options */
	FBSConfig GetCustomGameModeOptions() const;

	/** Saves a CustomGameMode to save slot using GetCustomGameModeOptions(), displays the saved text, refreshes game mode template combo box and selects it */
	bool SaveCustomAndReselect(const FText& SuccessMessage = FText());

	/** Changes the Save and Start Button states depending on what is selected in ComboBox_GameModeName and TextBox_CustomGameModeName */
	UFUNCTION()
	void UpdateSaveStartButtonStates();
	
	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void ShowAudioFormatSelect(const bool bStartFromDefaultGameMode);

	/** Returns whether or not BSConfig is identical to the currently selected template option */
	bool IsCurrentConfigIdenticalToSelectedCustom();

	/** Checks to see if the GameModeName ComboBox or the CustomGameModeName text box has a matching custom game mode that is already saved,
	 *  and calls ShowConfirmOverwriteMessage and returning true if so */
	bool CheckForExistingAndDisplayOverwriteMessage(const bool bStartGameAfter);
	
	/** Only called by CheckForExistingAndDisplayOverwriteMessage. Initializes a PopupMessage and binds to the buttons,
	 *  calling SaveCustomAndReselect if they choose to override. Optionally starts the game afterwards */
	void ShowConfirmOverwriteMessage(const bool bStartGameAfter);

	/** Initializes a PopupMessage asking the player if they want to overwrite an existing custom game mode, calling SaveCustomAndReselect if they choose to override */
	void ShowConfirmOverwriteMessage_Import(const FBSConfig& ImportedConfig);

	/** Changes the SelectedGameMode depending on input button */
	UFUNCTION()
	void OnButtonClicked_SelectedDefaultGameMode(const UBSButton* Button);

	/** Changes the Difficulty depending on input button */
	UFUNCTION()
	void OnButtonClicked_SelectedDifficulty(const UBSButton* Button);

	/** The Button_CustomizeFromStandard and Button_PlayFromStandard bind to this function */
	UFUNCTION()
	void OnButtonClicked_DefaultGameMode(const UBSButton* Button);

	/** Any Custom Game Mode Button binds to this function */
	UFUNCTION()
	void OnButtonClicked_CustomGameModeButton(const UBSButton* Button);

	/** Any Menu Button binds to this function */
	UFUNCTION()
	void OnButtonClicked_MenuButton(const UBSButton* Button);

	/** Saves the custom game mode to slot, repopulates ComboBox_GameModeName, and selects the new custom game mode */
	void OnButtonClicked_SaveCustom();

	/** Calls CheckForExistingAndDisplayOverwriteMessage, and calls ShowAudioFormatSelect if no existing found */
	void OnButtonClicked_SaveCustomAndStart();

	/** Creates a confirm pop up widget and binds to its buttons, removing the selected custom if confirmed */
	void OnButtonClicked_RemoveSelectedCustom();

	/** Creates a confirm pop up widget and binds to its buttons, removing all custom if confirmed */
	void OnButtonClicked_RemoveAllCustom();

	/** Creates an import custom widget and adds to viewport */
	void OnButtonClicked_ImportCustom();

	/** Copies the game mode to clipboard and updates Text */
	void OnButtonClicked_ExportCustom();

	/** Plays TransitionCustomGameModeView */
	void TransitionGameModeViewToCreator();
	
	/** Plays TransitionCustomGameModeView */
	void TransitionGameModeViewToProperty();

	/** Collapses Box_CreatorView, unbinds TransitionCustomGameModeView, changes CustomGameModesWidget_Current, executes OnCreatorViewVisibilityChanged */
	UFUNCTION()
	void OnTransitionCompleted_ToPropertyView();
	
	/** Collapses Box_PropertyView, unbinds TransitionCustomGameModeView, changes CustomGameModesWidget_Current, executes OnCreatorViewVisibilityChanged */
	UFUNCTION()
	void OnTransitionCompleted_ToCreatorView();

	/** Called when the GameModeTemplate selected option changes in either CustomGameModeWidget */
	void OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty);

	/** Synchronizes properties like CustomGameModeName between CreatorView and PropertyView */
	void SynchronizeStartWidgets();

	/** Returns a TooltipImage widget created and placed inside the BoxToPlaceIn */
	UTooltipImage* ConstructWarningEMarkWidget(UHorizontalBox* BoxToPlaceIn);

	/** Sets the SavedText and plays FadeInFadeOut for the SavedTextWidget corresponding to the CustomGameModesWidget_Current */
	void SetAndPlaySavedText(const FText& InText);

	/** Called when one of the custom game modes widgets has at least one breaking game mode option, or none. Updates the value of bGameModeBreakingOptionPresent
	 *  and Broadcasts OnGameModeBreakingChange if the value is different */
	void OnGameModeBreakingOptionPresentStateChanged(const bool bIsPresent);
	
	/** The BaseGameMode for a selected Preset Game Mode */
	EBaseGameMode PresetSelection_PresetGameMode;
	
	/** The difficulty for a selected Preset Game Mode */
	EGameModeDifficulty PresetSelection_Difficulty;

	/** The custom game mode config */
	FBSConfig GameModeConfig;

	/** Pointer to the custom game mode config, shared with all CustomGameModeWidgets and their children */
	FBSConfig* BSConfig;

	/** Whether or not one of the custom game modes widgets has at least one breaking game mode option, or none */
	bool bGameModeBreakingOptionPresent = true;
};
