// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveGamePlayerSettings.h"
#include "SaveLoadInterface.h"
#include "Blueprint/UserWidget.h"
#include "Delegates/DelegateCombinations.h"
#include "CrossHairSettingsWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnCrossHairSettingsChanged, const FPlayerSettings&);

class USavedTextWidget;
class UColorSelectWidget;
class UCrossHairWidget;
class USlider;
class UImage;
class UEditableTextBox;
class UButton;

UCLASS()
class USERINTERFACE_API UCrossHairSettingsWidget : public UUserWidget, public ISaveLoadInterface
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;

public:

	/** Executed after new CrossHair settings are saved. Parent widget (SettingsMenuWidget) and calls its
	 *  OnPlayerSettingsChanged delegate */
	FOnCrossHairSettingsChanged OnCrossHairSettingsChanged;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UCrossHairWidget* CrossHairWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UColorSelectWidget* ColorSelectWidget;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* InnerOffsetValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineLengthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* LineWidthValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineOpacityValue;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* OutlineWidthValue;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* InnerOffsetSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineLengthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* LineWidthSlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineOpacitySlider;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* OutlineWidthSlider;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USavedTextWidget* SavedTextWidget;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* ResetToDefaultButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* RevertCrossHairButton;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* SaveCrossHairButton;

private:

	UFUNCTION()
	void OnColorChanged(const FLinearColor& NewColor);
	UFUNCTION()
	void OnInnerOffsetValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineLengthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnLineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineOpacityValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnOutlineWidthValueChange(const FText& NewValue, ETextCommit::Type CommitType);
	
	UFUNCTION()
	void OnInnerOffsetSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineLengthSliderChange(const float NewValue);
	UFUNCTION()
	void OnLineWidthSliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineOpacitySliderChange(const float NewValue);
	UFUNCTION()
	void OnOutlineWidthSliderChange(const float NewValue);

	UFUNCTION()
	void OnResetToDefaultButtonClicked();
	UFUNCTION()
	void OnRevertCrossHairButtonClicked();
	UFUNCTION()
	void OnSaveCrossHairButtonClicked();

	float OnEditableTextBoxChanged(const FText& NewTextValue, UEditableTextBox* TextBoxToChange,
												USlider* SliderToChange,
												const float GridSnapSize, const float Min, const float Max);

	float OnSliderChanged(const float NewValue, UEditableTextBox* TextBoxToChange,
										   const float GridSnapSize);
	
	/** Fills out all CrossHair Settings given PlayerSettings */
	void SetCrossHairOptions(const FPlayerSettings& CrossHairSettings);
	/** The PlayerSettings that were initially loaded */
	FPlayerSettings InitialCrossHairSettings;
	/** The PlayerSettings that are changed when interacting with the CrossHairSettingsWidget */
	FPlayerSettings NewCrossHairSettings;
};