﻿// Copyright 2022-2023 Markoleptic Games, SP. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MenuOptionWidget.h"
#include "ConstantMinMaxMenuOptionWidget.generated.h"

class UCommonTextBlock;

UENUM(BlueprintType)
enum class EConstantMinMaxType : uint8
{
	Constant UMETA(DisplayName="Constant"),
	Min UMETA(DisplayName="Min"),
	Max UMETA(DisplayName="Max"),
};
ENUM_RANGE_BY_FIRST_AND_LAST(EConstantMinMaxType, EConstantMinMaxType::Constant,
	EConstantMinMaxType::Max);

class UConstantMinMaxMenuOptionWidget;
DECLARE_MULTICAST_DELEGATE_FourParams(FOnMinMaxMenuOptionChanged, UConstantMinMaxMenuOptionWidget* Widget,
	const bool bChecked, const float MinOrConstantValue, const float MaxValue);

UCLASS()
class USERINTERFACE_API UConstantMinMaxMenuOptionWidget : public UMenuOptionWidget
{
	GENERATED_BODY()

public:
	/** Set's the widget's enabled state and adds a tooltip for the entire widget if a Key is provided,
	 *  otherwise the tooltip will be cleared. */
	virtual void SetMenuOptionEnabledState(const EMenuOptionEnabledState EnabledState) override;
	
	/** Sets the custom enabled state of the menu option */
	virtual UWidget* SetSubMenuOptionEnabledState(const TSubclassOf<UWidget> SubWidget, const EMenuOptionEnabledState State) override;
	
	/** Returns the value for the MinOrConstant slider */
	float GetMinSliderValue(const bool bClamped) const;

	/** Returns the value for the Max slider, NOT the max value of a slider */
	float GetMaxSliderValue(const bool bClamped) const;

	/** Returns the value for the constant slider */
	float GetConstantSliderValue(const bool bClamped) const;

	/** Returns the value for the MinOrConstant EditableTextBox */
	float GetMinEditableTextBoxValue(const bool bClamped) const;

	/** Returns the value for the Max EditableTextBox */
	float GetMaxEditableTextBoxValue(const bool bClamped) const;

	/** Returns the value for the constant EditableTextBox */
	float GetConstantEditableTextBoxValue(const bool bClamped) const;

	/** Returns true if the if the CheckBox is checked and only one slider and editable text box is visible */
	bool IsInConstantMode() const;
	
	/** Sets the Min and Max values of the sliders and sets the grid snap size */
	void SetValues(const float Min, const float Max, const float SnapSize);

	/** Sets the value for the ConstantOrMin slider & EditableTextBox */
	void SetValue_Min(const float Value) const;

	/** Sets the value for the Max slider & EditableTextBox */
	void SetValue_Max(const float Value) const;

	/** Sets the value for both sliders & EditableTextBoxes */
	void SetValue_Constant(const float Value) const;

	/** Sets the checked state for the Checkbox and calls UpdateMinMaxDependencies to update visibility stuff */
	void SetConstantMode(const bool bUseConstantMode) const;

	/** Locks or unlocks the Checkbox for constant mode */
	void SetConstantModeLocked(const bool bLock) const;

	/** Sets the the value of bUseMinAsConstant */
	void SetUseMinAsConstant(const bool bInUseMinAsConstant) { bUseMinAsConstant = bInUseMinAsConstant; }
	
	/** Locks or unlocks the slider and sets the EditableTextBox to read-only or not */
	void SetSliderAndTextBoxEnabledStates(const bool bEnabled) const;

	/** Returns the current snap size */
	float GetSnapSize() const { return GridSnapSize; }

	/** Broadcast when the slider value changes or the EditableTextBox has text committed to it */
	FOnMinMaxMenuOptionChanged OnMinMaxMenuOptionChanged;

protected:
	virtual void NativeConstruct() override;
	virtual void NativePreConstruct() override;
	virtual void SetStyling() override;
		
	/** If true, collapses BSBox_Max and MinText. Otherwise sets them visible and sets the MinText */
	void UpdateMinMaxDependencies(const bool bConstant) const;

	UFUNCTION()
	void OnSliderChanged_Min(const float Value);
	UFUNCTION()
	void OnSliderChanged_Max(const float Value);
	UFUNCTION()
	void OnTextCommitted_Min(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnTextCommitted_Max(const FText& Text, ETextCommit::Type CommitType);
	UFUNCTION()
	void OnCheckStateChanged_CheckBox(const bool bChecked);
	
	float GridSnapSize = 1.f;

	/** Whether or not to use the minimum or maximum slider when the checkbox is checked. */
	bool bUseMinAsConstant = true;

	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_CheckBox_Tooltip;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Left_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Right_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Description_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Min;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_Min;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UBSHorizontalBox* BSBox_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Left_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UHorizontalBox* Box_Right_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USpacer* Indent_Left_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Description_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	USlider* Slider_Max;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UEditableTextBox* EditableTextBox_Max;
	
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCommonTextBlock* TextBlock_CheckBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UCheckBox* CheckBox;

	UPROPERTY(EditInstanceOnly, Category = "ConstantMinMaxMenuOptionWidget")
	FText MinText = FText::FromString("Min");

	UPROPERTY(EditInstanceOnly, Category = "ConstantMinMaxMenuOptionWidget")
	FText MaxText = FText::FromString("Max");
};
