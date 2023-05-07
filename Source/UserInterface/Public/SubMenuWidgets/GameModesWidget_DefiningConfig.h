// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalEnums.h"
#include "GlobalStructs.h"
#include "SaveLoadInterface.h"
#include "WidgetComponents/BSSettingCategoryWidget.h"
#include "GameModesWidget_DefiningConfig.generated.h"

class UBSHorizontalBox;
class UHorizontalBox;
class UComboBoxString;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnRepopulateGameModeOptions, const FBSConfig& BSConfig);
DECLARE_MULTICAST_DELEGATE(FOnDefiningConfigUpdate_SaveStartButtonStates);

/** SettingCategoryWidget for the GameModesWidget that holds Defining configuration settings */
UCLASS()
class USERINTERFACE_API UGameModesWidget_DefiningConfig : public UBSSettingCategoryWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	friend class UGameModesWidget;
	virtual void NativeConstruct() override;
	virtual void InitSettingCategoryWidget() override;
	
public:
	void InitializeDefiningConfig(const FBS_DefiningConfig& InDefiningConfig, const EDefaultMode& BaseGameMode);
	FBS_DefiningConfig GetDefiningConfig() const;

	/* Broadcast when a game mode selection change has occurred */
	FOnRepopulateGameModeOptions OnRepopulateGameModeOptions;

	/* Broadcast when an event that requires a Save/Start button refresh is required */
	FOnDefiningConfigUpdate_SaveStartButtonStates OnDefiningConfigUpdate_SaveStartButtonStates;

	/** Updates ComboBox_GameModeName with CustomGameModes */
	void PopulateGameModeNameComboBox(const FString& GameModeOptionToSelect);

	/** Calls PopulateGameModeNameComboBox but changes the GameModeOptionToSelect parameter based on how the game mode was saved */
	void PopulateGameModeNameComboBoxAfterSave();

protected:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UBSHorizontalBox* BSBox_GameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UBSHorizontalBox* BSBox_CustomGameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UBSHorizontalBox* BSBox_BaseGameMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UBSHorizontalBox* BSBox_GameModeDifficulty;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* ComboBox_GameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UEditableTextBox* TextBox_CustomGameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* ComboBox_BaseGameMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Custom Game Modes | General")
	UComboBoxString* ComboBox_GameModeDifficulty;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_GameModeTemplate;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_CustomGameModeName;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_BaseGameMode;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Tooltip")
	UTooltipImage* QMark_GameModeDifficulty;

	UFUNCTION()
	void OnTextChanged_CustomGameMode(const FText& NewCustomGameModeText);
	UFUNCTION()
	void OnSelectionChanged_GameModeName(const FString SelectedGameModeName, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_BaseGameMode(const FString SelectedBaseGameMode, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_GameModeDifficulty(const FString SelectedDifficulty, const ESelectInfo::Type SelectionType);
};
