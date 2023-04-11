// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalDelegates.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/GameModeButton.h"
#include "WidgetComponents/TooltipImage.h"
#include "WidgetComponents/TooltipWidget.h"
#include "WidgetComponents/ConstrainedSlider.h"
#include "WidgetComponents/BeatGridSettingsWidget.h"
#include "Blueprint/UserWidget.h"
#include "GameModesWidget.generated.h"

class UGameModes_TargetSpread;
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
class USERINTERFACE_API UGameModesWidget : public UUserWidget, public ISaveLoadInterface, public ITooltipInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

	virtual UTooltipWidget* ConstructTooltipWidget() override;

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
	UPROPERTY(EditDefaultsOnly, Category = "Classes | Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "Classes | TargetSpread")
	TSubclassOf<UGameModes_TargetSpread> TargetSpreadClass;

	TObjectPtr<UGameModes_TargetSpread> TargetSpread;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Navigation")
	UVerticalBox* TargetSpreadBox;
	
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
	UBeatGridSettingsWidget* BeatGridSpacingConstrained;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UBorder* BeatTrackSpecificSettings;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | BeatTrack")
	UConstrainedSlider* TargetSpeedConstrained;

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
	UTooltipImage* DynamicTargetScaleQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* EnableAIQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* AlphaQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* EpsilonQMark;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* GammaQMark;

	private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();
	
	void SetHiddenConfigParameters(FBSConfig& Config);

	/** Changes the background colors for the Game Mode of the ClickedButton */
	void SetGameModeButtonBackgroundColor(const UGameModeButton* ClickedButton) const;
	
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

	/** Returns the FBSConfig corresponding to the input DefaultMode string */
	FBSConfig FindDefaultGameMode(const FString& GameModeName) const;

	/** Returns the FBSConfig corresponding to the input CustomGameModeName string */
	FBSConfig FindCustomGameMode(const FString& CustomGameModeName) const;

	/** Returns whether or not the DefaultMode is part of the game's default game modes */
	bool IsDefaultGameMode(const FString& GameModeName) const;

	/** Returns whether or not the DefaultMode is already a custom game mode name */
	bool IsCustomGameMode(const FString& GameModeName) const;
	
	UFUNCTION(Category = "Navigation")
	void SlideButtons(const USlideRightButton* ActiveButton);
	
	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_DefaultGameModes() { SlideButtons(DefaultGameModesButton); }

	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_CustomGameModes() { SlideButtons(CustomGameModesButton); }

	/** Switches to CustomGameModes and populates the CustomGameModeOptions with what's selected in DefaultGameModes */
	UFUNCTION(Category = "Navigation")
	void OnButtonClicked_CustomizeFromStandard();

	/** Saves the custom game mode to slot, repopulates GameModeNameComboBox, and selects the new custom game mode */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustom();

	/** Passes SelectedGameMode to GameInstance without saving, fades to black, and opens the Range level */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_StartWithoutSaving();

	/** Saves the custom game mode and passes it to GameInstance, fades to black, and opens the Range level */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_SaveCustomAndStart();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_PlayFromStandard();

	/** Passes SelectedGameMode to GameInstance, fades to black, and opens the Range level */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_StartCustom();

	/** Calls RemoveCustomGameMode */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_RemoveSelectedCustom();
	
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_RemoveAllCustom();

	/** Overwrites a custom game mode by invoking SaveCustomGameModeToSlot and calls DefaultPlayerController to hide the message */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_ConfirmOverwrite();

	/** Overwrites a custom game mode by invoking SaveCustomGameModeToSlot and calls DefaultPlayerController to hide the message. Calls ShowAudioFormatSelect after */
	UFUNCTION(Category = "SaveStart")
	void OnButtonClicked_ConfirmOverwriteAndStartGame();

	/** Does not overwrite a custom game mode, and calls DefaultPlayerController to hide the message */
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
	
	/** Changes the CustomGameModeName in SelectedGameMode, and changes the Custom Save Button states */
	UFUNCTION()
	void OnTextChanged_CustomGameMode(const FText& NewCustomGameModeText);

	/** Populate Game Mode Options and changes the Custom Save Button states */
	UFUNCTION()
	void OnSelectionChanged_GameModeName(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType);
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

	/** The array of Default Game Mode */
	TArray<FBSConfig> DefaultModes;
	
	/** The Spread type for a selected Default Game Mode */
	ESpreadType DefaultSpreadType;
	
	/** The Default Mode type for a selected Default Game Mode */
	EDefaultMode DefaultMode;
	
	/** The difficulty for a selected Default Game Mode */
	EGameModeDifficulty DefaultDifficulty;
};
