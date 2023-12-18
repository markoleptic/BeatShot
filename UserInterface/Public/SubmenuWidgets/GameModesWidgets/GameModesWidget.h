// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BSWidgetInterface.h"
#include "HttpRequestInterface.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "OverlayWidgets/PopupWidgets/PopupMessageWidget.h"
#include "WidgetComponents/BSCarouselNavBar.h"
#include "GameModesWidget.generated.h"

class UCommonWidgetCarousel;
class UBSVerticalBox;
class UDefaultGameModeOptionWidget;
class UCGMW_Base;
class UCGMW_PropertyView;
class UCGMW_CreatorView;
class UGameModeSharingWidget;
class UTooltipImage;
class UAudioSelectWidget;
class UHorizontalBox;
class USavedTextWidget;
class UPopupMessageWidget;
class UVerticalBox;
class UBorder;
class UEditableTextBox;
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

USTRUCT(BlueprintType)
struct FDefaultGameModeParams
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText GameModeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AltDescriptionText;

	FDefaultGameModeParams()
	{
		GameModeName = FText();
		AltDescriptionText = FText();
	}
	FORCEINLINE bool operator==(const FDefaultGameModeParams& Other) const
	{
		return GameModeName.EqualTo(Other.GameModeName);
	}
	FORCEINLINE bool operator<(const FDefaultGameModeParams& Other) const
	{
		return GameModeName.ToString() < Other.GameModeName.ToString();
	}
};

DECLARE_MULTICAST_DELEGATE_OneParam(FRequestSimulateTargetManagerStateChange, const bool bSimulate)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameModeBreakingChange, const bool bIsGameModeBreaking);

/** The base widget for selecting or customizing a game mode. The custom portion is split into multiple
 *  SettingsCategoryWidgets. Includes a default game modes section */
UCLASS()
class USERINTERFACE_API UGameModesWidget : public UUserWidget, public ISaveLoadInterface, public IBSWidgetInterface, public IHttpRequestInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void NativeDestruct() override;
	virtual UBSGameModeDataAsset* GetGameModeDataAsset() const override { return GameModeDataAsset.Get(); }
	virtual UTooltipWidget* ConstructTooltipWidget() override { return nullptr; }

public:
	/** Returns BSConfig */
	FBSConfig* GetConfigPointer() const { return BSConfig; }

	/** Ends the game mode preview */
	void StopGameModePreview();
	
	/** Whether or not this widget is MainMenu child or a PostGameMenu child */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Default", meta = (ExposeOnSpawn="true"))
	bool bIsMainMenuChild;

	/** Executes when the user is exiting the GameModesWidget, broadcast to GameInstance to handle transition */
	FOnGameModeStateChanged OnGameModeStateChanged;
	
	/** Broadcast false when any non-defining config option is false. Broadcasts true only if all are true.
	 *  Only Broadcasts if different than the previous */
	FOnGameModeBreakingChange OnGameModeBreakingChange;

	/** Called to request the start or stop of a game mode preview */
	FRequestSimulateTargetManagerStateChange RequestSimulateTargetManagerStateChange;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget")
	TArray<FText> NavBarButtonText_DefaultCustom;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget")
	TArray<FText> NavBarButtonText_CreatorProperty;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|CustomGameModes")
	TObjectPtr<UBSGameModeDataAsset> GameModeDataAsset;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|Classes|AudioSelect")
	TSubclassOf<UPopupMessageWidget> PopupMessageClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|Classes|AudioSelect")
	TSubclassOf<UAudioSelectWidget> AudioSelectClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|Classes|Tooltip")
	TSubclassOf<UTooltipImage> WarningEMarkClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|Classes|Custom Game Modes")
	TSubclassOf<UGameModeSharingWidget> GameModeSharingClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|Classes|Tooltip")
	TSubclassOf<UTooltipWidget> TooltipWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|DefaultGameModes")
	TSubclassOf<UDefaultGameModeOptionWidget> DefaultGameModesWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "GameModesWidget|DefaultGameModes", meta=(ForceInlineRow))
	TMap<EBaseGameMode, FDefaultGameModeParams> DefaultGameModesParams;
	
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
	TObjectPtr<UCGMW_CreatorView> CustomGameModesWidget_CreatorView;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	TObjectPtr<UCGMW_PropertyView> CustomGameModesWidget_PropertyView;
	UPROPERTY()
	TObjectPtr<UCGMW_Base> CustomGameModesWidget_Current;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget_PropertyView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_DefaultGameModes;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UVerticalBox* Box_CustomGameModes;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget))
	UVerticalBox* Box_PropertyView;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSVerticalBox* Box_DefaultGameModesOptions;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_SaveCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_StartFromCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RemoveSelectedCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_RemoveAllCustom;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_CustomizeFromPreset;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_StartFromPreset;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ImportCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ExportCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_ClearRLHistory;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBorder* Border_DifficultySelect;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_NormalDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_HardDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSButton* Button_DeathDifficulty;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonWidgetCarousel* Carousel_DefaultCustom;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSCarouselNavBar* CarouselNavBar_DefaultCustom;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonWidgetCarousel* Carousel_CreatorProperty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSCarouselNavBar* CarouselNavBar_CreatorProperty;

