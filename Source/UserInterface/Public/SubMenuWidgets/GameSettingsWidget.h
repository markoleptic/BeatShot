// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "GameSettingsWidget.generated.h"

class UCheckBox;
class UEditableTextBox;
class UButton;
class UColorSelectWidget;
class USavedTextWidget;

UCLASS()
class USERINTERFACE_API UGameSettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:
	/** Returns OnPlayerSettingsChangedDelegate_Game, the delegate that is broadcast when this class saves Game settings */
	FOnPlayerSettingsChanged_Game& GetPublicGameSettingsChangedDelegate() { return OnPlayerSettingsChangedDelegate_Game; }

protected:
	/** Fills all widgets with values from NewSettings */
	void PopulateSettings();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* StartTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* PeakTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* EndTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UCheckBox* UseSeparateOutlineColorCheckbox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* TargetOutlineColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* BeatGridInactiveColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Streak")
	UEditableTextBox* CombatTextFrequency;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Streak")
	UCheckBox* ShowStreakCombatTextCheckBox;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* SaveButton_Game;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* ResetButton_Game;

	UFUNCTION()
	void OnStartTargetColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnPeakTargetColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnEndTargetColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void UseSeparateOutlineColorCheckStateChanged(const bool bIsChecked);
	UFUNCTION()
	void OnTargetOutlineColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnBeatGridInactiveColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnCombatTextFrequencyValueChanged(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnShowCombatTextCheckStateChanged(const bool bIsChecked);
	
	/** Saves the Game Settings */
	UFUNCTION()
	void OnSaveButtonClicked_Game();

	/** Reset Game Settings to defaults and repopulates in Settings Menu. Does not automatically save */
	UFUNCTION()
	void OnResetButtonClicked_Game();

	/** The Player Settings that are changed during setting menu interaction */
	UPROPERTY()
	FPlayerSettings_Game NewSettings;
};
