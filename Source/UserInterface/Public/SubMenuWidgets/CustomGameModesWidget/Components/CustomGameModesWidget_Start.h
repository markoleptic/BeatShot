// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CustomGameModesWidgetComponent.h"
#include "SubMenuWidgets/CustomGameModesWidget/CustomGameModesWidgetBase.h"
#include "CustomGameModesWidget_Start.generated.h"

class UEditableTextBoxOptionWidget;
class UCheckBoxOptionWidget;
class UComboBoxOptionWidget;

UCLASS()
class USERINTERFACE_API UCustomGameModesWidget_Start : public UCustomGameModesWidgetComponent
{
	GENERATED_BODY()

public:
	virtual void InitComponent(FBSConfig* InConfigPtr, TObjectPtr<UCustomGameModesWidgetComponent> InNext) override;

	/** Broadcast when the user changes anything in a start widget */
	FRequestGameModeTemplateUpdate RequestGameModeTemplateUpdate;

	/** Returns the value of EditableTextBoxOption_CustomGameModeName */
	FString GetNewCustomGameModeName() const;

	/** Returns the options for a start widget since they're not all shared with BSConfig pointer */
	FStartWidgetProperties GetStartWidgetProperties() const;

	/** Sets the value of EditableTextBoxOption_CustomGameModeName */
	void SetNewCustomGameModeName(const FString& InCustomGameModeName) const;

	/** Sets the options for a start widget since they're not all shared with BSConfig pointer */
	void SetStartWidgetProperties(const FStartWidgetProperties& InProperties);

	/** Updates the Difficulty ComboBox selection if different from BSConfig, or if none selected with Preset. Returns true if the selection was changed */
	bool UpdateDifficultySelection(const FString& GameModeTemplateString, const EGameModeDifficulty& Difficulty) const;

	/** Updates the Difficulty ComboBox visibility based on the type of game mode. Returns true if the visibility was changed */
	bool UpdateDifficultyVisibility(const FString& GameModeTemplateString) const;
	
	/** Updates the GameModeTemplate ComboBox visibility based on the type of game mode. Returns true if the visibility was changed */
	bool UpdateGameModeTemplateVisibility(const bool bIsPreset, const bool bIsCustom) const;

	/** Clears all GameModeTemplate options and repopulates */
	void RefreshGameModeTemplateOptions() const;

protected:
	virtual void NativeConstruct() override;
	virtual bool UpdateAllOptionsValid() override;
	virtual void UpdateOptionsFromConfig() override;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBoxOptionWidget* CheckBoxOption_UseTemplate;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_GameModeTemplates;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UComboBoxOptionWidget* ComboBoxOption_GameModeDifficulty;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBoxOptionWidget* EditableTextBoxOption_CustomGameModeName;
	
	UFUNCTION()
	void OnCheckStateChanged_UseTemplate(const bool bChecked);
	UFUNCTION()
	void OnTextChanged_CustomGameModeName(const FText& Text);
	UFUNCTION()
	void OnSelectionChanged_GameModeTemplates(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnSelectionChanged_GameModeDifficulty(const TArray<FString>& Selected, const ESelectInfo::Type SelectionType);
};