private:
	/** Binds all widget delegates to functions */
	void BindAllDelegates();

	/** Sets button defaults and default enabled states */
	void SetupButtons();

	/** Finds the associated default game mode, sets the StartWidgetProperties for both CustomGameModesWidgets, and
	 *  calls PopulateGameModeOptions */
	void InitCustomGameModesWidgetOptions(const EBaseGameMode& BaseGameMode, const EGameModeDifficulty& Difficulty);

	/** Initializes all Default Game Mode Menu Options found in Box_DefaultGameModesOptions */
	void InitDefaultGameModesWidgets();

	UFUNCTION()
	void OnCarouselWidgetIndexChanged_DefaultCustom(UCommonWidgetCarousel* InCarousel, const int32 NewIndex);

	UFUNCTION()
	void OnCarouselWidgetIndexChanged_CreatorProperty(UCommonWidgetCarousel* InCarousel, const int32 NewIndex);

	/** Initializes all Custom game mode options based on the BSConfig */
	void PopulateGameModeOptions(const FBSConfig& InBSConfig);

	/** Retrieves all Custom game mode options and returns a BSConfig with those options */
	FBSConfig GetCustomGameModeOptions() const;

	/** Saves a CustomGameMode to save slot using GetCustomGameModeOptions(), displays the saved text,
	 *  refreshes game mode template combo box and selects it */
	bool SaveCustomAndReselect(const FText& SuccessMessage = FText());

	/** Changes the Save and Start Button states depending on what is selected in ComboBox_GameModeName and
	 *  TextBox_CustomGameModeName */
	UFUNCTION()
	void UpdateSaveStartButtonStates();

	/** Checks to see if SelectedGameMode is valid, Binds to ScreenFadeToBlackFinish, and ends the game mode */
	void ShowAudioFormatSelect(const bool bStartFromDefaultGameMode);

	/** Returns whether or not BSConfig is identical to the currently selected template option */
	bool IsCurrentConfigIdenticalToSelectedCustom();

	/** Checks to see if the GameModeName ComboBox or the CustomGameModeName text box has a matching custom game mode
	 *  that is already saved */
	bool DoesCustomGameModeExist();

	/** Initializes a PopupMessage asking the player if they want to overwrite an existing custom game mode, calling
	 *  SaveCustomAndReselect if they choose to override */
	void ShowConfirmOverwriteMessage_Import(const FBSConfig& ImportedConfig);

	/** Changes the SelectedGameMode depending on input button */
	UFUNCTION()
	void OnButtonClicked_SelectedDefaultGameMode(const UBSButton* Button);

	/** Changes the Difficulty depending on input button */
	UFUNCTION()
	void OnButtonClicked_SelectedDifficulty(const UBSButton* Button);

	/** The Button_CustomizeFromPreset and Button_StartFromPreset bind to this function */
	UFUNCTION()
	void OnButtonClicked_DefaultGameMode(const UBSButton* Button);

	/** Any Custom Game Mode Button binds to this function */
	UFUNCTION()
	void OnButtonClicked_CustomGameModeButton(const UBSButton* Button);

	/** Saves the custom game mode to slot, repopulates ComboBox_GameModeName, and selects the new custom game mode */
	void OnButtonClicked_SaveCustom();

	/** Calls DoesCustomGameModeExist, and calls ShowAudioFormatSelect if no existing found */
	void OnButtonClicked_StartFromCustom();

	/** Creates a confirm pop up widget and binds to its buttons, removing the selected custom if confirmed */
	void OnButtonClicked_RemoveSelectedCustom();

	/** Creates a confirm pop up widget and binds to its buttons, removing all custom if confirmed */
	void OnButtonClicked_RemoveAllCustom();

	/** Creates an import custom widget and adds to viewport */
	void OnButtonClicked_ImportCustom();

	/** Copies the game mode to clipboard and updates Text */
	void OnButtonClicked_ExportCustom();

	/** Creates a confirm pop up widget and binds to its buttons, clearing the RL history of a game mode if confirmed */
	void OnButtonClicked_ClearRLHistory();

	/** Callback function for receiving an access token response */
	void OnAccessTokenResponseReceived(const FString& AccessToken, FString GameModeNameToRemove);

	/** Callback function for receiving a delete scores response */
	void OnDeleteScoresResponseReceived(const int32 NumScoresRemoved, const int32 ResponseCode, FString GameModeNameToRemove);

	/** Called when the GameModeTemplate selected option changes in either CustomGameModeWidget */
	void OnRequestGameModeTemplateUpdate(const FString& InGameMode, const EGameModeDifficulty& Difficulty);

	/** Synchronizes properties like CustomGameModeName between CreatorView and PropertyView */
	void SynchronizeStartWidgets();

	/** Sets the SavedText and plays FadeInFadeOut for the SavedTextWidget corresponding to the CustomGameModesWidget_Current */
	void SetAndPlaySavedText(const FText& InText);

	/** Called when one of the custom game modes widgets has at least one breaking game mode option, or none.
	 *  Updates the value of bGameModeBreakingOptionPresent and Broadcasts OnGameModeBreakingChange if the value is different */
	void OnGameModeBreakingOptionPresentStateChanged(const bool bIsPresent);

	/** Restarts the game mode preview */
	void RefreshGameModePreview();

	/** The BaseGameMode for a selected Preset Game Mode */
	EBaseGameMode PresetSelection_PresetGameMode;

	/** The difficulty for a selected Preset Game Mode */
	EGameModeDifficulty PresetSelection_Difficulty;

	/** The custom game mode config */
	FBSConfig GameModeConfig;

	/** Pointer to the custom game mode config, shared with all CustomGameModeWidgets and their children */
	FBSConfig* BSConfig;

	/** Whether or not one of the custom game modes widgets has at least one breaking game mode option, or none */
	bool bGameModeBreakingOptionPresent = false;

	FOnAccessTokenResponse OnAccessTokenResponse;
	FOnDeleteScoresResponse OnDeleteScoresResponse;
};
