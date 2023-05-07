// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget_Game.generated.h"

class UCheckBox;
class UEditableTextBox;
class UButton;
class UColorSelectWidget;
class USavedTextWidget;

/** Settings category widget holding game settings */
UCLASS()
class USERINTERFACE_API USettingsMenuWidget_Game : public UUserWidget, public ISaveLoadInterface
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
	UColorSelectWidget* ColorSelect_StartTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* ColorSelect_PeakTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* ColorSelect_EndTargetColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UCheckBox* Checkbox_UseSeparateOutlineColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* ColorSelect_TargetOutlineColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Target Color")
	UColorSelectWidget* ColorSelect_BeatGridInactiveColor;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Streak")
	UEditableTextBox* Value_CombatTextFrequency;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Streak")
	UCheckBox* CheckBox_ShowStreakCombatText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* Button_Save;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Saving")
	UButton* Button_Reset;

	UFUNCTION()
	void OnColorChanged_StartTarget(const FLinearColor& NewColor);
	UFUNCTION()
	void OnOnColorChanged_PeakTarget(const FLinearColor& NewColor);
	UFUNCTION()
	void OnOnColorChanged_EndTarget(const FLinearColor& NewColor);
	UFUNCTION()
	void OnColorChanged_TargetOutline(const FLinearColor& NewColor);
	UFUNCTION()
	void OnColorChanged_BeatGridInactive(const FLinearColor& NewColor);
	
	UFUNCTION()
	void CheckStateChanged_UseSeparateOutlineColor(const bool bIsChecked);
	UFUNCTION()
	void OnCheckStateChanged_ShowCombatText(const bool bIsChecked);
	
	UFUNCTION()
	void OnValueChanged_CombatTextFrequency(const FText& NewValue, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnButtonClicked_Save();
	UFUNCTION()
	void OnButtonClicked_Reset();

	/** The Player Settings that are changed during setting menu interaction */
	UPROPERTY()
	FPlayerSettings_Game NewSettings;
};
